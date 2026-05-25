//
//  Socket.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/Socket.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Ruby_Protocol_QUIC_Socket;

void Init_Ruby_Protocol_QUIC_Socket(VALUE Protocol_QUIC);

::Protocol::QUIC::Socket * Ruby_Protocol_QUIC_Socket_get(VALUE self);

VALUE Ruby_Protocol_QUIC_Socket_allocate(VALUE klass);

#ifdef __cplusplus
}
#endif
