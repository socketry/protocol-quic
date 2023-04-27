//
//  PacketHeader.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "PacketHeader.hpp"

VALUE Protocol_QUIC_PacketHeader = Qnil;

static void Protocol_QUIC_PacketHeader_free(void *data) {
	if (data) {
		delete reinterpret_cast<ngtcp2_pkt_hd *>(data);
	}
}

static size_t Protocol_QUIC_PacketHeader_size(const void *data) {
	return sizeof(ngtcp2_pkt_hd);
}

const rb_data_type_t Protocol_QUIC_PacketHeader_type = {
	.wrap_struct_name = "Protocol::QUIC::PacketHeader",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_PacketHeader_free,
		.dsize = Protocol_QUIC_PacketHeader_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

ngtcp2_pkt_hd * Protocol_QUIC_PacketHeader_get(VALUE self)
{
	ngtcp2_pkt_hd *address;
	
	TypedData_Get_Struct(self, ngtcp2_pkt_hd, &Protocol_QUIC_PacketHeader_type, address);
	
	return address;
}

VALUE Protocol_QUIC_PacketHeader_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_PacketHeader_type, NULL);
}

static VALUE Protocol_QUIC_PacketHeader_initialize(VALUE self) {
	DATA_PTR(self) = new ngtcp2_pkt_hd();
	
	return self;
}

void Init_Protocol_QUIC_PacketHeader(VALUE Protocol_QUIC) {
	Protocol_QUIC_PacketHeader =
			rb_define_class_under(Protocol_QUIC, "PacketHeader", rb_cObject);

	rb_define_alloc_func(Protocol_QUIC_PacketHeader, Protocol_QUIC_PacketHeader_allocate);
	rb_define_method(Protocol_QUIC_PacketHeader, "initialize", Protocol_QUIC_PacketHeader_initialize, 0);
}
