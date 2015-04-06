
// TouchPositionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TouchPosition.h"

#include "TouchPositionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTouchPositionDlg 对话框

#define EDIT_ONE_WIDTH_THREE	845		//数据显示框宽度和高度 660
#define EDIT_ONE_WIDTH_TWO		650
#define EDIT_ONE_HEIGHT		170

#define LIST_SELECT_WIDTH_THREE	325		//select框宽度 245
#define LIST_SELECT_WIDTH_TWO	245

#define BIGGER_TIME_PER		0.5		//单次鼠标滚轮放大倍数

CTouchPositionDlg::CTouchPositionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTouchPositionDlg::IDD, pParent)
	, m_cFileBuffer(NULL)
	, m_nFileLength(0)
	, m_ListBox(NULL)
	, m_fMakeBig(1)
	, m_fOldMakeBig(1)
	, m_pChar(NULL)
	, m_TowOrThree(0)
	, m_nIsPosition(0)
	, m_DrawGraph(this)
	, FrameSize(0)
	, LineSize(0)
	, m_sDiamond_buf(NULL)
	, m_nShowDiamondPoint(0)
	, m_nShowDiamondData(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTouchPositionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTouchPositionDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, &CTouchPositionDlg::OnBnClickedButtonOpen)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CTouchPositionDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_WM_CTLCOLOR()
	ON_LBN_SELCHANGE(IDC_LIST_SELECT, &CTouchPositionDlg::OnLbnSelchangeListSelect)
	ON_WM_MOUSEWHEEL()
	ON_STN_CLICKED(IDC_STATIC_STATUS, &CTouchPositionDlg::OnStnClickedStaticStatus)
	ON_EN_SETFOCUS(IDC_EDIT_ONE, &CTouchPositionDlg::OnEnSetfocusEditOne)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_RADIO_TRACK, &CTouchPositionDlg::OnBnClickedRadioTrack)
	ON_BN_CLICKED(IDC_RADIO_POSITION, &CTouchPositionDlg::OnBnClickedRadioPosition)
	ON_BN_CLICKED(IDC_CHECK_DIAMOND_PONT_G, &CTouchPositionDlg::OnBnClickedCheckDiamondPontG)
	ON_BN_CLICKED(IDC_BUTTON_DIAMOND_POINT_D, &CTouchPositionDlg::OnBnClickedButtonDiamondPointD)
END_MESSAGE_MAP()


// CTouchPositionDlg 消息处理程序

BOOL CTouchPositionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_ListBox = ((CListBox*)this->GetDlgItem(IDC_LIST_SELECT));
	this->SetWindowPos(&CWnd::wndBottom,0,0,1200,768,SWP_NOMOVE);	//设置为1024*768

	this->GetDlgItem(IDC_BUTTON1)->ShowWindow(SW_HIDE); //SW_SHOW  SW_HIDE

	((CButton *)this->GetDlgItem(IDC_RADIO_POSITION))->SetCheck(true);
	((CButton *)this->GetDlgItem(IDC_RADIO_TRACK))->SetCheck(false);
	((CButton *)this->GetDlgItem(IDC_CHECK_DIAMOND_PONT_G))->SetCheck(true);
	m_nShowDiamondPoint = 1;
	m_nIsPosition = 1;
	m_dc = this->GetDlgItem(IDC_STATIC_STATUS)->GetDC();

	m_BigPoint.x =0;
	m_BigPoint.y =0;
	m_OldBigPoint.x=0;
	m_OldBigPoint.y=0;
	m_fMakeBig =1.0;
	m_fOldMakeBig = 1.0;

	if(m_Font.CreatePointFont(100,"Courier New")!=0)
	{
		this->GetDlgItem(IDC_EDIT_ONE)->SetFont(&m_Font);
		this->GetDlgItem(IDC_LIST_SELECT)->SetFont(&m_Font);
	}
	else
	{
		this->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA("没有找到Courier New字体");
	}
	this->GetDlgItem(IDC_EDIT_ONE)->SetWindowTextA("");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTouchPositionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTouchPositionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTouchPositionDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_nFileLength = 0;
	if(m_cFileBuffer != NULL)
	{
		delete m_cFileBuffer;
		m_cFileBuffer = NULL;
	}
	ReleaseDC(m_dc);
	CDialogEx::OnClose();
}

