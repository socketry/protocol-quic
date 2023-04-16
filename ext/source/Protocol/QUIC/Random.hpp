//
//  Random.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 16/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <random>
#include <array>

namespace Protocol
{
	namespace QUIC
	{
		class Random
		{
		public:
			static void generate_secure(std::uint8_t *buffer, std::size_t size);
			static void generate_secret(std::array<std::uint8_t, 32> &buffer);
			
			Random();
			~Random();
			
			void generate(std::uint8_t *buffer, std::size_t size);
			
		private:
			std::mt19937 _generator;
		};
	}
}
