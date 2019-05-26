#include "D3D.h"

namespace D3D
{
	void RenderVertices(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE primType, DWORD num_prims, PVOID vertex_data, DWORD vertex_length)
	{
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

		pDevice->DrawPrimitiveUP(primType, num_prims, vertex_data, vertex_length);
	}

	void DrawLine(IDirect3DDevice9* pDevice, float x1, float y1, float x2, float y2, D3DCOLOR color)
	{
		D3DVertex pVertex[2] = {
			{x1, y1, 0.0f, 1.0f, color},
			{x2, y2, 0.0f, 1.0f, color},
		};
		RenderVertices(pDevice, D3DPT_LINELIST, 1, pVertex, sizeof(D3DVertex));
	}

	void DrawRectangle(IDirect3DDevice9* pDevice, float x, float y, float width, float height, float thickness, D3DCOLOR color)
	{
		if (thickness == 0)
		{
			D3DVertex pVertex[4] = {
				{ x, y + height, 0.0f, 1.0f, color },
				{ x, y, 0.0f, 1.0f, color },
				{ x + width, y + height, 0.0f, 1.0f, color },
				{ x + width, y, 0.0f, 1.0f, color }
			};
			RenderVertices(pDevice, D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(D3DVertex));
		}
		else
		{
			D3DVertex pVertex[4] = {
			{ x - thickness, y + thickness + height, 0.0f, 1.0f, color },
			{ x - thickness, y, 0.0f, 1.0f, color },
			{ x, y + thickness + height, 0.0f, 1.0f, color },
			{ x, y, 0.0f, 1.0f, color },
			};
			RenderVertices(pDevice, D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(D3DVertex));

			pVertex[0] = { x - thickness, y - thickness, 0.0f, 1.0f, color };
			pVertex[1] = { x + width, y - thickness, 0.0f, 1.0f, color };
			pVertex[2] = { x - thickness, y, 0.0f, 1.0f, color };
			pVertex[3] = { x + width, y, 0.0f, 1.0f, color };
			RenderVertices(pDevice, D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(D3DVertex));

			pVertex[0] = { x + width, y - thickness, 0.0f, 1.0f, color };
			pVertex[1] = { x + width, y + height, 0.0f, 1.0f, color };
			pVertex[2] = { x + width + thickness, y - thickness, 0.0f, 1.0f, color };
			pVertex[3] = { x + width + thickness, y + height, 0.0f, 1.0f, color };
			RenderVertices(pDevice, D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(D3DVertex));

			pVertex[0] = { x, y + height, 0.0f, 1.0f, color };
			pVertex[1] = { x + width + thickness, y + height, 0.0f, 1.0f, color };
			pVertex[2] = { x, y + height + thickness, 0.0f, 1.0f, color };
			pVertex[3] = { x + width + thickness, y + height + thickness, 0.0f, 1.0f, color };
			RenderVertices(pDevice, D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(D3DVertex));
		}
	}

	void DrawCircle(IDirect3DDevice9* pDevice, float x, float y, float radius, float thickness, D3DCOLOR color)
	{
		if (thickness == 0)
		{
			D3DVertex* pVertex = new D3DVertex[CIRCLE_PRIMITIVES + 1];
			for (int i = 0; i <= CIRCLE_PRIMITIVES; i++) pVertex[i] = { x + radius * cos(D3DX_PI * (i / (CIRCLE_PRIMITIVES / 2.0f))), y + radius * sin(D3DX_PI * (i / (CIRCLE_PRIMITIVES / 2.0f))), 0.0f, 1.0f, color };

			RenderVertices(pDevice, D3DPT_TRIANGLEFAN, CIRCLE_PRIMITIVES, pVertex, sizeof(D3DVertex));
			delete[] pVertex;
		}
		else
		{
			D3DVertex* pVertex = new D3DVertex[CIRCLE_PRIMITIVES + 1];
			for (int i = 0; i <= CIRCLE_PRIMITIVES; i++) pVertex[i] = { x + radius * cos(D3DX_PI * (i / (CIRCLE_PRIMITIVES / 2.0f))), y - radius * sin(D3DX_PI * (i / (CIRCLE_PRIMITIVES / 2.0f))), 0.0f, 1.0f, color };

			RenderVertices(pDevice, D3DPT_LINESTRIP, CIRCLE_PRIMITIVES, pVertex, sizeof(D3DVertex));
			delete[] pVertex;
		}
	}

