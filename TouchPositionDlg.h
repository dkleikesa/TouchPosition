
// TouchPositionDlg.h : ͷ�ļ�
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


// CTouchPositionDlg �Ի���
class CTouchPositionDlg : public CDialogEx
{
	// ����
public:
	CTouchPositionDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// �Ի�������
	enum { IDD = IDD_TOUCHPOSITION_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


	// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

	unsigned char* m_cFileBuffer; //�����ļ�ָ��
	unsigned int m_nFileLength;		//�ļ�����
	
	int FrameSize;	// ÿ֡���ݵĴ�С
	int LineSize;	// ÿ�����ݸ���
	int m_TowOrThree; //�ļ���ʽ 0��2�㲻�����ζ��� 1:3�㲻�����ζ��� 2:3������ζ���
	int m_nIsPosition; // �������λ��ǻ��켣 1����������  0�����켣
	unsigned char *m_pChar; //��ǰ��ʾ������֡λ��
	int m_nShowDiamondPoint;	//ͼ�����Ƿ���ʾ����
	int m_nShowDiamondData;		//����������ʾ�������ݻ�����ʾԭʼ����
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
