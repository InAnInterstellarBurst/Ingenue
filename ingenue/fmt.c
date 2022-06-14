/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pch.h"
#include <stdarg.h>

#define IN_MAX_FMTS 256

typedef struct 
{
	InStr k;
	InFmtTranslationProc p;
} FmtEntry;

static uint16_t s_AddedFormats = 0;
static FmtEntry s_FormatTable[IN_MAX_FMTS];

InStr fmt_translate(InStrView v, va_list* va)
{
	for(uint16_t i = 0; i < s_AddedFormats; i++) {
		if(in_str_eq_strview(s_FormatTable[i].k, v)) {
			return s_FormatTable[i].p(va);
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


InStr fmt_str(va_list* va)
{
	InStr old = va_arg(*va, InStr);
	InStr s = in_str_alloc(old.length, old.allocator);
	return in_str_copy(s, old, 0);
}

InStr fmt_cstr(va_list* va)
{
	return in_str_immut_from_literal(va_arg(*va, char*));
}

InStr fmt_strview(va_list* va)
{
	InStrView v = va_arg(*va, InStrView);
	return in_str_alloc_from_view(v);
}

InStr fmt_bool(va_list* va)
{
	bool b = va_arg(*va, bool);
	if(b) {
		return in_str_immut_from_literal("true");
	} else {
		return in_str_immut_from_literal("false");
	}
}

InStr fmt_int(va_list* va)
{
	char buf[33];
	int val = va_arg(*va, int);
	itoa(val, buf, 10);
	return in_str_alloc_from_literal(buf, NULL);
}

InStr fmt_ptr(va_list* va)
{
	char buf[33];
	int val = va_arg(*va, int);
	itoa(val, buf, 16);
	return in_str_alloc_from_literal(buf, NULL);
}

void in_fmt_init(void)
{
	in_fmt_add_format(in_str_immut_from_literal("b"), fmt_bool);
	in_fmt_add_format(in_str_immut_from_literal("str"), fmt_str);
	in_fmt_add_format(in_str_immut_from_literal("cstr"), fmt_cstr);
	in_fmt_add_format(in_str_immut_from_literal("str_view"), fmt_strview);
	in_fmt_add_format(in_str_immut_from_literal("ptr"), fmt_ptr);
	in_fmt_add_format(in_str_immut_from_literal("int"), fmt_int);
}
