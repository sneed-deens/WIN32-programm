#include "stdafx.h"

inline void MainWindow::InitializeDpiScaling()
{
	float dpi = GetDpiForWindow(m_hwnd);
	DpiScaleX = dpi / 96.0f;
}

PWSTR MainWindow::Num2Wchar(int num)
{
	int len = 1;
	PWSTR str = new WCHAR[11];
	str[10] = L'\0';
	int n = 10;

	if (num == 0)
	{
		str[10 - len++] = L'0';
	}
	else if (num > 0)
	{
		while (num)
		{
			str[10 - len++] = num % n + L'0';
			num /= n;
		}
	}
	else if (num < 0)
	{
		num ^= 0xffffffff;
		num += 1;
		while (num)
		{
			str[10 - len++] = num % n + L'0';
			num /= n;
		}
		str[10 - len++] = L'-';
	}
	
	return str + (11 - len);
}

void MainWindow::CalculateLayout()
{
	if (pRenderTarget != NULL)
	{
		D2D1_SIZE_F size = pRenderTarget->GetSize();
		float x = size.width / 2;
		float y = size.height / 2;
		float radius = min(x, y);
		m_ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);

		
	}
}

void MainWindow::WriteMouseCoord(int x, int y)
{
	PWSTR txt;
	WORD len_text = wcslen(m_pszMouseCoord);
	WORD len_txt;
	WORD shift = 4;

	txt = Num2Wchar(x);
	len_txt = wcslen(txt);
	wmemcpy_s(m_pszMouseCoord + shift, len_text, txt, len_txt);
	shift += len_txt;

	WCHAR temp[] = L", y = ";
	wmemcpy_s(m_pszMouseCoord + shift, len_text, temp, 6);
	shift += 6;

	txt = Num2Wchar(y);
	len_txt = wcslen(txt);
	wmemcpy_s(m_pszMouseCoord + shift, len_text, txt, len_txt);
	shift += len_txt;

	m_pszMouseCoord[shift] = '\0';
}

HRESULT MainWindow::CreateGraphicsResources()
{
	HRESULT hr = S_OK;
	if (pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		hr = pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, D2D1::SizeU(rc.right, rc.bottom)),
			&pRenderTarget
		);

		if (SUCCEEDED(hr))
		{
			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGreen),
													  &pBrush);

			hr = pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black),
													  &pStrokeBrush);

			if (SUCCEEDED(hr))
			{
				CalculateLayout();
			}
		}
	}
	return hr;
}

void MainWindow::DiscardGraphicsResources()
{
	pRenderTarget.Release();
	pBrush.Release();
	for (auto r : m_ArrRects)
		delete r;
}

void MainWindow::OnPaint()
{
	HRESULT hr = CreateGraphicsResources();
	if (SUCCEEDED(hr))
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(m_hwnd, &ps);

		pRenderTarget->BeginDraw();

		pRenderTarget->Clear(m_color);
		pRenderTarget->FillEllipse(m_ellipse, pBrush);

		for (auto r : m_ArrRects)
		{
			pRenderTarget->DrawRectangle(r, pStrokeBrush, 1.0f);
		}

		hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{
			DiscardGraphicsResources();
		}
		D2D1_SIZE_U size = pRenderTarget->GetPixelSize();
		TextOutW(hdc, 0, size.height - 20, m_pszMouseCoord, wcslen(m_pszMouseCoord));

		EndPaint(m_hwnd, &ps);
	}
}

BOOL MainWindow::HitTest(int x, int y, D2D1_RECT_F  *r)
{
	if (x >= r->left && x <= r->right &&
		y <= r->bottom && y >= r->top)
		return 1;
	else
		return 0;
}

void MainWindow::Resize()
{

	if (pRenderTarget != NULL)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		pRenderTarget->Resize(D2D1::SizeU(rc.right, rc.bottom));
		CalculateLayout();
		InvalidateRect(m_hwnd, NULL, FALSE);
	}
}

void MainWindow::OnRButtonDown(int x, int y, DWORD flags)
{
	x = PixelsToDpi(x);
	y = PixelsToDpi(y);
	std::vector<D2D1_RECT_F*>::reverse_iterator it = m_ArrRects.rbegin();

	for (; it != m_ArrRects.rend(); ++it)
	{
		if (HitTest(x, y, *it))
		{
			delete *it;
			m_ArrRects.erase(it.base() - 1);
			InvalidateRect(m_hwnd, NULL, FALSE);
			break;
		}
	}
}

void MainWindow::OnLButtonDown(int x, int y, DWORD flags)
{
	SetCapture(m_hwnd);
	x = PixelsToDpi(x);
	y = PixelsToDpi(y);

	m_ptMouseAfterLButtonClick.x = float(x);
	m_ptMouseAfterLButtonClick.y = float(y);
}


