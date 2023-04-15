//
//  Socket.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 8/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <cstdint>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace Protocol
{
	namespace QUIC
	{
		union sockaddr_union {
			sockaddr_storage storage;
			sockaddr sa;
			sockaddr_in6 in6;
			sockaddr_in in;
		};
		
		struct Address {
			socklen_t length;
			union sockaddr_union data;
			std::uint32_t interface_index;
		};
		
		// msghdr_get_ecn gets ECN bits from |msg|.  |family| is the address
		// family from which packet is received.
		unsigned int msghdr_get_ecn(msghdr *msg, int family);

		// fd_set_ecn sets ECN bits |ecn| to |fd|.  |family| is the address
		// family of |fd|.
		void set_ecn(int fd, int family, unsigned int ecn);

		// fd_set_rec v_ecn sets socket option to |fd| so that it can receive
		// ECN bits.
		void set_recv_ecn(int fd, int family);

		// fd_set_ip_mtu_discover sets IP(V6)_MTU_DISCOVER socket option to fd.
		void set_ip_mtu_discover(int fd, int family);

		// fd_set_ip_dontfrag sets IP(V6)_DONTFRAG socket option to fd.
		void set_ip_dontfrag(int fd, int family);
		
		class Socket
		{
		public:
			Socket(int descriptor, const Address & address);
			Socket(int descriptor, const addrinfo * address);
			~Socket();
			
			int descriptor() const {return _descriptor;}
			const Address & address() const {return _address;}
			
			static Socket connect(const char * host, const char * service);
			
		private:
			int _descriptor = 0;
			Address _address;
		};
	}
}
