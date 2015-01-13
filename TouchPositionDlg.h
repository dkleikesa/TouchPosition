
// TouchPositionDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "atltypes.h"
#include "gdiplusgraphics.h"
#include "DrawGraph.h"
extern "C"
{
#include "CalcPoint.h"
}


// CTouchPositionDlg 对话框
class CTouchPositionDlg : public CDialogEx
{
	// 构造
public:
	CTouchPositionDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 对话框数据
	enum { IDD = IDD_TOUCHPOSITION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


	// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnBnClickedButtonOpen();
	CFont m_Font;
	CListBox* m_ListBox;
	void SetList(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void ReDrawWindow(void);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnClose();

	unsigned char* m_cFileBuffer; //数据文件指针
	unsigned int m_nFileLength;		//文件长度
	
	int FrameSize;	// 每帧数据的大小
	int LineSize;	// 每行数据个数
	int m_TowOrThree; //文件格式 0：2点不带菱形顶点 1:3点不带菱形顶点 2:3点带菱形顶点
	int m_nIsPosition; // 画三角形还是画轨迹 1：画三角形  0：画轨迹
	unsigned char *m_pChar; //当前显示的数据帧位置
	int m_nShowDiamondPoint;	//图形上是否显示顶点
	int m_nShowDiamondData;		//数据上是显示顶点数据还是显示原始数据
	CALC_DIAMOND_BUF *m_sDiamond_buf;

	CDC* m_dc;
	CRect m_Rect_BitWindow;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLbnSelchangeListSelect();

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	float m_fMakeBig;
	float m_fOldMakeBig;
	afx_msg void OnStnClickedStaticStatus();
	afx_msg void OnEnSetfocusEditOne();
	afx_msg void OnMove(int x, int y);
	POINT m_BigPoint;
	POINT m_OldBigPoint;

	DrawGraph m_DrawGraph;

	void SetListTwo(void);
	void SetListThree(void);

	afx_msg void OnBnClickedRadioTrack();
	afx_msg void OnBnClickedRadioPosition();
	void CTouchPositionDlg::DrawTrack(void);


	afx_msg void OnBnClickedCheckDiamondPontG();
	afx_msg void OnBnClickedButtonDiamondPointD();
};
