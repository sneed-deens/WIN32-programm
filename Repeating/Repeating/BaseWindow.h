#pragma once

template<class DERIVED_TYPE>
class BaseWindow
{
public:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		DERIVED_TYPE *pThis = NULL;

		if (uMsg == WM_NCCREATE)
		{
			CREATESTRUCT * pCreate = (CREATESTRUCT*)(lParam);
			pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

			pThis->m_hwnd = hwnd;
		}
		else
		{
			pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}
		if (pThis)
		{
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else
		{
			return DefWindowProcW(hwnd, uMsg, wParam, lParam);
		}
	}

	BaseWindow() : m_hwnd(NULL) {};

	BOOL Create(
		PCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		HWND hWndParent = NULL,
		HMENU hMenu = NULL
	)
	{
		WNDCLASSEXW wcex = {};

		//wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_DBLCLKS;
		wcex.lpfnWndProc = DERIVED_TYPE::WindowProc;
		wcex.hInstance = GetModuleHandleW(NULL);
		wcex.lpszClassName = ClassName();

		RegisterClassExW(&wcex);

		m_hwnd = CreateWindowExW(
			dwExStyle, ClassName(), lpWindowName, dwStyle,
			x, y, nWidth, nHeight, hWndParent, hMenu, GetModuleHandleW(NULL), this
		);

		return m_hwnd ? TRUE : FALSE;
	}

	HWND Window() const { return m_hwnd; }

protected:

	virtual PCWSTR ClassName() const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_hwnd;
};