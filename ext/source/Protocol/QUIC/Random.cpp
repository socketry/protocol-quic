//
//  Random.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 16/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Random.hpp"
#include "Defer.hpp"

#include <algorithm>
#include <stdexcept>

#include <openssl/rand.h>

namespace Protocol
{
	namespace QUIC
	{
		void Random::generate_secure(std::uint8_t *buffer, std::size_t size)
		{
			if (RAND_bytes(buffer, size) != 1)
				throw std::runtime_error("RAND_bytes failed!");
		}
		
		void Random::generate_secret(std::array<uint8_t, 32> & buffer)
		{
			std::array<uint8_t, 16> random_seed;
			std::array<uint8_t, 32> digest;
			
			generate_secure(random_seed.data(), random_seed.size());
			
			auto context = EVP_MD_CTX_new();
			if (context == nullptr) {
				throw std::runtime_error("EVP_MD_CTX_new failed!");
			}
			
			auto free_context = defer([&](){EVP_MD_CTX_free(context);});

			if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1) {
				throw std::runtime_error("EVP_DigestInit_ex failed!");
			}
			
			if (EVP_DigestUpdate(context, random_seed.data(), random_seed.size()) != 1) {
				throw std::runtime_error("EVP_DigestUpdate failed!");
			}
			
			unsigned int digest_length = digest.size();
			if (EVP_DigestFinal_ex(context, digest.data(), &digest_length) != 1) {
				throw std::runtime_error("EVP_DigestFinal_ex failed!");
			}
			
			std::copy_n(std::begin(digest), buffer.size(), buffer.data());
		}
		
		auto make_mt19937() {
			std::random_device rd;
			return std::mt19937(rd());
		}
		
		Random::Random() : _generator(make_mt19937())
		{
		}
		
		Random::~Random()
		{
		}
		
		void Random::generate(std::uint8_t *buffer, std::size_t size)
		{
			auto distribution = std::uniform_int_distribution<uint8_t>();
			std::generate(buffer, buffer+size, [&]() { return distribution(_generator); });
		}
	}
}
