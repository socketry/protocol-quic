
#include <ruby.h>

#include "Connection.hpp"
#include "ruby/internal/intern/vm.h"

#include <stdarg.h>
#include <stdio.h>

#include <ngtcp2/ngtcp2.h>

VALUE Protocol_QUIC_Connection = Qnil;

static void Protocol_QUIC_Connection_free(void *data)
{
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Connection *>(data);
	}
}

static size_t Protocol_QUIC_Connection_size(const void *data) {
	return sizeof(Protocol::QUIC::Connection);
}

const rb_data_type_t Protocol_QUIC_Connection_type = {
	.wrap_struct_name = "Protocol::QUIC::Connection",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_Connection_free,
		.dsize = Protocol_QUIC_Connection_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::Connection * Protocol_QUIC_Connection_get(VALUE self)
{
	Protocol::QUIC::Connection *connection;

	TypedData_Get_Struct(self, Protocol::QUIC::Connection, &Protocol_QUIC_Connection_type, connection);

	return connection;
}

void Init_Protocol_QUIC_Connection(VALUE Protocol_QUIC) {
	Protocol_QUIC_Connection =
			rb_define_class_under(Protocol_QUIC, "Connection", rb_cObject);

	rb_undef_alloc_func(Protocol_QUIC_Connection);
}
