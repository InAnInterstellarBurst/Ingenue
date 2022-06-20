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
*  Strings & Formatting
* ============================================
*/

typedef struct
{
	size_t length;
	char* data;
} InStr;

typedef struct 
{
	size_t capacity;
	InStr str;
	InAllocator* alloc;
} InStrBuf;

InStr incstr(char* cstr);
InStr in_str_substr(InStr s, size_t offset, size_t length);
bool in_str_eq(InStr a, InStr b);
uint32_t in_str_find(InStr s, char delim);
void in_str_print(InStr str);

void in_strbuf_free(InStrBuf buf);
InStrBuf in_strbuf_reserve(InStrBuf buf, size_t newsz);
InStrBuf in_strbuf_cpy_fixed(InStrBuf dst, InStr src, size_t len);
InStrBuf in_strbuf_cpy_grow(InStrBuf dst, InStr src, size_t len);

InStrBuf in_strbuf_alloc(size_t sz, InAllocator* alloc);
InStrBuf in_strbuf_alloc_format(InAllocator* alloc, InStr fmt, ...);
InStrBuf in_strbuf_alloc_format_va(InAllocator* alloc, InStr fmt, va_list va);


typedef InStrBuf(*InFmtTranslationProc)(InAllocator* alloc, InStr* outbuf, void* lparam);

void in_fmt_init_defaults(void);
size_t in_fmt_get_translator_count(void);
bool in_fmt_add_translator(InStr fmt, InFmtTranslationProc proc);

/**
* ============================================
*  Filesystem
* ============================================
*/
typedef enum
{
	IN_FILE_MODE_READONLY,
	IN_FILE_MODE_WRITEONLY,
	IN_FILE_MODE_READWRITE
} InFileMode;

typedef struct InFileHandle_ InFileHandle;
typedef struct
{
	bool open;
	size_t size;
	InStr path;
	InFileMode mode;
	InFileHandle* handle;
	InAllocator alloc;
} InFile;


InFile in_file_open(InStr path, InAllocator alloc, InFileMode mode);
InFile in_file_open_and_create(InStr path, InAllocator alloc, bool clear);
void in_file_close(InFile file);
bool in_file_delete_from_system_path(const char* path);

InStr in_file_read(InFile file, size_t from, size_t length);
bool in_file_write_str(InFile file, InStr str);
bool in_file_write_bytes(InFile file, uint8_t* bytes, size_t length);
void in_file_flush(InFile file);