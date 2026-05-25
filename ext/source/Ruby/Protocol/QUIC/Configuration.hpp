//
//  Configuration.h
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

#include <Protocol/QUIC/Configuration.hpp>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Ruby_Protocol_QUIC_Configuration;

void Init_Ruby_Protocol_QUIC_Configuration(VALUE Protocol_QUIC);

::Protocol::QUIC::Configuration * Ruby_Protocol_QUIC_Configuration_get(VALUE self);

#ifdef __cplusplus
}
#endif
