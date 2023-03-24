
// https://en.wikipedia.org/wiki/ANSI_escape_code
// https://espterm.github.io/docs/VT100%20escape%20codes.html

#define CONSOLE_CPP

#include <iostream>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include "Console.h"

void Console::Start()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD consoleMode;
	GetConsoleMode(handle, &consoleMode);
	consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	consoleMode |= DISABLE_NEWLINE_AUTO_RETURN;
	SetConsoleMode(handle, consoleMode);
}

void Console::Stop()
{
//	std::cout << "\x1b[39;49m";
	std::cout << "\x1b[0m";
}