
#include <ruby.h>
#include <ngtcp2/ngtcp2.h>

VALUE Protocol = Qnil;
VALUE Protocol_QUIC = Qnil;

#include "settings.h"

void Init_Protocol_QUIC(void)
{
	Protocol = rb_define_module("Protocol");
	Protocol_QUIC = rb_define_module_under(Protocol, "QUIC");
	
	Init_Protocol_QUIC_Settings();
}
