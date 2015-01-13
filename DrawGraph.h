#pragma once

extern "C"
{
#include "CalcPoint.h"
}

#define POINT_SIZE_FINALLY			8		//��ʶ���Ĵ�С
#define POINT_SIZE_DIAMOND			6		//���ζ���Ĵ�С

class CTouchPositionDlg;

class DrawGraph
{
public:
	DrawGraph(CTouchPositionDlg *pDlg);
	~DrawGraph(void);

public:
	CTouchPositionDlg *m_pDlg;

	Pen m_PenBlack;	//������-- ͶӰ
	Pen m_PenRed;	//������-- �켣
	Pen m_PenGreen;	//������-- �켣
	Pen m_PenOrange;//������-- �켣

	SolidBrush m_BrushBlue;		//ͶӰ��͸�������
	SolidBrush m_BrushRed;		//������
	SolidBrush m_BrushGreen;	//������
	SolidBrush m_BrushOrange;	//������
	SolidBrush m_BrushBlack;	//������
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

