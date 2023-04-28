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

VALUE Protocol_QUIC_Address = Qnil;

static void Protocol_QUIC_Address_free(void *data) {
	if (data) {
		delete reinterpret_cast<Protocol::QUIC::Address *>(data);
	}
}

static size_t Protocol_QUIC_Address_size(const void *data) {
	return sizeof(Protocol::QUIC::Address);
}

const rb_data_type_t Protocol_QUIC_Address_type = {
	.wrap_struct_name = "Protocol::QUIC::Address",
	.function = {
		.dmark = NULL,
		.dfree = Protocol_QUIC_Address_free,
		.dsize = Protocol_QUIC_Address_size,
	},
	.data = NULL,
	.flags = RUBY_TYPED_FREE_IMMEDIATELY,
};

Protocol::QUIC::Address * Protocol_QUIC_Address_get(VALUE self)
{
	Protocol::QUIC::Address *address;
	
	TypedData_Get_Struct(self, Protocol::QUIC::Address, &Protocol_QUIC_Address_type, address);
	
	return address;
}

VALUE Protocol_QUIC_Address_allocate(VALUE klass) {
	return TypedData_Wrap_Struct(klass, &Protocol_QUIC_Address_type, NULL);
}

static VALUE Protocol_QUIC_Address_initialize(VALUE self) {
	DATA_PTR(self) = new Protocol::QUIC::Address();
	
	return self;
}

static VALUE Protocol_QUIC_Address_inspect(VALUE self) {
	auto address = Protocol_QUIC_Address_get(self);
	auto string = address->to_string();
	
	VALUE class_name = rb_inspect(rb_class_of(self));
	
	return rb_sprintf("<%" PRIsVALUE ":%p '%s'>", class_name, (void*)self, string.c_str());
}

static VALUE  Protocol_QUIC_Address_family(VALUE self) {
	Protocol::QUIC::Address *address = Protocol_QUIC_Address_get(self);
	
	return RB_INT2NUM(address->family());
}

static VALUE Protocol_QUIC_Address_data(VALUE self) {
	Protocol::QUIC::Address *address = Protocol_QUIC_Address_get(self);
	
	return rb_str_new((const char *)&address->data.sa, address->length);
}

VALUE Protocol_QUIC_Address_wrap(VALUE klass, const Protocol::QUIC::Address & address)
{
	VALUE self = Protocol_QUIC_Address_allocate(klass);
	Protocol_QUIC_Address_initialize(self);
	
	Protocol::QUIC::Address *data = Protocol_QUIC_Address_get(self);
	
	*data = address;
	
	return self;
}

VALUE Protocol_QUIC_Address_s_resolve(VALUE klass, VALUE host, VALUE service, VALUE family, VALUE type, VALUE flags)
{
	StringValue(host);
	StringValue(service);
	
	auto addresses = Protocol::QUIC::Address::resolve(
		std::string_view(RSTRING_PTR(host), RSTRING_LEN(host)),
		std::string_view(RSTRING_PTR(service), RSTRING_LEN(service)),
		RB_NUM2INT(family),
		RB_NUM2INT(type),
		RB_NUM2INT(flags)
	);
	
	VALUE result = rb_ary_new_capa(addresses.size());
	
	for (auto & address : addresses) {
		rb_ary_push(result, Protocol_QUIC_Address_wrap(klass, address));
	}
	
	return result;
}

void Init_Protocol_QUIC_Address(VALUE Protocol_QUIC) {
	Protocol_QUIC_Address =
			rb_define_class_under(Protocol_QUIC, "Address", rb_cObject);
	
	rb_define_singleton_method(Protocol_QUIC_Address, "resolve", Protocol_QUIC_Address_s_resolve, 5);
	
	rb_define_alloc_func(Protocol_QUIC_Address, Protocol_QUIC_Address_allocate);
	rb_define_method(Protocol_QUIC_Address, "initialize", Protocol_QUIC_Address_initialize, 0);
	rb_define_method(Protocol_QUIC_Address, "inspect", Protocol_QUIC_Address_inspect, 0);
	
	rb_define_method(Protocol_QUIC_Address, "family", Protocol_QUIC_Address_family, 0);
	rb_define_method(Protocol_QUIC_Address, "data", Protocol_QUIC_Address_data, 0);
}
