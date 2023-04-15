//
//  Socket.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 8/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Socket.hpp"

#include <cstring>
#include <stdexcept>

#include <unistd.h>

namespace Protocol
{
	namespace QUIC
	{
		Socket::Socket(int descriptor, const Address & address) : _descriptor(descriptor), _address(address)
		{
		}
		
		Socket::Socket(int descriptor, const addrinfo * address) : _descriptor(descriptor)
		{
			if (address) {
				_address.length = address->ai_addrlen;
				std::memcpy(&_address.data, address->ai_addr, address->ai_addrlen);
			} else {
				_address.length = 0;
			}
		}
		
		Socket::~Socket()
		{
			if (_descriptor >= 0) {
				::close(_descriptor);
			}
		}
		
		template <typename Callback>
		struct Defer {
			Callback _callback;
			
			Defer(Callback callback) : _callback(callback) {}
			~Defer() {_callback();}
		};
		
		template <typename Callback>
		Defer<Callback> defer(Callback callback) {
			return Defer<Callback>(callback);
		}
		
		unsigned int get_ecn(msghdr *msg, int family) {
			switch (family) {
			case AF_INET:
				for (auto cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
					if (cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == IP_TOS && cmsg->cmsg_len) {
						return *reinterpret_cast<uint8_t *>(CMSG_DATA(cmsg));
					}
				}
				break;
			case AF_INET6:
				for (auto cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
					if (cmsg->cmsg_level == IPPROTO_IPV6 && cmsg->cmsg_type == IPV6_TCLASS && cmsg->cmsg_len) {
						return *reinterpret_cast<uint8_t *>(CMSG_DATA(cmsg));
					}
				}
				break;
			}
			
			return 0;
		}
		
		void set_ecn(int descriptor, int family, unsigned int ecn) {
			switch (family) {
			case AF_INET:
				if (setsockopt(descriptor, IPPROTO_IP, IP_TOS, &ecn, static_cast<socklen_t>(sizeof(ecn))) == -1) {
					throw std::runtime_error("set_ecn:setsockopt(IP_TOS): " + std::string(strerror(errno)));
				}
				break;
			case AF_INET6:
				if (setsockopt(descriptor, IPPROTO_IPV6, IPV6_TCLASS, &ecn, static_cast<socklen_t>(sizeof(ecn))) == -1) {
					throw std::runtime_error("set_ecn:setsockopt(IPV6_TCLASS): " + std::string(strerror(errno)));
				}
				break;
			}
		}
		
		void set_recv_ecn(int descriptor, int family) {
			unsigned int tos = 1;
			switch (family) {
			case AF_INET:
				if (setsockopt(descriptor, IPPROTO_IP, IP_RECVTOS, &tos, static_cast<socklen_t>(sizeof(tos))) == -1) {
					throw std::runtime_error("set_recv_ecn:setsockopt(IP_RECVTOS)" + std::string(strerror(errno)));
				}
				break;
			case AF_INET6:
				if (setsockopt(descriptor, IPPROTO_IPV6, IPV6_RECVTCLASS, &tos, static_cast<socklen_t>(sizeof(tos))) == -1) {
					throw std::runtime_error("set_recv_ecn:setsockopt(IPV6_RECVTCLASS)" + std::string(strerror(errno)));
				}
				break;
			}
		}

		void set_ip_mtu_discover(int descriptor, int family) {
#if defined(IP_MTU_DISCOVER) && defined(IPV6_MTU_DISCOVER)
			int value;
			
			switch (family) {
			case AF_INET:
				value = IP_PMTUDISC_DO;
				if (setsockopt(descriptor, IPPROTO_IP, IP_MTU_DISCOVER, &value, static_cast<socklen_t>(sizeof(value))) == -1) {
					throw std::runtime_error("setsockopt: IP_MTU_DISCOVER" + std::string(strerror(errno)));
				}
				break;
			case AF_INET6:
				value = IPV6_PMTUDISC_DO;
				if (setsockopt(descriptor, IPPROTO_IPV6, IPV6_MTU_DISCOVER, &value, static_cast<socklen_t>(sizeof(value))) == -1) {
					throw std::runtime_error("setsockopt: IPV6_MTU_DISCOVER" + std::string(strerror(errno)));
				}
				break;
			}
#endif
		}

		void set_ip_dontfrag(int descriptor, int family) {
#if defined(IP_DONTFRAG) && defined(IPV6_DONTFRAG)
			int value = 1;
			
			switch (family) {
			case AF_INET:
				if (setsockopt(descriptor, IPPROTO_IP, IP_DONTFRAG, &val, static_cast<socklen_t>(sizeof(value))) == -1) {
					throw std::runtime_error("set_ip_dontfrag:setsockopt(IP_DONTFRAG): " + std::string(strerror(errno)));
				}
				break;
			case AF_INET6:
				if (setsockopt(descriptor, IPPROTO_IPV6, IPV6_DONTFRAG, &val, static_cast<socklen_t>(sizeof(value))) == -1) {
					throw std::runtime_error("set_ip_dontfrag:setsockopt(IPV6_DONTFRAG): " + std::string(strerror(errno)));
				}
				break;
			}
#endif
		}
		
		int create_udp_socket(int domain) {
			auto descriptor = socket(domain, SOCK_DGRAM | SOCK_NONBLOCK, 0);
			
			if (descriptor == -1) {
				throw std::runtime_error("socket: " + std::string(strerror(errno)));
			}
			
			set_recv_ecn(descriptor, domain);
			set_ip_mtu_discover(descriptor, domain);
			set_ip_dontfrag(descriptor, domain);
			
			return descriptor;
		}
		
		Socket Socket::connect(const char *host, const char *service) {
			addrinfo hints{.ai_family = AF_UNSPEC, .ai_socktype = SOCK_DGRAM};
			addrinfo *results;
			
			if (auto error = getaddrinfo(host, service, &hints, &results); error != 0) {
				throw std::runtime_error("getaddrinfo: " + std::string(gai_strerror(error)));
			}
			
			auto free_results = defer([&]{
				freeaddrinfo(results);
			});
			
			int descriptor = -1;
			
			addrinfo *iterator = results;
			
			while (iterator) {
				descriptor = create_udp_socket(iterator->ai_family);
				if (descriptor != -1) {
					break;
				}
				
				iterator = iterator->ai_next;
			}
			
			if (!iterator) {
				throw std::runtime_error("Could not create socket");
			}
			
			return Socket(descriptor, iterator);
		}
	}
}
