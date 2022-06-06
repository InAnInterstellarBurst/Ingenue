/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <stdio.h> // >:(
#include <stdint.h>
#include <stdbool.h>

#if IN_DEBUG
	#if IN_WIN32
		#define IN_ASSERT(x) if(!x) { __debugbreak(); }
	#elif IN_LINUX
		#include <signal.h>
		#define IN_ASSERT(x) if(!x) { raise(SIGTRAP); }
	#endif
#else
	#define IN_ASSERT(x)
#endif

extern int in_main(int argc, char** argv);


typedef struct
{
	size_t length;
	size_t capacity;
	bool mutable;
	bool ownMemory;
	char* data;
} InStr;

typedef struct  
{
	size_t start;
	size_t length;
	InStr* str;
} InStrView;

static InStr gInNullStr = { 0 };

InStr in_str_alloc(size_t capacity);
InStr in_str_from_literal(char* literal);
InStr in_str_emplace_into(size_t capacity, void* mem);
void in_str_free(InStr str);

bool in_str_isnull(InStr s);
void in_str_puts(InStr str, FILE* stream);
void in_str_putv(InStrView v, FILE* stream);

InStr in_str_set_literal(InStr str, char* data);
InStr in_str_copy(InStr dst, InStr src, size_t len);
InStr in_str_copy_from_view(InStr dst, InStrView v);
InStr in_str_copy_literal(InStr dst, char* data);

char* in_str_alloc_cstr(InStr str);
InStr in_str_pop_at(InStr* str, char* at);
InStrView in_str_subview(InStr* str, char* from);
