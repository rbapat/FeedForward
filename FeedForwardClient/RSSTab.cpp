#include "RSSTab.h"


RSSTab::RSSTab(std::string tabName, std::string tabLink)
{
	this->tabName = tabName;
	this->tabLink = tabLink;
	tooltip = false;
	TT = false;
	page = 0;
	ttIndex = -1;
	summaryData = NULL;
	sent = false;
	alpha = 0;

	std::vector<RSSInfo *> tmp = ParseRSS(tabLink.c_str());

	for (auto it = tmp.begin(); it < tmp.end(); it++)
	{
		if (strlen((*it)->articleTitle) != 0)
		{
			vecRSS.push_back(*it);
		}
	}
}

int RSSTab::renderPageInfo(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, MouseClick *mouse, int max, int basex, int basey, int rootx, int rooty, int height, int menuWidth)
{
	RECT size = D3D::StringSize(pDevice, pFont, "T");

	int numPerPage = max / ((size.bottom - size.top) + TEXT_DY);
	int numPages = vecRSS.size() / numPerPage - 1;
	if (numPages < 0)
		numPages = 0;

	D3DXVECTOR2 pos = D3D::CenterString(pDevice, pFont, rootx, rooty + height - 75, menuWidth, 75, "Change Page (%d / %d)", page + 1, numPages + 1);
	RECT tSize = D3D::StringSize(pDevice, pFont, "Change Page (%d / %d)", page, numPerPage);
	D3D::DrawString(pDevice, pFont, pos.x, pos.y, D3DCOLOR_ARGB(alpha, 153, 0, 0), "Change Page (%d / %d)", page + 1, numPages + 1);

	bool hover1 = sqrt(pow((basex + pos.x + 15 + (tSize.right - tSize.left)) - mouse->cursor.x, 2) + pow(basey + pos.y + 8 - mouse->cursor.y, 2)) < 6, hover2 = sqrt(pow(basex + pos.x - 15 - mouse->cursor.x, 2) + pow(basey + pos.y + 8 - mouse->cursor.y, 2)) < 6;
	auto color1 = hover1 ? D3DCOLOR_ARGB(alpha, 255, 255, 255) : D3DCOLOR_ARGB(alpha, 153, 0, 0), color2 = hover2 ? D3DCOLOR_ARGB(alpha, 255, 255, 255) : D3DCOLOR_ARGB(alpha, 153, 0, 0);
	D3DVertex pVertex[6] = {
		{ pos.x + 10 + (tSize.right - tSize.left), pos.y + 3, 0.0f, 1.0f, color1 },
		{ pos.x + 20 + (tSize.right - tSize.left), pos.y + 8, 0.0f, 1.0f, color1 },
		{ pos.x + 10 + (tSize.right - tSize.left), pos.y + 13, 0.0f, 1.0f, color1 },
		{ pos.x - 20, pos.y + 8, 0.0f, 1.0f, color2 },
		{ pos.x - 10, pos.y + 3, 0.0f, 1.0f, color2 },
		{ pos.x - 10, pos.y + 13, 0.0f, 1.0f, color2 }
	};
	D3D::RenderVertices(pDevice, D3DPT_TRIANGLELIST, 2, pVertex, sizeof(D3DVertex));

	if (mouse->curClicked && !mouse->prevClicked)
	{
		if (hover1)
			page = page == numPages ? 0 : page + 1;
		
		if (hover2)
			page = page == 0 ? numPages : page - 1;
	}

	return numPerPage;
}

