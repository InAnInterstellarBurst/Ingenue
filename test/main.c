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
	in_fmt_init();

	InStr s = in_str_alloc(50, NULL);
	s = in_str_copy_literal(s, "Tesssssssssssssssssssst");
	InStrView v = {
		.length = 2,
		.start = 3,
		.str = s
	};

	InStr test = in_str_immut_from_literal("This {cstr} is {b} a {str} test {str_view}\n");
	InStr fmt = in_str_format(test, "HELLO!", true, s, v);
	in_str_puts(fmt, stdout);
	
	in_str_free(fmt);
	in_str_free(s);
	return 0;
}
