/**
* Copyright (c) 2022 Connor Mellon
*
* This Source Code Form is subject to the terms of the Mozilla Public
* License, v. 2.0. If a copy of the MPL was not distributed with this
* file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "pch.h"
#include <Windows.h>
#include <crtdbg.h>

void open_console(void)
{
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	HANDLE stdhdl = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consmode;
	GetConsoleMode(stdhdl, &consmode);
	SetConsoleMode(stdhdl, consmode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#if IN_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	open_console();
#endif

	gInDefaultMallocator = &(InAllocator) {
		.memalloc = malloc,
		.memrealloc = realloc,
		.memfree = free,
	};

	int r = in_main(__argc, __argv);
#if IN_DEBUG
	system("Pause");
#endif
	return r;
}