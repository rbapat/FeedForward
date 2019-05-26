#pragma once
#pragma comment(lib, "xmllite.lib")
#pragma comment(lib, "urlmon.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include <d3dx9.h>
#include <d3d9.h>
#include <string>
#include <vector>
#include <xmllite.h>

#include "D3D.h"
#include "Color.h"
#include "PyComm.h"

#define CHKHR(stmt)             do { hr = (stmt); if (FAILED(hr)) goto CleanUp; } while(0)
#define SAFE_RELEASE(I)         do { if (I){ I->Release(); } I = NULL; } while(0)
#define TEXT_DY 10
#define FADE_SPEED 10

struct MouseClick
{
	POINT cursor;
	bool curClicked;
	bool prevClicked;
	bool rClick;
	bool prevRClick;
};

struct RSSInfo
{
	char articleTitle[150];
	char articleLink[150];
};

class RSSTab
{
public:
	RSSTab(std::string tabName, std::string tabLink);
	std::string getTabName();
	std::string getTabLink();
	bool render(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, MouseClick* mouse, int basex, int basey, int rootx, int rooty, int height, int menuWidth);
	void renderTooltip(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, MouseClick* mouse, PyComm *scraper, int basex, int basey, int rootx, int rooty, int height, int menuWidth);
	void disable();
private:
	std::string tabName;
	std::string tabLink;
	std::vector<RSSInfo*> vecRSS;
	int ttIndex;
	int page;
	bool TT, exiting;
	bool tooltip, sent;
	int alpha;
	char* summaryData;

	std::vector<RSSInfo*> ParseRSS(const char* rssLink);
	RECT drawTruncatedString(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, D3DCOLOR color, int x, int y, int w, char *string);
	int renderPageInfo(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, MouseClick *mouse, int max, int basex, int basey, int rootx, int rooty, int height, int menuWidth);
	bool inRect(POINT mouse, int x, int y, int w, int h);
};
	