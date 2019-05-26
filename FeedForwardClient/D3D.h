#pragma once
#pragma comment(lib, "d3dx9.lib")
#include <Windows.h>
#include <iostream>
#include <d3dx9.h>
#include <d3d9.h>

#include "color.h"

#define CIRCLE_PRIMITIVES 100

struct D3DVertex
{
	float x, y, z, rhw;
	D3DCOLOR color;
};

namespace D3D
{
	void RenderVertices(IDirect3DDevice9* pDevice, D3DPRIMITIVETYPE primType, DWORD num_prims, PVOID vertex_data, DWORD vertex_length);
	void DrawLine(IDirect3DDevice9* pDevice, float x1, float y1, float x2, float y2, D3DCOLOR color);
	void DrawSpecialSplitString(IDirect3DDevice9* pDevice, ID3DXFont* pFont, float x, float y, float w, float h, D3DCOLOR color, const char* string);
	void DrawRectangle(IDirect3DDevice9* pDevice, float x, float y, float width, float height, float thickness, D3DCOLOR color);
	void DrawCircle(IDirect3DDevice9* pDevice, float x, float y, float radius, float thickness, D3DCOLOR color);
	void DrawString(IDirect3DDevice9* pDevice, ID3DXFont* pFont, float x, float y, D3DCOLOR color, const char* string, ...);
	void DrawSplitString(IDirect3DDevice9* pDevice, ID3DXFont* pFont, float x, float y, float w, float h, D3DCOLOR color, const char* string, ...);
	RECT StringSize(IDirect3DDevice9* pDevice, ID3DXFont* pFont, const char* string, ...);
	D3DXVECTOR2 CenterString(IDirect3DDevice9* pDevice, ID3DXFont* pFont, float x, float y, float w, float h, const char* string, ...);
}