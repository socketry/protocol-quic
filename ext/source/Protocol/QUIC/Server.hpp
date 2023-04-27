//
//  Server.h
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/Server.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Protocol_QUIC_Server;

void Init_Protocol_QUIC_Server(VALUE Protocol_QUIC);

Protocol::QUIC::Server * Protocol_QUIC_Server_get(VALUE self);

#ifdef __cplusplus
}
#endif
