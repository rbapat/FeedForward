#pragma once
#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <d3d9.h>
#include <d3dx9.h>
#include <thread>

#include "D3D.h"
#include "Color.h"
#include "RSSTab.h"
#include "PyComm.h"
#include "tinyxml2.h"

#define TITLE_HEIGHT 20
#define MENU_HEIGHT 400
#define TAB_WIDTH 120
#define TAB_HEIGHT 80
#define MENU_WIDTH 800
#define DX 10
#define DY 10
#define DATA_FONT_SIZE 20
#define MENU_FONT_SIZE 16

class Menu
{
public:
	Menu(LPDIRECT3DDEVICE9 pDevice, HWND hWnd);
	void render(D3DPRESENT_PARAMETERS presentParams);
	bool inRect(POINT mouse, int x, int y, int w, int h);
	void onLost();
	void onReset();
	void initializeFonts();

private:
	LPDIRECT3DDEVICE9 pDevice;
	LPD3DXFONT menuDataFont = NULL, menuTitleFont = NULL;
	POINT dragPos, dragRoot;
	HWND hWnd;
	std::vector<RSSTab *> rssLinks;
	MouseClick* mouse;
	PyComm* scraper;
	bool drag;
	int rootx, rooty, rssInd, titleWidth, tt, startInd, lastUpdate, index;

	void readRSSConfig();
};

