//
//  Reference.hpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#pragma once

#include <ruby.h>

struct ValueReference {
	VALUE _value;
	
	template <typename ReferenceType>
	ValueReference(VALUE value, ReferenceType & reference) : _value(value)
	{
		DATA_PTR(value) = reinterpret_cast<void*>(&reference);
	}
	
		template <typename ReferenceType>
	ValueReference(VALUE value, const ReferenceType & reference) : ValueReference(value, const_cast<ReferenceType&>(reference))
	{
	}
	
	~ValueReference()
	{
		DATA_PTR(_value) = NULL;
	}
};
