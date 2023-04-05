//
//  Client.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 5/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Client.hpp"

namespace Protocol
{
	namespace QUIC
	{
		Client::Client(uint32_t chosen_version, uint32_t original_version) : _chosen_version(chosen_version), _original_version(original_version) 
		{
		}
		
		Client::~Client()
		{
		}
	}
}
