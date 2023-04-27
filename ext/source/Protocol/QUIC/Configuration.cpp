
#include <ruby.h>

#include "Configuration.hpp"

#include <stdarg.h>
#include <stdio.h>

#include <ngtcp2/ngtcp2.h>

VALUE Protocol_QUIC_Configuration = Qnil;

static void Protocol_QUIC_Configuration_free(void *data)
{
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Configuration *>(data);
	}
}

static size_t Protocol_QUIC_Configuration_size(const void *data) {
	return sizeof(Protocol::QUIC::Configuration);
}

static const rb_data_type_t Protocol_QUIC_Configuration_type = {
	.wrap_struct_name = "Protocol::QUIC::Configuration",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_Configuration_free,
		.dsize = Protocol_QUIC_Configuration_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

static VALUE Protocol_QUIC_Configuration_allocate(VALUE klass) {
	auto configuration = new Protocol::QUIC::Configuration();

	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Configuration_type, configuration);
}

Protocol::QUIC::Configuration * Protocol_QUIC_Configuration_get(VALUE self)
{
	Protocol::QUIC::Configuration *configuration;

	TypedData_Get_Struct(self, Protocol::QUIC::Configuration, &Protocol_QUIC_Configuration_type, configuration);

	return configuration;
}

static VALUE Protocol_QUIC_Configuration_initialize(VALUE self) {
	Protocol::QUIC::Configuration *configuration;

	TypedData_Get_Struct(self, Protocol::QUIC::Configuration, &Protocol_QUIC_Configuration_type, configuration);

	return self;
}

static VALUE Protocol_QUIC_Configuration_static_secret(VALUE self) {
	Protocol::QUIC::Configuration *configuration;

	TypedData_Get_Struct(self, Protocol::QUIC::Configuration, &Protocol_QUIC_Configuration_type, configuration);
	
	return rb_str_new((const char *)configuration->static_secret.data(), configuration->static_secret.size());
}

void Init_Protocol_QUIC_Configuration(VALUE Protocol_QUIC) {
	Protocol_QUIC_Configuration =
			rb_define_class_under(Protocol_QUIC, "Configuration", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_Configuration, Protocol_QUIC_Configuration_allocate);
	rb_define_method(Protocol_QUIC_Configuration, "initialize", Protocol_QUIC_Configuration_initialize, 0);
	
	rb_define_method(Protocol_QUIC_Configuration, "static_secret", Protocol_QUIC_Configuration_static_secret, 0);
}
