#include "stdafx.h"
#include "ContentWindow.h"


#include "stdafx.h"
#include "resource.h"
#include "SolidWindow.h"
#include "Debug.h"

LRESULT ContentWindow::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		static UINT32 color = 0;
		const UINT32 mask_r = 0xff0000;
		const UINT32 mask_g = 0x00ff00;
		const UINT32 mask_b = 0x0000ff;
		color += 0x010204;

		HDC hdc = BeginPaint(_hwnd, &ps);
		//FillRect(hdc, &ps.rcPaint, CreateSolidBrush(RGB((color & mask_r) >> 16, (color & mask_g) >> 8, color & mask_b)));
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(_hwnd, &ps);
		return 0;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(_hwnd);
			break;
		default:
			return DefWindowProc(_hwnd, uMsg, wParam, lParam);
		}
		break;
	}
	default:
		return DefWindowProc(_hwnd, uMsg, wParam, lParam);
	}
}

void ContentWindow::updateClass(WNDCLASSEXW & wcex)
{
	return;

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ZRECOVERY));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ZRECOVERY);
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
}