void MainWindow::OnMouseMove(int x, int y, DWORD flags)
{
	x = PixelsToDpi(x);
	y = PixelsToDpi(y);
	WriteMouseCoord(x, y);
	InvalidateRect(m_hwnd, NULL, FALSE);

	if (flags & MK_LBUTTON)
	{
		if (flags & MK_SHIFT && m_bFlags & m_bFLAG1_SHFTDRG)
		{
			m_bFlags &= ~m_bFLAG4_DRG;
			m_bFlags &= ~m_bFLAG6_SHFTDRG_STOP;

			if (m_bFlags & m_bFLAG2)
			{
				m_bFlags &= ~m_bFLAG2;

				m_bFlags &= ~m_bFLAG3;
				for (auto r : m_ArrRects)
				{
					if (HitTest(x, y, r))
					{
						m_pCurRect = r;
						m_bFlags |= m_bFLAG3;
						break;
					}
				}
			}

			if (m_bFlags & m_bFLAG3)
			{
				m_pCurRect->left = x - (m_ptMouseAfterLButtonClick.x - m_pCurRect->left);
				m_pCurRect->right = x + (m_pCurRect->right - m_ptMouseAfterLButtonClick.x);
				m_pCurRect->top = y - (m_ptMouseAfterLButtonClick.y - m_pCurRect->top);
				m_pCurRect->bottom = y + (m_pCurRect->bottom - m_ptMouseAfterLButtonClick.y);
				m_ptMouseAfterLButtonClick.x = float(x);
				m_ptMouseAfterLButtonClick.y = float(y);
			}
		}
		else if(m_bFlags & m_bFLAG4_DRG)
		{
			m_bFlags &= ~m_bFLAG1_SHFTDRG;
			
			if (m_bFlags & m_bFLAG5)
			{
				m_bFlags &= ~m_bFLAG5;
				D2D1_RECT_F *pRect = new D2D1_RECT_F;
				MakeRightRect(x, y, pRect);
				m_ArrRects.push_back(pRect);
				m_pCurRect = pRect;
			}
			
			MakeRightRect(x, y, m_pCurRect);
		}
	}
}

void  MainWindow::MakeRightRect(int x, int y, D2D1_RECT_F *pRect)
{
	if (x >= m_ptMouseAfterLButtonClick.x &&
				y >= m_ptMouseAfterLButtonClick.y)
	{
		pRect->left = m_ptMouseAfterLButtonClick.x;
		pRect->top = m_ptMouseAfterLButtonClick.y;
		pRect->right = float(x);
		pRect->bottom = float(y);
	}
	else if (x >= m_ptMouseAfterLButtonClick.x &&
			 y < m_ptMouseAfterLButtonClick.y)
	{
		pRect->left = m_ptMouseAfterLButtonClick.x;
		pRect->top = float(y);
		pRect->right = float(x);
		pRect->bottom = m_ptMouseAfterLButtonClick.y;
	}
	else if (x < m_ptMouseAfterLButtonClick.x &&
			 y >= m_ptMouseAfterLButtonClick.y)
	{
		pRect->left = float(x);
		pRect->top = m_ptMouseAfterLButtonClick.y;
		pRect->right = m_ptMouseAfterLButtonClick.x;
		pRect->bottom = float(y);
	}
	else
	{
		pRect->left = float(x);
		pRect->top = float(y);
		pRect->right = m_ptMouseAfterLButtonClick.x;
		pRect->bottom = m_ptMouseAfterLButtonClick.y;
	}
}

void MainWindow::OnLButtonUp()
{
	m_bFlags |= m_bFLAG1_SHFTDRG;
	m_bFlags |= m_bFLAG2;
	m_bFlags |= m_bFLAG3;
	m_bFlags |= m_bFLAG4_DRG;
	m_bFlags |= m_bFLAG5;
	m_bFlags |= m_bFLAG6_SHFTDRG_STOP;
	ReleaseCapture();
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
		OnRButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_MOUSEMOVE:
		OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
		return 0;

	case WM_LBUTTONUP:
		OnLButtonUp();
		return 0;

	case WM_KEYUP:
		if (wParam & VK_SHIFT)
		{
			hCursor = LoadCursorW(NULL, IDC_CROSS);
			SetCursor(hCursor);
		}
		return 0;

	case WM_KEYDOWN:
		if (wParam & VK_SHIFT)
		{
			if (m_bFlags & m_bFLAG1_SHFTDRG)
			{
				hCursor = LoadCursorW(NULL, IDC_HAND);
				SetCursor(hCursor);
			}

			if (m_bFlags & m_bFLAG1_SHFTDRG && !(m_bFlags & m_bFLAG6_SHFTDRG_STOP))
			{
				POINT p;
				if (!GetCursorPos(&p)) return 0;
				if (!ScreenToClient(m_hwnd, &p)) return 0;
				OnMouseMove(p.x, p.y, MK_LBUTTON | MK_SHIFT);
			}
		}
		return 0;

	case WM_LBUTTONDBLCLK:
		m_color = D2D1::ColorF(rand() % 256 / 255.0f,
							   rand() % 256 / 255.0f,
							   rand() % 256 / 255.0f);
		InvalidateRect(m_hwnd, NULL, FALSE);
		return 0;

	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(hCursor);
			return TRUE;
		}
		return 0;

	case WM_SIZE:
		Resize();
		return 0;
		
	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
		{
			return -1;
		}
		InitializeDpiScaling();
		return 0;

	case WM_DESTROY:
		DiscardGraphicsResources();
		pFactory.Release();
		PostQuitMessage(0);
		return 0;

	}
	return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}