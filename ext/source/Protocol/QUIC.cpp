#include "QUIC.hpp"

#include "QUIC/Connection.hpp"
#include "QUIC/TLS/Context.hpp"

#include "QUIC/Address.hpp"
#include "QUIC/Binding.hpp"
#include "QUIC/Configuration.hpp"
#include "QUIC/PacketHeader.hpp"
#include "QUIC/Socket.hpp"

#include "QUIC/Server.hpp"
#include "QUIC/Client.hpp"
#include "QUIC/Stream.hpp"

VALUE Protocol_QUIC = Qnil;

void Init_Protocol_QUIC(void)
{
	VALUE Protocol = rb_define_module("Protocol");
	Protocol_QUIC = rb_define_module_under(Protocol, "QUIC");
	
	Init_Protocol_QUIC_TLS_Context(Protocol_QUIC);
	
	Init_Protocol_QUIC_Address(Protocol_QUIC);
	Init_Protocol_QUIC_Binding(Protocol_QUIC);
	Init_Protocol_QUIC_Configuration(Protocol_QUIC);
	Init_Protocol_QUIC_PacketHeader(Protocol_QUIC);
	Init_Protocol_QUIC_Socket(Protocol_QUIC);
	
	Init_Protocol_QUIC_Connection(Protocol_QUIC);
	Init_Protocol_QUIC_Server(Protocol_QUIC);
	Init_Protocol_QUIC_Client(Protocol_QUIC);
	
	Init_Protocol_QUIC_Stream(Protocol_QUIC);
}
