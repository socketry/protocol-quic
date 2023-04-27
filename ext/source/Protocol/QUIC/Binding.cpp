
#include "Binding.hpp"

#include "Socket.hpp"
#include "Address.hpp"
#include "PacketHeader.hpp"

#include "Configuration.hpp"
#include "TLS/ServerContext.hpp"

#include "Server.hpp"

#include "Reference.hpp"

#include "ruby/internal/core/rtypeddata.h"

class RubyBinding final : public Protocol::QUIC::Binding {
	VALUE _ruby_self;
	VALUE _ruby_configuration;
	VALUE _ruby_tls_context;
	
public:
	RubyBinding(VALUE self, VALUE configuration, VALUE tls_context) : Protocol::QUIC::Binding(*Protocol_QUIC_Configuration_get(configuration), *Protocol_QUIC_TLS_ServerContext_get(tls_context)), _ruby_self(self), _ruby_configuration(configuration), _ruby_tls_context(tls_context)
	{
	}
	
	~RubyBinding() {
	}
	
	Protocol::QUIC::Server * create_server(Protocol::QUIC::Socket &socket, const Protocol::QUIC::Address &address, const ngtcp2_pkt_hd &packet_header) override
	{
		VALUE ruby_socket = Protocol_QUIC_Socket_allocate(Protocol_QUIC_Socket);
		ValueReference ruby_socket_reference(ruby_socket, socket);
		
		VALUE ruby_address = Protocol_QUIC_Address_allocate(Protocol_QUIC_Address);
		ValueReference ruby_address_reference(ruby_address, address);
		
		VALUE ruby_packet_header = Protocol_QUIC_PacketHeader_allocate(Protocol_QUIC_PacketHeader);
		ValueReference ruby_packet_header_reference(ruby_packet_header, packet_header);
		
		VALUE server = rb_funcall(_ruby_self, rb_intern("create_server"), 3, ruby_socket, ruby_address, ruby_packet_header);
		
		return Protocol_QUIC_Server_get(server);
	}
	
	void mark() {
		rb_gc_mark(_ruby_self);
		rb_gc_mark(_ruby_configuration);
		rb_gc_mark(_ruby_tls_context);
	}
};

VALUE Protocol_QUIC_Binding = Qnil;

static void Protocol_QUIC_Binding_mark(void *data) {
	if (data) {
		reinterpret_cast<RubyBinding *>(data)->mark();
	}
}

static void Protocol_QUIC_Binding_free(void *data) {
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Binding *>(data);
	}
}

static size_t Protocol_QUIC_Binding_size(const void *data) {
	return sizeof(Protocol::QUIC::Binding);
}

static const rb_data_type_t Protocol_QUIC_Binding_type = {
	.wrap_struct_name = "Protocol::QUIC::Binding",
	.function = {
		.dmark = Protocol_QUIC_Binding_mark,
		.dfree = Protocol_QUIC_Binding_free,
		.dsize = Protocol_QUIC_Binding_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::Binding * Protocol_QUIC_Binding_get(VALUE self)
{
	Protocol::QUIC::Binding *binding;

	TypedData_Get_Struct(self, Protocol::QUIC::Binding, &Protocol_QUIC_Binding_type, binding);

	return binding;
}

static VALUE Protocol_QUIC_Binding_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Binding_type, NULL);
}

static VALUE Protocol_QUIC_Binding_initialize(VALUE self, VALUE configuration, VALUE tls_context) {
	// auto configuration = Protocol_QUIC_Configuration_get(configuration);
	// auto tls_context = Protocol_QUIC_TLS_Server_Context_get(tls_context);
	
	// DATA_PTR(self) = new Binding(self);

	return self;
}

void Init_Protocol_QUIC_Binding(VALUE Protocol_QUIC) {
	Protocol_QUIC_Binding =
			rb_define_class_under(Protocol_QUIC, "Binding", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_Binding, Protocol_QUIC_Binding_allocate);
	rb_define_method(Protocol_QUIC_Binding, "initialize", Protocol_QUIC_Binding_initialize, 2);
}
