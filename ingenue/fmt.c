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
#include <assert.h>

#define IN_MAX_FMTS 256

static uint32_t gTranslatorCount = 0;
static InFmtTranslationProc gFmtTable[IN_MAX_FMTS] = { 0 };

static uint32_t dumbhash(InStr str)
{
	uint32_t hash = 0;
	for(uint32_t i = 0; i < str.length; i++) {
		hash += (uint32_t)str.data[i];
		hash = (hash * (uint32_t)str.data[i]) % IN_MAX_FMTS;
	}

	return hash;
}

/**
 * It's ugly, it's slow, but it works
 * @param ssobuf There is no way an int is 256 chars :)
 */
static InStr translate_int32(InStrBuf ssoBuf, uint32_t val, size_t offset)
{
	InStr s = ssoBuf.str;
	bool significant = false;
	uint32_t div = (uint32_t)pow(10, 9);
	while(div >= 1) {
		uint8_t digit = (uint8_t)floor(val / div);
		val -= div * digit;
		div = div / 10;
		if(!significant) {
			significant = (digit != 0) || (div == 0);
		}

		if(significant) {
			s.data[offset + s.length++] = (char)(digit + '0');
		}
	}

	return s;
}

static InStr translate_addr(InStrBuf ssoBuf, uintptr_t p)
{
	InStrBuf s = ssoBuf;
	s = in_strbuf_cpy_fixed(s, incstr("0x"), 0);

	for(int i = (sizeof(p) << 3) - 4; i >= 0; i -= 4) {
		int v = (p >> i) & 0xf;
		if(v >= 0 && v < 10) {
			s.str.data[s.str.length++] = '0' + (char)v;
		} else {
			s.str.data[s.str.length++] = 'a' + (char)(v - 10);
		}
	}

	return s.str;
}


static InFmtResult fmt_str_base(InAllocator *alloc, InStrBuf ssoBuf, InStr s)
{
	InFmtResult r = { 0 };
	if(s.length <= ssoBuf.capacity) {
		r.usedSSO = true;
		r.smallString = in_strbuf_cpy_fixed(ssoBuf, s, 0);
	} else {
		r.bigBoi = in_strbuf_alloc(s.length, alloc);
		r.bigBoi = in_strbuf_cpy_grow(r.bigBoi, s, 0);
	}

	return r;
}

static InFmtResult fmt_str_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list *args)
{
	InStr s = va_arg(*args, InStr);
	return fmt_str_base(alloc, ssoBuf, s);
}

static InFmtResult fmt_cstr_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list *args)
{
	char *cs = va_arg(*args, char *);
	return fmt_str_base(alloc, ssoBuf, incstr(cs));
}

static InFmtResult fmt_strbuf_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list *args)
{
	InStrBuf buf = va_arg(*args, InStrBuf);
	return fmt_str_base(alloc, ssoBuf, buf.str);
}

static InFmtResult fmt_bool_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list *args)
{
	bool b = (bool)va_arg(*args, int);
	return fmt_str_base(alloc, ssoBuf, b ? incstr("true") : incstr("false"));
}

static InFmtResult fmt_ptr_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list *args)
{
	uintptr_t p = va_arg(*args, uintptr_t);
	return fmt_str_base(alloc, ssoBuf, translate_addr(ssoBuf, p));
}

static InFmtResult fmt_sint32_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list *args)
{
	int32_t val = va_arg(*args, int32_t);
	InStr s;
	if(val < 0) {
		s = translate_int32(ssoBuf, (uint32_t)(-val), 1);
		s.data[0] = '-';
		s.length++;
	} else {
		s = translate_int32(ssoBuf, (uint32_t)val, 0);
	}

	return fmt_str_base(alloc, ssoBuf, s);
}

static InFmtResult fmt_uint32_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list *args)
{
	uint32_t val = va_arg(*args, uint32_t);
	InStr s = translate_int32(ssoBuf, val, 0);
	return fmt_str_base(alloc, ssoBuf, s);
}

InFmtResult fmt_translate(InStr tok, InAllocator *alloc, InStrBuf ssoBuf, va_list *args)
{
	InFmtTranslationProc proc = gFmtTable[dumbhash(tok)];
	if(proc == NULL) {
		return (InFmtResult) { 0 };
	} else {
		return proc(alloc, ssoBuf, args);
	}
}


void in_fmt_init_defaults(void)
{
	in_fmt_add_translator(incstr("str"), fmt_str_proc);
	in_fmt_add_translator(incstr("cstr"), fmt_cstr_proc);
	in_fmt_add_translator(incstr("strbuf"), fmt_strbuf_proc);

	in_fmt_add_translator(incstr("b"), fmt_bool_proc);
	in_fmt_add_translator(incstr("i8"), fmt_sint32_proc);
	in_fmt_add_translator(incstr("u8"), fmt_uint32_proc);
	in_fmt_add_translator(incstr("i16"), fmt_sint32_proc);
	in_fmt_add_translator(incstr("u16"), fmt_uint32_proc);
	in_fmt_add_translator(incstr("i32"), fmt_sint32_proc);
	in_fmt_add_translator(incstr("u32"), fmt_uint32_proc);
	in_fmt_add_translator(incstr("ptr"), fmt_ptr_proc);
}

size_t in_fmt_get_translator_count(void)
{
	return gTranslatorCount;
}

void in_fmt_add_translator(InStr fmt, InFmtTranslationProc proc)
{
	assert(gTranslatorCount != IN_MAX_FMTS && gFmtTable[dumbhash(fmt)] == NULL);
	gFmtTable[dumbhash(fmt)] = proc;
}
