
#include "Client.hpp"

#include "Configuration.hpp"
#include "TLS/ClientContext.hpp"
#include "Socket.hpp"
#include "Address.hpp"
#include "Stream.hpp"

#include <unordered_map>

VALUE Protocol_QUIC_Client = Qnil;

class RubyClient : public Protocol::QUIC::Client {
public:
	VALUE self;
	
private:
	VALUE _configuration;
	VALUE _tls_context;
	VALUE _socket;
	VALUE _remote_address;
	std::unordered_map<Protocol::QUIC::StreamID, VALUE> _ruby_streams;
	
public:
	RubyClient(VALUE self, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE chosen_version) : Protocol::QUIC::Client(*Protocol_QUIC_Configuration_get(configuration), *Protocol_QUIC_TLS_ClientContext_get(tls_context), *Protocol_QUIC_Socket_get(socket), *Protocol_QUIC_Address_get(remote_address), RB_NUM2UINT(chosen_version)), self(self), _configuration(configuration), _tls_context(tls_context), _socket(socket), _remote_address(remote_address) {}
	virtual ~RubyClient() {}
	
	void handshake_completed() override
	{
		rb_funcall(self, rb_intern("handshake_completed"), 0);
	}
	
	Protocol::QUIC::Stream * create_stream(Protocol::QUIC::StreamID stream_id) override
	{
		VALUE stream = rb_funcall(self, rb_intern("create_stream"), 1, RB_LL2NUM(stream_id));
		_ruby_streams[stream_id] = stream;
		
		return Protocol_QUIC_Stream_get(stream);
	}
	
	void disconnect() override
	{
		Protocol::QUIC::Client::disconnect();
		_ruby_streams.clear();
	}
	
	void stream_close(Protocol::QUIC::Stream * stream, std::int32_t flags, std::uint64_t error_code) override
	{
		auto stream_id = stream->stream_id();
		Protocol::QUIC::Client::stream_close(stream, flags, error_code);
		_ruby_streams.erase(stream_id);
	}
	
	void stream_reset(Protocol::QUIC::Stream * stream, std::size_t final_size, std::uint64_t error_code) override
	{
		auto stream_id = stream->stream_id();
		Protocol::QUIC::Client::stream_reset(stream, final_size, error_code);
		_ruby_streams.erase(stream_id);
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

static void Protocol_QUIC_Client_mark(void *data)
{
	if (data) {
		reinterpret_cast<RubyClient *>(data)->mark();
	}
}

static void Protocol_QUIC_Client_compact(void *data)
{
	if (data) {
		reinterpret_cast<RubyClient *>(data)->compact();
	}
}

static void Protocol_QUIC_Client_free(void *data)
{
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Client *>(data);
	}
}

static size_t Protocol_QUIC_Client_size(const void *data) {
	return sizeof(RubyClient);
}

static const rb_data_type_t Protocol_QUIC_Client_type = {
	.wrap_struct_name = "Protocol::QUIC::Client",
	.function = {
		.dmark = Protocol_QUIC_Client_mark,
		.dfree = Protocol_QUIC_Client_free,
		.dsize = Protocol_QUIC_Client_size,
		.dcompact = Protocol_QUIC_Client_compact,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::Client * Protocol_QUIC_Client_get(VALUE self)
{
	Protocol::QUIC::Client *configuration;
	TypedData_Get_Struct(self, Protocol::QUIC::Client, &Protocol_QUIC_Client_type, configuration);
	return configuration;
}

static VALUE Protocol_QUIC_Client_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Client_type, NULL);
}

static VALUE Protocol_QUIC_Client_initialize(VALUE self, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE chosen_version) {
	Protocol::QUIC::Client *client = new RubyClient(self, configuration, tls_context, socket, remote_address, chosen_version);
	DATA_PTR(self) = client;
	return self;
}

static VALUE Protocol_QUIC_Client_connect(VALUE self) {
	Protocol::QUIC::Client *client = Protocol_QUIC_Client_get(self);
	
	client->connect();
	
	return Qnil;
}

void Init_Protocol_QUIC_Client(VALUE Protocol_QUIC) {
	Protocol_QUIC_Client =
			rb_define_class_under(Protocol_QUIC, "Client", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_Client, Protocol_QUIC_Client_allocate);
	rb_define_method(Protocol_QUIC_Client, "initialize", Protocol_QUIC_Client_initialize, 5);
	
	rb_define_method(Protocol_QUIC_Client, "connect", Protocol_QUIC_Client_connect, 0);
}
