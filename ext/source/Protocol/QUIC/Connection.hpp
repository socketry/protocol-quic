//
//  Connection.h
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/Connection.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Protocol_QUIC_Connection;

extern const rb_data_type_t Protocol_QUIC_Connection_type;

void Init_Protocol_QUIC_Connection(VALUE Protocol_QUIC);

Protocol::QUIC::Connection * Protocol_QUIC_Connection_get(VALUE self);

#ifdef __cplusplus
}
#endif
