
#include "Dispatcher.hpp"

#include "Socket.hpp"
#include "Address.hpp"
#include "PacketHeader.hpp"

#include "Configuration.hpp"
#include "TLS/ServerContext.hpp"

#include "Server.hpp"

#include "Reference.hpp"

#include <unordered_map>

class RubyDispatcher final : public Protocol::QUIC::Dispatcher {
public:
	VALUE self;
	
private:
	VALUE _ruby_configuration;
	VALUE _ruby_tls_context;
	std::unordered_map<Protocol::QUIC::Server *, VALUE> _ruby_servers;
	std::unordered_map<Protocol::QUIC::Socket *, VALUE> _ruby_sockets;
	
public:
	RubyDispatcher(VALUE self, VALUE configuration, VALUE tls_context) : Protocol::QUIC::Dispatcher(*Protocol_QUIC_Configuration_get(configuration), *Protocol_QUIC_TLS_ServerContext_get(tls_context)), self(self), _ruby_configuration(configuration), _ruby_tls_context(tls_context)
	{
	}
	
	~RubyDispatcher() {
	}
	
	VALUE ruby_configuration() noexcept {return _ruby_configuration;}
	VALUE ruby_tls_context() noexcept {return _ruby_tls_context;}
	
	Protocol::QUIC::Server * listen(VALUE ruby_socket)
	{
		auto socket = Protocol_QUIC_Socket_get(ruby_socket);
		
		_ruby_sockets[socket] = ruby_socket;
		
		return Protocol::QUIC::Dispatcher::listen(*socket);
	}
	
	Protocol::QUIC::Server * create_server(Protocol::QUIC::Socket &socket, const Protocol::QUIC::Address &address, const ngtcp2_pkt_hd &packet_header) override
	{
		auto iterator = _ruby_sockets.find(&socket);
		
		if (iterator == _ruby_sockets.end()) {
			rb_raise(rb_eRuntimeError, "Could not find Ruby socket wrapper for native socket.");
		}
		
		VALUE ruby_socket = iterator->second;
		VALUE ruby_address = Protocol_QUIC_Address_wrap(Protocol_QUIC_Address, address);
		
		VALUE ruby_packet_header = Protocol_QUIC_PacketHeader_allocate(Protocol_QUIC_PacketHeader);
		ValueReference ruby_packet_header_reference(ruby_packet_header, packet_header);
		
		VALUE server = rb_funcall(self, rb_intern("create_server"), 3, ruby_socket, ruby_address, ruby_packet_header);
		auto native_server = Protocol_QUIC_Server_get(server);
		
		_ruby_servers[native_server] = server;
		
		return native_server;
	}
	
	void remove(Protocol::QUIC::Server * server) override
	{
		Protocol::QUIC::Dispatcher::remove(server);
		_ruby_servers.erase(server);
	}
	
	void mark() {
		rb_gc_mark_movable(self);
		rb_gc_mark_movable(_ruby_configuration);
		rb_gc_mark_movable(_ruby_tls_context);
		
		for (auto & [server, ruby_server] : _ruby_servers) {
			rb_gc_mark_movable(ruby_server);
		}
		
		for (auto & [socket, ruby_socket] : _ruby_sockets) {
			rb_gc_mark_movable(ruby_socket);
		}
	}
	
	void compact() {
		self = rb_gc_location(self);
		_ruby_configuration = rb_gc_location(_ruby_configuration);
		_ruby_tls_context = rb_gc_location(_ruby_tls_context);
		
		for (auto & [server, ruby_server] : _ruby_servers) {
			ruby_server = rb_gc_location(ruby_server);
		}
		
		for (auto & [socket, ruby_socket] : _ruby_sockets) {
			ruby_socket = rb_gc_location(ruby_socket);
		}
	}
};

VALUE Protocol_QUIC_Dispatcher = Qnil;

static void Protocol_QUIC_Dispatcher_mark(void *data) {
	if (data) {
		reinterpret_cast<RubyDispatcher *>(data)->mark();
	}
}

static void Protocol_QUIC_Dispatcher_compact(void *data) {
	if (data) {
		reinterpret_cast<RubyDispatcher *>(data)->compact();
	}
}

static void Protocol_QUIC_Dispatcher_free(void *data) {
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Dispatcher *>(data);
	}
}

static size_t Protocol_QUIC_Dispatcher_size(const void *data) {
	return sizeof(RubyDispatcher);
}

static const rb_data_type_t Protocol_QUIC_Dispatcher_type = {
	.wrap_struct_name = "Protocol::QUIC::Dispatcher",
	.function = {
		.dmark = Protocol_QUIC_Dispatcher_mark,
		.dfree = Protocol_QUIC_Dispatcher_free,
		.dsize = Protocol_QUIC_Dispatcher_size,
		.dcompact = Protocol_QUIC_Dispatcher_compact,
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
	auto dispatcher = new RubyDispatcher(self, configuration, tls_context);
	DATA_PTR(self) = dispatcher;
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
	auto dispatcher = dynamic_cast<RubyDispatcher*>(Protocol_QUIC_Dispatcher_get(self));
	
	dispatcher->listen(socket);
	
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
