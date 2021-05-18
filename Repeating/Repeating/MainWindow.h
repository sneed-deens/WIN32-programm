#pragma once

class MainWindow : public BaseWindow<MainWindow>
{
	float DpiScaleX;
	template<typename T>
	float PixelsToDpi(T axis);
	inline void InitializeDpiScaling();

	HCURSOR hCursor;

	CComPtr<ID2D1Factory> pFactory;
	CComPtr<ID2D1HwndRenderTarget> pRenderTarget;
	CComPtr<ID2D1SolidColorBrush> pBrush;
	CComPtr<ID2D1SolidColorBrush> pStrokeBrush;
	HRESULT CreateGraphicsResources();
	void DiscardGraphicsResources();

	D2D1_ELLIPSE m_ellipse;
	D2D1_COLOR_F m_color = D2D1::ColorF(D2D1::ColorF::Aqua);
	std::vector<D2D1_RECT_F*> m_ArrRects;
	D2D1_RECT_F *m_pCurRect;
	D2D1_RECT_F m_rect_temp;
	BOOL HitTest(int x, int y, D2D1_RECT_F *r);
	void Resize();
	void CalculateLayout();
	void OnPaint();

	D2D1_POINT_2F m_ptMouseAfterLButtonClick;
	D2D1_POINT_2U m_ptMouseCoord; //in pixels
	WCHAR m_pszMouseCoord[21];
	PWSTR Num2Wchar(int num);
	void WriteMouseCoord(int x, int y);

#define m_bFLAG1_SHFTDRG		0x01
#define m_bFLAG2				0x02
#define m_bFLAG3				0x04
#define m_bFLAG4_DRG			0x08
#define m_bFLAG5				0x10
#define m_bFLAG6_SHFTDRG_STOP	0x20
#define m_bFLAG7				0x40
#define m_bFLAG8				0x80
	BYTE m_bFlags = 0xff;
	void OnRButtonDown(int x, int y, DWORD flags);
	void OnLButtonDown(int x, int y, DWORD flags);
	void OnMouseMove(int x, int y, DWORD flags);
	void MakeRightRect(int x, int y, D2D1_RECT_F *pRect);
	void OnLButtonUp();

public:

	MainWindow() : 
		DpiScaleX{ 1.0f },
		m_pCurRect{nullptr}, m_rect_temp{},
		m_ptMouseAfterLButtonClick{},m_ptMouseCoord{},m_pszMouseCoord{ L"x =                 " },
		hCursor {LoadCursor(NULL, IDC_CROSS)}
	{};

	PCWSTR ClassName() const { return L"My MainWindow Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

template<typename T>
inline float MainWindow::PixelsToDpi(T axis)
{
	return static_cast<float>(axis) / DpiScaleX;
}
