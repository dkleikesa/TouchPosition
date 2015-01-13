#pragma once

extern "C"
{
#include "CalcPoint.h"
}

#define POINT_SIZE_FINALLY			8		//已识别点的大小
#define POINT_SIZE_DIAMOND			6		//菱形定点的大小

class CTouchPositionDlg;

class DrawGraph
{
public:
	DrawGraph(CTouchPositionDlg *pDlg);
	~DrawGraph(void);

public:
	CTouchPositionDlg *m_pDlg;

	Pen m_PenBlack;	//划线用-- 投影
	Pen m_PenRed;	//划线用-- 轨迹
	Pen m_PenGreen;	//划线用-- 轨迹
	Pen m_PenOrange;//划线用-- 轨迹

	SolidBrush m_BrushBlue;		//投影半透明填充用
	SolidBrush m_BrushRed;		//画点用
	SolidBrush m_BrushGreen;	//画点用
	SolidBrush m_BrushOrange;	//画点用
	SolidBrush m_BrushBlack;	//画点用
	SolidBrush m_BrushYellow;	
	SolidBrush m_BrushGray;	


public:
	void DrawTrack_Onice(void);
	void DrawPoint_Track( Bitmap* bmp, Graphics* graphics);
	void DrawTrack_Two( Bitmap* bmp, Graphics* graphics);
	void DrawTrack_Three( Bitmap* bmp, Graphics* graphics);
	void Draw_Three(unsigned char* buf, Bitmap* bmp, Graphics* graphics);
	void Draw_Two(unsigned char* buf, Bitmap* bmp, Graphics* graphics);
	int DrawTriangle(unsigned char* buf);
	void DrawOnice(POINT* point,Bitmap* bmp,Graphics* graphics);
	void DrawPoint(POINT* point,Bitmap* bmp,Graphics* graphics,int order);
	void Draw_DiamondPoint(CALC_DIAMOND_BUF* DiamondBuf, Bitmap* bmp, Graphics* graphics);

};

