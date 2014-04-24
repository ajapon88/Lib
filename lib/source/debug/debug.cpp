#include "stdafx.h"
#include "lib/debug/debug.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <io.h>
#include <fcntl.h>

namespace lib {
namespace debug {
	
static int s_consoleHandle = -1;
static bool s_showConsole = false;

void showConsole()
{
	if (!s_showConsole) {
		AllocConsole();
		s_consoleHandle = _open_osfhandle((intptr_t)::GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		freopen ( "CONOUT$", "w", stdout );
		freopen ( "CONIN$", "r", stdin );
		s_showConsole = true;
	}
}

void hideConsole()
{
	if (s_showConsole) {
		FreeConsole();
		_close(s_consoleHandle);
		s_consoleHandle = -1;
		s_showConsole = false;
	}
}

} // namespace debug
} // namespace lib
