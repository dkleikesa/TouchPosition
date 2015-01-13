
// TouchPosition.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CTouchPositionApp:
// 有关此类的实现，请参阅 TouchPosition.cpp
//

class CTouchPositionApp : public CWinApp
{
public:
	CTouchPositionApp();

// 重写
public:
	virtual BOOL InitInstance();
	
	GdiplusStartupInput m_gdiplusStartupInput; 
	ULONG_PTR m_gdiplusToken;

// 实现

	DECLARE_MESSAGE_MAP()
	void DrawOnice(POINT* point);

	virtual int ExitInstance();
};

extern CTouchPositionApp theApp;