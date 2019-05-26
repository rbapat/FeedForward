#include "Menu.h"

Menu::Menu(LPDIRECT3DDEVICE9 pDevice, HWND hWnd) 
{
	this->pDevice = pDevice;
	this->hWnd = hWnd;
	mouse = new MouseClick();
	dragPos = { 0, 0 };
	dragRoot = { 0, 0 };
	rootx = 0;
	startInd = 0;
	titleWidth = TAB_WIDTH;
	rooty = 0;
	tt = -1;
	rssInd = -1;
	lastUpdate = 0;


	
	drag = false;


	initializeFonts();
	scraper = new PyComm(5555);
}

void Menu::readRSSConfig()
{
	if (GetTickCount() - lastUpdate < 600000)
		return;

	rssLinks.clear();

	tinyxml2::XMLDocument doc;
	doc.LoadFile("C:\\Users\\Rohan\\Desktop\\RSSFeeds.xml");

	auto element = doc.FirstChildElement("RSS");
	do
	{
		rssLinks.push_back(new RSSTab(element->FirstChildElement("title")->GetText(), element->FirstChildElement("link")->GetText()));
	} while ((element = element->NextSiblingElement("RSS")) && element);

	lastUpdate = GetTickCount();
}

void Menu::render(D3DPRESENT_PARAMETERS presentParams)
{
	readRSSConfig();
	GetCursorPos(&mouse->cursor);
	
	mouse->prevClicked = mouse->curClicked;
	mouse->prevRClick = mouse->rClick;
	mouse->curClicked = GetAsyncKeyState(VK_LBUTTON);
	mouse->rClick = GetAsyncKeyState(VK_RBUTTON);

	if ((drag && mouse->curClicked) || (!mouse->prevClicked && mouse->curClicked && inRect(mouse->cursor, rootx, rooty, titleWidth, TITLE_HEIGHT)))
	{
		if (!drag)
		{
			dragPos = mouse->cursor;
			dragRoot = { rootx, rooty };

		}

		rootx = dragRoot.x + (mouse->cursor.x - dragPos.x);
		rooty = dragRoot.y + (mouse->cursor.y - dragPos.y);

		SetWindowPos(hWnd, (HWND)0, rootx, rooty, TAB_WIDTH, MENU_HEIGHT, SWP_NOSIZE);
		drag = true;
	}
	else
	{
		drag = false;
	}

	int renderX = 0, renderY = 0;

	D3D::DrawRectangle(pDevice, 0, 0, titleWidth, TITLE_HEIGHT, 0, D3DCOLOR_DARKERGREY);
	renderY += TITLE_HEIGHT;
	for (int ind = startInd; ind < 4; ind++, renderY += TAB_HEIGHT) {
		if (startInd > 0)
		{
			auto dx = MENU_HEIGHT - (TAB_HEIGHT * 4);
			renderY += dx / 2;
		}
		D3DXVECTOR2 loc = D3D::CenterString(pDevice, menuTitleFont, renderX, renderY, TAB_WIDTH, TAB_HEIGHT, rssLinks.at(ind)->getTabName().c_str());
		bool cursorIn = inRect(mouse->cursor, rootx + renderX, rooty + renderY, TAB_WIDTH, TAB_HEIGHT);
		D3DCOLOR color = cursorIn ? D3DCOLOR_DARKGREY : D3DCOLOR_DARKISHGREY;

		if ((cursorIn && mouse->curClicked && !mouse->prevClicked))
		{
			if (rssInd == -1)
			{
				presentParams.BackBufferWidth = TAB_WIDTH + MENU_WIDTH;
				presentParams.BackBufferHeight = MENU_HEIGHT + TITLE_HEIGHT;
				

				onLost();
				pDevice->Reset(&presentParams);
				onReset();

				MoveWindow(hWnd, rootx, rooty, TAB_WIDTH + MENU_WIDTH, MENU_HEIGHT + TITLE_HEIGHT, false);
				titleWidth = TAB_WIDTH + MENU_WIDTH;
				initializeFonts();
				rssInd = ind;
			} else if(rssInd == ind) {
				presentParams.BackBufferWidth = TAB_WIDTH;
				presentParams.BackBufferHeight = MENU_HEIGHT + TITLE_HEIGHT;


				onLost();
				pDevice->Reset(&presentParams);
				onReset();

				MoveWindow(hWnd, rootx, rooty, TAB_WIDTH, MENU_HEIGHT + TITLE_HEIGHT, false);
				titleWidth = TAB_WIDTH;
				initializeFonts();
				rssInd = -1;
			} else {
				rssInd = ind;
			}
			
		}

		D3D::DrawRectangle(pDevice, renderX, renderY, TAB_WIDTH, TAB_HEIGHT, 0, rssInd == ind ? D3DCOLOR_DARKERGREY : color);

		if (loc.x == -1) {
			D3D::DrawSplitString(pDevice, menuTitleFont, renderX, renderY, TAB_WIDTH, TAB_HEIGHT, cursorIn || rssInd == ind ? D3DCOLOR_WHITE : D3DCOLOR_GREY, rssLinks.at(ind)->getTabName().c_str());
		}
		else {
			D3D::DrawString(pDevice, menuTitleFont, loc.x, loc.y, cursorIn || rssInd == ind ? D3DCOLOR_WHITE : D3DCOLOR_GREY,  rssLinks.at(ind)->getTabName().c_str());
		}

		if (rssInd == ind)
		{
			if (rssLinks.at(ind)->render(pDevice, menuDataFont, mouse, rootx, rooty, renderX + TAB_WIDTH + DX, TITLE_HEIGHT + DY, MENU_HEIGHT, MENU_WIDTH))
				tt = ind;
			else
				tt = -1;
		}
			
	}

	if (tt > -1)
		rssLinks.at(tt)->renderTooltip(pDevice, menuDataFont, mouse, scraper, rootx, rooty, TAB_WIDTH + DX, TITLE_HEIGHT + DY, MENU_HEIGHT, MENU_WIDTH);

	
}

bool Menu::inRect(POINT mouse, int x, int y, int w, int h)
{
	return (mouse.x > x && mouse.x < x + w && mouse.y > y && mouse.y < y + h);
}

void Menu::initializeFonts() {
	
	D3DXCreateFont(pDevice, DATA_FONT_SIZE, 0, 300, 1, 0, 1, 0, 4, 0 | (0 << 4), "Zekton", &menuDataFont);
	D3DXCreateFont(pDevice, MENU_FONT_SIZE, 0, 300, 1, 0, 1, 0, 4, 0 | (0 << 4), "Zekton", &menuTitleFont);
}

void Menu::onLost()
{
	menuDataFont->OnLostDevice();
	menuTitleFont->OnLostDevice();
}

void Menu::onReset()
{
	menuDataFont->OnResetDevice();
	menuTitleFont->OnResetDevice();
}
	
