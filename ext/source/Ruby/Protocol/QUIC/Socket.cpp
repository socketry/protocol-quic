//
//  Socket.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Socket.hpp"
#include "Address.hpp"

VALUE Ruby_Protocol_QUIC_Socket = Qnil;

static void Ruby_Protocol_QUIC_Socket_mark(void *data) {
	if (data) {
		auto socket = reinterpret_cast<::Protocol::QUIC::Socket *>(data);
		socket->monitor().mark();
	}
}

static void Ruby_Protocol_QUIC_Socket_compact(void *data) {
	if (data) {
		auto socket = reinterpret_cast<::Protocol::QUIC::Socket *>(data);
		socket->monitor().compact();
	}
}

static void Ruby_Protocol_QUIC_Socket_free(void *data) {
	if (data) {
		delete reinterpret_cast<::Protocol::QUIC::Socket *>(data);
	}
}

static size_t Ruby_Protocol_QUIC_Socket_size(const void *data) {
	return sizeof(::Protocol::QUIC::Socket);
}

const rb_data_type_t Ruby_Protocol_QUIC_Socket_type = {
	.wrap_struct_name = "Protocol::QUIC::Socket",
	.function = {
		.dmark = Ruby_Protocol_QUIC_Socket_mark,
		.dfree = Ruby_Protocol_QUIC_Socket_free,
		.dsize = Ruby_Protocol_QUIC_Socket_size,
		.dcompact = Ruby_Protocol_QUIC_Socket_compact,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

::Protocol::QUIC::Socket * Ruby_Protocol_QUIC_Socket_get(VALUE self)
{
	::Protocol::QUIC::Socket *socket;
	
	TypedData_Get_Struct(self, ::Protocol::QUIC::Socket, &Ruby_Protocol_QUIC_Socket_type, socket);
	
	return socket;
}

VALUE Ruby_Protocol_QUIC_Socket_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Ruby_Protocol_QUIC_Socket_type, NULL);
}

static VALUE Ruby_Protocol_QUIC_Socket_initialize(VALUE self, VALUE domain, VALUE type, VALUE protocol) {
	DATA_PTR(self) = new ::Protocol::QUIC::Socket(RB_NUM2INT(domain), RB_NUM2INT(type), RB_NUM2INT(protocol));
	
	return self;
}

static VALUE Ruby_Protocol_QUIC_Socket_local_address(VALUE self) {
	auto socket = Ruby_Protocol_QUIC_Socket_get(self);
	auto &address = socket->local_address();
	
	if (address) {
		return Ruby_Protocol_QUIC_Address_wrap(Ruby_Protocol_QUIC_Address, address);
	} else {
		return Qnil;
	}
}

static VALUE Ruby_Protocol_QUIC_Socket_remote_address(VALUE self) {
	auto socket = Ruby_Protocol_QUIC_Socket_get(self);
	auto &address = socket->remote_address();
	
	if (address) {
		return Ruby_Protocol_QUIC_Address_wrap(Ruby_Protocol_QUIC_Address, address);
	} else {
		return Qnil;
	}
}

static VALUE Ruby_Protocol_QUIC_Socket_bind(VALUE self, VALUE address) {
	auto socket = Ruby_Protocol_QUIC_Socket_get(self);
	
	return RTEST(
		socket->bind(*Ruby_Protocol_QUIC_Address_get(address))
	);
}

static VALUE Ruby_Protocol_QUIC_Socket_connect(VALUE self, VALUE address) {
	auto socket = Ruby_Protocol_QUIC_Socket_get(self);
	
	return RTEST(
		socket->connect(*Ruby_Protocol_QUIC_Address_get(address))
	);
}

void Init_Ruby_Protocol_QUIC_Socket(VALUE Protocol_QUIC) {
	Ruby_Protocol_QUIC_Socket = rb_define_class_under(Protocol_QUIC, "Socket", rb_cObject);
	
	rb_define_alloc_func(Ruby_Protocol_QUIC_Socket, Ruby_Protocol_QUIC_Socket_allocate);
	rb_define_method(Ruby_Protocol_QUIC_Socket, "initialize", Ruby_Protocol_QUIC_Socket_initialize, 3);
	
	rb_define_method(Ruby_Protocol_QUIC_Socket, "local_address", Ruby_Protocol_QUIC_Socket_local_address, 0);
	rb_define_method(Ruby_Protocol_QUIC_Socket, "remote_address", Ruby_Protocol_QUIC_Socket_remote_address, 0);
	
	rb_define_method(Ruby_Protocol_QUIC_Socket, "bind", Ruby_Protocol_QUIC_Socket_bind, 1);
	rb_define_method(Ruby_Protocol_QUIC_Socket, "connect", Ruby_Protocol_QUIC_Socket_connect, 1);
}
