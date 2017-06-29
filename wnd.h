#ifndef WND_H
#define WND_H

#include <windows.h>
#include <vector>
#include <list>
#include <functional>


struct window
{
	int w, h, bits;
	void* hWnd;
	void* back_buffer;
	void* fore_buffer;
	void* hBitmap;
	void* hBitmap_old;
	void* videoPtr;
	void* framebuffer;
};

using msg_handler = std::function<void(WPARAM wParam, LPARAM lParam)>;

struct msg_map
{
	window* wnd;
	UINT msg;
	msg_handler handler;
};


static std::list<msg_map> g_map;

void update_window(window* wnd)
{
	if (wnd->framebuffer)
		memcpy(wnd->videoPtr, wnd->framebuffer, wnd->w * wnd->h * wnd->bits);

	BitBlt((HDC)wnd->fore_buffer, 0, 0, wnd->w, wnd->h, (HDC)wnd->back_buffer, 0, 0, SRCCOPY);
}

void close_window(window* wnd)
{
	ReleaseDC((HWND)wnd->hWnd, (HDC)wnd->fore_buffer);
	SelectObject((HDC)wnd->back_buffer, wnd->hBitmap_old);

	DeleteObject(wnd->hBitmap);
	DeleteDC((HDC)wnd->back_buffer);

	CloseWindow((HWND)wnd->hWnd);
	
	delete wnd;
}

void register_msg(window* wnd,UINT msg, msg_handler _handler)
{
	g_map.push_back({wnd, msg,_handler });
}

static LRESULT on_events(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CLOSE)
	{
		PostQuitMessage(0);
		return TRUE;
	}

	for (auto& msg_item : g_map)
	{
		if (msg_item.wnd->hWnd == hWnd && msg_item.msg == msg) {
			msg_item.handler(wParam, lParam);
			return TRUE;
		}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

window* create_window(int w, int h, const char *title)
{
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)on_events, 0, 0, 0, NULL, NULL, NULL, NULL, "SCREEN3.1415926" };
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,	w * h * 4, 0, 0, 0, 0 } };

	wc.hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIconA(nullptr, IDI_APPLICATION);

	if (!RegisterClass(&wc)) return 0;

	auto hWnd = CreateWindowExA(0, "SCREEN3.1415926", title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, w, h, NULL, NULL, wc.hInstance, NULL);

	if (hWnd == NULL) return 0;

	HDC fore_dc = GetDC(hWnd);
	auto buffer_dc = CreateCompatibleDC(fore_dc);


	// 调整画布大小为实际w*h 的大小
	RECT rect = { 0, 0, w, h };
	AdjustWindowRect(&rect, GetWindowLong(hWnd, GWL_STYLE), 0);
	int wx = rect.right - rect.left;
	int wy = rect.bottom - rect.top;
	int sx = (GetSystemMetrics(SM_CXSCREEN) - wx) * 0.5;
	int sy = (GetSystemMetrics(SM_CYSCREEN) - wy) * 0.5;
	if (sy < 0) sy = 0;
	SetWindowPos(hWnd, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(hWnd);
	//ShowWindow(hWnd, SW_NORMAL);
	
	void* ptr = nullptr;
	auto hBitmap = CreateDIBSection(buffer_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	auto old_bitmap = (HBITMAP)SelectObject(buffer_dc, hBitmap);

	auto pRet = new window;
	memset(pRet, 0, sizeof(window));

	pRet->bits = 4;
	pRet->w = w;
	pRet->h = h;
	pRet->hWnd = hWnd;
	pRet->fore_buffer = fore_dc;
	pRet->back_buffer = buffer_dc;
	
	pRet->hBitmap = hBitmap;
	pRet->hBitmap_old = old_bitmap;
	pRet->videoPtr = ptr;

	return pRet;
}

#endif