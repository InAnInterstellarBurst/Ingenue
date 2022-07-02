/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <ingenue/ingenue.h>
#include <stdlib.h>

void *palloc(size_t s)
{
	puts("Allocd!");
	return malloc(s);
}

void *prealloc(void *d, size_t sz)
{
	puts("Reallocd!");
	return realloc(d, sz);
}

int in_main(int argc, char **argv)
{
	in_fmt_init_defaults();
	InAllocator printedalloc = {
		.memalloc = palloc,
		.memrealloc = prealloc,
		.memfree = free
	};

	InStrBuf buf = in_strbuf_alloc(10, &printedalloc);
	buf = in_strbuf_cpy_fixed(buf, incstr("bufffffff!"), 0);
	InStr str = incstr("InStr: {str}\nC-String: {cstr}\nInStrBuf: {strbuf}\ni8: {i8}\tu8: {u8}\ni16: {i16}\tu16: {u16}\ni32: {i32}\tu32: {u32}\nptr: {ptr}\tb: {b}\nError: {");
	InStrBuf we = in_strbuf_alloc_format(&printedalloc, str, incstr("Hi"), "Hello test", buf,
		-3, 3, -400, 400, -400000, 400000, &buf, false);
	in_str_print(we.str);
	in_strbuf_free(we);
	in_strbuf_free(buf);
	puts("");
	return 0;
}
