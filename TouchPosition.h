
// TouchPosition.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTouchPositionApp:
// �йش����ʵ�֣������ TouchPosition.cpp
//

class CTouchPositionApp : public CWinApp
{
public:
	CTouchPositionApp();

// ��д
public:
	virtual BOOL InitInstance();
	
	GdiplusStartupInput m_gdiplusStartupInput; 
	ULONG_PTR m_gdiplusToken;

// ʵ��

	DECLARE_MESSAGE_MAP()
	void DrawOnice(POINT* point);

	virtual int ExitInstance();
};

extern CTouchPositionApp theApp;