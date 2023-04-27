
#include "Server.hpp"

#include "Binding.hpp"
#include "Configuration.hpp"
#include "Connection.hpp"
#include "TLS/ServerContext.hpp"
#include "Socket.hpp"
#include "Address.hpp"
#include "PacketHeader.hpp"

#include "Stream.hpp"

VALUE Protocol_QUIC_Server = Qnil;

class RubyServer : public Protocol::QUIC::Server {
	VALUE _self;
public:
	RubyServer(VALUE self, VALUE binding, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE packet_header, VALUE ocid) : Protocol::QUIC::Server(*Protocol_QUIC_Binding_get(binding), *Protocol_QUIC_Configuration_get(configuration), *Protocol_QUIC_TLS_ServerContext_get(tls_context), *Protocol_QUIC_Socket_get(socket), *Protocol_QUIC_Address_get(remote_address), *Protocol_QUIC_PacketHeader_get(packet_header), nullptr), _self(self) {}
	virtual ~RubyServer() {}
	
	Protocol::QUIC::Stream * create_stream(Protocol::QUIC::StreamID stream_id) override
	{
		VALUE stream = rb_funcall(_self, rb_intern("create_stream"), 1, RB_LL2NUM(stream_id));
		
		return Protocol_QUIC_Stream_get(stream);
	}
};

static void Protocol_QUIC_Server_free(void *data)
{
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Server *>(data);
	}
}

static size_t Protocol_QUIC_Server_size(const void *data) {
	return sizeof(Protocol::QUIC::Server);
}

static const rb_data_type_t Protocol_QUIC_Server_type = {
	.wrap_struct_name = "Protocol::QUIC::Server",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_Server_free,
		.dsize = Protocol_QUIC_Server_size,
	},
	.parent = &Protocol_QUIC_Connection_type,
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::Server * Protocol_QUIC_Server_get(VALUE self)
{
	Protocol::QUIC::Server *server;

	TypedData_Get_Struct(self, Protocol::QUIC::Server, &Protocol_QUIC_Server_type, server);

	return server;
}

static VALUE Protocol_QUIC_Server_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Server_type, NULL);
}

// Binding & binding, Configuration & configuration, TLS::ServerContext & tls_context, Socket & socket, const Address & remote_address, const ngtcp2_pkt_hd & packet_header, ngtcp2_cid *ocid = nullptr
static VALUE Protocol_QUIC_Server_initialize(VALUE self, VALUE binding, VALUE configuration, VALUE tls_context, VALUE socket, VALUE remote_address, VALUE packet_header, VALUE ocid) {
	Protocol::QUIC::Server *server = new RubyServer(self, binding, configuration, tls_context, socket, remote_address, packet_header, ocid);

	DATA_PTR(self) = server;

	return self;
}

void Init_Protocol_QUIC_Server(VALUE Protocol_QUIC) {
	Protocol_QUIC_Server =
			rb_define_class_under(Protocol_QUIC, "Server", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_Server, Protocol_QUIC_Server_allocate);
	rb_define_method(Protocol_QUIC_Server, "initialize", Protocol_QUIC_Server_initialize, 7);
}
