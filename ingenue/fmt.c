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

static InFmtResult fmt_str_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list args)
{
	InStr s = va_arg(args, InStr);
	return fmt_str_base(alloc, ssoBuf, s);
}

static InFmtResult fmt_cstr_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list args)
{
	char *cs = va_arg(args, char *);
	return fmt_str_base(alloc, ssoBuf, incstr(cs));
}

static InFmtResult fmt_strbuf_proc(InAllocator *alloc, InStrBuf ssoBuf, va_list args)
{
	InStrBuf buf = va_arg(args, InStrBuf);
	return fmt_str_base(alloc, ssoBuf, buf.str);
}


InFmtResult fmt_translate(InStr tok, InAllocator *alloc, InStrBuf ssoBuf, va_list args)
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
}

size_t in_fmt_get_translator_count(void)
{
	return gTranslatorCount;
}

void in_fmt_add_translator(InStr fmt, InFmtTranslationProc proc)
{
	assert(gTranslatorCount != IN_MAX_FMTS && gFmtTable[dumbhash(fmt)] == NULL);
	printf("%d\n", dumbhash(fmt));
	gFmtTable[dumbhash(fmt)] = proc;
}
