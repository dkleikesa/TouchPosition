//							    _ooOoo_   
//							   o8888888o   
//							   88" . "88   
//							   (| -_- |)   
//							    O\ = /O   
//							____/`---'\____   
//							 .' \\| |// `.   
//						   / \\||| : |||// \   
//						 / _||||| -:- |||||- \   
//						   | | \\\ - /// | |   
//						 | \_| ''\---/'' | |   
//						  \ .-\__ `-` ___/-. /   
//					   ___`. .' /--.--\ `. . __   
//					."" '< `.___\_<|>_/___.' >'"".   
//				   | | : `- \`.;`\ _ /`;.`/ - ` : | |   
//					 \ \ `-. \_ __\ /__ _/ .-` / /   
//			 ======`-.____`-.___\_____/___.-`____.-'======   
//							    `=---='   
//
//             .............................................   
//                      佛祖镇楼                  BUG辟易 

#include "StdAfx.h"
#include "resource.h"
#include "TouchPositionDlg.h"
#include "CalcPoint.h"
#include "DrawGraph.h"
#include <io.h>  
#include <fcntl.h>  


DrawGraph::DrawGraph(CTouchPositionDlg *pDlg)
	: m_PenBlack(Color(0,0,0),1)
	, m_PenRed(Color(255,0,0),1)
	, m_PenGreen(Color(0,255,0),1)
	, m_PenOrange(Color(255,128,0),1)
	, m_BrushBlue(Color(80, 0, 0,255)) 
	, m_BrushRed(Color(255, 255, 0,0)) 
	, m_BrushGreen(Color(255,0 , 255,0)) 
	, m_BrushOrange(Color(255, 255,128,122))
	, m_BrushYellow(Color(255, 255,255,0))
	, m_BrushGray(Color(255, 0,162,232)) 
	, m_BrushRR(Color(255, 150,162,0)) 
	, m_BrushBlack(Color(255, 0,0,0)) 
{
	m_pDlg = pDlg;
	InitConsole();
}
void DrawGraph::InitConsole()  
{  
	int nRet= 0;  
	FILE* fp;  
	AllocConsole();  
	nRet= _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);  
	fp = _fdopen(nRet, "w");  
	*stdout = *fp;  
	setvbuf(stdout, NULL, _IONBF, 0);  
}  
DrawGraph::~DrawGraph(void)
{

}

void DrawGraph::ClearPointIDBuf(void)
{
	ClearPointID();
}
void DrawGraph::DrawTrack_Onice(void)
{

	CString str;
	CWnd *pWnd;
	pWnd = m_pDlg->GetDlgItem(IDC_STATIC_STATUS);
	if(pWnd == NULL)
	{
		return;	
	}
	if(m_pDlg->m_cFileBuffer == NULL)
		return;
	if(m_pDlg->m_nFileLength ==0)
		return;
	if(m_pDlg->m_cFileBuffer[0]!= 0xee)
	{
		m_pDlg->m_nFileLength = 0;
		if(m_pDlg->m_cFileBuffer != NULL)
		{
			delete m_pDlg->m_cFileBuffer;
			m_pDlg->m_cFileBuffer = NULL;
		}
		return;
	}
	str.Format("mB%f %d %d %d %d",m_pDlg->m_fMakeBig,m_pDlg->m_Rect_BitWindow.Width(),
		m_pDlg->m_Rect_BitWindow.Height(),m_pDlg->m_BigPoint.x,m_pDlg->m_BigPoint.y);
	pWnd->SetWindowTextA(str);
	pWnd->RedrawWindow();
	Bitmap bmp((int)(m_pDlg->m_Rect_BitWindow.Width()*(m_pDlg->m_fMakeBig)),	
		(int)(m_pDlg->m_Rect_BitWindow.Height() *(m_pDlg->m_fMakeBig)));
	Graphics* graphics = Graphics::FromImage(&bmp);
	graphics->SetSmoothingMode(SmoothingModeHighQuality);
	if(m_pDlg->m_TowOrThree ==0)
	{
		DrawTrack_Two(&bmp,graphics);
	}
	else //if(m_pDlg->m_TowOrThree ==1)
	{
		DrawTrack_Three(&bmp,graphics);
	}
	DrawPoint_Track(&bmp,graphics);
	Graphics g(m_pDlg->m_dc->GetSafeHdc());	

	g.DrawImage(&bmp,
		0,0,
		m_pDlg->m_BigPoint.x,m_pDlg->m_BigPoint.y,
		m_pDlg->m_Rect_BitWindow.Width(),
		m_pDlg->m_Rect_BitWindow.Height(),
		UnitPixel);
	delete graphics;
}

