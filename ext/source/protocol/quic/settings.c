
#include <ruby.h>

#include "quic.h"

#include <stdarg.h>
#include <stdio.h>

#include <ngtcp2/ngtcp2.h>

VALUE Protocol_QUIC_Settings = Qnil;

void Protocol_QUIC_Settings_free(void *data) {
	free(data);
}

size_t Protocol_QUIC_Settings_size(const void *data) {
	return sizeof(struct ngtcp2_settings);
}

static const rb_data_type_t Protocol_QUIC_Settings_type = {
	.wrap_struct_name = "Protocol::QUIC::Settings",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_Settings_free,
		.dsize = Protocol_QUIC_Settings_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

VALUE Protocol_QUIC_Settings_allocate(VALUE klass) {
	ngtcp2_settings *settings = ALLOC(ngtcp2_settings);

	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Settings_type, settings);
}

static void ngtcp2_printf_stderr(void *user_data, const char *format, ...) {
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
}

VALUE Protocol_QUIC_Settings_initialize(VALUE self) {
	ngtcp2_settings *settings;

	TypedData_Get_Struct(self, ngtcp2_settings, &Protocol_QUIC_Settings_type, settings);

	ngtcp2_settings_default(settings);
	settings->log_printf = ngtcp2_printf_stderr;

	return self;
}

VALUE Protocol_QUIC_Settings_maximum_transmit_payload_size(VALUE self) {
	ngtcp2_settings *settings;
	
	TypedData_Get_Struct(self, ngtcp2_settings, &Protocol_QUIC_Settings_type, settings);
	
	return SIZET2NUM(settings->max_tx_udp_payload_size);
}

VALUE Protocol_QUIC_Settings_token(VALUE self) {
	ngtcp2_settings *settings;
	
	TypedData_Get_Struct(self, ngtcp2_settings, &Protocol_QUIC_Settings_type, settings);
	
	return rb_str_new((const char *)settings->token, settings->tokenlen);
}

VALUE Protocol_QUIC_Settings_maximum_window(VALUE self) {
	ngtcp2_settings *settings;
	
	TypedData_Get_Struct(self, ngtcp2_settings, &Protocol_QUIC_Settings_type, settings);
	
	return RB_ULL2NUM(settings->max_window);
}

VALUE Protocol_QUIC_Settings_maximum_stream_window(VALUE self) {
	ngtcp2_settings *settings;
	
	TypedData_Get_Struct(self, ngtcp2_settings, &Protocol_QUIC_Settings_type, settings);
	
	return RB_ULL2NUM(settings->max_stream_window);
}

void Init_Protocol_QUIC_Settings(void) {
	Protocol_QUIC_Settings =
			rb_define_class_under(Protocol_QUIC, "Settings", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_Settings, Protocol_QUIC_Settings_allocate);
	rb_define_method(Protocol_QUIC_Settings, "initialize", Protocol_QUIC_Settings_initialize, 0);
	
	rb_define_method(Protocol_QUIC_Settings, "maximum_transmit_payload_size", Protocol_QUIC_Settings_maximum_transmit_payload_size, 0);
	rb_define_method(Protocol_QUIC_Settings, "token", Protocol_QUIC_Settings_token, 0);
	rb_define_method(Protocol_QUIC_Settings, "maximum_window", Protocol_QUIC_Settings_maximum_window, 0);
	rb_define_method(Protocol_QUIC_Settings, "maximum_stream_window", Protocol_QUIC_Settings_maximum_stream_window, 0);
}
