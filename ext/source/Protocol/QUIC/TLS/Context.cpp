//
//  Context.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Context.hpp"

#include "ClientContext.hpp"
#include "ServerContext.hpp"

VALUE Protocol_QUIC_TLS_Context = Qnil;

static void Protocol_QUIC_TLS_Context_free(void *data) {
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::TLS::Context *>(data);
	}
}

static size_t Protocol_QUIC_TLS_Context_size(const void *data) {
	return sizeof(Protocol::QUIC::TLS::Context);
}

const rb_data_type_t Protocol_QUIC_TLS_Context_type = {
	.wrap_struct_name = "Protocol::QUIC::TLS::Context",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_TLS_Context_free,
		.dsize = Protocol_QUIC_TLS_Context_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::TLS::Context * Protocol_QUIC_TLS_Context_get(VALUE self)
{
	Protocol::QUIC::TLS::Context *tls_context;

	TypedData_Get_Struct(self, Protocol::QUIC::TLS::Context, &Protocol_QUIC_TLS_Context_type, tls_context);

	return tls_context;
}

static VALUE Protocol_QUIC_TLS_Context_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_TLS_Context_type, NULL);
}

static VALUE Protocol_QUIC_TLS_Context_initialize(VALUE self) {
	DATA_PTR(self) = new Protocol::QUIC::TLS::Context();

	return self;
}

static VALUE Protocol_QUIC_TLS_Context_add_protocol(VALUE self, VALUE name) {
	StringValue(name);
	
	Protocol::QUIC::TLS::Context *tls_context;
	TypedData_Get_Struct(self, Protocol::QUIC::TLS::Context, &Protocol_QUIC_TLS_Context_type, tls_context);
	
	tls_context->add_protocol(
		std::string(RSTRING_PTR(name), RSTRING_LEN(name))
	);
	
	return self;
}

static VALUE Protocol_QUIC_TLS_Context_protocols(VALUE self) {
	Protocol::QUIC::TLS::Context *tls_context;
	TypedData_Get_Struct(self, Protocol::QUIC::TLS::Context, &Protocol_QUIC_TLS_Context_type, tls_context);
	
	VALUE protocols = rb_ary_new();
	
	for (auto & protocol : tls_context->protocols()) {
		rb_ary_push(protocols, rb_str_new(protocol.data(), protocol.size()));
	}
	
	return protocols;
}

void Init_Protocol_QUIC_TLS_Context(VALUE Protocol_QUIC) {
	VALUE Protocol_QUIC_TLS = rb_define_module_under(Protocol_QUIC, "TLS");
	
	Protocol_QUIC_TLS_Context =
			rb_define_class_under(Protocol_QUIC_TLS, "Context", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_TLS_Context, Protocol_QUIC_TLS_Context_allocate);
	rb_define_method(Protocol_QUIC_TLS_Context, "initialize", Protocol_QUIC_TLS_Context_initialize, 0);
	
	rb_define_method(Protocol_QUIC_TLS_Context, "add_protocol", Protocol_QUIC_TLS_Context_add_protocol, 1);
	rb_define_method(Protocol_QUIC_TLS_Context, "protocols", Protocol_QUIC_TLS_Context_protocols, 0);
	
	Init_Protocol_QUIC_TLS_ClientContext(Protocol_QUIC_TLS);
	Init_Protocol_QUIC_TLS_ServerContext(Protocol_QUIC_TLS);
}