void CTouchPositionDlg::OnBnClickedButtonOpen()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(true);
	fileDlg.m_ofn.lpstrFilter="DAT Files(*.dat)\0*.dat\0All Files(*.*)\0*.*\0\0";
	fileDlg.m_ofn.lpstrDefExt="dat";
	CFile fToFile;

	if(IDOK==fileDlg.DoModal())
	{	
		m_nFileLength = 0;
		if(m_cFileBuffer != NULL)
		{
			delete m_cFileBuffer;
			m_cFileBuffer = NULL;
		}
		m_ListBox->ResetContent();
		this->GetDlgItem(IDC_EDIT_ONE)->SetWindowTextA("");
		this->GetDlgItem(IDC_STATIC_STATUS)->RedrawWindow();

		if(!fToFile.Open(fileDlg.GetPathName(),CFile::modeRead))
		{
			this->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA("打开文件失败！");
			return ;
		}
		else
		{

			if(fToFile.GetLength() > 0xc0000000)
			{
				fToFile.Close();
				this->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA("不支持大于3G的文件！");
				return;
			}
			m_nFileLength =(int) fToFile.GetLength();
			m_cFileBuffer=new unsigned char[m_nFileLength];
			if(m_cFileBuffer == NULL)
			{
				m_nFileLength = 0;
				fToFile.Close();
				this->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA("内存分配失败！");
				return;
			}
			if(fToFile.Read(m_cFileBuffer,m_nFileLength) != m_nFileLength)
			{
				m_nFileLength = 0;
				if(m_cFileBuffer != NULL)
				{
					delete m_cFileBuffer;
					m_cFileBuffer = NULL;
				}
				fToFile.Close();
				this->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA("文件读取失败！");
				return;
			}
			//this->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA(fToFile.GetFilePath());
			this->SetWindowTextA(fToFile.GetFilePath());

			fToFile.Close();
			m_DrawGraph.ClearPointIDBuf();
			SetList();
			
			//	str.Format("打开文件成功！长度=%d",m_nFileLength);
			//	this->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA(str);

		}
	}
}

void CTouchPositionDlg::SetList(void)
{
	if(m_cFileBuffer == NULL)
		return;
	if(m_nFileLength < 260)
		goto error;
	if(m_cFileBuffer[0]!= 0xee)
		goto error;

	if(m_cFileBuffer[21]==0xea)
	{
		m_TowOrThree=0;			//2点的
		FrameSize = 260;
		LineSize = 26;
		this->SetListTwo();
	}
	else if(m_cFileBuffer[29]==0xea) 	//3点的
	{
		if(m_nFileLength < 340)
			goto error;
		if(m_nFileLength == 340)
		{
			m_TowOrThree=1;		//不带菱形定点数据的
			FrameSize = 340;
			LineSize = 34;
			this->SetListThree();
		}
		else if(m_cFileBuffer[340]==0xee)
		{
			m_TowOrThree=1;		//不带菱形定点数据的
			LineSize = 34;
			FrameSize = 340;
		}
		else
		{
			m_TowOrThree=2;		//带有菱形定点数据的
			LineSize = 34;
			FrameSize = 340 + sizeof(CALC_DIAMOND_BUF);
		}	
		this->SetListThree();
	}

	ReDrawWindow();
	m_ListBox->SetCurSel(0);
	if(m_nIsPosition==1)
		OnLbnSelchangeListSelect();
	else
		DrawTrack();
	//	str.Format("list count=%d",m_ListBox->GetCount());
	//	MessageBox(str);
	return;
error:
	m_nFileLength = 0;
	if(m_cFileBuffer != NULL)
	{
		delete m_cFileBuffer;
		m_cFileBuffer = NULL;
	}
	return;

}

void CTouchPositionDlg::SetListThree(void)
{
	CString str,strtem;
	int nGroupNum = m_nFileLength/(FrameSize);
	if(nGroupNum == 0)
		return;

	for(int i=0;i<nGroupNum;i++)
	{
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+1]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+2]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+7]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+8]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+9]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+10]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+12]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+13]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+14]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+15]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+17]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+18]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+19]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+20]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+0x21]);
		str += strtem;
		m_ListBox->AddString(str);
		str.Empty();

	}
}

