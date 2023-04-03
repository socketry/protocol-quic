#!/usr/bin/env ruby

require "mkmf"

$srcs = [
	File.expand_path("protocol/quic.c", __dir__),
	File.expand_path("protocol/quic/settings.c", __dir__),
]

$VPATH << "$(srcdir)/protocol/quic"
$LDFLAGS += " -lngtcp2"

create_makefile("Protocol_QUIC")
