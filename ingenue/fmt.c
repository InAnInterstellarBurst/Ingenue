/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pch.h"
#include <math.h>
#include <stdarg.h>

#define IN_MAX_FMTS 256

typedef struct 
{
	InStr k;
	InFmtTranslationProc p;
} FmtEntry;

static uint16_t s_AddedFormats = 0;
static FmtEntry s_FormatTable[IN_MAX_FMTS];

/**
 * It's ugly, it's slow, but it works
 */
static InStr translate_int32(InAllocator* alloc, uint32_t val, size_t offset)
{
	InStr s = in_str_alloc(10 + offset, alloc);
	if(in_str_isnull(s)) {
		return gInNullStr;
	}

	bool significant = false;
	uint32_t div = (uint32_t)pow(10, 9);
	while(div >= 1) {
		uint8_t digit = (uint8_t)floor(val / div);
		val -= div * digit;
		div = div / 10;
		if(!significant) {
			significant = (digit != 0) || (div == 1);
		}

		if(significant) {
			s.data[offset + s.length++] = (char)(digit + '0');
		}
	}

	return s;
}

static InStr translate_addr(InAllocator* alloc, uintptr_t p)
{
	InStr s = in_str_alloc(18, alloc);
	s = in_str_set_from_literal(s, "0x");
	if(in_str_isnull(s)) {
		return gInNullStr;
	}

	for(int i = (sizeof(p) << 3) - 4; i >= 0; i-= 4) {
		int v = (p >> i) & 0xf;
		if(v >= 0 && v < 10) {
			s.data[s.length++] = '0' + (char)v;
		} else {
			s.data[s.length++] = 'a' + (char)(v - 10);
		}
	}

	return s;
}


InStr fmt_translate(InStrView v, InAllocator* alloc, va_list va)
{
	for(uint16_t i = 0; i < s_AddedFormats; i++) {
		if(in_str_eq_strview(s_FormatTable[i].k, v)) {
			return s_FormatTable[i].p(alloc, va);
		}
	}

	return in_str_immut_from_literal("");
}

void in_fmt_print(FILE* stream, InAllocator* alloc, InStr fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	InStr s = in_str_format_va(fmt, alloc, l);
	in_str_puts(s, stream);
	in_str_free(s);
	va_end(l);
}

bool in_fmt_add_format(InStr k, InFmtTranslationProc v)
{
	if(s_AddedFormats < IN_MAX_FMTS) {
		s_FormatTable[s_AddedFormats] = (FmtEntry){ .k = k, .p = v };
		s_AddedFormats++;
		return true;
	}

	return false;
}

InStr fmt_str(InAllocator* alloc, va_list va)
{
	InStr old = va_arg(va, InStr);
	InStr s = in_str_alloc(old.length, alloc);
	return in_str_copy(s, old, 0);
}

InStr fmt_cstr(InAllocator* alloc, va_list va)
{
	return in_str_immut_from_literal(va_arg(va, char*));
}

InStr fmt_strview(InAllocator* alloc, va_list va)
{
	InStrView v = va_arg(va, InStrView);
	return in_str_alloc_from_view(v);
}

InStr fmt_bool(InAllocator* alloc, va_list va)
{
	bool b = va_arg(va, int);
	if(b) {
		return in_str_immut_from_literal("true");
	} else {
		return in_str_immut_from_literal("false");
	}
}

InStr fmt_sint_32(InAllocator* alloc, va_list va)
{
	int32_t val = va_arg(va, int32_t);
	if(val < 0) {
		InStr s = translate_int32(alloc, (uint32_t)(-val), 1);
		s.data[0] = '-';
		s.length++;
		return s;
	} else {
		return translate_int32(alloc, (uint32_t)val, 0);
	}
}

InStr fmt_uint_32(InAllocator* alloc, va_list va)
{
	uint32_t val = va_arg(va, uint32_t);
	return translate_int32(alloc, val, 0);
}

InStr fmt_ptr(InAllocator* alloc, va_list va)
{
	uintptr_t val = va_arg(va, uintptr_t);
	return translate_addr(alloc, val);
}

void in_fmt_init(void)
{
	in_fmt_add_format(in_str_immut_from_literal("b"), fmt_bool);
	in_fmt_add_format(in_str_immut_from_literal("str"), fmt_str);
	in_fmt_add_format(in_str_immut_from_literal("cstr"), fmt_cstr);
	in_fmt_add_format(in_str_immut_from_literal("str_view"), fmt_strview);

	in_fmt_add_format(in_str_immut_from_literal("ptr"), fmt_ptr);
	in_fmt_add_format(in_str_immut_from_literal("i32"), fmt_sint_32);
	in_fmt_add_format(in_str_immut_from_literal("u32"), fmt_uint_32);
	in_fmt_add_format(in_str_immut_from_literal("i16"), fmt_sint_32);
	in_fmt_add_format(in_str_immut_from_literal("u16"), fmt_uint_32);
	in_fmt_add_format(in_str_immut_from_literal("i8"), fmt_sint_32);
	in_fmt_add_format(in_str_immut_from_literal("u8"), fmt_uint_32);
}