void CTouchPositionDlg::SetListTwo(void)
{
	CString str,strtem;
	int nGroupNum = m_nFileLength/(FrameSize);
	if(nGroupNum == 0)
		return;

	for(int i=0;i<nGroupNum;i++)
	{
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+1]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+2]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+7]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+8]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+9]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+10]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+12]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+13]);
		str += strtem;
		strtem.Format("%02x", (int)m_cFileBuffer[i*FrameSize+14]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+15]);
		str += strtem;
		strtem.Format("%02x ",(int)m_cFileBuffer[i*FrameSize+25]);
		str += strtem;
		m_ListBox->AddString(str);
		str.Empty();

	}
}



void CTouchPositionDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	ReDrawWindow();
	if(m_nIsPosition == 1)
		m_DrawGraph.DrawTriangle(m_pChar);
	else 
		m_DrawGraph.DrawTrack_Onice();
}


void CTouchPositionDlg::ReDrawWindow(void)
{
	CRect rect;
	GetClientRect(&rect);

	CWnd *pWnd;
	int nWidth;

	pWnd = this->GetDlgItem(IDC_EDIT_ONE);
	if(pWnd != NULL )
	{
		if(m_TowOrThree ==0)
		{
			nWidth=EDIT_ONE_WIDTH_TWO;
		}
		else if(m_TowOrThree ==1)
		{
			nWidth = EDIT_ONE_WIDTH_THREE;
		}
		else if(m_TowOrThree ==2)
		{
			nWidth = EDIT_ONE_WIDTH_THREE;
		}
		pWnd->SetWindowPos(NULL,
			5,
			rect.bottom-EDIT_ONE_HEIGHT-5,
			nWidth,EDIT_ONE_HEIGHT,
			SWP_SHOWWINDOW);
		pWnd->RedrawWindow();
	}

	pWnd = this->GetDlgItem(IDC_LIST_SELECT);
	if(pWnd != NULL )
	{
		if(m_TowOrThree ==0)
		{
			nWidth=LIST_SELECT_WIDTH_TWO;
		}
		else if(m_TowOrThree ==1)
		{
			nWidth = LIST_SELECT_WIDTH_THREE;
		}
		else if(m_TowOrThree ==2)
		{
			nWidth = LIST_SELECT_WIDTH_THREE;
		}
		pWnd->SetWindowPos(NULL ,
			rect.right-nWidth-5,
			60,
			nWidth,
			rect.bottom-55,
			SWP_SHOWWINDOW);
		pWnd->RedrawWindow();
	}
	pWnd = this->GetDlgItem(IDC_BUTTON_OPEN);
	if(pWnd != NULL )
	{
		pWnd->SetWindowPos(NULL ,
		rect.right-55-15,
		8,
		55,
		20,
		SWP_SHOWWINDOW);
		pWnd->RedrawWindow();
	}
	pWnd = this->GetDlgItem(IDC_RADIO_POSITION);
	if(pWnd != NULL )
	{
		pWnd->SetWindowPos(NULL ,
		rect.right-115-10,
		8,
		45,
		20,
		SWP_SHOWWINDOW);
		pWnd->RedrawWindow();
	}
	pWnd = this->GetDlgItem(IDC_RADIO_TRACK);
	if(pWnd != NULL )
	{
		pWnd->SetWindowPos(NULL ,
		rect.right-160-10,
		8,
		45,
		20,
		SWP_SHOWWINDOW);
		pWnd->RedrawWindow();
	}
	pWnd = this->GetDlgItem(IDC_CHECK_DIAMOND_PONT_G);
	if(pWnd != NULL )
	{
		pWnd->SetWindowPos(NULL ,
		rect.right-230-10,
		8,
		70,
		20,
		SWP_SHOWWINDOW);
		pWnd->RedrawWindow();
	}
	pWnd = this->GetDlgItem(IDC_BUTTON_DIAMOND_POINT_D);
	if(pWnd != NULL )
	{
		pWnd->SetWindowPos(NULL ,
			rect.right-230-10,
			35,
			80,
			20,
			SWP_SHOWWINDOW);
		pWnd->RedrawWindow();
	}
	

	m_Rect_BitWindow.top = 10;
	m_Rect_BitWindow.left = 5;
	if(m_TowOrThree==0)
	{
		m_Rect_BitWindow.right = rect.right - LIST_SELECT_WIDTH_TWO - 5 - 5; 
	}
	else if(m_TowOrThree==1)
	{
		m_Rect_BitWindow.right = rect.right - LIST_SELECT_WIDTH_THREE - 5 - 5; 
	}
	else if(m_TowOrThree==2)
	{
		m_Rect_BitWindow.right = rect.right - LIST_SELECT_WIDTH_THREE - 5 - 5; 
	}
	m_Rect_BitWindow.bottom = rect.bottom-EDIT_ONE_HEIGHT - 5 - 10;
	if ((m_Rect_BitWindow.bottom *WB_WIDTH/WB_HEIGH) <= m_Rect_BitWindow.right ) 
		m_Rect_BitWindow.right = m_Rect_BitWindow.bottom *WB_WIDTH/WB_HEIGH;
	else
		m_Rect_BitWindow.bottom = m_Rect_BitWindow.right*WB_HEIGH/WB_WIDTH;

	pWnd = this->GetDlgItem(IDC_STATIC_STATUS);
	if(pWnd != NULL )
		pWnd->SetWindowPos(NULL,	
		m_Rect_BitWindow.left,
		m_Rect_BitWindow.top,
		m_Rect_BitWindow.Width(),
		m_Rect_BitWindow.Height(),
		SWP_SHOWWINDOW);

}


void CTouchPositionDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码

	/*
	CALC_POINT calc_point,calc_point1;
	calc_point.x = 10;
	calc_point.y= 10;
	calc_point1.x =14;
	calc_point1.y =14;

	CALC_DIAMOND a;
	CALC_POINT b;
	memset(&a,0,sizeof(CALC_DIAMOND));
	memset(&b,0,sizeof(CALC_POINT));
	a.strPoint0.x=10;
	a.strPoint0.y=10;
	a.strPoint1.x=7;
	a.strPoint1.y=8;
	a.strPoint2.x=12;
	a.strPoint2.y=6;
	a.strPoint3.x=3;
	a.strPoint3.y=2;


	CalclDiamondCentre(&a,&b);
	CString str;
	str.Format("aver=%d %d\n",b.x,b.y);
	MessageBox(str);
	*/
}

HBRUSH CTouchPositionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if(nCtlColor == CTLCOLOR_DLG )
	{
		return ::CreateSolidBrush(RGB(225,225,225));
	}
	if(nCtlColor == CTLCOLOR_STATIC )
	{
		return ::CreateSolidBrush(RGB(255,255,255));
	}

	//return ::CreateSolidBrush(RGB(0,0,255));
	//return ::CreateSolidBrush(RGB(0,0,255));
	// TODO:  在此更改 DC 的任何特性

	// todO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CTouchPositionDlg::OnLbnSelchangeListSelect()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str,strtemp;
	if(m_cFileBuffer == NULL)
		return;
	if(m_nFileLength ==0)
		return;
	if(m_cFileBuffer[0]!= 0xee)
	{
		m_nFileLength = 0;
		if(m_cFileBuffer != NULL)
		{
			delete m_cFileBuffer;
			m_cFileBuffer = NULL;
		}
		return;
	}
	if(m_ListBox->GetCount() == 0)
		return;

	unsigned int nCur = m_ListBox->GetCurSel();
	m_BigPoint.x =0;
	m_BigPoint.y =0;
	m_OldBigPoint.x=0;
	m_OldBigPoint.y=0;
	m_fMakeBig =1.0;
	m_fOldMakeBig = 1.0;

	m_pChar = &m_cFileBuffer[nCur*FrameSize+LineSize];
	for(int i = 0;i < LineSize*10;)
	{
		strtemp.Format("%02x ",(int)m_cFileBuffer[nCur*FrameSize+i]);
		str += strtemp;
		i++;
		if(i%LineSize == 0)
		{
			str += "\r\n";
		}
	}

	if(m_TowOrThree ==2)
	{
		char buff[80*9*5 + 80*9*4 + 80*6];
		int ScanCount=0;
		memset(buff,0,sizeof(buff));
		m_sDiamond_buf=(CALC_DIAMOND_BUF *)&m_cFileBuffer[nCur*FrameSize+LineSize*10];
		ScanCount = ((m_cFileBuffer[nCur*FrameSize+1]<<8) | m_cFileBuffer[nCur*FrameSize+2]);
		PrintDiamond(m_sDiamond_buf,buff,ScanCount);
		str += buff;
		
	}
	//this->GetDlgItem(IDC_EDIT_ONE)->ShowWindow()
	this->GetDlgItem(IDC_EDIT_ONE)->SetWindowTextA(str);
	if((m_nShowDiamondData == 1)&&(m_TowOrThree==2))
	{
		this->GetDlgItem(IDC_EDIT_ONE)->SendMessage(EM_SCROLL, SB_PAGEDOWN, 0);
		this->GetDlgItem(IDC_EDIT_ONE)->SendMessage(EM_SCROLL, SB_LINEDOWN, 0);
	}

	if(m_nIsPosition==0)
		DrawTrack();
	else
		m_DrawGraph.DrawTriangle(m_pChar);
	//	str.Format("select %d",nCur);
	//	MessageBox(str);
	return;
}


