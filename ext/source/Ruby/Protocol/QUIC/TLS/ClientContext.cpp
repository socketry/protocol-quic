//
//  ClientContext.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "ClientContext.hpp"
#include "Context.hpp"

VALUE Ruby_Protocol_QUIC_TLS_ClientContext = Qnil;

static void Ruby_Protocol_QUIC_TLS_ClientContext_free(void *data) {
	if (data) {
		delete reinterpret_cast<::Protocol::QUIC::TLS::ClientContext *>(data);
	}
}

static size_t Ruby_Protocol_QUIC_TLS_ClientContext_size(const void *data) {
	return sizeof(::Protocol::QUIC::TLS::ClientContext);
}

static const rb_data_type_t Ruby_Protocol_QUIC_TLS_ClientContext_type = {
	.wrap_struct_name = "Protocol::QUIC::TLS::ClientContext",
	.function = {
		.dmark = NULL,
		.dfree = Ruby_Protocol_QUIC_TLS_ClientContext_free,
		.dsize = Ruby_Protocol_QUIC_TLS_ClientContext_size,
	},
	.parent = &Ruby_Protocol_QUIC_TLS_Context_type,
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

::Protocol::QUIC::TLS::ClientContext * Ruby_Protocol_QUIC_TLS_ClientContext_get(VALUE self)
{
	::Protocol::QUIC::TLS::ClientContext *data;

	TypedData_Get_Struct(self, ::Protocol::QUIC::TLS::ClientContext, &Ruby_Protocol_QUIC_TLS_ClientContext_type, data);

	return data;
}

static VALUE Ruby_Protocol_QUIC_TLS_ClientContext_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Ruby_Protocol_QUIC_TLS_ClientContext_type, NULL);
}

static VALUE Ruby_Protocol_QUIC_TLS_ClientContext_initialize(VALUE self) {
	DATA_PTR(self) = new ::Protocol::QUIC::TLS::ClientContext();

	return self;
}

void Init_Ruby_Protocol_QUIC_TLS_ClientContext(VALUE Protocol_QUIC_TLS) {
	Ruby_Protocol_QUIC_TLS_ClientContext =
			rb_define_class_under(Protocol_QUIC_TLS, "ClientContext", Ruby_Protocol_QUIC_TLS_Context);

	rb_define_alloc_func(Ruby_Protocol_QUIC_TLS_ClientContext, Ruby_Protocol_QUIC_TLS_ClientContext_allocate);
	rb_define_method(Ruby_Protocol_QUIC_TLS_ClientContext, "initialize", Ruby_Protocol_QUIC_TLS_ClientContext_initialize, 0);
}
