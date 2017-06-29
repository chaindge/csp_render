#ifndef CONSOLEWINIMPL_H
#define CONSOLEWINIMPL_H
#include <fstream>
#include <windows.h>

#include "IConsole.h"

class ConsoleWinImpl : public IConsole
{
public:
	ConsoleWinImpl();
	virtual ~ConsoleWinImpl();

	virtual bool init(const std::string& title );

	virtual void flush();

	virtual void writeAndResetBuffer();

protected:
	int getWindowCreateX();
	int getWindowCreateY();
protected:
	static const int UM_SETSCROLL = WM_USER + 32;
	static const int CONSOLE_WINDOW_WIDTH = 400;
	static const int CONSOLE_WINDOW_HEIGHT = 400;
	static bool WND_CLASS_REGISTERD;
	static int CONSOLE_WINDOW_LEFT;
	static int CONSOLE_WINDOW_TOP;

	HWND m_hWnd;

	std::ofstream m_LogOut;

	friend LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};
#endif