#include "ConsoleWinImpl.h"
#include <map>

bool ConsoleWinImpl::WND_CLASS_REGISTERD = false;
int ConsoleWinImpl::CONSOLE_WINDOW_LEFT = 0;
int ConsoleWinImpl::CONSOLE_WINDOW_TOP = 0;

ConsoleWinImpl::ConsoleWinImpl()
{
	m_hWnd = nullptr;
	m_bDestroyed = true;
}

ConsoleWinImpl::~ConsoleWinImpl()
{
	if (!m_bDestroyed) {
		writeAndResetBuffer();
		DestroyWindow(m_hWnd);
	}

	m_LogOut.close();
}

int ConsoleWinImpl::getWindowCreateX()
{
	int create_x = CONSOLE_WINDOW_LEFT;

	// update next x position
	CONSOLE_WINDOW_LEFT += 20;
	if (CONSOLE_WINDOW_LEFT > GetSystemMetrics(SM_CXSCREEN))
		CONSOLE_WINDOW_LEFT = 0;
	
	return create_x;
}

int ConsoleWinImpl::getWindowCreateY()
{
	int create_y = CONSOLE_WINDOW_TOP;

	// update next x position
	CONSOLE_WINDOW_TOP += 20;
	if (CONSOLE_WINDOW_TOP > GetSystemMetrics(SM_CYSCREEN))
		CONSOLE_WINDOW_TOP = 0;

	return create_y;
}

bool ConsoleWinImpl::init(const std::string& title)
{
	m_LogOut.open(title + ".txt");

	if (!WND_CLASS_REGISTERD) {

		WNDCLASSEX wc = { sizeof(WNDCLASSEX),CS_HREDRAW | CS_VREDRAW,(WNDPROC)windowProc,0,0,GetModuleHandleA(0),LoadIconA(nullptr, IDI_APPLICATION),0,(HBRUSH)GetStockObject(GRAY_BRUSH),0,"Debug",0 };
		if (!RegisterClassEx(&wc)) {
			MessageBox(0, "Registration of Debug Console Failed!", "Error!", 0);
			return false;
		}
		WND_CLASS_REGISTERD = true;
	}

	int win_x = getWindowCreateX();
	int win_y = getWindowCreateY();

	m_hWnd = CreateWindowExA(0,"Debug", title.c_str(), WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU | WS_VSCROLL | WS_THICKFRAME, win_x, win_y, CONSOLE_WINDOW_WIDTH, CONSOLE_WINDOW_HEIGHT, 0, 0, GetModuleHandle(0), this);

	if (!m_hWnd) {
		MessageBox(m_hWnd, "CreateWindowEx Failed!", "Error!", 0);
		return false;
	}

	m_bDestroyed = false;
	UpdateWindow(m_hWnd);
	return true;
}

void ConsoleWinImpl::flush()
{
	if (!m_bDestroyed) {
		m_bFlushed = true;
		SendMessage(m_hWnd, UM_SETSCROLL,0, 0);
	}
}

void ConsoleWinImpl::writeAndResetBuffer()
{
	for (auto& line : m_vBuffer) {
		m_LogOut << line << std::endl;
	}

	clearBuffer();
}

LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static std::map<HWND, ConsoleWinImpl*> ConsoleMap;

	static int cxClient, cyClient;
	static int cxChar, cyChar, cxCaps, cyPage;

	int iVertPos;
	TEXTMETRIC tm;
	SCROLLINFO si;

	RECT rect;
	GetClientRect(hWnd, &rect);
	cxClient = rect.right;
	cyClient = rect.bottom;

	auto pConsole = ConsoleMap[hWnd];

	switch (msg)
	{
	case WM_CREATE:
	{
		auto pCT = LPCREATESTRUCT(lParam);
		pConsole = (ConsoleWinImpl*)pCT->lpCreateParams;
		ConsoleMap[hWnd] = pConsole;

		HDC hdc = GetDC(hWnd);
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar * 0.5;
		cyChar = tm.tmHeight + tm.tmExternalLeading;

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = 0;
		si.nPage = cyClient / cyChar;

		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		ReleaseDC(hWnd, hdc);
		break;
	}

	case WM_KEYUP: 
	{
		if (wParam == VK_ESCAPE)
		{
			SendMessage(hWnd, WM_DESTROY, 0, 0);
		}
		break;
	}
	
	case WM_SIZE:
	{
		if (pConsole == nullptr)break;
		int nPage = (int)cyClient / cyChar;

		si.cbSize = sizeof(si);
		si.nMin = 0;
		si.nMax = pConsole->m_vBuffer.size();
		si.nPage = nPage;
		si.fMask = SIF_RANGE | SIF_PAGE;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		InvalidateRect(hWnd, 0, TRUE);
		break;
	}

	case WM_MOUSEWHEEL:
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		iVertPos = si.nPos;

		int zDelta = (short)HIWORD(wParam);
		zDelta *= 0.025;

		si.nPos -= zDelta;

		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hWnd, SB_VERT, &si);

		if (si.nPos != iVertPos) {
			UpdateWindow(hWnd);
			InvalidateRect(hWnd, 0, TRUE);
		}
		break;
	}

	case WM_VSCROLL:
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		iVertPos = si.nPos;

		switch (LOWORD(wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;

		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

		case SB_LINEUP:
			si.nPos -= 1;
			break;

		case SB_LINEDOWN:
			si.nPos += 1;
			break;

		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;

		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hWnd, SB_VERT, &si);

		if (si.nPos != iVertPos) {
			UpdateWindow(hWnd);
			InvalidateRect(hWnd, 0, TRUE);
		}

		break;
	}

	case ConsoleWinImpl::UM_SETSCROLL:
	{
		if (pConsole == nullptr)break;

		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hWnd, SB_VERT, &si);

		si.nMin = 0;
		si.nMax = pConsole->m_vBuffer.size();
		si.nPos = si.nMax;
		si.fMask = SIF_RANGE | SIF_POS;
		SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

		ScrollWindow(hWnd, 0, cyChar * si.nPos, 0, 0);
		InvalidateRect(hWnd, 0, TRUE);
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		SetBkMode(ps.hdc, TRANSPARENT);
		SetTextColor(ps.hdc, pConsole->getTextColor());

		if (pConsole->m_vBuffer.size() > 0)
		{
			si.cbSize = sizeof(si);
			si.fMask = SIF_POS;
			GetScrollInfo(hWnd, SB_VERT, &si);

			iVertPos = si.nPos;

			int PageSize = (int)(cyClient / cyChar) - 1;
			int textLines = pConsole->m_vBuffer.size();

			int StartIndex = si.nPos;
			if (StartIndex > textLines) {
				StartIndex = textLines;
			}

			int EndIndex = StartIndex + PageSize + 1;

			auto iter_begin = pConsole->m_vBuffer.begin() + StartIndex;
			
			int line = 0;

			for (iter_begin; StartIndex != EndIndex && iter_begin != pConsole->m_vBuffer.end(); ++iter_begin,++StartIndex)
			{
				TextOut(ps.hdc, 0, cyChar* line++, iter_begin->c_str(), iter_begin->size());
			}
		}
		EndPaint(hWnd, &ps);
		break;
	}

	case WM_DESTROY:
	{
		pConsole->m_bDestroyed = true;
		DestroyWindow(hWnd);
		break;
	}

	default:break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
