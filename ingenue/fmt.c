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

InStrBuf fmt_translate(InStr tok, InAllocator* alloc, InStr* outBuf, void* lparam)
{
	InFmtTranslationProc proc = gFmtTable[dumbhash(tok)];
	if(proc == NULL) {
		return (InStrBuf){ 0 };
	} else {
		return proc(alloc, outBuf, lparam);
	}
}


void in_fmt_init_defaults(void)
{
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
