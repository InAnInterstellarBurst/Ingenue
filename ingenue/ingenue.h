/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#pragma once

#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define in_min(x, y) ((x < y) ? x : y)

#if IN_DEBUG
#if IN_WIN32
#define IN_ASSERT(x)    \
	if(!(x)) {          \
		__debugbreak(); \
	}
#elif IN_LINUX
#include <signal.h>
#define IN_ASSERT(x)    \
	if(!(x)) {          \
		raise(SIGTRAP); \
	}
#endif
#else
#define IN_ASSERT(x)
#endif


extern int in_main(int argc, char **argv);

/**
* ============================================
*  Allocation
* ============================================
*/
typedef void *(*InAllocProc)(size_t);
typedef void *(*InReallocProc)(void *, size_t);
typedef void (*InFreeProc)(void *);

typedef struct
{
	InAllocProc memalloc;
	InReallocProc memrealloc;
	InFreeProc memfree;
} InAllocator;

extern InAllocator *gInDefaultMallocator;

/**
* ============================================
*  Strings & Formatting
* ============================================
*/
typedef struct
{
	size_t length;
	char *data;
} InStr;

typedef struct
{
	size_t capacity;
	InStr str;
	InAllocator *alloc;
} InStrBuf;

static inline InStr incstr(char *cstr)
{
	return (InStr) {
		.length = strlen(cstr),
		.data = cstr
	};
}

InStr in_str_substr(InStr s, size_t offset, size_t length);
bool in_str_eq(InStr a, InStr b);
uint32_t in_str_find(InStr s, char delim);
void in_str_print(InStr str);

void in_strbuf_free(InStrBuf buf);
InStrBuf in_strbuf_reserve(InStrBuf buf, size_t newsz);
InStrBuf in_strbuf_cpy_fixed(InStrBuf dst, InStr src, size_t len);
InStrBuf in_strbuf_cpy_grow(InStrBuf dst, InStr src, size_t len);

InStrBuf in_strbuf_alloc(size_t sz, InAllocator *alloc);
InStrBuf in_strbuf_alloc_format(InAllocator *alloc, InStr fmt, ...);
InStrBuf in_strbuf_alloc_format_va(InAllocator *alloc, InStr fmt, va_list va);


typedef struct
{
	bool usedSSO;
	InStrBuf bigBoi;
	InStrBuf smallString;
} InFmtResult;

typedef InFmtResult (*InFmtTranslationProc)(InAllocator *alloc, InStrBuf ssoBuf, va_list *args);

void in_fmt_init_defaults(void);
size_t in_fmt_get_translator_count(void);
void in_fmt_add_translator(InStr fmt, InFmtTranslationProc proc);

void in_fmt_print(InAllocator *alloc, InStr fmt, ...);

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
	size_t cursor;
	InStr path;
	InFileMode mode;
	InFileHandle *handle;
	InAllocator *alloc;
} InFile;

extern const size_t gInFilePositionEOF;


InFile in_file_create(char *path, InAllocator *alloc);
InFile in_file_open(char *path, InAllocator *alloc, InFileMode mode);
void in_file_close(InFile file);
bool in_file_delete_from_system(const char *path);
void in_file_flush(InFile file);

bool in_file_set_cursor_pos(InFile *file, size_t pos); // true is cursor moved
bool in_file_write_str(InFile file, InStr str); // true if **all** bytes written
bool in_file_write_bytes(InFile file, uint8_t *bytes, size_t length); // true if **all** bytes written
InStrBuf in_file_read(InFile file, size_t length, InStrBuf buf); // NullStr on fail
InStrBuf in_file_read_all(InFile file, InStrBuf buf);