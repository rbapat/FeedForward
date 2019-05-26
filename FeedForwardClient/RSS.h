#pragma once
#include <Windows.h>
#include <iostream>
#include <d3dx9.h>
#include <d3d9.h>
#include <string>

class RSSTab
{
	RSSTab(std::string tabName, std::string tabLink);
	void render(LPDIRECT3DDEVICE9 pDevice);
};

