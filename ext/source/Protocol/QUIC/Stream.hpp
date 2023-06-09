//
//  Stream.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/Stream.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Protocol_QUIC_Stream;

void Init_Protocol_QUIC_Stream(VALUE Protocol_QUIC);

Protocol::QUIC::Stream * Protocol_QUIC_Stream_get(VALUE self);

VALUE Protocol_QUIC_Stream_allocate(VALUE klass);

#ifdef __cplusplus
}
#endif
