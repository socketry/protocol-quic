//
//  ClientContext.h
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/TLS/ClientContext.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Protocol_QUIC_TLS_ClientContext;

void Init_Protocol_QUIC_TLS_ClientContext(VALUE Protocol_QUIC_TLS);

Protocol::QUIC::TLS::ClientContext * Protocol_QUIC_TLS_ClientContext_get(VALUE self);

#ifdef __cplusplus
}
#endif
