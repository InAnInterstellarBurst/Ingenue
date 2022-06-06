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
	InStr test = in_str_immut_from_literal("{Hello} this is {a} {rigerous t}{est");
	in_str_format(test);
	return 0;
}
