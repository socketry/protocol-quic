//
//  Stream.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Stream.hpp"
#include "Connection.hpp"
#include <Protocol/QUIC/BufferedStream.hpp>

VALUE Protocol_QUIC_Stream = Qnil;

class RubyStream : public Protocol::QUIC::BufferedStream {
	VALUE _self;
public:
	RubyStream(VALUE self, VALUE connection, VALUE stream_id) : Protocol::QUIC::BufferedStream(*Protocol_QUIC_Connection_get(connection), RB_NUM2LL(stream_id)), _self(self) {}
	virtual ~RubyStream() {}
};

static void Protocol_QUIC_Stream_free(void *data) {
	if (data) {
		delete reinterpret_cast<RubyStream *>(data);
	}
}

static size_t Protocol_QUIC_Stream_size(const void *data) {
	return sizeof(RubyStream);
}

const rb_data_type_t Protocol_QUIC_Stream_type = {
	.wrap_struct_name = "Protocol::QUIC::Stream",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_Stream_free,
		.dsize = Protocol_QUIC_Stream_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::Stream * Protocol_QUIC_Stream_get(VALUE self)
{
	Protocol::QUIC::Stream *address;
	
	TypedData_Get_Struct(self, Protocol::QUIC::Stream, &Protocol_QUIC_Stream_type, address);
	
	return address;
}

VALUE Protocol_QUIC_Stream_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Stream_type, NULL);
}

static VALUE Protocol_QUIC_Stream_initialize(VALUE self, VALUE connection, VALUE stream_id) {
	DATA_PTR(self) = new RubyStream(self, connection, stream_id);
	
	return self;
}

void Init_Protocol_QUIC_Stream(VALUE Protocol_QUIC) {
	Protocol_QUIC_Stream =
			rb_define_class_under(Protocol_QUIC, "Stream", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_Stream, Protocol_QUIC_Stream_allocate);
	rb_define_method(Protocol_QUIC_Stream, "initialize", Protocol_QUIC_Stream_initialize, 2);
}
