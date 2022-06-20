/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <ingenue/ingenue.h>
#include <stdlib.h>

void* palloc(size_t s)
{
	puts("Allocd!");
	return malloc(s);
}

void* prealloc(void* d, size_t sz)
{
	puts("Reallocd!");
	return realloc(d, sz);
}

int in_main(int argc, char** argv)
{
	InAllocator printedalloc = {
		.memalloc = palloc,
		.memrealloc = prealloc,
		.memfree = free
	};

	InStr str = incstr("Hello {his} {is} a {pretty long} {string!");
	InStrBuf we = in_strbuf_alloc_format(&printedalloc, str);
	in_str_print(we.str);
	in_strbuf_free(we);
	puts("");
	return 0;
}
