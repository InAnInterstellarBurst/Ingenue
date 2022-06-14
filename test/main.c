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
	InStr test = in_str_immut_from_literal("This {b} is {b} a {b} test {b}");
	InStr fmt = in_str_format(test, true, true, false, true);
	in_str_puts(fmt, stdout);
	puts("");
	in_str_free(fmt);
	return 0;
}
