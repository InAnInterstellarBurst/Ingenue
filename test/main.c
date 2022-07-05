/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <ingenue/ingenue.h>
#include <stdlib.h>
#include <stdio.h>

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

	InFile f = in_file_open("testfile", &printedalloc, IN_FILE_MODE_READONLY);
	if(in_file_write_str(f, incstr("\n\nHi"))) {
		in_fmt_print(&printedalloc, incstr("Oh god\n"));
	}

	InStrBuf radiospeel = in_strbuf_alloc(f.size, &printedalloc);
	radiospeel = in_file_read_all(f, radiospeel);
	in_file_close(f);

	InFile newfile = in_file_open("newfile!", &printedalloc, IN_FILE_MODE_WRITEONLY);
	if(!newfile.open) {
		newfile = in_file_create("newfile!", &printedalloc);
	}
	in_file_write_str(newfile, radiospeel.str);
	in_strbuf_free(radiospeel);
	puts("");
	return 0;
}
