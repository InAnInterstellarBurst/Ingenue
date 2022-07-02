/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pch.h"
#include <stdarg.h>

InAllocator *gInDefaultMallocator = NULL;

extern InFmtResult fmt_translate(InStr tok, InAllocator *alloc, InStrBuf ssoBuf, va_list va);

InStr incstr(char *cstr)
{
	return (InStr) {
		.length = strlen(cstr),
		.data = cstr
	};
}

InStr in_str_substr(InStr s, size_t offset, size_t length)
{
	if(s.length < offset + length) {
		return (InStr) { 0 };
	}

	if(length == 0) {
		length = (s.length - offset);
	}

	return (InStr) {
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
		return (InStrBuf) { 0 };
	}

	buf.capacity = newsz * sizeof(char);
	buf.str.data = buf.alloc->memrealloc(buf.str.data, newsz);
	if(buf.str.data == NULL) {
		return (InStrBuf) { 0 };
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
		return (InStrBuf) { 0 }; // Fixed copy, fail
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
		size_t newcap = dst.str.length + cpylen;
		dst = in_strbuf_reserve(dst, newcap);
	}

	memcpy(&dst.str.data[dst.str.length], src.data, cpylen);
	dst.str.length += cpylen;
	return dst;
}

InStrBuf in_strbuf_alloc(size_t sz, InAllocator *alloc)
{
	if(alloc == NULL) {
		alloc = gInDefaultMallocator;
	}

	InStr str = { 0 };
	str.data = alloc->memalloc(sz * sizeof(char));
	return (InStrBuf) {
		.capacity = sz,
		.str = str,
		.alloc = alloc
	};
}

InStrBuf in_strbuf_alloc_format(InAllocator *alloc, InStr fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	InStrBuf r = in_strbuf_alloc_format_va(alloc, fmt, l);
	va_end(l);
	return r;
}

// This is probably ineffective and hard to parse but yk what it works so :)
InStrBuf in_strbuf_alloc_format_va(InAllocator *alloc, InStr fmt, va_list va)
{
	InStrBuf result = in_strbuf_alloc(fmt.length * 4, alloc);
	while(true) {
		uint32_t tokStartIdx = in_str_find(fmt, '{');
		if(tokStartIdx == UINT32_MAX) {
			break;
		}

		result = in_strbuf_cpy_grow(result, fmt, tokStartIdx);
		fmt = in_str_substr(fmt, tokStartIdx + 1, fmt.length - tokStartIdx - 1);
		uint32_t tokEndIdx = in_str_find(fmt, '}');
		if(tokEndIdx == UINT32_MAX) {
			result = in_strbuf_cpy_grow(result, incstr("[Formatting error: Unmatched open brace] "), 0);
			break;
		}

		InStr tok = in_str_substr(fmt, 0, tokEndIdx);
		fmt = in_str_substr(fmt, tokEndIdx + 1, fmt.length - tokEndIdx - 1);

		// Translate token and append
		char bfr[256];
		InStrBuf strbuf = {
			.capacity = 256,
			.alloc = NULL,
			.str = {
				.length = 0,
				.data = bfr,
			}
		};

		InFmtResult translated = fmt_translate(tok, alloc, strbuf, va);
		if(translated.usedSSO) {
			result = in_strbuf_cpy_grow(result, translated.smallString.str, 0);
		} else {
			result = in_strbuf_cpy_grow(result, translated.bigBoi.str, 0);
			in_strbuf_free(translated.bigBoi);
		}
	}

	InStr final = in_str_substr(fmt, 0, fmt.length);
	result = in_strbuf_cpy_grow(result, final, 0);
	return result;
}
