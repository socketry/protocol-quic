
#include "Dispatcher.hpp"

#include "Socket.hpp"
#include "Address.hpp"
#include "PacketHeader.hpp"

#include "Configuration.hpp"
#include "TLS/ServerContext.hpp"

#include "Server.hpp"

#include "Reference.hpp"

class RubyDispatcher final : public Protocol::QUIC::Dispatcher {
	VALUE _ruby_self;
	VALUE _ruby_configuration;
	VALUE _ruby_tls_context;
	
public:
	RubyDispatcher(VALUE self, VALUE configuration, VALUE tls_context) : Protocol::QUIC::Dispatcher(*Protocol_QUIC_Configuration_get(configuration), *Protocol_QUIC_TLS_ServerContext_get(tls_context)), _ruby_self(self), _ruby_configuration(configuration), _ruby_tls_context(tls_context)
	{
	}
	
	~RubyDispatcher() {
	}
	
	VALUE ruby_self() noexcept {return _ruby_self;}
	VALUE ruby_configuration() noexcept {return _ruby_configuration;}
	VALUE ruby_tls_context() noexcept {return _ruby_tls_context;}
	
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

VALUE Protocol_QUIC_Dispatcher = Qnil;

static void Protocol_QUIC_Dispatcher_mark(void *data) {
	if (data) {
		reinterpret_cast<RubyDispatcher *>(data)->mark();
	}
}

static void Protocol_QUIC_Dispatcher_free(void *data) {
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Dispatcher *>(data);
	}
}

static size_t Protocol_QUIC_Dispatcher_size(const void *data) {
	return sizeof(Protocol::QUIC::Dispatcher);
}

static const rb_data_type_t Protocol_QUIC_Dispatcher_type = {
	.wrap_struct_name = "Protocol::QUIC::Dispatcher",
	.function = {
		.dmark = Protocol_QUIC_Dispatcher_mark,
		.dfree = Protocol_QUIC_Dispatcher_free,
		.dsize = Protocol_QUIC_Dispatcher_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::Dispatcher * Protocol_QUIC_Dispatcher_get(VALUE self)
{
	Protocol::QUIC::Dispatcher *dispatcher;

	TypedData_Get_Struct(self, Protocol::QUIC::Dispatcher, &Protocol_QUIC_Dispatcher_type, dispatcher);

	return dispatcher;
}

static VALUE Protocol_QUIC_Dispatcher_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Dispatcher_type, NULL);
}

static VALUE Protocol_QUIC_Dispatcher_initialize(VALUE self, VALUE configuration, VALUE tls_context) {
	DATA_PTR(self) = new RubyDispatcher(self, configuration, tls_context);
	return self;
}

static VALUE Protocol_QUIC_Dispatcher_configuration(VALUE self) {
	auto dispatcher = dynamic_cast<RubyDispatcher*>(Protocol_QUIC_Dispatcher_get(self));
	
	return dispatcher->ruby_configuration();
}

static VALUE Protocol_QUIC_Dispatcher_tls_context(VALUE self) {
	auto dispatcher = dynamic_cast<RubyDispatcher*>(Protocol_QUIC_Dispatcher_get(self));
	
	return dispatcher->ruby_tls_context();
}

static VALUE Protocol_QUIC_Dispatcher_listen(VALUE self, VALUE socket) {
	auto dispatcher = Protocol_QUIC_Dispatcher_get(self);
	
	dispatcher->listen(*Protocol_QUIC_Socket_get(socket));
	
	return Qnil;
}

void Init_Protocol_QUIC_Dispatcher(VALUE Protocol_QUIC) {
	Protocol_QUIC_Dispatcher =
			rb_define_class_under(Protocol_QUIC, "Dispatcher", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_Dispatcher, Protocol_QUIC_Dispatcher_allocate);
	rb_define_method(Protocol_QUIC_Dispatcher, "initialize", Protocol_QUIC_Dispatcher_initialize, 2);
	
	rb_define_method(Protocol_QUIC_Dispatcher, "configuration", Protocol_QUIC_Dispatcher_configuration, 0);
	rb_define_method(Protocol_QUIC_Dispatcher, "tls_context", Protocol_QUIC_Dispatcher_tls_context, 0);
	
	rb_define_method(Protocol_QUIC_Dispatcher, "listen", Protocol_QUIC_Dispatcher_listen, 1);
}
