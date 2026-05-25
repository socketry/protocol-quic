
#include <ruby.h>

#include "Configuration.hpp"

#include <stdarg.h>
#include <stdio.h>

#include <ngtcp2/ngtcp2.h>

VALUE Ruby_Protocol_QUIC_Configuration = Qnil;

static void Ruby_Protocol_QUIC_Configuration_free(void *data)
{
	if (data) {
		delete reinterpret_cast<::Protocol::QUIC::Configuration *>(data);
	}
}

static size_t Ruby_Protocol_QUIC_Configuration_size(const void *data) {
	return sizeof(::Protocol::QUIC::Configuration);
}

static const rb_data_type_t Ruby_Protocol_QUIC_Configuration_type = {
	.wrap_struct_name = "Protocol::QUIC::Configuration",
	.function = {
		.dmark = NULL,
		.dfree = Ruby_Protocol_QUIC_Configuration_free,
		.dsize = Ruby_Protocol_QUIC_Configuration_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE Ruby_Protocol_QUIC_Configuration_allocate(VALUE klass) {
	auto configuration = new ::Protocol::QUIC::Configuration();

	return TypedData_Wrap_Struct(klass, &Ruby_Protocol_QUIC_Configuration_type, configuration);
}

::Protocol::QUIC::Configuration * Ruby_Protocol_QUIC_Configuration_get(VALUE self)
{
	::Protocol::QUIC::Configuration *configuration;

	TypedData_Get_Struct(self, ::Protocol::QUIC::Configuration, &Ruby_Protocol_QUIC_Configuration_type, configuration);

	return configuration;
}

static VALUE Ruby_Protocol_QUIC_Configuration_initialize(VALUE self) {
	::Protocol::QUIC::Configuration *configuration;

	TypedData_Get_Struct(self, ::Protocol::QUIC::Configuration, &Ruby_Protocol_QUIC_Configuration_type, configuration);

	return self;
}

static VALUE Ruby_Protocol_QUIC_Configuration_static_secret(VALUE self) {
	::Protocol::QUIC::Configuration *configuration;

	TypedData_Get_Struct(self, ::Protocol::QUIC::Configuration, &Ruby_Protocol_QUIC_Configuration_type, configuration);
	
	return rb_str_new((const char *)configuration->static_secret.data(), configuration->static_secret.size());
}

void Init_Ruby_Protocol_QUIC_Configuration(VALUE Protocol_QUIC) {
	Ruby_Protocol_QUIC_Configuration =
			rb_define_class_under(Protocol_QUIC, "Configuration", rb_cObject);

	rb_define_alloc_func(Ruby_Protocol_QUIC_Configuration, Ruby_Protocol_QUIC_Configuration_allocate);
	rb_define_method(Ruby_Protocol_QUIC_Configuration, "initialize", Ruby_Protocol_QUIC_Configuration_initialize, 0);
	
	rb_define_method(Ruby_Protocol_QUIC_Configuration, "static_secret", Ruby_Protocol_QUIC_Configuration_static_secret, 0);
}