BOOL CTouchPositionDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CRect rect;
	this->GetDlgItem(IDC_STATIC_STATUS)->GetWindowRect(&rect);
	if((pt.x <=rect.left)||(pt.y <=rect.top)||(pt.x >=rect.right)||(pt.y>=rect.bottom))
		return 0;
	if(m_cFileBuffer == NULL)
		return 0;
	if(m_nFileLength ==0)
		return 0;
	if(m_cFileBuffer[0]!= 0xee)
	{
		m_nFileLength = 0;
		if(m_cFileBuffer != NULL)
		{
			delete m_cFileBuffer;
			m_cFileBuffer = NULL;
		}
		return 0;
	}
	if(m_ListBox->GetCount() == 0)
		return 0;
	//调整放大倍数
	if(zDelta>0)
		m_fMakeBig += BIGGER_TIME_PER;
	if(zDelta<0)
		m_fMakeBig -= BIGGER_TIME_PER;
	if(m_fMakeBig>15)		//最大是17 设成15比较保险
		m_fMakeBig = 15;
	if(m_fMakeBig<1)
		m_fMakeBig = 1;

	//获取显示区域
	m_BigPoint.x =(long)((pt.x-rect.left+m_OldBigPoint.x)/m_fOldMakeBig*m_fMakeBig-(pt.x-rect.left));
	m_BigPoint.y = (long)((pt.y-rect.top+m_OldBigPoint.y)/m_fOldMakeBig*m_fMakeBig-(pt.y-rect.top));
	if(m_BigPoint.x<=0)
		m_BigPoint.x=0;
	if(m_BigPoint.y<=0)
		m_BigPoint.y=0;
	if(m_Rect_BitWindow.Width()*m_fMakeBig - m_BigPoint.x<m_Rect_BitWindow.Width())
		m_BigPoint.x = (long)(m_Rect_BitWindow.Width()*(m_fMakeBig -1 ));
	if(m_Rect_BitWindow.Height()*m_fMakeBig - m_BigPoint.y<m_Rect_BitWindow.Height())
		m_BigPoint.y = (long)(m_Rect_BitWindow.Height()*(m_fMakeBig -1 ));

	/*
	CString str;
	str.Format("mu %d %d wn %d %d Bp %d %d oBp %d %d mB%f",pt.x,pt.y,rect.top,rect.left,m_BigPoint.x,m_BigPoint.y,m_OldBigPoint.x,m_OldBigPoint.y,m_fMakeBig);
	this->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA(str);
	*/
	m_OldBigPoint.x = m_BigPoint.x;
	m_OldBigPoint.y = m_BigPoint.y ;
	m_fOldMakeBig = m_fMakeBig;
	if(m_nIsPosition == 1)
		m_DrawGraph.DrawTriangle(m_pChar);
	else
		m_DrawGraph.DrawTrack_Onice();

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CTouchPositionDlg::OnStnClickedStaticStatus()
{
	// TODO: 在此添加控件通知处理程序代码
	this->GetDlgItem(IDC_STATIC_STATUS)->SetFocus();	
}