void DrawGraph::DrawPoint_Track( Bitmap* bmp, Graphics* graphics)
{
	unsigned char *buf;
	POINT PointTem[3];
	if(m_pDlg->m_TowOrThree ==0)
	{
		buf  = m_pDlg->m_pChar- m_pDlg->LineSize;
		for(int j=0;j<2;j++)
		{
			if((buf[7+j*5] !=0)||(buf[8+j*5] !=0)||(buf[9+j*5] !=0)||(buf[10+j*5] !=0))
			{
				PointTem[0].x = ((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1;
				PointTem[0].y = ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00));
				DrawPoint(PointTem,bmp,graphics,j);
			}
		}
	}
	else //if(m_pDlg->m_TowOrThree ==1)
	{
		buf  = m_pDlg->m_pChar-m_pDlg->LineSize;
		for(int j=0;j<3;j++)
		{
			if((buf[7+j*5] !=0)||(buf[8+j*5] !=0)||(buf[9+j*5] !=0)||(buf[10+j*5] !=0))
			{
				PointTem[0].x = ((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1;
				PointTem[0].y = ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00));
				DrawPoint(PointTem,bmp,graphics,j);
			}
		}
	}
}

void DrawGraph::DrawTrack_Two( Bitmap* bmp, Graphics* graphics)
{

	unsigned char *buf = m_pDlg->m_cFileBuffer;
	int nGroupNum = m_pDlg->m_nFileLength/ m_pDlg->FrameSize;
	if(nGroupNum == 0)
		return;
	PointF pointf_temp;
	PointF *point1 = NULL;
	PointF *point2 = NULL;
	try
	{
		int point1_idx = 0;
		int point2_idx = 0;
		int point1_last  = 1;
		int point2_last = 1;
		point1 = new PointF[nGroupNum];
		point2 = new PointF[nGroupNum];
		for(int i=0;i<nGroupNum;i++)
		{
			for(int j=0;j<2;j++)
			{
				if((buf[7+j*5] !=0)||(buf[8+j*5] !=0)||(buf[9+j*5] !=0)||(buf[10+j*5] !=0))
				{
					if(j==0)
					{
						point1_last = 0;
						point1[point1_idx].X =(REAL)( ((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1);
						point1[point1_idx].Y =(REAL)( 456 - ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00)));
						point1[point1_idx].X = (point1[point1_idx].X*m_pDlg->m_Rect_BitWindow.Width()*m_pDlg->m_fMakeBig/640) ;
						point1[point1_idx].Y = (point1[point1_idx].Y*m_pDlg->m_Rect_BitWindow.Height()* m_pDlg->m_fMakeBig/456) ;
						point1_idx++;
					}
					else
					{
						point2_last = 0;
						point2[point2_idx].X = (REAL)(((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1);
						point2[point2_idx].Y = (REAL)(456 - ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00)));
						point2[point2_idx].X = (point2[point2_idx].X*m_pDlg->m_Rect_BitWindow.Width()*m_pDlg->m_fMakeBig/640) ;
						point2[point2_idx].Y = (point2[point2_idx].Y*m_pDlg->m_Rect_BitWindow.Height()* m_pDlg->m_fMakeBig/456) ;
						point2_idx++;
					}
				}
				else
				{
					if(j==0)
					{
						if(point1_last ==0 )
						{
							point1[point1_idx].X = 0;
							point1[point1_idx].Y = 0;
							point1_idx++;
							point1_last = 1;
						}
					}
					else
					{
						if(point2_last ==0 )
						{
							point2[point2_idx].X = 0;
							point2[point2_idx].Y = 0;
							point2_idx++;
							point2_last = 1;
						}
					}
				}
			}
			buf +=m_pDlg->FrameSize;
		}

		//PPPP1:

		point1_last=0;
		if(point1_idx == 0)
		{
			goto PPPP2;
		}
		else if(point1_idx == 1)
		{
			//graphics->DrawEllipse( &m_PenRed,point1[0].X-1.5,point1[0].Y-1.5,3,3);
			graphics->DrawLine(&m_PenRed,point1[0],point1[0]);
		}
		else
		{
			pointf_temp = point1[0];
			for(int i=1;i<point1_idx;i++)
			{
				if((point1[i].X !=0)||(point1[i].Y !=0))
				{
					if(point1_last==1)
					{
						pointf_temp = point1[i];
						//graphics->DrawEllipse( &m_PenRed,pointf_temp.X-1.5,pointf_temp.Y-1.5,3,3);
					}
					graphics->DrawLine(&m_PenRed,pointf_temp,point1[i]);
					pointf_temp = point1[i];
					point1_last = 0;
				}
				else
				{
					point1_last = 1;
				}
			}
		}


PPPP2:

		point2_last=0;
		if(point2_idx == 0)
		{
			goto PPPP3;
		}
		else if(point2_idx == 1)
		{
			//graphics->DrawEllipse( &m_PenGreen,point2[0].X-1.5,point2[0].Y-1.5,3,3);
			graphics->DrawLine(&m_PenGreen,point2[0],point2[0]);
		}
		else
		{
			pointf_temp = point2[0];
			for(int i=1;i<point2_idx;i++)
			{
				if((point2[i].X !=0)||(point2[i].Y !=0))
				{
					if(point2_last==1)
					{
						pointf_temp = point2[i];
						//	graphics->DrawEllipse( &m_PenGreen,pointf_temp.X-1.5,pointf_temp.Y-1.5,3,3);
					}
					graphics->DrawLine(&m_PenGreen,pointf_temp,point2[i]);
					pointf_temp = point2[i];
					point2_last = 0;
				}
				else
				{
					point2_last = 1;
				}
			}
		}

PPPP3:
		delete [] point1;
		delete [] point2;
	}
	catch(CException* e)
	{
		delete [] point1;
		delete [] point2;
		e->Delete();
	}
}

