
#include "Server.hpp"

#include "Dispatcher.hpp"
#include "Configuration.hpp"
#include "Connection.hpp"
#include "TLS/ServerContext.hpp"
#include "Socket.hpp"
#include "Address.hpp"
#include "PacketHeader.hpp"

#include "Stream.hpp"
#include "Callback.hpp"

#include <unordered_map>

VALUE Ruby_Protocol_QUIC_Server = Qnil;

namespace Ruby::Protocol::QUIC {

	class Server : public ::Protocol::QUIC::Server, public StreamOwner {
	public:
		VALUE self;
	
	private:
		VALUE _dispatcher;
		VALUE _configuration;
		VALUE _tls_context;
		VALUE _socket;
		VALUE _remote_address;
		std::unordered_map<::Protocol::QUIC::StreamID, VALUE> _ruby_streams;
	
		static bool remote_bidirectional(::Protocol::QUIC::StreamID stream_id)
		{
			// Client-initiated bidirectional streams have the two least significant bits 0b00 (RFC 9000 §2.1).
			return (stream_id & 0x3) == 0;
		}
	
	public:
		Server(VALUE self, VALUE dispatcher, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE packet_header, VALUE ocid) : ::Protocol::QUIC::Server(*Ruby_Protocol_QUIC_Dispatcher_get(dispatcher), *Ruby_Protocol_QUIC_Configuration_get(configuration), *Ruby_Protocol_QUIC_TLS_ServerContext_get(tls_context), *Ruby_Protocol_QUIC_Socket_get(socket), *Ruby_Protocol_QUIC_Address_get(remote_address), *Ruby_Protocol_QUIC_PacketHeader_get(packet_header), nullptr), self(self), _dispatcher(dispatcher), _configuration(configuration), _tls_context(tls_context), _socket(socket), _remote_address(remote_address)
		{
			// In the body, after the base class has been constructed, so that a constructor that throws never leaves the context counted:
			StreamOwner::tls_context = Ruby_Protocol_QUIC_TLS_ServerContext_get(tls_context);
			retain_context(StreamOwner::tls_context);
		}
		
		virtual ~Server() {}
	
		::Protocol::QUIC::Stream * create_stream(::Protocol::QUIC::StreamID stream_id) override
		{
			VALUE argument = RB_LL2NUM(stream_id);
			VALUE stream = protected_call(self, "create_stream", 1, &argument);
			
			if (NIL_P(stream)) throw std::runtime_error("create_stream returned nil");
			
			_ruby_streams[stream_id] = stream;
		
			return Ruby_Protocol_QUIC_Stream_get(stream);
		}
	
		void disconnect() override
		{
			::Protocol::QUIC::Server::disconnect();
			_ruby_streams.clear();
		}
	
		// The idle timeout is a silent close (RFC 9000 §10.1): do not try to send CONNECTION_CLOSE, which would fail.
		Status handle_error(int result, std::string_view reason = "") override
		{
			if (result == NGTCP2_ERR_IDLE_CLOSE) {
				set_last_error(result, reason);
				disconnect();
				return Status::DRAINING;
			}
			
			return ::Protocol::QUIC::Server::handle_error(result, reason);
		}

		void stream_close(::Protocol::QUIC::Stream * stream, std::int32_t flags, std::uint64_t error_code) override
		{
			auto stream_id = stream->stream_id();
			::Protocol::QUIC::Server::stream_close(stream, flags, error_code);
			_ruby_streams.erase(stream_id);
			
			// ngtcp2 does not raise the stream limit by itself; allow the client to open a new stream for each one that is closed.
			if (remote_bidirectional(stream_id)) {
				ngtcp2_conn_extend_max_streams_bidi(_connection, 1);
			}
		}
	
		// Unlike the base class, keep the stream registered after RESET_STREAM: ngtcp2 still calls `stream_close` for it later, and the base class would fail to find it then.
		void stream_reset(::Protocol::QUIC::Stream * stream, std::size_t final_size, std::uint64_t error_code) override
		{
			stream->reset(final_size, error_code);
		}
	
