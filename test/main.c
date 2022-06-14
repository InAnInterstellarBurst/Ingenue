/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <ingenue/ingenue.h>
#include <sys/time.h>
#include <stdlib.h>

uint32_t avg_custom()
{
	InStr s = in_str_immut_from_literal("TESSSSSSSSSSSSSSSSSSSSSSSSSSSt");
	uint32_t sumdiff = 0;
	struct timeval start, stop;
	for(int i = 0; i < 20; i++) {
		gettimeofday(&start, NULL);
		InStr e = in_str_format(in_str_immut_from_literal("{i32} {i32} {u32}\n{i16} {i16} {u16}\n{i8} {i8} {u8}\n{ptr}\n"), NULL, 
			INT32_MAX, -INT32_MAX, UINT32_MAX, INT16_MAX, -INT16_MAX, UINT16_MAX, INT8_MAX, -INT8_MAX, UINT8_MAX, &s);
		gettimeofday(&stop, NULL);
		sumdiff += (uint32_t)(stop.tv_usec - start.tv_usec);
		in_str_free(e);
	}

	return sumdiff / 20;
}

uint32_t avg_printf()
{
	InStr s = in_str_immut_from_literal("TESSSSSSSSSSSSSSSSSSSSSSSSSSSt");
	uint32_t sumdiff = 0;
	struct timeval start, stop;
	for(int i = 0; i < 20; i++) {
		gettimeofday(&start, NULL);
		char* newbuf = malloc((size_t)5000);
		snprintf(newbuf, (size_t)5000, "%d %d %d\n%d %d %d\n%d %d %d\n%p\n", 
			INT32_MAX, -INT32_MAX, UINT32_MAX, INT16_MAX, -INT16_MAX, UINT16_MAX, INT8_MAX, -INT8_MAX, UINT8_MAX, (void*)&s);
		free(newbuf);
		gettimeofday(&stop, NULL);
		sumdiff += (uint32_t)(stop.tv_usec - start.tv_usec);
	}

	return sumdiff / 20;
}

int in_main(int argc, char** argv)
{
	in_fmt_init();
	in_fmt_print(stdout, NULL, in_str_immut_from_literal("We took {u32} microseconds\nPrintf took {u32} microseconds\n"), avg_custom(), avg_printf());
	return 0;
}