	void DrawString(IDirect3DDevice9* pDevice, ID3DXFont* pFont, float x, float y, D3DCOLOR color, const char* string, ...)
	{
		va_list args;
		char cBuffer[16000];

		va_start(args, string);
		vsprintf_s(cBuffer, string, args);
		va_end(args);

		RECT pRect;
		pRect.left = x;
		pRect.top = y;
		if(pFont)
			pFont->DrawText(NULL, cBuffer, strlen(cBuffer), &pRect, DT_NOCLIP, color);
	}

	RECT StringSize(IDirect3DDevice9* pDevice, ID3DXFont* pFont, const char* string, ...)
	{
		va_list args;
		char cBuffer[16000];

		va_start(args, string);
		vsprintf_s(cBuffer, string, args);
		va_end(args);

		RECT pRect = RECT();
		pFont->DrawText(NULL, cBuffer, strlen(cBuffer), &pRect, DT_CALCRECT, D3DCOLOR_RGBA(0, 0, 0, 0));
		return pRect;
	}

	D3DXVECTOR2 CenterString(IDirect3DDevice9* pDevice, ID3DXFont* pFont, float x, float y, float w, float h, const char* string, ...)
	{
		va_list args;
		char cBuffer[16000];

		va_start(args, string);
		vsprintf_s(cBuffer, string, args);
		va_end(args);

		RECT strRect = StringSize(pDevice, pFont, cBuffer);
		DWORD height = (strRect.bottom - strRect.top), width = (strRect.right - strRect.left);
		DWORD xPadding = (w - width) / 2, yPadding = (h - height) / 2;

		if (strRect.right - strRect.left > w || strRect.bottom - strRect.top > h)
			return {-1, -1};
		else
			return { x + xPadding, y + yPadding };
	}

	void DrawSpecialSplitString(IDirect3DDevice9* pDevice, ID3DXFont* pFont, float x, float y, float w, float h, D3DCOLOR color, const char* string)
	{
		auto size = D3D::StringSize(pDevice, pFont, "_");
		auto dX = (size.bottom - size.top);
		int charPerLine = (int)(w / (size.right - size.left));
		
		std::string tmp = "";
		int ind;
		for (ind = 0; ind < strlen(string); ind++)
		{
			if (string[ind] == '|' || tmp.size() > charPerLine)
			{

				while (string[ind] != '|' && string[ind] != ' ')
				{
					tmp.push_back(string[ind]);
					ind++;
				}

				D3D::DrawString(pDevice, pFont, x, y, color, tmp.c_str());
				y += dX + 5;
				tmp.clear();
			}
			else
			{
				tmp.push_back(string[ind]);
			}
		}

		if(tmp.size() > 0)
			D3D::DrawString(pDevice, pFont, x, y, color, tmp.c_str());

	}

	void DrawSplitString(IDirect3DDevice9* pDevice, ID3DXFont* pFont, float x, float y, float w, float h, D3DCOLOR color, const char* string, ...)
	{
		va_list args;
		char cBuffer[16000];

		va_start(args, string);
		vsprintf_s(cBuffer, string, args);
		va_end(args);
		
		int split = -1;
		for (int ind = 0; !split && ind < sizeof(cBuffer) || cBuffer[ind] != 0x00; ind++) {
			if (cBuffer[ind] == ' ')
				split = ind;
		}

		if (split) {
			auto string = std::string(cBuffer);
			std::string first = string.substr(0, split), second = string.substr(split + 1);
			
			auto size = D3D::StringSize(pDevice, pFont, first.c_str());
			D3DXVECTOR2 pos1 = D3D::CenterString(pDevice, pFont, x, y, w, h, first.c_str()), pos2 = D3D::CenterString(pDevice, pFont, x, y, w, h, second.c_str());

			D3D::DrawString(pDevice, pFont, pos1.x, pos1.y - (size.bottom - size.top) / 2, color, first.c_str());
			D3D::DrawString(pDevice, pFont, pos2.x, pos1.y + (size.bottom - size.top) / 2, color, second.c_str());


		}
	}
}