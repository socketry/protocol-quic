//
//  ServerContext.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "ServerContext.hpp"
#include "Context.hpp"

VALUE Ruby_Protocol_QUIC_TLS_ServerContext = Qnil;

static void Ruby_Protocol_QUIC_TLS_ServerContext_free(void *data) {
	if (data) {
		delete reinterpret_cast<::Protocol::QUIC::TLS::ServerContext *>(data);
	}
}

static size_t Ruby_Protocol_QUIC_TLS_ServerContext_size(const void *data) {
	return sizeof(::Protocol::QUIC::TLS::ServerContext);
}

static const rb_data_type_t Ruby_Protocol_QUIC_TLS_ServerContext_type = {
	.wrap_struct_name = "Protocol::QUIC::TLS::ServerContext",
	.function = {
		.dmark = NULL,
		.dfree = Ruby_Protocol_QUIC_TLS_ServerContext_free,
		.dsize = Ruby_Protocol_QUIC_TLS_ServerContext_size,
	},
	.parent = &Ruby_Protocol_QUIC_TLS_Context_type,
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

::Protocol::QUIC::TLS::ServerContext * Ruby_Protocol_QUIC_TLS_ServerContext_get(VALUE self)
{
	::Protocol::QUIC::TLS::ServerContext *data;

	TypedData_Get_Struct(self, ::Protocol::QUIC::TLS::ServerContext, &Ruby_Protocol_QUIC_TLS_ServerContext_type, data);

	return data;
}

static VALUE Ruby_Protocol_QUIC_TLS_ServerContext_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Ruby_Protocol_QUIC_TLS_ServerContext_type, NULL);
}

static VALUE Ruby_Protocol_QUIC_TLS_ServerContext_initialize(VALUE self) {
	DATA_PTR(self) = new ::Protocol::QUIC::TLS::ServerContext();

	return self;
}

void Init_Ruby_Protocol_QUIC_TLS_ServerContext(VALUE Protocol_QUIC_TLS) {
	Ruby_Protocol_QUIC_TLS_ServerContext =
			rb_define_class_under(Protocol_QUIC_TLS, "ServerContext", Ruby_Protocol_QUIC_TLS_Context);

	rb_define_alloc_func(Ruby_Protocol_QUIC_TLS_ServerContext, Ruby_Protocol_QUIC_TLS_ServerContext_allocate);
	rb_define_method(Ruby_Protocol_QUIC_TLS_ServerContext, "initialize", Ruby_Protocol_QUIC_TLS_ServerContext_initialize, 0);
}
