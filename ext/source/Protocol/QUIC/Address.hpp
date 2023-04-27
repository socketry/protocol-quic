//
//  Address.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/Address.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Protocol_QUIC_Address;

void Init_Protocol_QUIC_Address(VALUE Protocol_QUIC);

Protocol::QUIC::Address * Protocol_QUIC_Address_get(VALUE self);

VALUE Protocol_QUIC_Address_allocate(VALUE klass);
VALUE Protocol_QUIC_Address_wrap(VALUE klass, const Protocol::QUIC::Address & address);

#ifdef __cplusplus
}
#endif
