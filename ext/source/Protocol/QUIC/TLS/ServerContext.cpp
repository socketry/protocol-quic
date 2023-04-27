//
//  ServerContext.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "ServerContext.hpp"
#include "Context.hpp"

VALUE Protocol_QUIC_TLS_ServerContext = Qnil;

static void Protocol_QUIC_TLS_ServerContext_free(void *data) {
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::TLS::ServerContext *>(data);
	}
}

static size_t Protocol_QUIC_TLS_ServerContext_size(const void *data) {
	return sizeof(Protocol::QUIC::TLS::ServerContext);
}

static const rb_data_type_t Protocol_QUIC_TLS_ServerContext_type = {
	.wrap_struct_name = "Protocol::QUIC::TLS::ServerContext",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_TLS_ServerContext_free,
		.dsize = Protocol_QUIC_TLS_ServerContext_size,
	},
	.parent = &Protocol_QUIC_TLS_Context_type,
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::TLS::ServerContext * Protocol_QUIC_TLS_ServerContext_get(VALUE self)
{
	Protocol::QUIC::TLS::ServerContext *data;

	TypedData_Get_Struct(self, Protocol::QUIC::TLS::ServerContext, &Protocol_QUIC_TLS_ServerContext_type, data);

	return data;
}

static VALUE Protocol_QUIC_TLS_ServerContext_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_TLS_ServerContext_type, NULL);
}

static VALUE Protocol_QUIC_TLS_ServerContext_initialize(VALUE self) {
	DATA_PTR(self) = new Protocol::QUIC::TLS::ServerContext();

	return self;
}

void Init_Protocol_QUIC_TLS_ServerContext(VALUE Protocol_QUIC_TLS) {
	Protocol_QUIC_TLS_ServerContext =
			rb_define_class_under(Protocol_QUIC_TLS, "ServerContext", Protocol_QUIC_TLS_Context);

	rb_define_alloc_func(Protocol_QUIC_TLS_ServerContext, Protocol_QUIC_TLS_ServerContext_allocate);
	rb_define_method(Protocol_QUIC_TLS_ServerContext, "initialize", Protocol_QUIC_TLS_ServerContext_initialize, 0);
}
