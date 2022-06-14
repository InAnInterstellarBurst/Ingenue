/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pch.h"

int main(int argc, char** argv)
{
	gInDefaultMallocator = &(InAllocator){
		.memalloc = malloc,
		.memrealloc = realloc,
		.memfree = free,
	};

	return in_main(argc, argv);
}
