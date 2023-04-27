//
//  Client.h
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/Client.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Protocol_QUIC_Client;

void Init_Protocol_QUIC_Client(VALUE Protocol_QUIC);

Protocol::QUIC::Client * Protocol_QUIC_Client_get(VALUE self);

#ifdef __cplusplus
}
#endif
