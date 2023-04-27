//
//  PacketHeader.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <ngtcp2/ngtcp2.h>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Protocol_QUIC_PacketHeader;

void Init_Protocol_QUIC_PacketHeader(VALUE Protocol_QUIC);

ngtcp2_pkt_hd * Protocol_QUIC_PacketHeader_get(VALUE self);

VALUE Protocol_QUIC_PacketHeader_allocate(VALUE klass);

#ifdef __cplusplus
}
#endif