void CTouchPositionDlg::OnEnSetfocusEditOne()
{
	// TODO: 在此添加控件通知处理程序代码
	//this->GetDlgItem(IDC_EDIT_ONE)->HideCaret();
}


void CTouchPositionDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
	if(m_nIsPosition == 1)
		m_DrawGraph.DrawTriangle(m_pChar);
	else
		m_DrawGraph.DrawTrack_Onice();
}

void CTouchPositionDlg::OnBnClickedRadioTrack()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton *)this->GetDlgItem(IDC_RADIO_POSITION))->SetCheck(false);
	((CButton *)this->GetDlgItem(IDC_RADIO_TRACK))->SetCheck(true);
	m_nIsPosition = 0;

	OnLbnSelchangeListSelect();
}


void CTouchPositionDlg::OnBnClickedRadioPosition()
{
	// TODO: 在此添加控件通知处理程序代码
	((CButton *)this->GetDlgItem(IDC_RADIO_POSITION))->SetCheck(true);
	((CButton *)this->GetDlgItem(IDC_RADIO_TRACK))->SetCheck(false);
	m_nIsPosition = 1;
	OnLbnSelchangeListSelect();
}


void CTouchPositionDlg::DrawTrack(void)
{
	if(m_cFileBuffer == NULL)
		return;
	if(m_nFileLength ==0)
		return;
	if(m_cFileBuffer[0]!= 0xee)
	{
		m_nFileLength = 0;
		if(m_cFileBuffer != NULL)
		{
			delete m_cFileBuffer;
			m_cFileBuffer = NULL;
		}
		return;
	}
	if(m_ListBox->GetCount() == 0)
		return;
	if(m_nIsPosition==1)
		return;

	m_BigPoint.x =0;
	m_BigPoint.y =0;
	m_OldBigPoint.x=0;
	m_OldBigPoint.y=0;
	m_fMakeBig =1.0;
	m_fOldMakeBig = 1.0;

	m_DrawGraph.DrawTrack_Onice();

}

void CTouchPositionDlg::OnBnClickedCheckDiamondPontG()
{
	// TODO: 在此添加控件通知处理程序代码
	if (((CButton *)this->GetDlgItem(IDC_CHECK_DIAMOND_PONT_G))->GetCheck())
	{
		m_nShowDiamondPoint = 1;
	}
	else
	{
		m_nShowDiamondPoint = 0;
	}
	if(m_nIsPosition == 1)
		m_DrawGraph.DrawTriangle(m_pChar);
	else
		m_DrawGraph.DrawTrack_Onice();
	
}


void CTouchPositionDlg::OnBnClickedButtonDiamondPointD()
{
	// TODO: 在此添加控件通知处理程序代码
	CString str;
	this->GetDlgItemTextA(IDC_BUTTON_DIAMOND_POINT_D,str);
	if(str == "显示顶点数据")
	{
		m_nShowDiamondData = 1;
		this->GetDlgItem(IDC_BUTTON_DIAMOND_POINT_D)->SetWindowTextA("显示原始数据");
		if(m_TowOrThree==2)
		{
			this->GetDlgItem(IDC_EDIT_ONE)->SendMessage(EM_SCROLL, SB_PAGEDOWN, 0);
			this->GetDlgItem(IDC_EDIT_ONE)->SendMessage(EM_SCROLL, SB_LINEDOWN, 0);
		}
	}
	else
	{
		m_nShowDiamondData = 0;
		this->GetDlgItem(IDC_BUTTON_DIAMOND_POINT_D)->SetWindowTextA("显示顶点数据");
		if(m_TowOrThree==2)
		{
			this->GetDlgItem(IDC_EDIT_ONE)->SendMessage(EM_SCROLL, SB_PAGEUP, 0);
			this->GetDlgItem(IDC_EDIT_ONE)->SendMessage(EM_SCROLL, SB_LINEUP, 0);
		}
	}
}