		void mark() {
			rb_gc_mark_movable(self);
			rb_gc_mark_movable(_dispatcher);
			rb_gc_mark_movable(_configuration);
			rb_gc_mark_movable(_tls_context);
			rb_gc_mark_movable(_socket);
			rb_gc_mark_movable(_remote_address);
		
			for (auto & [stream_id, ruby_stream] : _ruby_streams) {
				rb_gc_mark_movable(ruby_stream);
			}
		}
	
		void compact() {
			self = rb_gc_location(self);
			_dispatcher = rb_gc_location(_dispatcher);
			_configuration = rb_gc_location(_configuration);
			_tls_context = rb_gc_location(_tls_context);
			_socket = rb_gc_location(_socket);
			_remote_address = rb_gc_location(_remote_address);
		
			for (auto & [stream_id, ruby_stream] : _ruby_streams) {
				ruby_stream = rb_gc_location(ruby_stream);
			}
		}
	};

}

static void Ruby_Protocol_QUIC_Server_mark(void *data)
{
	if (data) {
		reinterpret_cast<Ruby::Protocol::QUIC::Server *>(data)->mark();
	}
}

static void Ruby_Protocol_QUIC_Server_compact(void *data)
{
	if (data) {
		reinterpret_cast<Ruby::Protocol::QUIC::Server *>(data)->compact();
	}
}

static void Ruby_Protocol_QUIC_Server_free(void *data)
{
	if (data) {
		Ruby::Protocol::QUIC::release_connection(reinterpret_cast<Ruby::Protocol::QUIC::Server *>(data));
	}
}

static size_t Ruby_Protocol_QUIC_Server_size(const void *data) {
	return sizeof(Ruby::Protocol::QUIC::Server);
}

static const rb_data_type_t Ruby_Protocol_QUIC_Server_type = {
	.wrap_struct_name = "Protocol::QUIC::Server",
	.function = {
		.dmark = Ruby_Protocol_QUIC_Server_mark,
		.dfree = Ruby_Protocol_QUIC_Server_free,
		.dsize = Ruby_Protocol_QUIC_Server_size,
		.dcompact = Ruby_Protocol_QUIC_Server_compact,
	},
	.parent = &Ruby_Protocol_QUIC_Connection_type,
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

::Protocol::QUIC::Server * Ruby_Protocol_QUIC_Server_get(VALUE self)
{
	::Protocol::QUIC::Server *server;

	TypedData_Get_Struct(self, ::Protocol::QUIC::Server, &Ruby_Protocol_QUIC_Server_type, server);

	return server;
}

static VALUE Ruby_Protocol_QUIC_Server_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Ruby_Protocol_QUIC_Server_type, NULL);
}

// Dispatcher & dispatcher, Configuration & configuration, TLS::ServerContext & tls_context, Socket & socket, const Address & remote_address, const ngtcp2_pkt_hd & packet_header, ngtcp2_cid *ocid = nullptr
static VALUE Ruby_Protocol_QUIC_Server_initialize(VALUE self, VALUE dispatcher, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE packet_header, VALUE ocid) {
	return Ruby::Protocol::QUIC::guard([&]{
		DATA_PTR(self) = new Ruby::Protocol::QUIC::Server(self, dispatcher, configuration, tls_context, socket, remote_address, packet_header, ocid);
		return self;
	});
}

static VALUE Ruby_Protocol_QUIC_Server_send_packets(VALUE self) {
	::Protocol::QUIC::Server *server = Ruby_Protocol_QUIC_Server_get(self);
	
	return Ruby::Protocol::QUIC::guard([&]{
		server->send_packets();
		return Qnil;
	});
}

void Init_Ruby_Protocol_QUIC_Server(VALUE Protocol_QUIC) {
	Ruby_Protocol_QUIC_Server =
			rb_define_class_under(Protocol_QUIC, "Server", rb_cObject);

	rb_define_alloc_func(Ruby_Protocol_QUIC_Server, Ruby_Protocol_QUIC_Server_allocate);
	rb_define_method(Ruby_Protocol_QUIC_Server, "initialize", Ruby_Protocol_QUIC_Server_initialize, 7);
	
	rb_define_method(Ruby_Protocol_QUIC_Server, "send_packets", Ruby_Protocol_QUIC_Server_send_packets, 0);
}
