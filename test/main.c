/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include <ingenue/ingenue.h>

int in_main(int argc, char** argv)
{
	InStr s = in_str_from_literal("Hello");
	InStr s1 = in_str_alloc(10);
	s1 = in_str_copy(s1, s, 0);
	
	InStrView v = { 0 };
	v.length = 3;
	v.start = 2;
	v.str = &s;
	s1 = in_str_copy_from_view(s1, v);

	in_str_puts(s1, stdout);
	puts("");
	in_str_free(s1);
	return 0;
}
