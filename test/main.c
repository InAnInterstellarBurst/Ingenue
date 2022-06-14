/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <ingenue/ingenue.h>
#include <stdlib.h>

void* printed_malloc(size_t s)
{
	puts("Allocd!");
	return malloc(s);
}

void* printed_realloc(void* d, size_t s)
{
	puts("reallocd!");
	return realloc(d, s);
}

int in_main(int argc, char** argv)
{
	in_fmt_init();

	InAllocator alloc = { .memalloc = printed_malloc, .memrealloc = printed_realloc, .memfree = free };

	InStr s = in_str_immut_from_literal("TESSSSSSSSSSSSSSSSSSSSSSSSSSSt");
	InStrView v = {
		.length = 2,
		.start = 3,
		.str = s
	};

	InStr test = in_str_immut_from_literal("This {cstr} is {b} a {str} test {str_view}\n");
	InStr fmt = in_str_format(test, &alloc, "HELLO!", true, s, v);
	in_str_puts(fmt, stdout);
	
	in_str_free(fmt);
	in_str_free(s);
	return 0;
}