bool RSSTab::render(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, MouseClick* mouse, int basex, int basey, int rootx, int rooty, int height, int menuWidth)
{
	//if going into tooltip, fade down
	if (tooltip && !TT)
		alpha = alpha >= FADE_SPEED ? alpha - FADE_SPEED : 0;

	//if coming into this, fade up
	if (alpha == 0 && !TT && !tooltip)
		alpha = alpha <= 255 - FADE_SPEED ? 255 : alpha + FADE_SPEED;

	//if should go to tooltip
	if (!exiting && (TT || alpha == 0))
	{
		TT = true;
		return true;
	}
	else if (exiting) //if exiting tooltip
		return true;
		

	D3D::DrawRectangle(pDevice, rootx, rooty, menuWidth, height, 0, D3DCOLOR_DARKISHGREY);
	
	int max = rooty + height - 75;
	int numPerPage = renderPageInfo(pDevice, pFont, mouse, max, basex, basey, rootx, rooty, height, menuWidth);
	
	for (int x = 0; x < numPerPage; x++)
	{
		int ind = page * numPerPage + x;
		if (rooty + TEXT_DY > max || ind == vecRSS.size())
			break;
		
		RECT size = D3D::StringSize(pDevice, pFont, vecRSS.at(ind)->articleTitle);
		
		if ((size.right - size.left) > menuWidth)
		{
			float ratio = menuWidth / (size.right - size.left);
			continue;
		}
		else
		{
			bool hover = !tooltip && inRect(mouse->cursor, basex + rootx, basey + rooty - (TEXT_DY / 2), (size.right - size.left), (size.bottom - size.top) + TEXT_DY);
			D3D::DrawString(pDevice, pFont, rootx, rooty, hover ? D3DCOLOR_WHITE : D3DCOLOR_ARGB(alpha, 153, 0, 0), vecRSS.at(ind)->articleTitle);
			rooty += (size.bottom - size.top) + TEXT_DY;
			if (hover && mouse->curClicked && !mouse->prevClicked)
			{
				ShellExecuteA(0, 0, vecRSS.at(ind)->articleLink, 0, 0, SW_SHOW);
			}

			if (hover && mouse->rClick && !mouse->prevRClick)
			{
				tooltip = true;
				ttIndex = ind;
			}
		}

		
	}

	return tooltip;
}

std::vector<RSSInfo*> RSSTab::ParseRSS(const char* rssLink)
{
	HRESULT hr = S_OK;
	IStream* spStream = NULL;
	IXmlReader* _spXmlReader = NULL;
	const wchar_t* pwszPrefix = NULL;
	const wchar_t* pwszLocalName = NULL;
	const wchar_t* pwszValue = NULL;
	bool read = false, link = false, title = false, linkSet = false;
	std::vector<RSSInfo*> tmp;
	tmp.push_back(new RSSInfo());


	CHKHR(URLOpenBlockingStream(NULL, rssLink, &spStream, NULL, NULL));
	CHKHR(CreateXmlReader(IID_IXmlReader, (void**)& _spXmlReader, NULL));
	CHKHR(_spXmlReader->SetInput(spStream));

	XmlNodeType nodeType;

	UINT cwchPrefix;

	while (S_OK == (hr = _spXmlReader->Read(&nodeType)))
	{
		const UINT buffSize = 8;
		wchar_t buff[buffSize];
		UINT charsRead = 0;

		if (linkSet)
		{
			tmp.push_back(new RSSInfo());
			linkSet = false;
		}

		if (nodeType == XmlNodeType_Element || nodeType == XmlNodeType_EndElement)
		{
			CHKHR(_spXmlReader->GetPrefix(&pwszPrefix, &cwchPrefix));
			CHKHR(_spXmlReader->GetLocalName(&pwszLocalName, NULL));
			if (!wcscmp(pwszLocalName, L"item") || !wcscmp(pwszLocalName, L"entry")) {

				if (nodeType == XmlNodeType_Element)
					read = true;
				else if (nodeType == XmlNodeType_EndElement)
					read = false;
			}

		}

		if (read)
		{
			switch (nodeType)
			{

			case XmlNodeType_Element:

				if (!wcscmp(pwszLocalName, L"title"))
					title = true;
				else if (!wcscmp(pwszLocalName, L"link"))
					link = true;
				break;
			case XmlNodeType_EndElement:
				if (!wcscmp(pwszLocalName, L"title"))
					title = false;
				else if (!wcscmp(pwszLocalName, L"link"))
					link = false;
				break;

			case XmlNodeType_Text:
			case XmlNodeType_CDATA:
				CHKHR(_spXmlReader->GetValue(&pwszValue, NULL));
				if (wcslen(pwszValue) == 0)
					break;

				if (title && !wcsstr(pwszValue, L"<"))
					sprintf(tmp.at(tmp.size() - 1)->articleTitle, "%ws", pwszValue);
				else if (link && !wcsstr(pwszValue, L"<"))
				{
					sprintf(tmp.at(tmp.size() - 1)->articleLink, "%ws", pwszValue);
					linkSet = true;
				}


				break;
			}
		}
	}

CleanUp:
	SAFE_RELEASE(spStream);
	SAFE_RELEASE(_spXmlReader);

	return tmp;
}
std::string RSSTab::getTabName()
{
	return tabName;
}

