
#include "Client.hpp"

#include "Configuration.hpp"
#include "TLS/ClientContext.hpp"
#include "Socket.hpp"
#include "Address.hpp"
#include "Stream.hpp"

VALUE Protocol_QUIC_Client = Qnil;

class RubyClient : public Protocol::QUIC::Client {
	VALUE _self;
public:
	RubyClient(VALUE self, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE chosen_version) : Protocol::QUIC::Client(*Protocol_QUIC_Configuration_get(configuration), *Protocol_QUIC_TLS_ClientContext_get(tls_context), *Protocol_QUIC_Socket_get(socket), *Protocol_QUIC_Address_get(remote_address), RB_NUM2UINT(chosen_version)), _self(self) {}
	virtual ~RubyClient() {}
	
	void handshake_completed() override
	{
		rb_funcall(_self, rb_intern("handshake_completed"), 0);
	}
	
	Protocol::QUIC::Stream * create_stream(Protocol::QUIC::StreamID stream_id) override
	{
		VALUE stream = rb_funcall(_self, rb_intern("create_stream"), 1, RB_LL2NUM(stream_id));
		
		return Protocol_QUIC_Stream_get(stream);
	}
};

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
		.dmark = NULL,
		.dfree = Protocol_QUIC_Client_free,
		.dsize = Protocol_QUIC_Client_size,
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
