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

#define in_min(x, y) ((x < y) ? x : y)

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

/**
* ============================================
*  Allocation
* ============================================
*/
typedef void*(*InAllocProc)(size_t);
typedef void*(*InReallocProc)(void*, size_t);
typedef void(*InFreeProc)(void*);

typedef struct  
{
	InAllocProc memalloc;
	InReallocProc memrealloc;
	InFreeProc memfree;
} InAllocator;

extern InAllocator* gInDefaultMallocator;

/**
* ============================================
*  Strings
* ============================================
*/
typedef struct
{
	size_t length;
	size_t capacity;
	bool mutable;
	bool ownMemory;
	char* data;
	InAllocator* allocator;
} InStr;

typedef struct  
{
	size_t start;
	size_t length;
	InStr str;
} InStrView;

typedef struct
{
	InStrView snipped;
	InStrView remaining;
} InStrRangedView;

static InStr gInNullStr = { 0 };

InStr in_str_alloc(size_t capacity, InAllocator* allocator);
InStr in_str_alloc_from_literal(char* literal, InAllocator* alloc);
InStr in_str_alloc_from_view(InStrView v);
InStr in_str_emplace_into(size_t capacity, void* mem);
InStr in_str_immut_from_literal(char* literal);
void in_str_free(InStr str);

bool in_str_isnull(InStr s);
bool in_str_eq(InStr a, InStr b);
bool in_str_eq_strview(InStr a, InStrView b);

void in_str_puts(InStr str, FILE* stream);
void in_str_putv(InStrView v, FILE* stream);

InStr in_str_set_from_literal(InStr str, char* data);
InStr in_str_copy(InStr dst, InStr src, size_t len);
InStr in_str_copy_from_view(InStr dst, InStrView v);
InStr in_str_copy_from_view_realloc(InStr dst, InStrView v);
InStr in_str_copy_literal(InStr dst, char* data);
InStr in_str_copy_realloc(InStr dst, InStr src, size_t len);

InStrView in_str_subview_at_first(InStr str, char from);
InStrView in_str_subview_at_first_v(InStrView v, char from);
InStrRangedView in_str_subview_between(InStr s, char open, char close);
InStrRangedView in_str_subview_between_v(InStrView v, char open, char close);

InStr in_str_format(InStr fmt, InAllocator* alloc, ...);
InStr in_str_format_va(InStr fmt, InAllocator* alloc, va_list args);


typedef InStr(*InFmtTranslationProc)(InAllocator* alloc, va_list va);

void in_fmt_init(void);
void in_fmt_print(FILE* stream, InAllocator* alloc, InStr fmt, ...);
bool in_fmt_add_format(InStr k, InFmtTranslationProc v);