RECT RSSTab::drawTruncatedString(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, D3DCOLOR color, int x, int y, int w, char *string)
{
	if (strlen(string) == 0)
		return { 0, 0, 0, 0 };

	RECT size = D3D::StringSize(pDevice, pFont, string);
	
	if ((size.right - size.left) > w) 
	{
		float ratio = w / (size.right - size.left);
		return { 0, 0, 0, 0 };
	}
	else
	{
		D3D::DrawString(pDevice, pFont, x, y, color, string);
		return size;
	}
}

void RSSTab::disable()
{
	tooltip = false;
}

std::string RSSTab::getTabLink()
{
	return tabLink;
}

bool RSSTab::inRect(POINT mouse, int x, int y, int w, int h)
{
	return (mouse.x > x && mouse.x < x + w && mouse.y > y && mouse.y < y + h);
}

void RSSTab::renderTooltip(LPDIRECT3DDEVICE9 pDevice, LPD3DXFONT pFont, MouseClick* mouse, PyComm *scraper, int basex, int basey, int rootx, int rooty, int height, int menuWidth)
{
	if (exiting || (TT && tooltip && ttIndex > -1))
	{
		if(!exiting) alpha = alpha >= 255 - FADE_SPEED ? 255 : alpha + FADE_SPEED; //if entering tooltip


		D3D::DrawString(pDevice, pFont, rootx, rooty, D3DCOLOR_ARGB(alpha, 153, 0, 0), vecRSS.at(ttIndex)->articleTitle);
		if (!sent)
		{
			summaryData = scraper->fetchData(vecRSS.at(ttIndex)->articleLink);
			sent = true;
		}

		if (summaryData)
		{
			auto size = D3D::StringSize(pDevice, pFont, vecRSS.at(ttIndex)->articleTitle);
			D3D::DrawSpecialSplitString(pDevice, pFont, rootx, rooty + (size.bottom - size.top) + 15, menuWidth, height - ((size.bottom - size.top) + 5), D3DCOLOR_ARGB(alpha, 153, 0, 0), summaryData);
		}
			
		RECT size = D3D::StringSize(pDevice, pFont, "Back");
		auto h = rooty + height - (size.bottom - size.top) - 15;
		bool hover = inRect(mouse->cursor, basex + rootx + 25, basey + h + 2, 50 + (size.right - size.left),(size.bottom - size.top));
		D3DCOLOR color = hover ? D3DCOLOR_ARGB(alpha, 255, 255, 255) : D3DCOLOR_ARGB(alpha, 153, 0, 0);
		D3DVertex pVertex[3] = {
			{ rootx + 30, h + (size.bottom - size.top) / 2, 0.0f, 1.0f,  color },
			{ rootx + 40, h + 2, 0.0f, 1.0f,  color},
			{ rootx + 40, h + (size.bottom - size.top) - 2, 0.0f, 1.0f,  color }
		};

		D3D::RenderVertices(pDevice, D3DPT_TRIANGLELIST, 1, pVertex, sizeof(D3DVertex));
		D3D::DrawString(pDevice, pFont, rootx + 50, h, color, "Back");

		if (hover && mouse->curClicked && !mouse->prevClicked)
		{
			TT = false;
			exiting = true;
		}
		
	}

	if (exiting && alpha == 0)
	{
		tooltip = false;
		exiting = false;
		TT = false;
		sent = false;


		if (summaryData)
		{
			free(summaryData);
			summaryData = NULL;
		}

	}
		
}