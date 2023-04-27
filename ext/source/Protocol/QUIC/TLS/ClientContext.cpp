//
//  ClientContext.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "ClientContext.hpp"
#include "Context.hpp"

VALUE Protocol_QUIC_TLS_ClientContext = Qnil;

static void Protocol_QUIC_TLS_ClientContext_free(void *data) {
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::TLS::ClientContext *>(data);
	}
}

static size_t Protocol_QUIC_TLS_ClientContext_size(const void *data) {
	return sizeof(Protocol::QUIC::TLS::ClientContext);
}

static const rb_data_type_t Protocol_QUIC_TLS_ClientContext_type = {
	.wrap_struct_name = "Protocol::QUIC::TLS::ClientContext",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_TLS_ClientContext_free,
		.dsize = Protocol_QUIC_TLS_ClientContext_size,
	},
	.parent = &Protocol_QUIC_TLS_Context_type,
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::TLS::ClientContext * Protocol_QUIC_TLS_ClientContext_get(VALUE self)
{
	Protocol::QUIC::TLS::ClientContext *data;

	TypedData_Get_Struct(self, Protocol::QUIC::TLS::ClientContext, &Protocol_QUIC_TLS_ClientContext_type, data);

	return data;
}

static VALUE Protocol_QUIC_TLS_ClientContext_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_TLS_ClientContext_type, NULL);
}

static VALUE Protocol_QUIC_TLS_ClientContext_initialize(VALUE self) {
	DATA_PTR(self) = new Protocol::QUIC::TLS::ClientContext();

	return self;
}

void Init_Protocol_QUIC_TLS_ClientContext(VALUE Protocol_QUIC_TLS) {
	Protocol_QUIC_TLS_ClientContext =
			rb_define_class_under(Protocol_QUIC_TLS, "ClientContext", Protocol_QUIC_TLS_Context);

	rb_define_alloc_func(Protocol_QUIC_TLS_ClientContext, Protocol_QUIC_TLS_ClientContext_allocate);
	rb_define_method(Protocol_QUIC_TLS_ClientContext, "initialize", Protocol_QUIC_TLS_ClientContext_initialize, 0);
}
