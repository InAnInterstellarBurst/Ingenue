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
	InStr s = in_str_immut_from_literal(" HI HI HE LLO !!! ");
	in_str_puts(s, stdout);
	puts("");

	InStrRangedView v = in_str_subview_between(s, ' ');
	while(!in_str_isnull(v.snipped.str)) {
		InStr vs = in_str_alloc_from_view(v.snipped);
		in_str_puts(vs, stdout);
		puts("");
		v = in_str_subview_between_v(v.full, ' ');
		in_str_free(vs);
	}

	return 0;
}