void DrawGraph::DrawTrack_Three( Bitmap* bmp, Graphics* graphics)
{
	int point1_idx = 0;
	int point2_idx = 0;
	int point3_idx = 0;
	int point1_last = 1;
	int point2_last = 1;
	int point3_last = 1;
	unsigned char *buf = m_pDlg->m_cFileBuffer;
	int nGroupNum = m_pDlg->m_nFileLength/m_pDlg->FrameSize;
	if(nGroupNum == 0)
		return;
	PointF pointf_temp;
	PointF *point1 = NULL;
	PointF *point2 = NULL;
	PointF *point3 = NULL;

	try
	{
		point1 = new PointF[nGroupNum];
		point2 = new PointF[nGroupNum];
		point3 = new PointF[nGroupNum];


		for(int i=0;i<nGroupNum;i++)
		{
			for(int j=0;j<3;j++)
			{
				if((buf[7+j*5] !=0)||(buf[8+j*5] !=0)||(buf[9+j*5] !=0)||(buf[10+j*5] !=0))
				{
					if(j==0)
					{
						point1_last = 0;
						point1[point1_idx].X = (REAL)(((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1);
						point1[point1_idx].Y = (REAL)(456 - ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00)));
						point1[point1_idx].X = (point1[point1_idx].X*m_pDlg->m_Rect_BitWindow.Width()* m_pDlg->m_fMakeBig/640) ;
						point1[point1_idx].Y = (point1[point1_idx].Y*m_pDlg->m_Rect_BitWindow.Height()* m_pDlg->m_fMakeBig/456) ;
						point1_idx++;
					}
					else if(j==1)
					{
						point2_last = 0;
						point2[point2_idx].X = (REAL)(((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1);
						point2[point2_idx].Y = (REAL)(456 - ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00)));
						point2[point2_idx].X = (point2[point2_idx].X*m_pDlg->m_Rect_BitWindow.Width()* m_pDlg->m_fMakeBig/640) ;
						point2[point2_idx].Y = (point2[point2_idx].Y*m_pDlg->m_Rect_BitWindow.Height()* m_pDlg->m_fMakeBig/456) ;
						point2_idx++;
					}
					else
					{
						point3_last = 0;
						point3[point3_idx].X = (REAL)(((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1);
						point3[point3_idx].Y = (REAL)(456 - ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00)));
						point3[point3_idx].X = (point3[point3_idx].X*m_pDlg->m_Rect_BitWindow.Width()* m_pDlg->m_fMakeBig/640) ;
						point3[point3_idx].Y = (point3[point3_idx].Y*m_pDlg->m_Rect_BitWindow.Height()* m_pDlg->m_fMakeBig/456) ;
						point3_idx++;
					}

				}
				else
				{
					if(j==0)
					{
						if(point1_last ==0 )
						{
							point1[point1_idx].X = 0;
							point1[point1_idx].Y = 0;
							point1_idx++;
							point1_last = 1;
						}
					}
					else if(j == 1)
					{
						if(point2_last ==0 )
						{
							point2[point2_idx].X = 0;
							point2[point2_idx].Y = 0;
							point2_idx++;
							point2_last = 1;
						}
					}
					else
					{
						if(point3_last ==0 )
						{
							point3[point3_idx].X = 0;
							point3[point3_idx].Y = 0;
							point3_idx++;
							point3_last = 1;
						}
					}
				}
			}
			buf += m_pDlg->FrameSize;
		}

		//PPP1:

		point1_last=0;
		if(point1_idx == 0)
		{
			goto PPP2;
		}
		else if(point1_idx == 1)
		{
			//	graphics->DrawEllipse( &m_PenRed,point1[0].X-1.5,point1[0].Y-1.5,3,3);
			graphics->DrawLine(&m_PenRed,point1[0],point1[0]);
		}
		else
		{
			pointf_temp = point1[0];
			for(int i=1;i<point1_idx;i++)
			{
				if((point1[i].X !=0)||(point1[i].Y !=0))
				{
					if(point1_last==1)
					{
						pointf_temp = point1[i];
						//graphics->DrawEllipse( &m_PenRed,pointf_temp.X-1.5,pointf_temp.Y-1.5,3,3);
					}
					graphics->DrawLine(&m_PenRed,pointf_temp,point1[i]);
					pointf_temp = point1[i];
					point1_last = 0;
				}
				else
				{
					point1_last = 1;
				}
			}
		}


PPP2:

		point2_last=0;
		if(point2_idx == 0)
		{
			goto PPP3;
		}
		else if(point2_idx == 1)
		{
			//graphics->DrawEllipse( &m_PenGreen,point2[0].X-1.5,point2[0].Y-1.5,3,3);
			graphics->DrawLine(&m_PenGreen,point2[0],point2[0]);
		}
		else
		{
			pointf_temp = point2[0];
			for(int i=1;i<point2_idx;i++)
			{
				if((point2[i].X !=0)||(point2[i].Y !=0))
				{
					if(point2_last==1)
					{
						pointf_temp = point2[i];
						//graphics->DrawEllipse( &m_PenGreen,pointf_temp.X-1.5,pointf_temp.Y-1.5,3,3);
					}
					graphics->DrawLine(&m_PenGreen,pointf_temp,point2[i]);
					pointf_temp = point2[i];
					point2_last = 0;
				}
				else
				{
					point2_last = 1;
				}
			}
		}

PPP3:

		point3_last=0;
		if(point3_idx == 0)
		{
			goto PPP4;
		}
		else if(point3_idx == 1)
		{
			//graphics->DrawEllipse( &m_PenOrange,point3[0].X-1.5,point3[0].Y-1.5,3,3);
			graphics->DrawLine(&m_PenOrange,point3[0],point3[0]);
		}
		else
		{
			pointf_temp = point3[0];
			for(int i=1;i<point3_idx;i++)
			{
				if((point3[i].X !=0)||(point3[i].Y !=0))
				{
					if(point3_last==1)
					{
						pointf_temp = point3[i];
						//graphics->DrawEllipse( &m_PenOrange,pointf_temp.X-1.5,pointf_temp.Y-1.5,3,3);
					}
					graphics->DrawLine(&m_PenOrange,pointf_temp,point3[i]);
					pointf_temp = point3[i];
					point3_last = 0;
				}
				else
				{
					point3_last = 1;
				}
			}
		}

PPP4:
		delete [] point1;
		delete [] point2;
		delete [] point3;
	}
	catch(CException* e)
	{
		delete [] point1;
		delete [] point2;
		delete [] point3;
		e->Delete();
	}
}

