#include "stdafx.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
					_In_opt_ HINSTANCE hPrevInstance,
					_In_ PWSTR lpCmdLine,
					_In_ INT nCmdShow)
{
	MainWindow win;

	if (!win.Create(L"I learn WIN32", WS_OVERLAPPEDWINDOW))
	{
		return 0;
	}

	ShowWindow(win.Window(), nCmdShow);

	MSG msg = {};
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		//TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}