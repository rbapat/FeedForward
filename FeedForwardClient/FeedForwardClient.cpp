#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include <d3d9.h>
#include <d3dx9.h>
#include "Menu.h"

HWND hWnd;
Menu* menu;
bool fore = true;
HHOOK _hook;
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;

LRESULT __stdcall HookCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0 && wParam == WM_KEYDOWN)
	{
		KBDLLHOOKSTRUCT kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
		if (kbdStruct.vkCode == VK_HOME)
		{
			if (fore)
			{
				SetWindowPos(hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
				fore = false;
			}
			else
			{
				SetForegroundWindow(hWnd);
				fore = true;
			}
		}

		else if (kbdStruct.vkCode == VK_END && fore)
		{
			exit(1);
		}
	}


	return CallNextHookEx(_hook, nCode, wParam, lParam);
}

D3DPRESENT_PARAMETERS initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;

	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);
	return d3dpp;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "FeedForward";

	RegisterClassEx(&wc);

	hWnd = CreateWindowExA(NULL,
		"FeedForward",
		"FeedForward RSS Client",
		WS_POPUP,
		0, 0,
		120, 400,
		NULL,
		NULL,
		hInstance,
		NULL);
	SetWindowLong(hWnd, GWL_STYLE, 0);
	ShowWindow(hWnd, nCmdShow);

	auto presentParams = initD3D(hWnd);
	_hook = SetWindowsHookEx(WH_KEYBOARD_LL, HookCallback, NULL, 0);

	MSG msg;

	menu = new Menu(d3ddev, hWnd);

	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
			break;

		if (fore)
		{
			d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_DARKISHGREY, 1.0f, 0);

			d3ddev->BeginScene();
			menu->render(presentParams);
			d3ddev->EndScene();

			d3ddev->Present(NULL, NULL, NULL, NULL);
		}

	}

	d3ddev->Release();
	d3d->Release();
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}