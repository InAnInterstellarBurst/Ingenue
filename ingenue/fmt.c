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

InStr fmt_translate(InStrView v)
{
	for(uint16_t i = 0; i < s_AddedFormats; i++) {
		if(s_FormatTable[i].k == v)
	}
}

void in_fmt_print(FILE* stream, InStr fmt, ...)
{
	va_list l;
	va_start(l, fmt);
	InStr s = in_str_format_va(fmt, l);
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