void DrawGraph::Draw_Three(unsigned char* buf, Bitmap* bmp, Graphics* graphics)
{
	POINT PointTem[3];
	//X方向上5个矩形
	for(int i =0;i<5;i++)
	{
		//TriangleA
		PointTem[0].x = 0+128*i;
		PointTem[0].y = 456-1;
		PointTem[1].y = 0;
		PointTem[2].y = 0; 
		for(int j=0;j<3;j++)
		{
			if((buf[2+j*2] !=0)||(buf[3+j*2] !=0))
			{
				PointTem[1].x = buf[2+j*2]+128*i-1;
				PointTem[2].x = buf[3+j*2]+128*i-1;
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleB
		PointTem[0].x = 128+128*i-1;
		PointTem[0].y = 456-1;
		PointTem[1].y = 0;
		PointTem[2].y = 0; 
		for(int j=0;j<3;j++)
		{
			if((buf[9+j*2] !=0)||(buf[10+j*2] !=0))
			{
				PointTem[1].x = buf[9+j*2]+128*i-1;
				PointTem[2].x = buf[10+j*2]+128*i-1;
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleC
		PointTem[0].x = 0+128*i;
		PointTem[0].y = 0;
		PointTem[1].y = 456-1;
		PointTem[2].y = 456-1; 
		for(int j=0;j<3;j++)
		{
			if((buf[0x10+j*2] !=0)||(buf[0x11+j*2] !=0))
			{
				PointTem[1].x = buf[0x10+j*2]+128*i-1;
				PointTem[2].x = buf[0x11+j*2]+128*i-1;
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleD
		PointTem[0].x = 128+128*i-1;
		PointTem[0].y = 0;
		PointTem[1].y = 456-1;
		PointTem[2].y = 456-1; 
		for(int j=0;j<3;j++)
		{
			if((buf[0x17+j*2] !=0)||(buf[0x18+j*2] !=0))
			{
				PointTem[1].x = buf[0x17+j*2]+128*i-1;
				PointTem[2].x = buf[0x18+j*2]+128*i-1;
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		buf += m_pDlg->LineSize;
	}
	//Y方向上3个矩形
	for(int i =3;i>0;i--)
	{
		//TriangleA
		PointTem[0].x = 0;
		PointTem[0].y = 18*4+128*i-1;
		PointTem[1].x = 640-1;
		PointTem[2].x = 640-1; 
		for(int j=0;j<3;j++)
		{
			if((buf[2+j*2] != 0)||(buf[3+j*2] != 0))
			{
				PointTem[1].y = 128*i+18*4-buf[2+j*2];
				PointTem[2].y = 128*i+18*4-buf[3+j*2];
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleB
		PointTem[0].x = 0;
		PointTem[0].y = 128*i-128+18*4;
		PointTem[1].x = 640-1;
		PointTem[2].x = 640-1; 
		for(int j=0;j<3;j++)
		{
			if((buf[9+j*2] !=0)||(buf[10+j*2] !=0))
			{
				PointTem[1].y = 128*i+18*4-buf[9+j*2];
				PointTem[2].y = 128*i+18*4-buf[10+j*2];
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleC
		PointTem[0].x = 640-1;
		PointTem[0].y = 18*4+128*i;;
		PointTem[1].x = 0;
		PointTem[2].x = 0; 
		for(int j=0;j<3;j++)
		{
			if((buf[0x10+j*2] !=0)||(buf[0x11+j*2] !=0))
			{
				PointTem[1].y = 128*i+18*4-buf[0x10+j*2];
				PointTem[2].y = 128*i+18*4-buf[0x11+j*2];
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleD
		PointTem[0].x = 640-1;
		PointTem[0].y = 18*4+128*i-128;
		PointTem[1].x = 0;
		PointTem[2].x = 0; 
		for(int j=0;j<3;j++)
		{
			if((buf[0x17+j*2] !=0)||(buf[0x18+j*2] !=0))
			{
				PointTem[1].y = 128*i+18*4-buf[0x17+j*2];
				PointTem[2].y = 128*i+18*4-buf[0x18+j*2];
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		buf +=  m_pDlg->LineSize;
	}

	//Y方向剩余的18个管

	//TriangleA
	PointTem[0].x = 0;
	PointTem[0].y = 18*4-1;
	PointTem[1].x = 640-1;
	PointTem[2].x = 640-1; 
	for(int j=0;j<3;j++)
	{
		if((buf[2+j*2] != 0)||(buf[3+j*2] != 0))
		{
			PointTem[1].y = 18*4-buf[2+j*2];
			PointTem[2].y = 18*4-buf[3+j*2];
			DrawOnice(PointTem,bmp,graphics);
		}
	}
	//TriangleB
	PointTem[0].x = 0;
	PointTem[0].y = 0;
	PointTem[1].x = 640-1;
	PointTem[2].x = 640-1; 
	for(int j=0;j<3;j++)
	{
		if((buf[9+j*2] !=0)||(buf[10+j*2] !=0))
		{
			PointTem[1].y = 18*4-buf[9+j*2];
			PointTem[2].y = 18*4-buf[10+j*2];
			DrawOnice(PointTem,bmp,graphics);
		}
	}
	//TriangleC
	PointTem[0].x = 640-1;
	PointTem[0].y = 18*4-1;
	PointTem[1].x = 0;
	PointTem[2].x = 0;
	for(int j=0;j<3;j++)
	{
		if((buf[0x10+j*2] !=0)||(buf[0x11+j*2] !=0))
		{
			PointTem[1].y = 18*4-buf[0x10+j*2];
			PointTem[2].y = 18*4-buf[0x11+j*2];
			DrawOnice(PointTem,bmp,graphics);
		}
	}
	//TriangleD
	PointTem[0].x = 640-1;
	PointTem[0].y = 0;
	PointTem[1].x = 0;
	PointTem[2].x = 0;
	for(int j=0;j<3;j++)
	{
		if((buf[0x17+j*2] !=0)||(buf[0x18+j*2] !=0))
		{
			PointTem[1].y = 18*4-buf[0x17+j*2];
			PointTem[2].y = 18*4-buf[0x18+j*2];
			DrawOnice(PointTem,bmp,graphics);
		}
	}
	buf +=  m_pDlg->LineSize;

	if(m_pDlg->m_TowOrThree ==2)	//如果文件中有 菱形数据的话 就不画点了
		return;
	buf -= m_pDlg->LineSize*10;
	for(int j=0;j<3;j++)
	{
		if((buf[7+j*5] !=0)||(buf[8+j*5] !=0)||(buf[9+j*5] !=0)||(buf[10+j*5] !=0))
		{
			PointTem[0].x = ((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1;
			PointTem[0].y = ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00));
			DrawPoint(PointTem,bmp,graphics,j);
		}
	}

}

void DrawGraph::Draw_Two(unsigned char* buf, Bitmap* bmp, Graphics* graphics)
{
	POINT PointTem[3];
	//X方向上5个矩形
	for(int i =0;i<5;i++)
	{
		//TriangleA
		PointTem[0].x = 0+128*i;
		PointTem[0].y = 456-1;
		PointTem[1].y = 0;
		PointTem[2].y = 0; 
		for(int j=0;j<2;j++)
		{
			if((buf[2+j*2] !=0)||(buf[3+j*2] !=0))
			{
				PointTem[1].x = buf[2+j*2]+128*i-1;
				PointTem[2].x = buf[3+j*2]+128*i-1;
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleB
		PointTem[0].x = 128+128*i-1;
		PointTem[0].y = 456-1;
		PointTem[1].y = 0;
		PointTem[2].y = 0; 
		for(int j=0;j<2;j++)
		{
			if((buf[7+j*2] !=0)||(buf[8+j*2] !=0))
			{
				PointTem[1].x = buf[7+j*2]+128*i-1;
				PointTem[2].x = buf[8+j*2]+128*i-1;
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleC
		PointTem[0].x = 0+128*i;
		PointTem[0].y = 0;
		PointTem[1].y = 456-1;
		PointTem[2].y = 456-1; 
		for(int j=0;j<2;j++)
		{
			if((buf[12+j*2] !=0)||(buf[13+j*2] !=0))
			{
				PointTem[1].x = buf[12+j*2]+128*i-1;
				PointTem[2].x = buf[13+j*2]+128*i-1;
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleD
		PointTem[0].x = 128+128*i-1;
		PointTem[0].y = 0;
		PointTem[1].y = 456-1;
		PointTem[2].y = 456-1; 
		for(int j=0;j<2;j++)
		{
			if((buf[17+j*2] !=0)||(buf[18+j*2] !=0))
			{
				PointTem[1].x = buf[17+j*2]+128*i-1;
				PointTem[2].x = buf[18+j*2]+128*i-1;
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		buf +=  m_pDlg->LineSize;
	}
	//Y方向上3个矩形
	for(int i =3;i>0;i--)
	{
		//TriangleA
		PointTem[0].x = 0;
		PointTem[0].y = 18*4+128*i-1;
		PointTem[1].x = 640-1;
		PointTem[2].x = 640-1; 
		for(int j=0;j<2;j++)
		{
			if((buf[2+j*2] != 0)||(buf[3+j*2] != 0))
			{
				PointTem[1].y = 128*i+18*4-buf[2+j*2];
				PointTem[2].y = 128*i+18*4-buf[3+j*2];
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleB
		PointTem[0].x = 0;
		PointTem[0].y = 128*i-128+18*4;
		PointTem[1].x = 640-1;
		PointTem[2].x = 640-1; 
		for(int j=0;j<2;j++)
		{
			if((buf[7+j*2] !=0)||(buf[8+j*2] !=0))
			{
				PointTem[1].y = 128*i+18*4-buf[7+j*2];
				PointTem[2].y = 128*i+18*4-buf[8+j*2];
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleC
		PointTem[0].x = 640-1;
		PointTem[0].y = 18*4+128*i;;
		PointTem[1].x = 0;
		PointTem[2].x = 0; 
		for(int j=0;j<2;j++)
		{
			if((buf[12+j*2] !=0)||(buf[13+j*2] !=0))
			{
				PointTem[1].y = 128*i+18*4-buf[12+j*2];
				PointTem[2].y = 128*i+18*4-buf[13+j*2];
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		//TriangleD
		PointTem[0].x = 640-1;
		PointTem[0].y = 18*4+128*i-128;
		PointTem[1].x = 0;
		PointTem[2].x = 0; 
		for(int j=0;j<2;j++)
		{
			if((buf[17+j*2] !=0)||(buf[18+j*2] !=0))
			{
				PointTem[1].y = 128*i+18*4-buf[17+j*2];
				PointTem[2].y = 128*i+18*4-buf[18+j*2];
				DrawOnice(PointTem,bmp,graphics);
			}
		}
		buf +=  m_pDlg->LineSize;
	}

	//Y方向剩余的18个管

	//TriangleA
	PointTem[0].x = 0;
	PointTem[0].y = 18*4-1;
	PointTem[1].x = 640-1;
	PointTem[2].x = 640-1; 
	for(int j=0;j<2;j++)
	{
		if((buf[2+j*2] != 0)||(buf[3+j*2] != 0))
		{
			PointTem[1].y = 18*4-buf[2+j*2];
			PointTem[2].y = 18*4-buf[3+j*2];
			DrawOnice(PointTem,bmp,graphics);
		}
	}
	//TriangleB
	PointTem[0].x = 0;
	PointTem[0].y = 0;
	PointTem[1].x = 640-1;
	PointTem[2].x = 640-1; 
	for(int j=0;j<2;j++)
	{
		if((buf[7+j*2] !=0)||(buf[8+j*2] !=0))
		{
			PointTem[1].y = 18*4-buf[7+j*2];
			PointTem[2].y = 18*4-buf[8+j*2];
			DrawOnice(PointTem,bmp,graphics);
		}
	}
	//TriangleC
	PointTem[0].x = 640-1;
	PointTem[0].y = 18*4-1;
	PointTem[1].x = 0;
	PointTem[2].x = 0;
	for(int j=0;j<2;j++)
	{
		if((buf[12+j*2] !=0)||(buf[13+j*2] !=0))
		{
			PointTem[1].y = 18*4-buf[12+j*2];
			PointTem[2].y = 18*4-buf[13+j*2];
			DrawOnice(PointTem,bmp,graphics);
		}
	}
	//TriangleD
	PointTem[0].x = 640-1;
	PointTem[0].y = 0;
	PointTem[1].x = 0;
	PointTem[2].x = 0;
	for(int j=0;j<2;j++)
	{
		if((buf[17+j*2] !=0)||(buf[18+j*2] !=0))
		{
			PointTem[1].y = 18*4-buf[17+j*2];
			PointTem[2].y = 18*4-buf[18+j*2];
			DrawOnice(PointTem,bmp,graphics);
		}
	}
	buf +=  m_pDlg->LineSize;

	if(m_pDlg->m_TowOrThree ==2)	//如果文件中有 菱形数据的话 就不画点了
		return;
	buf -= m_pDlg->FrameSize;
	for(int j=0;j<2;j++)
	{
		if((buf[7+j*5] !=0)||(buf[8+j*5] !=0)||(buf[9+j*5] !=0)||(buf[10+j*5] !=0))
		{
			PointTem[0].x = ((buf[8+j*5]&0xff)|((buf[7+j*5]<<8)&0x0000ff00))-1;
			PointTem[0].y = ((buf[10+j*5]&0xff)|((buf[9+j*5]<<8)&0x0000ff00));
			DrawPoint(PointTem,bmp,graphics,j);
		}
	}

}

void DrawGraph::Draw_DiamondPoint(CALC_DIAMOND_BUF* DiamondBuf, Bitmap* bmp, Graphics* graphics)
{
	POINT PointTem;
	int i,j;
	if (m_pDlg->m_nShowDiamondPoint == 0)
		goto SHOW_CALC_POINTTT;
	for(i = 0;i < SCAN_X_SQUARE_NUM; i++)
	{
		if(DiamondBuf->strXSquareDiamond[i].ulDiamondCount ==0)
			continue;
		for(j=0;j<9;j++)
		{
			if(DiamondBuf->strXSquareDiamond[i].strDiamondPoint[j].ulDiamondNum == 0)
				continue;
			PointTem.x = CALCL_REL_TO_ABS(DIAMOND_X_GET_P(DiamondBuf,i,j,0).x,i);
			PointTem.y = DIAMOND_X_GET_P(DiamondBuf,i,j,0).y;
			DrawPoint(&PointTem , bmp, graphics,0x1e );
			PointTem.x = CALCL_REL_TO_ABS(DIAMOND_X_GET_P(DiamondBuf,i,j,1).x,i);
			PointTem.y = DIAMOND_X_GET_P(DiamondBuf,i,j,1).y;
			DrawPoint(&PointTem , bmp, graphics,0x1e );
			PointTem.x = CALCL_REL_TO_ABS(DIAMOND_X_GET_P(DiamondBuf,i,j,2).x,i);
			PointTem.y = DIAMOND_X_GET_P(DiamondBuf,i,j,2).y;
			DrawPoint(&PointTem , bmp, graphics,0x1e );
			PointTem.x = CALCL_REL_TO_ABS(DIAMOND_X_GET_P(DiamondBuf,i,j,3).x,i);
			PointTem.y = DIAMOND_X_GET_P(DiamondBuf,i,j,3).y;
			DrawPoint(&PointTem , bmp, graphics,0x1e );
		}
	}

	for(i = 0;i < SCAN_Y_SQUARE_NUM; i++)
	{
		if(DiamondBuf->strYSquareDiamond[i].ulDiamondCount ==0)
			continue;
		for(j=0;j<9;j++)
		{
			if(DiamondBuf->strYSquareDiamond[i].strDiamondPoint[j].ulDiamondNum == 0)
				continue;
			PointTem.x = DIAMOND_Y_GET_P(DiamondBuf,i,j,0).x;
			PointTem.y =CALCL_REL_TO_ABS(DIAMOND_Y_GET_P(DiamondBuf,i,j,0).y,i);
			DrawPoint(&PointTem , bmp, graphics,0x1f );
			PointTem.x = DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x;
			PointTem.y =CALCL_REL_TO_ABS(DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y,i);
			DrawPoint(&PointTem , bmp, graphics,0x1f );
			PointTem.x = DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x;
			PointTem.y =CALCL_REL_TO_ABS(DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y,i);
			DrawPoint(&PointTem , bmp, graphics,0x1f);
			PointTem.x = DIAMOND_Y_GET_P(DiamondBuf,i,j,3).x;
			PointTem.y =CALCL_REL_TO_ABS(DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y,i);
			DrawPoint(&PointTem , bmp, graphics,0x1f );
		}
	}
SHOW_CALC_POINTTT:
	struct PT_BUF point[20];
	int pointnum = CalcPoint(DiamondBuf,point);
	POINT poi;
	for(i = 0;i<pointnum ;i ++)
	{
		poi.x =point[i].pt_val.x;
		poi.y =point[i].pt_val.y;
		DrawPoint(&poi,bmp,graphics,point[i].id - 1);
	}
	
}

int DrawGraph::DrawTriangle(unsigned char* buf)
{

	if (buf == NULL)
		return 0;

	CString str;
	str.Format("mB%f %d %d %d %d",m_pDlg->m_fMakeBig,m_pDlg->m_Rect_BitWindow.Width(),m_pDlg->m_Rect_BitWindow.Height(),m_pDlg->m_BigPoint.x,m_pDlg->m_BigPoint.y);
	m_pDlg->GetDlgItem(IDC_STATIC_STATUS)->SetWindowTextA(str);
	m_pDlg->GetDlgItem(IDC_STATIC_STATUS)->RedrawWindow();
	Bitmap bmp((int)((m_pDlg->m_Rect_BitWindow.Width())*m_pDlg->m_fMakeBig),	
		(int)(m_pDlg->m_Rect_BitWindow.Height() *m_pDlg->m_fMakeBig));
	Graphics* graphics = Graphics::FromImage(&bmp);
	graphics->SetSmoothingMode(SmoothingModeHighQuality);
	if(m_pDlg->m_TowOrThree ==0)
	{
		Draw_Two(buf,&bmp,graphics);
	}
	else if(m_pDlg->m_TowOrThree ==1)
	{
		Draw_Three(buf,&bmp,graphics);
	}
	else if(m_pDlg->m_TowOrThree ==2)
	{
		Draw_Three(buf,&bmp,graphics);
		Draw_DiamondPoint(m_pDlg->m_sDiamond_buf,&bmp,graphics);
	}

	Graphics g(m_pDlg->m_dc->GetSafeHdc());	

	g.DrawImage(&bmp,
		0,0,
		m_pDlg->m_BigPoint.x,m_pDlg->m_BigPoint.y,
		m_pDlg->m_Rect_BitWindow.Width(),
		m_pDlg->m_Rect_BitWindow.Height(),
		UnitPixel);
	delete graphics;

	return 0;
}

void DrawGraph::DrawOnice(POINT* point,Bitmap* bmp,Graphics* graphics )
{
	PointF pointf[3];

	for(int i = 0;i<3;i++)
	{
		pointf[i].X = (point[i].x*m_pDlg->m_Rect_BitWindow.Width()*m_pDlg->m_fMakeBig/640) ;
		pointf[i].Y = (point[i].y*m_pDlg->m_Rect_BitWindow.Height()* m_pDlg->m_fMakeBig/456) ;
	}

	graphics->FillPolygon(&m_BrushBlue,pointf,3,FillModeAlternate);	

	graphics->DrawLine(&m_PenBlack,pointf[0],pointf[1]);
	graphics->DrawLine(&m_PenBlack,pointf[1],pointf[2]);
	graphics->DrawLine(&m_PenBlack,pointf[2],pointf[0]);
	//CString str;
	//str.Format("%f %f %f %f %f %f",pointf[0].X,pointf[0].Y,pointf[1].X,pointf[1].Y,pointf[2].X,pointf[2].Y);
	//MessageBox(str);
}

void DrawGraph::DrawPoint(POINT* point,Bitmap* bmp,Graphics* graphics,int order )
{
	SolidBrush *brush;
	point->y = 456 - point->y; //两种坐标转换
	int x = (int)(point[0].x*m_pDlg->m_Rect_BitWindow.Width()/640 * m_pDlg->m_fMakeBig)-POINT_SIZE_FINALLY/2;
	int y = (int)(point[0].y*m_pDlg->m_Rect_BitWindow.Height()/456 * m_pDlg->m_fMakeBig)-POINT_SIZE_FINALLY/2;
	switch(order&0x0000000f)
	{
	case 0x0: brush = &m_BrushRed; break;
	case 0x1: brush = &m_BrushGreen; break;
	case 0x2: brush = &m_BrushOrange; break;
	case 0x3: brush = &m_BrushRR; break;
	case 0xe: brush = &m_BrushYellow; break;	//
	case 0xf: brush = &m_BrushGray; break;	//
	default: brush = &m_BrushBlack; break;
	}
	if ((order&0x10) == 0x10)
	{
		graphics->FillEllipse(brush,x,y,POINT_SIZE_DIAMOND,POINT_SIZE_DIAMOND);
	}
	else
	{
		graphics->FillEllipse(brush,x,y,POINT_SIZE_FINALLY,POINT_SIZE_FINALLY);
	}
	

}
