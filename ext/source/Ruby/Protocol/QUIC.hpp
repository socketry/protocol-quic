#pragma once

#include <ruby.h>

#ifdef __cplusplus
extern "C" {
#endif

extern VALUE Protocol_QUIC;

void Init_Ruby_Protocol_QUIC(void);

#ifdef __cplusplus
}
#endif
