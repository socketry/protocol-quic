//
//  Socket.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Socket.hpp"
#include "Address.hpp"

VALUE Protocol_QUIC_Socket = Qnil;

static void Protocol_QUIC_Socket_free(void *data) {
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Socket *>(data);
	}
}

static size_t Protocol_QUIC_Socket_size(const void *data) {
	return sizeof(Protocol::QUIC::Socket);
}

const rb_data_type_t Protocol_QUIC_Socket_type = {
	.wrap_struct_name = "Protocol::QUIC::Socket",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_Socket_free,
		.dsize = Protocol_QUIC_Socket_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::Socket * Protocol_QUIC_Socket_get(VALUE self)
{
	Protocol::QUIC::Socket *socket;
	
	TypedData_Get_Struct(self, Protocol::QUIC::Socket, &Protocol_QUIC_Socket_type, socket);
	
	return socket;
}

VALUE Protocol_QUIC_Socket_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Socket_type, NULL);
}

static VALUE Protocol_QUIC_Socket_initialize(VALUE self, VALUE domain, VALUE type, VALUE protocol) {
	DATA_PTR(self) = new Protocol::QUIC::Socket(RB_NUM2INT(domain), RB_NUM2INT(type), RB_NUM2INT(protocol));
	
	return self;
}

static VALUE Protocol_QUIC_Socket_local_address(VALUE self) {
	auto socket = Protocol_QUIC_Socket_get(self);
	auto &address = socket->local_address();
	
	if (address) {
		return Protocol_QUIC_Address_wrap(Protocol_QUIC_Address, address);
	} else {
		return Qnil;
	}
}

static VALUE Protocol_QUIC_Socket_remote_address(VALUE self) {
	auto socket = Protocol_QUIC_Socket_get(self);
	auto &address = socket->remote_address();
	
	if (address) {
		return Protocol_QUIC_Address_wrap(Protocol_QUIC_Address, address);
	} else {
		return Qnil;
	}
}

void Init_Protocol_QUIC_Socket(VALUE Protocol_QUIC) {
	Protocol_QUIC_Socket =
			rb_define_class_under(Protocol_QUIC, "Socket", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_Socket, Protocol_QUIC_Socket_allocate);
	rb_define_method(Protocol_QUIC_Socket, "initialize", Protocol_QUIC_Socket_initialize, 3);
	
	rb_define_method(Protocol_QUIC_Socket, "local_address", Protocol_QUIC_Socket_local_address, 0);
	rb_define_method(Protocol_QUIC_Socket, "remote_address", Protocol_QUIC_Socket_remote_address, 0);
}
