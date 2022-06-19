/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pch.h"
#include <stdarg.h>

InAllocator* gInDefaultMallocator = NULL;

InStr in_str_create_cstr(char* cstr)
{
	return (InStr){
		.length = strlen(cstr),
		.data = cstr
	};
}

InStr in_str_substr(InStr s, size_t offset, size_t length)
{
	if(s.length < offset + length) {
		return (InStr){ 0 };
	}

	if(length == 0) {
		length = (s.length - offset);
	}

	return (InStr){
		.length = length,
		.data = &s.data[offset]
	};
}

bool in_str_eq(InStr a, InStr b)
{
	bool samelen = (a.length == b.length);
	return (strncmp(a.data, b.data, a.length) == 0) && samelen;
}

uint32_t in_str_find(InStr s, char delim)
{
	for(uint32_t i = 0; i < s.length; i++) {
		if(s.data[i] == delim) {
			return i;
		}
	}

	return UINT32_MAX;
}

void in_str_print(InStr str)
{
	fwrite(str.data, sizeof(char), str.length, stdout);
}

void in_strbuf_free(InStrBuf buf)
{
	buf.alloc->memfree(buf.str.data);
}

InStrBuf in_strbuf_reserve(InStrBuf buf, size_t newsz)
{
	if(buf.str.data == NULL) {
		return (InStrBuf){ 0 };
	}

	buf.capacity = newsz * sizeof(char);
	buf.str.data = buf.alloc->memrealloc(buf.str.data, newsz);
	if(buf.str.data == NULL) {
		return (InStrBuf){ 0 };
	}

	return buf;
}

InStrBuf in_strbuf_cpy_fixed(InStrBuf dst, InStr src, size_t len)
{
	size_t cpylen = len;
	if(cpylen == 0 || cpylen > src.length) {
		cpylen = src.length;
	}

	if(dst.capacity < dst.str.length + cpylen) {
		return (InStrBuf){ 0 }; // Fixed copy, fail
	}

	memcpy(&dst.str.data[dst.str.length], src.data, cpylen);
	dst.str.length += cpylen;
	return dst;
}

InStrBuf in_strbuf_cpy_grow(InStrBuf dst, InStr src, size_t len)
{
	size_t cpylen = len;
	if(cpylen == 0 || cpylen > src.length) {
		cpylen = src.length;
	}

	if(dst.capacity < dst.str.length + cpylen) {
		size_t newcap = dst.str.length + cpylen + 4; // Throw in a few extra chars for good measure :)
		dst = in_strbuf_reserve(dst, newcap);
	}

	memcpy(&dst.str.data[dst.str.length], src.data, cpylen);
	dst.str.length += cpylen;
	return dst;
}

InStrBuf in_strbuf_alloc(size_t sz, InAllocator* alloc)
{
	if(alloc == NULL) {
		alloc = gInDefaultMallocator;
	}

	InStr str = { 0 };
	str.data = alloc->memalloc(sz * sizeof(char));
	return (InStrBuf){
		.capacity = sz,
		.str = str,
		.alloc = alloc
	};
}

InStrBuf in_strbuf_alloc_format(InAllocator* alloc, InStr fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	InStrBuf r = in_strbuf_alloc_format_va(alloc, fmt, l);
	va_end(l);
	return r;
}

// This is probably ineffective and hard to parse but yk what it works so :)
InStrBuf in_strbuf_alloc_format_va(InAllocator* alloc, InStr fmt, va_list va)
{
	uint32_t tokStartIdx = in_str_find(fmt, '{');
	InStr tokStart = in_str_substr(fmt, tokStartIdx + 1, 0);
	uint32_t tokEndIdx = in_str_find(tokStart, '}');

	InStrBuf result = in_strbuf_alloc(fmt.length * 4, alloc);
	result = in_strbuf_cpy_grow(result, in_str_substr(fmt, 0, tokStartIdx), 0);
	while(true) {
		result = in_strbuf_cpy_grow(result, in_str_substr(tokStart, 0, tokEndIdx), 0);
		tokStartIdx = in_str_find(tokStart, '{');
		if(tokStartIdx == UINT32_MAX) {
			break;
		}

		InStr before = in_str_substr(tokStart, tokEndIdx + 1, tokStartIdx - tokEndIdx - 1);
		result = in_strbuf_cpy_grow(result, before, 0);
		tokStart = in_str_substr(tokStart, tokStartIdx + 1, 0);
		uint32_t newEndIdx = in_str_find(tokStart, '}');
		if(newEndIdx == UINT32_MAX) {
			tokEndIdx = tokStartIdx; // Incase we end on unclosed {
			break;
		}
		
		tokEndIdx = newEndIdx;
	}

	InStr final = in_str_substr(tokStart, tokEndIdx + 1, tokStart.length - tokEndIdx - 1);
	result = in_strbuf_cpy_grow(result, final, 0);
	return result;
}
