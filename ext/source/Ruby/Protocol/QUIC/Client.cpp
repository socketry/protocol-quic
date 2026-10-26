
#include "Client.hpp"

#include "Configuration.hpp"
#include "TLS/ClientContext.hpp"
#include "Socket.hpp"
#include "Address.hpp"
#include "Stream.hpp"
#include "Connection.hpp"
#include "Callback.hpp"

#include <ngtcp2/ngtcp2.h>

#include <unordered_map>

VALUE Ruby_Protocol_QUIC_Client = Qnil;

namespace Ruby::Protocol::QUIC {

	class Client : public ::Protocol::QUIC::Client, public StreamOwner {
	public:
		VALUE self;
	
	private:
		VALUE _configuration;
		VALUE _tls_context;
		VALUE _socket;
		VALUE _remote_address;
		std::unordered_map<::Protocol::QUIC::StreamID, VALUE> _ruby_streams;
	
	public:
		Client(VALUE self, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE chosen_version) : ::Protocol::QUIC::Client(*Ruby_Protocol_QUIC_Configuration_get(configuration), *Ruby_Protocol_QUIC_TLS_ClientContext_get(tls_context), *Ruby_Protocol_QUIC_Socket_get(socket), *Ruby_Protocol_QUIC_Address_get(remote_address), RB_NUM2UINT(chosen_version)), self(self), _configuration(configuration), _tls_context(tls_context), _socket(socket), _remote_address(remote_address)
		{
			StreamOwner::tls_context = Ruby_Protocol_QUIC_TLS_ClientContext_get(tls_context);
			retain_context(StreamOwner::tls_context);
		}
		
		virtual ~Client() {}
	
		void handshake_completed() override
		{
			protected_call(self, "handshake_completed");
		}
	
		::Protocol::QUIC::Stream * create_stream(::Protocol::QUIC::StreamID stream_id) override
		{
			VALUE argument = RB_LL2NUM(stream_id);
			VALUE stream = protected_call(self, "create_stream", 1, &argument);
			
			if (NIL_P(stream)) throw std::runtime_error("create_stream returned nil");
			
			_ruby_streams[stream_id] = stream;
		
			return Ruby_Protocol_QUIC_Stream_get(stream);
		}
	
		// A connection closed by the peer (draining) or by the idle timeout must not try to send CONNECTION_CLOSE.
		// Thrown to leave Client::connect after an idle timeout; see handle_error.
		struct IdleClosed {};
	
		Status handle_error(int result, std::string_view reason = "") override
		{
			if (result == NGTCP2_ERR_DRAINING) {
				set_last_error(result, reason);
				return Status::DRAINING;
			}
			
			// After an idle timeout the connection is neither closing nor draining, so Client::connect would keep
			// retrying with an expired timer and never return. It is a silent close (RFC 9000 §10.1): leave the loop.
			// Only C++ frames are unwound: handle_expiry is not called from within ngtcp2.
			if (result == NGTCP2_ERR_IDLE_CLOSE) {
				set_last_error(result, reason);
				throw IdleClosed{};
			}
			
			return ::Protocol::QUIC::Client::handle_error(result, reason);
		}
	
		void disconnect() override
		{
			::Protocol::QUIC::Client::disconnect();
			_ruby_streams.clear();
		}
	
		void stream_close(::Protocol::QUIC::Stream * stream, std::int32_t flags, std::uint64_t error_code) override
		{
			auto stream_id = stream->stream_id();
			::Protocol::QUIC::Client::stream_close(stream, flags, error_code);
			_ruby_streams.erase(stream_id);
		}
	
		// Keep the stream registered after RESET_STREAM; ngtcp2 calls `stream_close` for it later.
		void stream_reset(::Protocol::QUIC::Stream * stream, std::size_t final_size, std::uint64_t error_code) override
		{
			stream->reset(final_size, error_code);
		}
	
		void mark() {
			rb_gc_mark_movable(self);
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

static void Ruby_Protocol_QUIC_Client_mark(void *data)
{
	if (data) {
		reinterpret_cast<Ruby::Protocol::QUIC::Client *>(data)->mark();
	}
}

static void Ruby_Protocol_QUIC_Client_compact(void *data)
{
	if (data) {
		reinterpret_cast<Ruby::Protocol::QUIC::Client *>(data)->compact();
	}
}

static void Ruby_Protocol_QUIC_Client_free(void *data)
{
	if (data) {
		Ruby::Protocol::QUIC::release_connection(reinterpret_cast<Ruby::Protocol::QUIC::Client *>(data));
	}
}

static size_t Ruby_Protocol_QUIC_Client_size(const void *data) {
	return sizeof(Ruby::Protocol::QUIC::Client);
}

static const rb_data_type_t Ruby_Protocol_QUIC_Client_type = {
	.wrap_struct_name = "Protocol::QUIC::Client",
	.function = {
		.dmark = Ruby_Protocol_QUIC_Client_mark,
		.dfree = Ruby_Protocol_QUIC_Client_free,
		.dsize = Ruby_Protocol_QUIC_Client_size,
		.dcompact = Ruby_Protocol_QUIC_Client_compact,
	},
	.parent = &Ruby_Protocol_QUIC_Connection_type,
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

::Protocol::QUIC::Client * Ruby_Protocol_QUIC_Client_get(VALUE self)
{
	::Protocol::QUIC::Client *configuration;
	TypedData_Get_Struct(self, ::Protocol::QUIC::Client, &Ruby_Protocol_QUIC_Client_type, configuration);
	return configuration;
}

static VALUE Ruby_Protocol_QUIC_Client_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Ruby_Protocol_QUIC_Client_type, NULL);
}

static VALUE Ruby_Protocol_QUIC_Client_initialize(VALUE self, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE chosen_version) {
	return Ruby::Protocol::QUIC::guard([&]{
		DATA_PTR(self) = new Ruby::Protocol::QUIC::Client(self, configuration, tls_context, socket, remote_address, chosen_version);
		return self;
	});
}

static VALUE Ruby_Protocol_QUIC_Client_connect(VALUE self) {
	auto client = dynamic_cast<Ruby::Protocol::QUIC::Client *>(Ruby_Protocol_QUIC_Client_get(self));
	
	return Ruby::Protocol::QUIC::guard([&]{
		try {
			client->connect();
		} catch (Ruby::Protocol::QUIC::Client::IdleClosed &) {
			client->disconnect();
		}
		return Qnil;
	});
}

void Init_Ruby_Protocol_QUIC_Client(VALUE Protocol_QUIC) {
	Ruby_Protocol_QUIC_Client =
			rb_define_class_under(Protocol_QUIC, "Client", rb_cObject);

	rb_define_alloc_func(Ruby_Protocol_QUIC_Client, Ruby_Protocol_QUIC_Client_allocate);
	rb_define_method(Ruby_Protocol_QUIC_Client, "initialize", Ruby_Protocol_QUIC_Client_initialize, 5);
	
	rb_define_method(Ruby_Protocol_QUIC_Client, "connect", Ruby_Protocol_QUIC_Client_connect, 0);
}
