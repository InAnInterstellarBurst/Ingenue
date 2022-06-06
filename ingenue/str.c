/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pch.h"

InStr in_str_alloc(size_t capacity)
{
	InStr s = { 0 };
	s.length    = 0;
	s.mutable   = true;
	s.ownMemory = true;
	s.capacity  = capacity;
	s.data      = malloc(capacity);
	
	if(s.data == NULL) {
		return gInNullStr;
	}

	return s;
}

InStr in_str_from_literal(char* literal)
{
	InStr s = { 0 };
	s.length    = strlen(literal);
	s.mutable   = false;
	s.ownMemory = false;
	s.capacity  = s.length;
	s.data      = literal;
	return s;
}

InStr in_str_emplace_into(size_t capacity, void* mem)
{
	InStr s = { 0 };
	s.length    = 0;
	s.mutable   = true;
	s.ownMemory = false;
	s.capacity  = capacity;
	s.data      = mem;
	return s;
}

void in_str_free(InStr str)
{
	if(str.ownMemory) {
		free(str.data);
	}
}

char* in_str_alloc_cstr(InStr str)
{
	char* cstr = calloc(str.length + 1, sizeof(char));
	if(cstr != NULL) {
		memcpy(cstr, str.data, str.length);
	}
	return cstr;
}

// C++ eat ur heart out
bool in_str_isnull(InStr s)
{
	return (s.capacity == 0 && s.data == NULL);
}

void in_str_puts(InStr str, FILE* stream)
{
	fwrite(str.data, sizeof(char), str.length, stream);
}

void in_str_putv(InStrView v, FILE* stream)
{
	fwrite(&v.str->data[v.start], sizeof(char), v.length, stream);
}

InStr in_str_copy(InStr dst, InStr src, size_t len)
{
	IN_ASSERT(dst.mutable);
	size_t cpylen = len;
	if(len == 0 || len > src.length) {
		cpylen = src.length;
	}

	size_t newlen = dst.length + cpylen;
	if(dst.capacity >= newlen) {
		InStr s = dst;
		s.length = newlen;
		memcpy(&s.data[dst.length], src.data, cpylen);
		return s;
	} else {
		IN_ASSERT(false);
		return gInNullStr;
	}
}

InStr in_str_copy_from_view(InStr dst, InStrView v)
{
	v.str->data += v.start; // Jesus fuck
	InStr s = in_str_copy(dst, *v.str, v.length);
	v.str->data -= v.start;
	return s;
}

InStr in_str_pop_at(InStr* str, char* at)
{
	return (InStr){ 0 };
}

InStrView in_str_subview(InStr* str, char* from)
{
	return (InStrView){ 0 };
}