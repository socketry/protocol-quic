//
//  Address.cpp
//  This file is part of the "Protocol::QUIC" project and released under the MIT License.
//
//  Created by Samuel Williams on 27/4/2023.
//  Copyright, 2023, by Samuel Williams. All rights reserved.
//

#include "Address.hpp"
#include "ruby/backward/cxxanyargs.hpp"
#include "ruby/internal/intern/object.h"

VALUE Ruby_Protocol_QUIC_Address = Qnil;

static void Ruby_Protocol_QUIC_Address_free(void *data) {
	if (data) {
		delete reinterpret_cast<::Protocol::QUIC::Address *>(data);
	}
}

static size_t Ruby_Protocol_QUIC_Address_size(const void *data) {
	return sizeof(::Protocol::QUIC::Address);
}

const rb_data_type_t Ruby_Protocol_QUIC_Address_type = {
	.wrap_struct_name = "Protocol::QUIC::Address",
	.function = {
		.dmark = NULL,
		.dfree = Ruby_Protocol_QUIC_Address_free,
		.dsize = Ruby_Protocol_QUIC_Address_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

::Protocol::QUIC::Address * Ruby_Protocol_QUIC_Address_get(VALUE self)
{
	::Protocol::QUIC::Address *address;
	
	TypedData_Get_Struct(self, ::Protocol::QUIC::Address, &Ruby_Protocol_QUIC_Address_type, address);
	
	return address;
}

VALUE Ruby_Protocol_QUIC_Address_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Ruby_Protocol_QUIC_Address_type, NULL);
}

static VALUE Ruby_Protocol_QUIC_Address_initialize(VALUE self) {
	DATA_PTR(self) = new ::Protocol::QUIC::Address();
	
	return self;
}

static VALUE Ruby_Protocol_QUIC_Address_inspect(VALUE self) {
	auto address = Ruby_Protocol_QUIC_Address_get(self);
	auto string = address->to_string();
	
	VALUE class_name = rb_inspect(rb_class_of(self));
	
	return rb_sprintf("<%" PRIsVALUE ":%p '%s'>", class_name, (void*)self, string.c_str());
}

static VALUE  Ruby_Protocol_QUIC_Address_family(VALUE self) {
	::Protocol::QUIC::Address *address = Ruby_Protocol_QUIC_Address_get(self);
	
	return RB_INT2NUM(address->family());
}

static VALUE Ruby_Protocol_QUIC_Address_data(VALUE self) {
	::Protocol::QUIC::Address *address = Ruby_Protocol_QUIC_Address_get(self);
	
	return rb_str_new((const char *)&address->data.sa, address->length);
}

VALUE Ruby_Protocol_QUIC_Address_wrap(VALUE klass, const ::Protocol::QUIC::Address & address)
{
	VALUE self = Ruby_Protocol_QUIC_Address_allocate(klass);
	Ruby_Protocol_QUIC_Address_initialize(self);
	
	::Protocol::QUIC::Address *data = Ruby_Protocol_QUIC_Address_get(self);
	
	*data = address;
	
	return self;
}

VALUE Ruby_Protocol_QUIC_Address_s_resolve(VALUE klass, VALUE host, VALUE service, VALUE family, VALUE type, VALUE flags)
{
	StringValue(host);
	StringValue(service);
	
	auto addresses = ::Protocol::QUIC::Address::resolve(
		std::string_view(RSTRING_PTR(host), RSTRING_LEN(host)),
		std::string_view(RSTRING_PTR(service), RSTRING_LEN(service)),
		RB_NUM2INT(family),
		RB_NUM2INT(type),
		RB_NUM2INT(flags)
	);
	
	VALUE result = rb_ary_new_capa(addresses.size());
	
	for (auto & address : addresses) {
		rb_ary_push(result, Ruby_Protocol_QUIC_Address_wrap(klass, address));
	}
	
	return result;
}

void Init_Ruby_Protocol_QUIC_Address(VALUE Protocol_QUIC) {
	Ruby_Protocol_QUIC_Address =
			rb_define_class_under(Protocol_QUIC, "Address", rb_cObject);
	
	rb_define_singleton_method(Ruby_Protocol_QUIC_Address, "resolve", Ruby_Protocol_QUIC_Address_s_resolve, 5);
	
	rb_define_alloc_func(Ruby_Protocol_QUIC_Address, Ruby_Protocol_QUIC_Address_allocate);
	rb_define_method(Ruby_Protocol_QUIC_Address, "initialize", Ruby_Protocol_QUIC_Address_initialize, 0);
	rb_define_method(Ruby_Protocol_QUIC_Address, "inspect", Ruby_Protocol_QUIC_Address_inspect, 0);
	
	rb_define_method(Ruby_Protocol_QUIC_Address, "family", Ruby_Protocol_QUIC_Address_family, 0);
	rb_define_method(Ruby_Protocol_QUIC_Address, "data", Ruby_Protocol_QUIC_Address_data, 0);
}
