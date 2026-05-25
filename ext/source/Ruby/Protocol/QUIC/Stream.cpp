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

VALUE Ruby_Protocol_QUIC_Stream = Qnil;

namespace Ruby::Protocol::QUIC {

	class Stream : public ::Protocol::QUIC::BufferedStream {
	public:
		VALUE self;
	
	private:
		VALUE _connection;
	public:
		Stream(VALUE self, VALUE connection, VALUE stream_id) : ::Protocol::QUIC::BufferedStream(*Ruby_Protocol_QUIC_Connection_get(connection), RB_NUM2LL(stream_id)), self(self), _connection(connection) {}
	
		virtual ~Stream() {}
	
		void mark() {
			rb_gc_mark_movable(self);
			rb_gc_mark_movable(_connection);
		}
	
		void compact() {
			self = rb_gc_location(self);
			_connection = rb_gc_location(_connection);
		}
	};

}

static void Ruby_Protocol_QUIC_Stream_mark(void *data) {
	if (data) {
		reinterpret_cast<Ruby::Protocol::QUIC::Stream *>(data)->mark();
	}
}

static void Ruby_Protocol_QUIC_Stream_compact(void *data) {
	if (data) {
		reinterpret_cast<Ruby::Protocol::QUIC::Stream *>(data)->compact();
	}
}

static void Ruby_Protocol_QUIC_Stream_free(void *data) {
	if (data) {
		delete reinterpret_cast<Ruby::Protocol::QUIC::Stream *>(data);
	}
}

static size_t Ruby_Protocol_QUIC_Stream_size(const void *data) {
	return sizeof(Ruby::Protocol::QUIC::Stream);
}

const rb_data_type_t Ruby_Protocol_QUIC_Stream_type = {
	.wrap_struct_name = "Protocol::QUIC::Stream",
	.function = {
		.dmark = Ruby_Protocol_QUIC_Stream_mark,
		.dfree = Ruby_Protocol_QUIC_Stream_free,
		.dsize = Ruby_Protocol_QUIC_Stream_size,
		.dcompact = Ruby_Protocol_QUIC_Stream_compact,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

::Protocol::QUIC::Stream * Ruby_Protocol_QUIC_Stream_get(VALUE self)
{
	::Protocol::QUIC::Stream *address;
	
	TypedData_Get_Struct(self, ::Protocol::QUIC::Stream, &Ruby_Protocol_QUIC_Stream_type, address);
	
	return address;
}

VALUE Ruby_Protocol_QUIC_Stream_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Ruby_Protocol_QUIC_Stream_type, NULL);
}

static VALUE Ruby_Protocol_QUIC_Stream_initialize(VALUE self, VALUE connection, VALUE stream_id) {
	DATA_PTR(self) = new Ruby::Protocol::QUIC::Stream(self, connection, stream_id);
	
	return self;
}

void Init_Ruby_Protocol_QUIC_Stream(VALUE Protocol_QUIC) {
	Ruby_Protocol_QUIC_Stream =
			rb_define_class_under(Protocol_QUIC, "Stream", rb_cObject);

	rb_define_alloc_func(Ruby_Protocol_QUIC_Stream, Ruby_Protocol_QUIC_Stream_allocate);
	rb_define_method(Ruby_Protocol_QUIC_Stream, "initialize", Ruby_Protocol_QUIC_Stream_initialize, 2);
}
