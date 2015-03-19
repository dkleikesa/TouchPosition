#ifdef  __CC_ARM
#include "whiteboard.h"
#endif

#include "CalcPoint.h"

PT_STATUS_L g_PointStatus;	//ɨ���ID��״̬

void ClearPointID(void)
{
	memset(&g_PointStatus,0,sizeof(PT_STATUS_L));
}

//��ȡ��ǰС�ھ�����ֵ��λ��
static __INLINE signed int GetThresholdPos(CALC_DISTANCE *arr,unsigned int len,unsigned int threshold)
{
	signed int i = 0;
	for(i=0;i<len;i++)
	{
		if (arr[i].distance >= threshold)
		{
			return i;
		}
	}
	return i;
}
//��ȡ�������ĵ�
static __INLINE void CalclDiamondCentre(CALC_DIAMOND *a,CALC_POINT *b,unsigned int xOry,unsigned int index)
{
	if(xOry == 0)
	{
		b->x = CALCL_REL_TO_ABS(CALCL_X_DIAMOND_CENTRE_X(*a),index);
		b->y = CALCL_X_DIAMOND_CENTRE_Y(*a);
	}
	else
	{
		b->x = CALCL_Y_DIAMOND_CENTRE_X(*a);
		b->y = CALCL_REL_TO_ABS(CALCL_Y_DIAMOND_CENTRE_Y(*a),index);
	}

}

//��ȡ�������ĵ�
static __INLINE void CalclDiamondCentreEdge(CALC_DIAMOND *a,CALC_POINT *b,unsigned int xOry,unsigned int index)
{
	if(xOry == 0)
	{
		b->x = CALCL_REL_TO_ABS(CALCL_X_DIAMOND_CENTRE_X_E(*a),index);
		b->y = CALCL_X_DIAMOND_CENTRE_Y(*a);
	}
	else
	{
		b->x = CALCL_Y_DIAMOND_CENTRE_X(*a);
		b->y = CALCL_REL_TO_ABS(CALCL_Y_DIAMOND_CENTRE_Y_E(*a),index);
	}

}

//��ȡ������
//Ϊ�˽�ʡ������ ����ƽ�� ����ʾ����
static __INLINE unsigned int CalclDistance(CALC_POINT *a,CALC_POINT *b)
{
	return CALCL_SQUAR((a->x) - (b->x)) + CALCL_SQUAR((a->y)-(b->y));
}
#ifndef  __CC_ARM
static __INLINE unsigned int CalclDistance_Short(POINT_C *a,POINT_C *b)
#else
static __INLINE unsigned int CalclDistance_Short(struct POINT *a,struct POINT *b)
#endif
{
	return CALCL_SQUAR((a->x) - (b->x)) + CALCL_SQUAR((a->y)-(b->y));
}

static __INLINE unsigned short  AdjustXRec_X (CALC_DIAMOND* p,signed int y0,unsigned char rec)
{
	if ((((CALC_DIAMOND*)p)->strPoint0.y - ((CALC_DIAMOND*)p)->strPoint3.y) == 0)
	{
		return (unsigned short)CALCL_REL_TO_ABS(CALCL_X_DIAMOND_CENTRE_X(*p),rec);
	}
	return (unsigned short)CALCL_REL_TO_ABS(ADJUST_XREC_X_REL(p,y0),rec);
}
static __INLINE int  AdjustXRec_X_L (CALC_DIAMOND* p,signed int y0)
{
	if ((((CALC_DIAMOND*)p)->strPoint0.y - ((CALC_DIAMOND*)p)->strPoint3.y) == 0)
	{
		return CALCL_X_DIAMOND_CENTRE_X(*p);
	}
	return ADJUST_XREC_X_REL(p,y0);
}

static __INLINE unsigned short  AdjustYRec_Y(CALC_DIAMOND* p,signed int x0,unsigned char rec) 
{
	if ((((CALC_DIAMOND*)p)->strPoint3.x - ((CALC_DIAMOND*)p)->strPoint0.x) == 0)
	{
		return (unsigned short) CALCL_REL_TO_ABS(CALCL_Y_DIAMOND_CENTRE_Y(*p),rec);
	}
	return (unsigned short)CALCL_REL_TO_ABS(ADJUST_YREC_Y_REL(p,x0),rec);
}
static __INLINE int  AdjustYRec_Y_L(CALC_DIAMOND* p,signed int x0 ) 
{
	if ((((CALC_DIAMOND*)p)->strPoint3.x - ((CALC_DIAMOND*)p)->strPoint0.x) == 0)
	{
		return  CALCL_Y_DIAMOND_CENTRE_Y(*p);
	}
	return ADJUST_YREC_Y_REL(p,x0);
}

static __INLINE void GetNumOne(signed char* dis,signed int k)  
{
	while (k >0) 
	{
		k--;
		dis[k] = 1;
	}
}
static __INLINE void ClearPos(CACL_PROJECTION_COUNT* dis,unsigned int k)  
{

	char tem ;
	tem = (char)(k>>8);
	if (tem == 2)
	{
		dis->CPos[(char)k] = 0;
	}
	else if (tem == 3)
	{
		dis->BPos[(char)k] = 0;
	}
	tem = (char)(k>>24);
	if (tem == 0)
	{
		dis->APos[(char)k] = 0;
	}
	else if (tem == 1)
	{
		dis->DPos[((char)(k>>16))] = 0;
	}
}


// ��ӡ������������
//DiamondBuf: ��������ָ��
//OutBuf�� �����buff
//ScanCount: ��ǰ���ݱ��
void PrintDiamond(CALC_DIAMOND_BUF *DiamondBuf,char *OutBuf,unsigned int ScanCount)
{
	unsigned int n=0;
	unsigned int i,j;
	n = sprintf(OutBuf,"scan count = 0x%x\r\n",ScanCount);
	n += sprintf(OutBuf+n,"------------------------------------------------------------------------------\r\n");
	n += sprintf(OutBuf+n,"XSquareNum     Sum  DiamondNum    point0      point1      point2      point3\r\n");
	for(i = 0;i < SCAN_X_SQUARE_NUM; i++)
	{
		if(DiamondBuf->strXSquareDiamond[i].ulDiamondCount ==0)
			continue;
		for(j=0;j<DiamondBuf->strXSquareDiamond[i].ulDiamondCount;j++)
		{
			n += sprintf(OutBuf+n,"%8d%9d   0x%08x (%4d,%4d) (%4d,%4d) (%4d,%4d) (%4d,%4d)\r\n",i,
				DIAMOND_X_GET_C(DiamondBuf,i),DIAMOND_X_GET_N(DiamondBuf,i,j),
				CALCL_REL_TO_ABS(DIAMOND_X_GET_P(DiamondBuf,i,j,0).x,i),DIAMOND_X_GET_P(DiamondBuf,i,j,0).y,
				CALCL_REL_TO_ABS(DIAMOND_X_GET_P(DiamondBuf,i,j,1).x,i),DIAMOND_X_GET_P(DiamondBuf,i,j,1).y,
				CALCL_REL_TO_ABS(DIAMOND_X_GET_P(DiamondBuf,i,j,2).x,i),DIAMOND_X_GET_P(DiamondBuf,i,j,2).y,
				CALCL_REL_TO_ABS(DIAMOND_X_GET_P(DiamondBuf,i,j,3).x,i),DIAMOND_X_GET_P(DiamondBuf,i,j,3).y);
		}
	}
	n += sprintf(OutBuf+n,"------------------------------------------------------------------------------\r\n");
	n += sprintf(OutBuf+n,"YSquareNum     Sum  DiamondNum    point0      point1      point2      point3\r\n");
	for(i = 0;i < SCAN_Y_SQUARE_NUM; i++)
	{
		if(DiamondBuf->strYSquareDiamond[i].ulDiamondCount ==0)
			continue;
		for(j=0;j<DiamondBuf->strYSquareDiamond[i].ulDiamondCount;j++)
		{
			n += sprintf(OutBuf+n,"%8d%9d   0x%08x (%4d,%4d) (%4d,%4d) (%4d,%4d) (%4d,%4d)\r\n",i,
				DIAMOND_Y_GET_C(DiamondBuf,i),DIAMOND_Y_GET_N(DiamondBuf,i,j),
				DIAMOND_Y_GET_P(DiamondBuf,i,j,0).x,CALCL_REL_TO_ABS(DIAMOND_Y_GET_P(DiamondBuf,i,j,0).y,i),
				DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x,CALCL_REL_TO_ABS(DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y,i),
				DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x,CALCL_REL_TO_ABS(DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y,i),
				DIAMOND_Y_GET_P(DiamondBuf,i,j,3).x,CALCL_REL_TO_ABS(DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y,i));
		}
	}
	n += sprintf(OutBuf+n,"------------------------------------------------------------------------------\r\n");
}

//���㴥����
//DiamondBuf: ��������ָ��	
//point: �õ����������ݻ���
//����ֵ��������ĸ���
int CalcPoint(CALC_DIAMOND_BUF *DiamondBuf,struct PT_BUF *point)
{
	unsigned int i,j,k,m,xDiamondNum,yDiamondNum,disNUM,disNUM_f,PointNum;
	signed int xPointNum[SCAN_X_SQUARE_NUM],xPointNumSum;
	signed int yPointNum[SCAN_Y_SQUARE_NUM],yPointNumSum;
	signed int PointNumTmp,PointNumTmp_t;
	signed int PosTem;
	signed int xSure, ySure;
	signed int DiamondNumTmp;
	signed int tmp;
	signed char xNeedSub[SCAN_X_SQUARE_NUM],yNeedSub[SCAN_Y_SQUARE_NUM];
	CACL_DIAMOND_CENTER XPoint[MAX_POINT_REC*MAX_POINT_REC*SCAN_X_SQUARE_NUM];	//x�������� ���ĵ�buff
	CACL_DIAMOND_CENTER YPoint[MAX_POINT_REC*MAX_POINT_REC*SCAN_Y_SQUARE_NUM]; //y�������� ���ĵ�buff
	CALC_DISTANCE DistanceTmp[MAX_POINT_REC*MAX_POINT_REC*SCAN_X_SQUARE_NUM*MAX_POINT_REC*MAX_POINT_REC*SCAN_Y_SQUARE_NUM];	//���ĵ���buff
	CACL_PROJECTION_COUNT XProjectionCount[SCAN_X_SQUARE_NUM];	//X����ÿ�����������ÿ������������ͶӰ�ĸ���
	CACL_PROJECTION_COUNT YProjectionCount[SCAN_Y_SQUARE_NUM];	//Y����ÿ�����������ÿ������������ͶӰ�ĸ���
	xDiamondNum = 0;	//x������Ч���� ���ĵ����
	yDiamondNum = 0;	//y������Ч���� ���ĵ����
	disNUM = 0;	//���ĵ������
	disNUM_f = 0;
	PointNum = 0;	//���յ�ĸ���
	xPointNumSum = 0;
	yPointNumSum = 0;
	xSure = 0;
	ySure = 0;
	DiamondNumTmp = 0;
	PointNumTmp_t = 0;	//����ϸ�������ҳ��������е�ĸ���
	PointNumTmp = 0; //�������ĵ�����������Ĵ���������� ���ܴ��ڼٵ�����ٵ�
	memset(XProjectionCount,0xff,sizeof(XProjectionCount));	//x�����������������ʵ�ʵ����
	memset(YProjectionCount,0xff,sizeof(YProjectionCount));	//y�������μ��������ʵ�ʵ����
	memset(xNeedSub,0,sizeof(xNeedSub));
	memset(yNeedSub,0,sizeof(yNeedSub));
	//���������X�����������ĵ�
	for(i = 0;i < SCAN_X_SQUARE_NUM; i++)
	{	
		if(DIAMOND_X_GET_C(DiamondBuf,i) ==0)
			continue;

		for(j=0;j<DIAMOND_X_GET_C(DiamondBuf,i);j++)
		{
			if(DIAMOND_X_GET_N(DiamondBuf,i,j) == 0)
				continue;
			//����AD�����������ڷֱ��м���ͶӰ
			switch (DIAMOND_X_GET_N(DiamondBuf,i,j)&0xff000000)
			{
			case 0x00000000: 
				XProjectionCount[i].ACount = 
					max(XProjectionCount[i].ACount,(char)(DIAMOND_X_GET_N(DiamondBuf,i,j)>>16));
				break;
			case 0x01000000:
				XProjectionCount[i].DCount = 
					max(XProjectionCount[i].DCount,(char)(DIAMOND_X_GET_N(DiamondBuf,i,j)>>16));
				break;
			}
			//����BC�����������ڷֱ��м���ͶӰ
			switch (DIAMOND_X_GET_N(DiamondBuf,i,j)&0x0000ff00)
			{
			case 0x00000200:
				XProjectionCount[i].CCount = 
					max(XProjectionCount[i].CCount,(char)(DIAMOND_X_GET_N(DiamondBuf,i,j)));
				break;
			case 0x00000300:
				XProjectionCount[i].BCount = 
					max(XProjectionCount[i].BCount,(char)(DIAMOND_X_GET_N(DiamondBuf,i,j)));
				break;
			}

			if((DIAMOND_X_GET_P(DiamondBuf,i,j,1).y ==0) || (DIAMOND_X_GET_P(DiamondBuf,i,j,1).y ==TATAL_HEIGHT) ||			//������
				(DIAMOND_X_GET_P(DiamondBuf,i,j,2).y ==0)|| (DIAMOND_X_GET_P(DiamondBuf,i,j,2).y ==TATAL_HEIGHT))
			{
				if(DIAMOND_X_GET_P(DiamondBuf,i,j,0).x == DIAMOND_X_GET_P(DiamondBuf,i,j,3).x)	//�߽�������
				{

					if (DIAMOND_X_GET_P(DiamondBuf,i,j,2).y == DIAMOND_X_GET_P(DiamondBuf,i,j,3).y)
					{
						DIAMOND_X_GET_P(DiamondBuf,i,j,2).x = DIAMOND_X_GET_P(DiamondBuf,i,j,0).x;
						DIAMOND_X_GET_P(DiamondBuf,i,j,2).y = DIAMOND_X_GET_P(DiamondBuf,i,j,1).y;
					}
					else if(DIAMOND_X_GET_P(DiamondBuf,i,j,1).y == DIAMOND_X_GET_P(DiamondBuf,i,j,3).y)
					{
						DIAMOND_X_GET_P(DiamondBuf,i,j,1).x = DIAMOND_X_GET_P(DiamondBuf,i,j,0).x;
						DIAMOND_X_GET_P(DiamondBuf,i,j,1).y = DIAMOND_X_GET_P(DiamondBuf,i,j,2).y;
					}
					CalclDiamondCentreEdge(&(DiamondBuf->strXSquareDiamond[i].strDiamondPoint[j]),&XPoint[xDiamondNum].Point,0,i);
				}
				else	//�����ڵ�������
				{
					if ((DIAMOND_X_GET_P(DiamondBuf,i,j,1).y ==0)||(DIAMOND_X_GET_P(DiamondBuf,i,j,1).y ==TATAL_HEIGHT))
					{
						XPoint[xDiamondNum].Point.y = DIAMOND_X_GET_P(DiamondBuf,i,j,2).y;
						XPoint[xDiamondNum].Point.x = CALCL_AVERAGE_TWO( DIAMOND_X_GET_P(DiamondBuf,i,j,2).x,
							AdjustXRec_X_L(&DIAMOND_X_GET_D(DiamondBuf,i,j), DIAMOND_X_GET_P(DiamondBuf,i,j,2).y));
					}
					if ((DIAMOND_X_GET_P(DiamondBuf,i,j,2).y ==0)|| (DIAMOND_X_GET_P(DiamondBuf,i,j,2).y ==TATAL_HEIGHT))
					{
						XPoint[xDiamondNum].Point.y = DIAMOND_X_GET_P(DiamondBuf,i,j,1).y;
						XPoint[xDiamondNum].Point.x  = CALCL_AVERAGE_TWO(DIAMOND_X_GET_P(DiamondBuf,i,j,1).x , 
							AdjustXRec_X_L(&DIAMOND_X_GET_D(DiamondBuf,i,j), DIAMOND_X_GET_P(DiamondBuf,i,j,1).y));
					}

					XPoint[xDiamondNum].Point.x = CALCL_REL_TO_ABS(XPoint[xDiamondNum].Point.x,i);
				}
			}
			else	//����
			{
				if ((DIAMOND_X_GET_P(DiamondBuf,i,j,3).y ==0)&&(DIAMOND_X_GET_P(DiamondBuf,i,j,0).y ==TATAL_HEIGHT)) //�߽�����
				{
					CalclDiamondCentreEdge(&(DiamondBuf->strXSquareDiamond[i].strDiamondPoint[j]),&XPoint[xDiamondNum].Point,0,i);
				}
				else	//����������
				{
					CalclDiamondCentre(&(DiamondBuf->strXSquareDiamond[i].strDiamondPoint[j]),&XPoint[xDiamondNum].Point,0,i);
				}

			}
			tmp = LENGTH_PRE;
			if ((DIAMOND_X_GET_P(DiamondBuf,i,j,2).x > tmp)||((DIAMOND_X_GET_P(DiamondBuf,i,j,0).x==tmp)&&(DIAMOND_X_GET_P(DiamondBuf,i,j,3).x==tmp)))
			{
				xNeedSub[i]++;
			}


			XPoint[xDiamondNum].Rec = i;
			XPoint[xDiamondNum].Diamond = j;
			if (XPoint[xDiamondNum].Point.x > TOTAL_LENGTH|| XPoint[xDiamondNum].Point.y>TATAL_HEIGHT)
			{
				continue;
			}
			if (XPoint[xDiamondNum].Point.x < 0|| XPoint[xDiamondNum].Point.y < 0)
			{
				continue;
			}
			xDiamondNum++;
		}
	}

	//���������Y�����������ĵ�
	for(i = 0;i < SCAN_Y_SQUARE_NUM; i++)
	{
		if(DIAMOND_Y_GET_C(DiamondBuf,i) ==0)
			continue;
		for(j=0;j<DIAMOND_Y_GET_C(DiamondBuf,i);j++)
		{
			if(DIAMOND_Y_GET_N(DiamondBuf,i,j) == 0)
				continue;

			//����AD�����������ڷֱ��м���ͶӰ
			switch (DIAMOND_Y_GET_N(DiamondBuf,i,j)&0xff000000)
			{
			case 0x00000000: 
				YProjectionCount[i].ACount = 
					max(YProjectionCount[i].ACount,(char)(DIAMOND_Y_GET_N(DiamondBuf,i,j)>>16));
				break;
			case 0x01000000:
				YProjectionCount[i].DCount = 
					max(YProjectionCount[i].DCount,(char)(DIAMOND_Y_GET_N(DiamondBuf,i,j)>>16));
				break;
			}
			//����BC�����������ڷֱ��м���ͶӰ
			switch (DIAMOND_Y_GET_N(DiamondBuf,i,j)&0x0000ff00)
			{
			case 0x00000200:
				YProjectionCount[i].CCount = 
					max(YProjectionCount[i].CCount,(char)(DIAMOND_Y_GET_N(DiamondBuf,i,j)));
				break;
			case 0x00000300:
				YProjectionCount[i].BCount = 
					max(YProjectionCount[i].BCount,(char)(DIAMOND_Y_GET_N(DiamondBuf,i,j)));
				break;
			}

			if((DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x ==0) || (DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x ==TOTAL_LENGTH) ||			//������
				(DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x ==0)|| (DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x ==TOTAL_LENGTH))
			{
				if(DIAMOND_Y_GET_P(DiamondBuf,i,j,0).y == DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y)	//�߽�������
				{

					if (DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y == DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y)
					{
						DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y = DIAMOND_Y_GET_P(DiamondBuf,i,j,0).y;
						DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x = DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x;
					}
					else if(DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y == DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y)
					{
						DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y = DIAMOND_Y_GET_P(DiamondBuf,i,j,0).y;
						DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x = DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x;
					}
					CalclDiamondCentreEdge(&(DiamondBuf->strYSquareDiamond[i].strDiamondPoint[j]),&YPoint[yDiamondNum].Point,1,i);
				}
				else	//�����ڵ�������
				{
					if ((DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x ==0)||(DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x ==TOTAL_LENGTH))
					{
						YPoint[yDiamondNum].Point.x = DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x;
						YPoint[yDiamondNum].Point.y = CALCL_AVERAGE_TWO(DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y ,
							AdjustYRec_Y_L(&DIAMOND_Y_GET_D(DiamondBuf,i,j), DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x));
					}
					if ((DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x ==0)|| (DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x ==TOTAL_LENGTH))
					{
						YPoint[yDiamondNum].Point.x = DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x;
						YPoint[yDiamondNum].Point.y = CALCL_AVERAGE_TWO(DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y,
							AdjustYRec_Y_L(&DIAMOND_Y_GET_D(DiamondBuf,i,j), DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x));
					}
					YPoint[yDiamondNum].Point.y = CALCL_REL_TO_ABS(YPoint[yDiamondNum].Point.y,i);
				}
			}
			else//����
			{
				if ((DIAMOND_Y_GET_P(DiamondBuf,i,j,3).x ==0)||(DIAMOND_Y_GET_P(DiamondBuf,i,j,0).x ==TOTAL_LENGTH))
				{
					CalclDiamondCentreEdge(&(DiamondBuf->strYSquareDiamond[i].strDiamondPoint[j]),&YPoint[yDiamondNum].Point,1,i);
				}
				else
				{
					CalclDiamondCentre(&(DiamondBuf->strYSquareDiamond[i].strDiamondPoint[j]),&YPoint[yDiamondNum].Point,1,i);
				}

			}
			tmp = LENGTH_PRE;
			if ((DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y > tmp)||((DIAMOND_Y_GET_P(DiamondBuf,i,j,0).y==tmp)&&(DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y==tmp)))
			{
				yNeedSub[i]++;
			}

			YPoint[yDiamondNum].Rec = i;
			YPoint[yDiamondNum].Diamond = j;
			if (YPoint[yDiamondNum].Point.x > TOTAL_LENGTH|| YPoint[yDiamondNum].Point.y>TATAL_HEIGHT)
			{
				continue;
			}
			if (YPoint[yDiamondNum].Point.x < 0 || YPoint[yDiamondNum].Point.y < 0)
			{
				continue;
			}
			yDiamondNum++;
		}
	}

	/*for(i = 0; i<xDiamondNum ;i ++)
	{
	point[i].pt_val.x = XPoint[i].Point.x;
	point[i].pt_val.y = XPoint[i].Point.y;

	}
	j = 0;
	for(;i<(yDiamondNum+xDiamondNum);i++)
	{
	point[i].pt_val.x = YPoint[j].Point.x;
	point[i].pt_val.y = YPoint[j].Point.y;
	j++;
	}
	return yDiamondNum+xDiamondNum;*/
	//������������ĵ���
	for(i = 0; i<xDiamondNum ;i ++)
	{
		for(j = 0;j<yDiamondNum;j++)
		{
			DistanceTmp[disNUM].distance = CalclDistance(&XPoint[i].Point,&YPoint[j].Point);
			DistanceTmp[disNUM].XPos = (char)i;
			DistanceTmp[disNUM].YPos = (char)j;
			disNUM++;
		}
	}
	if(disNUM ==0)
		goto CALC_POINT_END;

	disNUM_f = disNUM;
	memset(xPointNum,0,sizeof(xPointNum));	//x�����������������ʵ�ʵ����
	memset(yPointNum,0,sizeof(yPointNum));	//y�������μ��������ʵ�ʵ����
	for(i = 0;i < SCAN_X_SQUARE_NUM; i++)
	{
		xPointNum[i] = max((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2),
			(XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2));
		if (i>=1)
		{
			if (xNeedSub[i-1]>0)
			{
				if ( xPointNum[i]>0)
				{
					xPointNum[i]--;
				}
			}
		}
		xPointNumSum += xPointNum[i];
		for (j=0;j<3;j++)
		{
			XProjectionCount[i].APos[j]=0;
			XProjectionCount[i].DPos[j]=0;
			XProjectionCount[i].CPos[j]=0;
			XProjectionCount[i].BPos[j]=0;
		}
		GetNumOne(XProjectionCount[i].APos,XProjectionCount[i].ACount+1);
		GetNumOne(XProjectionCount[i].DPos,XProjectionCount[i].DCount+1);
		GetNumOne(XProjectionCount[i].CPos,XProjectionCount[i].CCount+1);
		GetNumOne(XProjectionCount[i].BPos,XProjectionCount[i].BCount+1);
	}
	for(i = 0;i < SCAN_Y_SQUARE_NUM; i++)
	{
		yPointNum[i] =  max((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2),
			(YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2));

		if (i>=1)
		{
			if (yNeedSub[i-1]>0)
			{
				if ( yPointNum[i]>0)
				{
					yPointNum[i]--;
				}
			}
		}
		yPointNumSum += yPointNum[i];
		for (j=0;j<3;j++)
		{
			YProjectionCount[i].APos[j]=0;
			YProjectionCount[i].DPos[j]=0;
			YProjectionCount[i].CPos[j]=0;
			YProjectionCount[i].BPos[j]=0;
		}

		GetNumOne(YProjectionCount[i].APos,YProjectionCount[i].ACount+1);
		GetNumOne(YProjectionCount[i].DPos,YProjectionCount[i].DCount+1);
		GetNumOne(YProjectionCount[i].CPos,YProjectionCount[i].CCount+1);
		GetNumOne(YProjectionCount[i].BPos,YProjectionCount[i].BCount+1);
	}

	PointNum = ((xPointNumSum>yPointNumSum) ? xPointNumSum : yPointNumSum);	//������ʵ�ʵ���Ӧ���������е� �ϴ�ֵ

	if (xPointNumSum == PointNum )
	{
		xSure= 1;
	}
	else
	{
		j = 0;
		for(i = 0;i < SCAN_X_SQUARE_NUM; i++)
		{
			if (xPointNum[i] == 0)
			{
				j++;
			}
			else
			{
				k = i;
			}
		}
		if (j == (SCAN_X_SQUARE_NUM -1) )
		{
			xSure = 1;
			xPointNum[k] = PointNum;
		}
	}
	if (yPointNumSum ==PointNum )
	{
		ySure= 1;
	}
	else
	{
		j = 0;
		for(i = 0;i < SCAN_Y_SQUARE_NUM; i++)
		{
			if (yPointNum[i] == 0)
			{
				j++;
			}
			else
			{
				k = i;
			}
		}
		if (j == (SCAN_Y_SQUARE_NUM -1) )
		{
			ySure= 1;
			yPointNum[k] = PointNum;
		}
	}

	shell_sort(DistanceTmp,disNUM);	//����
	PointNumTmp = GetThresholdPos(DistanceTmp, disNUM,DISTANCE_THRESHOLD);	//��ȡ�����
	PRINTFF("xPoint=%d yPoint=%d PointNum=%d PointNumTmp=%d\r\n",xPointNumSum,yPointNumSum,PointNum,PointNumTmp);
	/*3175-3375 ���*/
	/*2f75 �ٵ���ͨ�������жϵ���� ��Ҫͨ��ͶӰ��ȷ��*/ 
	/*2c75 �ٵ�ͨ���Ҿ�������жϳ���ȷ��*/
	/*2575 ���θ���*/
RESTART_LESS:
	//���������ɣ����ܻ��ٵ� ͨ��������ֵ�� �����
	if (PointNumTmp < PointNum)	
	{
		while ((DistanceTmp[PointNumTmp].distance < DISTANCE_THRESHOLD_WC) && (PointNumTmp < PointNum)&&(PointNumTmp<disNUM)) 
		{
			PointNumTmp++;	//ȷʵ�ٵ� �ǾͰ��� �ٵ�������
		}
	}
	PointNumTmp_t = PointNumTmp;

	//goto COPY_POINT;
	for(i = 0;i < SCAN_X_SQUARE_NUM; i++)  
	{
		if(xPointNum[i] ==0)	//û�е� �Թ�
			continue;
		//if ((PointNum == 3)&&(DIAMOND_X_GET_C(DiamondBuf,i)==4))//3��4��ͶӰʱ �п��ܴ��ڹ������� Ҳ���ܲ�����Ĭ�ϴ��� 
		//{
		//	continue;
		//}
		if((xSure == 1)||(xPointNum[i] == 3))
			//if(xSure == 1)
		{

			if(((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2) > xPointNum[i]) ||// AD����������ͶӰ�������ڵ������ô �����һ������֡
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2) > xPointNum[i])) // BC����������ͶӰ�������ڵ���� ����һ�����������֡
			{
				continue;
			}

			//3��� ������ 2��� ������ �������ظ������� ���ظ���ͶӰ
			if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2)==xPointNum[i])&&
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2)==xPointNum[i])) 
			{
				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//ɾ��ʹ��ͬһ�����ε�����
						if (DistanceTmp[j].XPos == DistanceTmp[k].XPos)
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����

						if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//�������ͬһ���������� û�пɱ���
							continue;

						if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) & 0xffff0000) == //ɾ��AD���������������ظ�ʹ��ͶӰ�ĵĵ�
							(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) & 0xffff0000))
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) &0x0000ffff) ==  //ɾ��BC���������������ظ�ʹ��ͶӰ�ĵĵ�
							(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) &0x0000ffff))
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

					}
				}
			}

			//3��� ������ 2��� ������ �������ظ������� AD�����ظ���ͶӰ BC ����ÿ��ͶӰ���е�
			if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2)==xPointNum[i])&&
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2) < xPointNum[i])) 
			{
				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//ɾ��ʹ��ͬһ�����ε�����
						if (DistanceTmp[j].XPos == DistanceTmp[k].XPos)
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����

						if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//�������ͬһ���������� û�пɱ���
							continue;

						if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) & 0xffff0000) == //ɾ��AD���������������ظ�ʹ��ͶӰ�ĵĵ�
							(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) & 0xffff0000))
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						//ÿ��ͶӰ���е� ����������

					}
				}
			}

			//3��� ������ 2��� һ���� �������ظ������� BC�����ظ���ͶӰ AD����ÿ��ͶӰ���е�
			if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2) < xPointNum[i])&&
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2)  == xPointNum[i]))
			{
				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//ɾ��ʹ��ͬһ�����ε�����
						if (DistanceTmp[j].XPos == DistanceTmp[k].XPos)
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����

						if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//�������ͬһ���������� û�пɱ���
							continue;

						if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) &0x0000ffff) ==  //ɾ��BC���������������ظ�ʹ��ͶӰ�ĵĵ�
							(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) &0x0000ffff))
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						//ÿ��ͶӰ���е� ����������

					}
				}
			}
			//ÿ��ͶӰ���е�
			if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2) < xPointNum[i])&&
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2) < xPointNum[i])) 
			{

			}

		}

	}

	for(i = 0;i < SCAN_Y_SQUARE_NUM; i++) 
	{
		if(yPointNum[i] ==0)
			continue;

		//if ((PointNum == 3)&&(DIAMOND_Y_GET_C(DiamondBuf,i)==4))//3��4��ͶӰʱ �п��ܴ��ڹ������� Ҳ���ܲ�����Ĭ�ϲ����� 
		//{
		//	continue;
		//}
		if((ySure == 1)||(yPointNum[i] == 3))
			//if(ySure == 1)
		{

			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) > yPointNum[i])||
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) > yPointNum[i]))// AD����BC����������ͶӰ�������ڵ������ô����һ������֡
			{
				continue;
			}

			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) == yPointNum[i])&& // AD����������ͶӰ�������ڵ������ô�Ͳ��ܴ���ͶӰ���õ����
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) == yPointNum[i]))// BC����������ͶӰ�������ڵ������ô�Ͳ��ܴ���ͶӰ���õ����
			{
				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//ɾ��ʹ��ͬһ�����ε�����
						if (DistanceTmp[j].YPos == DistanceTmp[k].YPos)
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����
						if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//��������㲻��һ���������� ���߱��������
							continue;

						if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) &0xffff0000) == 
							(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) &0xffff0000))
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) &0x0000ffff) == 
							(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) &0x0000ffff))
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}
					}
				}
			}
			//3��� ������ 2��� ������ �������ظ������� AD�����ظ���ͶӰ BC ����ÿ��ͶӰ���е�
			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) == yPointNum[i])&& // AD����������ͶӰ�������ڵ������ô�Ͳ��ܴ���ͶӰ���õ����
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) < yPointNum[i]))// BC����������ͶӰ����С�ڵ������ôÿ�����ζ��е�
			{

				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//ɾ��ʹ��ͬһ�����ε�����
						if (DistanceTmp[j].YPos == DistanceTmp[k].YPos)
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����
						if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//��������㲻��һ���������� ���߱��������
							continue;

						if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) &0xffff0000) == 
							(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) &0xffff0000))
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}
						//ÿ��ͶӰ���е� ����������

					}
				}
			}
			//3��� ������ 2��� һ���� �������ظ������� BC�����ظ���ͶӰ AD����ÿ��ͶӰ���е�
			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) < yPointNum[i])&& 
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) == yPointNum[i]))
			{

				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//ɾ��ʹ��ͬһ�����ε�����
						if (DistanceTmp[j].YPos == DistanceTmp[k].YPos)
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}

						//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����
						if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//��������㲻��һ���������� ���߱��������
							continue;

						if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) &0x0000ffff) == 
							(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) &0x0000ffff))
						{
							if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
							{
								DeleteAtDistance(DistanceTmp,j,&disNUM);
							}
							else
							{
								DeleteAtDistance(DistanceTmp,k,&disNUM);
							}
							PointNumTmp --;
							goto RESTART_LESS;
						}
						//ÿ��ͶӰ���е� ����������
					}
				}
			}
			//ÿ��ͶӰ���е�
			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) < yPointNum[i])&& 
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) < yPointNum[i]))
			{
			}

		}
	}

#if 1
	//ÿ��ͶӰ���е�
	if (PointNumTmp == PointNum)
	{
		for (j = 0;j < PointNumTmp;j++)
		{
			ClearPos(&XProjectionCount[XPoint[DistanceTmp[j].XPos].Rec],
				DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond));
			ClearPos(&YProjectionCount[YPoint[DistanceTmp[j].YPos].Rec],
				DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond));
		}
		for(i = 0;i < SCAN_X_SQUARE_NUM; i++)  
		{
			if(xPointNum[i] ==0)	//û�е� �Թ�
				continue;

			for (j=0;j<3;j++)
			{
				if(XProjectionCount[i].APos[j]==0)
				{
					continue;
				}
				DiamondNumTmp = 0;
				//					PRINTFF("X A projection %d %d %d no point\r\n",i,j,XProjectionCount[i].APos[j]);
				DiamondNumTmp |= (j<<16);

				for (k=0;k<3;k++)
				{
					if(XProjectionCount[i].CPos[k]!=0)
					{

						DiamondNumTmp |= (0x0200|k);
						PRINTFF("X AC find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
						ADD_LOST_POINT_X;

					}
				}
			}

			for (j=0;j<3;j++)
			{
				if(XProjectionCount[i].APos[j]==0)
				{
					continue;
				}
				DiamondNumTmp = 0;
				//				PRINTFF("X A projection %d %d %d no point\r\n",i,j,XProjectionCount[i].APos[j]);
				DiamondNumTmp |= (j<<16);

				for (k=0;k<3;k++)
				{

					if(XProjectionCount[i].BPos[k]!=0)
					{

						DiamondNumTmp |= (0x0300|k);
						PRINTFF("X AB find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
						ADD_LOST_POINT_X;

					}
				}
			}

			for (j=0;j<3;j++)
			{
				if(XProjectionCount[i].DPos[j]==0)
				{
					continue;
				}
				DiamondNumTmp = 0;
				//				PRINTFF("X D projection %d %d %d no point\r\n",i,j,XProjectionCount[i].DPos[j]);
				DiamondNumTmp |= ((0x0100|j)<<16);

				for (k=0;k<3;k++)
				{
					if(XProjectionCount[i].CPos[k]!=0)
					{

						DiamondNumTmp |= (0x0200|k);
						PRINTFF("X DC find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
						ADD_LOST_POINT_X;
					}
				}
			}

			for (j=0;j<3;j++)
			{
				if(XProjectionCount[i].DPos[j]==0)
				{
					continue;
				}
				DiamondNumTmp = 0;
				//				PRINTFF("X A projection %d %d %d no point\r\n",i,j,XProjectionCount[i].APos[j]);
				DiamondNumTmp |= ((0x0100|j)<<16);

				for (k=0;k<3;k++)
				{

					if(XProjectionCount[i].BPos[k]!=0)
					{

						DiamondNumTmp |= (0x0300|k);
						PRINTFF("X DB find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
						ADD_LOST_POINT_X;
					}
				}
			}



		}
		//		if (PointNumTmp > PointNum)
		//		goto RESTART_MORE;
		//..Y------------------------------------------
		for(i = 0;i < SCAN_Y_SQUARE_NUM; i++) 
		{
			if(yPointNum[i] ==0)
				continue;
			for (j=0;j<3;j++)
			{
				if(YProjectionCount[i].APos[j]==0)
				{
					continue;
				}
				DiamondNumTmp = 0;
				//					PRINTFF("X A projection %d %d %d no point\r\n",i,j,XProjectionCount[i].APos[j]);
				DiamondNumTmp |= (j<<16);

				for (k=0;k<3;k++)
				{
					if(YProjectionCount[i].CPos[k]!=0)
					{

						DiamondNumTmp |= (0x0200|k);
						PRINTFF("Y AC find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
						ADD_LOST_POINT_Y;

					}
				}
			}

			for (j=0;j<3;j++)
			{
				if(YProjectionCount[i].APos[j]==0)
				{
					continue;
				}
				DiamondNumTmp = 0;
				//				PRINTFF("X A projection %d %d %d no point\r\n",i,j,YProjectionCount[i].APos[j]);
				DiamondNumTmp |= (j<<16);

				for (k=0;k<3;k++)
				{

					if(YProjectionCount[i].BPos[k]!=0)
					{

						DiamondNumTmp |= (0x0300|k);
						PRINTFF("Y AB find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
						ADD_LOST_POINT_Y;

					}
				}
			}

			for (j=0;j<3;j++)
			{
				if(YProjectionCount[i].DPos[j]==0)
				{
					continue;
				}
				DiamondNumTmp = 0;
				//				PRINTFF("X D projection %d %d %d no point\r\n",i,j,YProjectionCount[i].DPos[j]);
				DiamondNumTmp |= ((0x0100|j)<<16);

				for (k=0;k<3;k++)
				{
					if(YProjectionCount[i].CPos[k]!=0)
					{

						DiamondNumTmp |= (0x0200|k);
						PRINTFF("Y DC find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
						ADD_LOST_POINT_Y;

					}
				}
			}

			for (j=0;j<3;j++)
			{
				if(YProjectionCount[i].DPos[j]==0)
				{
					continue;
				}
				DiamondNumTmp = 0;
				//				PRINTFF("X A projection %d %d %d no point\r\n",i,j,YProjectionCount[i].APos[j]);
				DiamondNumTmp |= ((0x0100|j)<<16);

				for (k=0;k<3;k++)
				{

					if(YProjectionCount[i].BPos[k]!=0)
					{

						DiamondNumTmp |= (0x0300|k);
						PRINTFF("Y DB find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
						ADD_LOST_POINT_Y;
					}
				}
			}



		}

	}
#endif


	//���ܴ��ڱ߽�㣬����ϸ������Ҫ���⴦��
	if (PointNumTmp < PointNum)
		//	if (PointNumTmp < 0)
	{
		for(i = PointNumTmp;i < disNUM_f; i++)
		{	
			//PRINTFF("X ratio = %d \r\n",DIAMON_WRH_X(DIAMOND_X_GET_D(DiamondBuf,XPoint[DistanceTmp[i].XPos].Rec,XPoint[DistanceTmp[i].XPos].Diamond)));
			if(DIAMON_WRH_X(DIAMOND_X_GET_D(DiamondBuf,XPoint[DistanceTmp[i].XPos].Rec,XPoint[DistanceTmp[i].XPos].Diamond))>=DIAMON_RATIO)
			{
				//PRINTFF("X new point %d\r\n",(XPoint[DistanceTmp[i].XPos].Point.x - YPoint[DistanceTmp[i].YPos].Point.x));
				if (abs(XPoint[DistanceTmp[i].XPos].Point.x - YPoint[DistanceTmp[i].YPos].Point.x) <= LONG_DIA_THRESHOLD)
				{
					PRINTFF("X new point\r\n");
					ExcDistance(DistanceTmp,PointNumTmp,i);
					PointNumTmp++;
				}
			}
			//PRINTFF("Y ratio = %d \r\n",DIAMON_WRH_Y(DIAMOND_Y_GET_D(DiamondBuf,YPoint[DistanceTmp[i].YPos].Rec,YPoint[DistanceTmp[i].YPos].Diamond)));
			if(DIAMON_WRH_Y(DIAMOND_Y_GET_D(DiamondBuf,YPoint[DistanceTmp[i].YPos].Rec,YPoint[DistanceTmp[i].YPos].Diamond))>=DIAMON_RATIO)
			{
				//PRINTFF("Y new point %d\r\n",(XPoint[DistanceTmp[i].XPos].Point.y - YPoint[DistanceTmp[i].YPos].Point.y));
				if (abs(XPoint[DistanceTmp[i].XPos].Point.y - YPoint[DistanceTmp[i].YPos].Point.y) <= LONG_DIA_THRESHOLD)
				{
					PRINTFF("Y new point\r\n");
					ExcDistance(DistanceTmp,PointNumTmp,i);
					PointNumTmp++;
				}
			}
		}
		PointNumTmp_t = PointNumTmp;
		//#if 1
RESTART_MORE:
		for(i = 0;i < SCAN_X_SQUARE_NUM; i++)  
		{
			if(xPointNum[i] ==0)	//û�е� �Թ�
				continue;
			if((xSure == 1)||(xPointNum[i] == 3))
			{

				if(((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2) > xPointNum[i]) ||// AD����������ͶӰ�������ڵ������ô �����һ������֡
					((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2) > xPointNum[i])) // BC����������ͶӰ�������ڵ���� ����һ�����������֡
				{
					continue;
				}

				//3��� ������ 2��� ������ �������ظ������� ���ظ���ͶӰ
				if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2)==xPointNum[i])&&
					((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2)==xPointNum[i])) 
				{
					for (j = 0;j < PointNumTmp;j++)
					{
						for (k = j+1 ;k < PointNumTmp ;k++)
						{
							//ɾ��ʹ��ͬһ�����ε�����
							if (DistanceTmp[j].XPos == DistanceTmp[k].XPos)
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.x - YPoint[DistanceTmp[j].YPos].Point.x) >
									abs(XPoint[DistanceTmp[k].XPos].Point.x - YPoint[DistanceTmp[k].YPos].Point.x))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("X delete point 1\r\n");
								goto RESTART_MORE;
							}

							//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����

							if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//�������ͬһ���������� û�пɱ���
								continue;

							if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) & 0xffff0000) == //ɾ��AD���������������ظ�ʹ��ͶӰ�ĵĵ�
								(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) & 0xffff0000))
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.x - YPoint[DistanceTmp[j].YPos].Point.x) >
									abs(XPoint[DistanceTmp[k].XPos].Point.x - YPoint[DistanceTmp[k].YPos].Point.x))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("X delete point 2\r\n");
								goto RESTART_MORE;
							}

							if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) &0x0000ffff) ==  //ɾ��BC���������������ظ�ʹ��ͶӰ�ĵĵ�
								(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) &0x0000ffff))
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.x - YPoint[DistanceTmp[j].YPos].Point.x) >
									abs(XPoint[DistanceTmp[k].XPos].Point.x - YPoint[DistanceTmp[k].YPos].Point.x))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("X delete point 3\r\n");
								goto RESTART_MORE;
							}
						}
					}
				}
				//3��� ������ 2��� ������ �������ظ������� AD�����ظ���ͶӰ BC ����ÿ��ͶӰ���е�
				if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2)==xPointNum[i])&&
					((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2) < xPointNum[i])) 
				{
					for (j = 0;j < PointNumTmp;j++)
					{
						for (k = j+1 ;k < PointNumTmp ;k++)
						{
							//ɾ��ʹ��ͬһ�����ε�����
							if (DistanceTmp[j].XPos == DistanceTmp[k].XPos)
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.x - YPoint[DistanceTmp[j].YPos].Point.x) >
									abs(XPoint[DistanceTmp[k].XPos].Point.x - YPoint[DistanceTmp[k].YPos].Point.x))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("X delete point 4\r\n");
								goto RESTART_MORE;
							}

							//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����

							if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//�������ͬһ���������� û�пɱ���
								continue;

							if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) & 0xffff0000) == //ɾ��AD���������������ظ�ʹ��ͶӰ�ĵĵ�
								(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) & 0xffff0000))
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.x - YPoint[DistanceTmp[j].YPos].Point.x) >
									abs(XPoint[DistanceTmp[k].XPos].Point.x - YPoint[DistanceTmp[k].YPos].Point.x))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("X delete point 5\r\n");
								goto RESTART_MORE;
							}

							//ÿ��ͶӰ���е� ����������

						}
					}
				}

				//3��� ������ 2��� һ���� �������ظ������� BC�����ظ���ͶӰ AD����ÿ��ͶӰ���е�
				if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2) < xPointNum[i])&&
					((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2)  == xPointNum[i]))
				{
					for (j = 0;j < PointNumTmp;j++)
					{
						for (k = j+1 ;k < PointNumTmp ;k++)
						{
							//ɾ��ʹ��ͬһ�����ε�����
							if (DistanceTmp[j].XPos == DistanceTmp[k].XPos)
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.x - YPoint[DistanceTmp[j].YPos].Point.x) >
									abs(XPoint[DistanceTmp[k].XPos].Point.x - YPoint[DistanceTmp[k].YPos].Point.x))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("X delete point 6\r\n");
								goto RESTART_MORE;
							}

							//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����

							if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//�������ͬһ���������� û�пɱ���
								continue;

							if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) &0x0000ffff) ==  //ɾ��BC���������������ظ�ʹ��ͶӰ�ĵĵ�
								(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) &0x0000ffff))
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.x - YPoint[DistanceTmp[j].YPos].Point.x) >
									abs(XPoint[DistanceTmp[k].XPos].Point.x - YPoint[DistanceTmp[k].YPos].Point.x))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("X delete point 7\r\n");
								goto RESTART_MORE;
							}

							//ÿ��ͶӰ���е� ����������

						}
					}
				}
			}
		}
#if 1
		for(i = 0;i < SCAN_Y_SQUARE_NUM; i++) 
		{
			if(yPointNum[i] ==0)
				continue;

			if((ySure == 1)||(yPointNum[i] == 3))
			{
				if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) > yPointNum[i])||
					((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) > yPointNum[i]))// AD����BC����������ͶӰ�������ڵ������ô����һ������֡
				{
					continue;
				}

				if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) == yPointNum[i])&& // AD����������ͶӰ�������ڵ������ô�Ͳ��ܴ���ͶӰ���õ����
					((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) == yPointNum[i]))// BC����������ͶӰ�������ڵ������ô�Ͳ��ܴ���ͶӰ���õ����
				{
					for (j = 0;j < PointNumTmp;j++)
					{
						for (k = j+1 ;k < PointNumTmp ;k++)
						{
							//ɾ��ʹ��ͬһ�����ε�����
							if (DistanceTmp[j].YPos == DistanceTmp[k].YPos)
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.y - YPoint[DistanceTmp[j].YPos].Point.y) >
									abs(XPoint[DistanceTmp[k].XPos].Point.y - YPoint[DistanceTmp[k].YPos].Point.y))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("Y delete point 1\r\n");
								goto RESTART_MORE;
							}

							//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����
							if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//��������㲻��һ���������� ���߱��������
								continue;

							if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) &0xffff0000) == 
								(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) &0xffff0000))
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.y - YPoint[DistanceTmp[j].YPos].Point.y) >
									abs(XPoint[DistanceTmp[k].XPos].Point.y - YPoint[DistanceTmp[k].YPos].Point.y))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("Y delete point 2\r\n");
								goto RESTART_MORE;
							}

							if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) &0x0000ffff) == 
								(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) &0x0000ffff))
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.y - YPoint[DistanceTmp[j].YPos].Point.y) >
									abs(XPoint[DistanceTmp[k].XPos].Point.y - YPoint[DistanceTmp[k].YPos].Point.y))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("Y delete point 3\r\n");
								goto RESTART_MORE;
							}
						}
					}
				}
				//3��� ������ 2��� ������ �������ظ������� AD�����ظ���ͶӰ BC ����ÿ��ͶӰ���е�
				if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) == yPointNum[i])&& // AD����������ͶӰ�������ڵ������ô�Ͳ��ܴ���ͶӰ���õ����
					((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) < yPointNum[i]))// BC����������ͶӰ����С�ڵ������ôÿ�����ζ��е�
				{

					for (j = 0;j < PointNumTmp;j++)
					{
						for (k = j+1 ;k < PointNumTmp ;k++)
						{
							//ɾ��ʹ��ͬһ�����ε�����
							if (DistanceTmp[j].YPos == DistanceTmp[k].YPos)
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.y - YPoint[DistanceTmp[j].YPos].Point.y) >
									abs(XPoint[DistanceTmp[k].XPos].Point.y - YPoint[DistanceTmp[k].YPos].Point.y))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("Y delete point 4\r\n");
								goto RESTART_MORE;
							}

							//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����
							if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//��������㲻��һ���������� ���߱��������
								continue;

							if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) &0xffff0000) == 
								(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) &0xffff0000))
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.y - YPoint[DistanceTmp[j].YPos].Point.y) >
									abs(XPoint[DistanceTmp[k].XPos].Point.y - YPoint[DistanceTmp[k].YPos].Point.y))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("Y delete point 5\r\n");
								goto RESTART_MORE;
							}
							//ÿ��ͶӰ���е� ����������

						}
					}
				}
				//3��� ������ 2��� һ���� �������ظ������� BC�����ظ���ͶӰ AD����ÿ��ͶӰ���е�
				if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) < yPointNum[i])&& 
					((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) == yPointNum[i]))
				{

					for (j = 0;j < PointNumTmp;j++)
					{
						for (k = j+1 ;k < PointNumTmp ;k++)
						{
							//ɾ��ʹ��ͬһ�����ε�����
							if(abs(XPoint[DistanceTmp[j].XPos].Point.y - YPoint[DistanceTmp[j].YPos].Point.y) >
								abs(XPoint[DistanceTmp[k].XPos].Point.y - YPoint[DistanceTmp[k].YPos].Point.y))
							{
								if (DistanceTmp[j].distance >  DistanceTmp[k].distance)
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("Y delete point 6\r\n");
								goto RESTART_MORE;
							}

							//ĳЩ����£�һ��ͶӰ��ֻ��һ���㣬����ɾ��ͬһ��ͶӰ���غϵ�����
							if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//��������㲻��һ���������� ���߱��������
								continue;

							if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) &0x0000ffff) == 
								(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) &0x0000ffff))
							{
								if(abs(XPoint[DistanceTmp[j].XPos].Point.y - YPoint[DistanceTmp[j].YPos].Point.y) >
									abs(XPoint[DistanceTmp[k].XPos].Point.y - YPoint[DistanceTmp[k].YPos].Point.y))
								{
									ForceDeleteAtDistance(DistanceTmp,j,&PointNumTmp);
								}
								else
								{
									ForceDeleteAtDistance(DistanceTmp,k,&PointNumTmp);
								}
								PRINTFF("Y delete point 7\r\n");
								goto RESTART_MORE;
							}
							//ÿ��ͶӰ���е� ����������
						}
					}
				}
			}
		}

		//������١�����
		if (PointNumTmp < PointNum)
			//	if (PointNumTmp < 0)
		{
			for (j = 0;j < PointNumTmp;j++)
			{
				ClearPos(&XProjectionCount[XPoint[DistanceTmp[j].XPos].Rec],
					DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond));
				ClearPos(&YProjectionCount[YPoint[DistanceTmp[j].YPos].Rec],
					DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond));
			}
			//	DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond)
			//YProjectionCount[i].pos[0]
			for(i = 0;i < SCAN_X_SQUARE_NUM; i++)  
			{
				if(xPointNum[i] ==0)	//û�е� �Թ�
					continue;

				for (j=0;j<3;j++)
				{
					if(XProjectionCount[i].APos[j]==0)
					{
						continue;
					}
					DiamondNumTmp = 0;
					//					PRINTFF("X A projection %d %d %d no point\r\n",i,j,XProjectionCount[i].APos[j]);
					DiamondNumTmp |= (j<<16);

					for (k=0;k<3;k++)
					{
						if(XProjectionCount[i].CPos[k]!=0)
						{

							DiamondNumTmp |= (0x0200|k);
							PRINTFF("X AC find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
							ADD_LOST_POINT_X;

						}
					}
				}

				for (j=0;j<3;j++)
				{
					if(XProjectionCount[i].APos[j]==0)
					{
						continue;
					}
					DiamondNumTmp = 0;
					//				PRINTFF("X A projection %d %d %d no point\r\n",i,j,XProjectionCount[i].APos[j]);
					DiamondNumTmp |= (j<<16);

					for (k=0;k<3;k++)
					{

						if(XProjectionCount[i].BPos[k]!=0)
						{

							DiamondNumTmp |= (0x0300|k);
							PRINTFF("X AB find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
							ADD_LOST_POINT_X;

						}
					}
				}

				for (j=0;j<3;j++)
				{
					if(XProjectionCount[i].DPos[j]==0)
					{
						continue;
					}
					DiamondNumTmp = 0;
					//				PRINTFF("X D projection %d %d %d no point\r\n",i,j,XProjectionCount[i].DPos[j]);
					DiamondNumTmp |= ((0x0100|j)<<16);

					for (k=0;k<3;k++)
					{
						if(XProjectionCount[i].CPos[k]!=0)
						{

							DiamondNumTmp |= (0x0200|k);
							PRINTFF("X DC find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
							ADD_LOST_POINT_X;
						}
					}
				}

				for (j=0;j<3;j++)
				{
					if(XProjectionCount[i].DPos[j]==0)
					{
						continue;
					}
					DiamondNumTmp = 0;
					//				PRINTFF("X A projection %d %d %d no point\r\n",i,j,XProjectionCount[i].APos[j]);
					DiamondNumTmp |= ((0x0100|j)<<16);

					for (k=0;k<3;k++)
					{

						if(XProjectionCount[i].BPos[k]!=0)
						{

							DiamondNumTmp |= (0x0300|k);
							PRINTFF("X DB find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
							ADD_LOST_POINT_X;
						}
					}
				}



			}
			//		if (PointNumTmp > PointNum)
			//		goto RESTART_MORE;
			//..Y------------------------------------------
			for(i = 0;i < SCAN_Y_SQUARE_NUM; i++) 
			{
				if(yPointNum[i] ==0)
					continue;
				for (j=0;j<3;j++)
				{
					if(YProjectionCount[i].APos[j]==0)
					{
						continue;
					}
					DiamondNumTmp = 0;
					//					PRINTFF("X A projection %d %d %d no point\r\n",i,j,XProjectionCount[i].APos[j]);
					DiamondNumTmp |= (j<<16);

					for (k=0;k<3;k++)
					{
						if(YProjectionCount[i].CPos[k]!=0)
						{

							DiamondNumTmp |= (0x0200|k);
							PRINTFF("Y AC find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
							ADD_LOST_POINT_Y;

						}
					}
				}

				for (j=0;j<3;j++)
				{
					if(YProjectionCount[i].APos[j]==0)
					{
						continue;
					}
					DiamondNumTmp = 0;
					//				PRINTFF("X A projection %d %d %d no point\r\n",i,j,YProjectionCount[i].APos[j]);
					DiamondNumTmp |= (j<<16);

					for (k=0;k<3;k++)
					{

						if(YProjectionCount[i].BPos[k]!=0)
						{

							DiamondNumTmp |= (0x0300|k);
							PRINTFF("Y AB find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
							ADD_LOST_POINT_Y;

						}
					}
				}

				for (j=0;j<3;j++)
				{
					if(YProjectionCount[i].DPos[j]==0)
					{
						continue;
					}
					DiamondNumTmp = 0;
					//				PRINTFF("X D projection %d %d %d no point\r\n",i,j,YProjectionCount[i].DPos[j]);
					DiamondNumTmp |= ((0x0100|j)<<16);

					for (k=0;k<3;k++)
					{
						if(YProjectionCount[i].CPos[k]!=0)
						{

							DiamondNumTmp |= (0x0200|k);
							PRINTFF("Y DC find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
							ADD_LOST_POINT_Y;

						}
					}
				}

				for (j=0;j<3;j++)
				{
					if(YProjectionCount[i].DPos[j]==0)
					{
						continue;
					}
					DiamondNumTmp = 0;
					//				PRINTFF("X A projection %d %d %d no point\r\n",i,j,YProjectionCount[i].APos[j]);
					DiamondNumTmp |= ((0x0100|j)<<16);

					for (k=0;k<3;k++)
					{

						if(YProjectionCount[i].BPos[k]!=0)
						{

							DiamondNumTmp |= (0x0300|k);
							PRINTFF("Y DB find diamond  %d %d %08x no point\r\n",j,k,DiamondNumTmp);
							ADD_LOST_POINT_Y;
						}
					}
				}



			}

		}
#endif
	}



	if (PointNumTmp>PointNum)
	{

	}
COPY_POINT:
	//����������Чֵ
	PRINTFF("Last PointNumTmp=%d\r\n",PointNumTmp);
	for(i=0;i<PointNumTmp;i++)
	{
		point[i].pt_val.x = XPoint[DistanceTmp[i].XPos].Point.x;
		point[i].pt_val.y = YPoint[DistanceTmp[i].YPos].Point.y;
	}
#if 1
	// ����������У��
	for (j = 0;j < PointNumTmp;j++)
	{
		for (k = j+1 ;k < PointNumTmp ;k++)
		{
			if(XPoint[DistanceTmp[j].XPos].Rec == XPoint[DistanceTmp[k].XPos].Rec)	//�������ͬһ���������� û�пɱ���
			{
				if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) & 0xffff0000) == 
					(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) & 0xffff0000))
				{
					point[j].pt_val.x  = AdjustXRec_X(&DiamondBuf->strXSquareDiamond[XPoint[DistanceTmp[j].XPos].Rec].strDiamondPoint[XPoint[DistanceTmp[j].XPos].Diamond],
						YPoint[DistanceTmp[j].YPos].Point.y,XPoint[DistanceTmp[j].XPos].Rec);
					point[k].pt_val.x  = AdjustXRec_X(&DiamondBuf->strXSquareDiamond[XPoint[DistanceTmp[k].XPos].Rec].strDiamondPoint[XPoint[DistanceTmp[k].XPos].Diamond],
						YPoint[DistanceTmp[k].YPos].Point.y,XPoint[DistanceTmp[j].XPos].Rec);
				}
				if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) & 0x0000ffff) == 
					(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[k].XPos].Rec,XPoint[DistanceTmp[k].XPos].Diamond) & 0x0000ffff))
				{
					point[j].pt_val.x = AdjustXRec_X(&DiamondBuf->strXSquareDiamond[XPoint[DistanceTmp[j].XPos].Rec].strDiamondPoint[XPoint[DistanceTmp[j].XPos].Diamond],
						YPoint[DistanceTmp[j].YPos].Point.y,XPoint[DistanceTmp[j].XPos].Rec);
					point[k].pt_val.x = AdjustXRec_X(&DiamondBuf->strXSquareDiamond[XPoint[DistanceTmp[k].XPos].Rec].strDiamondPoint[XPoint[DistanceTmp[k].XPos].Diamond],
						YPoint[DistanceTmp[k].YPos].Point.y,XPoint[DistanceTmp[j].XPos].Rec);
				}
			}
			if(YPoint[DistanceTmp[j].YPos].Rec == YPoint[DistanceTmp[k].YPos].Rec)	//��������㲻��һ���������� ���߱��������
			{

				if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) & 0xffff0000) == 
					(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) & 0xffff0000))
				{
					point[j].pt_val.y  = AdjustYRec_Y(&DiamondBuf->strYSquareDiamond[YPoint[DistanceTmp[j].YPos].Rec].strDiamondPoint[YPoint[DistanceTmp[j].YPos].Diamond],
						XPoint[DistanceTmp[j].XPos].Point.x,YPoint[DistanceTmp[j].YPos].Rec);
					point[k].pt_val.y  = AdjustYRec_Y(&DiamondBuf->strYSquareDiamond[YPoint[DistanceTmp[k].YPos].Rec].strDiamondPoint[YPoint[DistanceTmp[k].YPos].Diamond],
						XPoint[DistanceTmp[k].XPos].Point.x,YPoint[DistanceTmp[j].YPos].Rec);
				}
				if ((DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond) & 0x0000ffff) == 
					(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[k].YPos].Rec,YPoint[DistanceTmp[k].YPos].Diamond) & 0x0000ffff))
				{
					point[j].pt_val.y  = AdjustYRec_Y(&DiamondBuf->strYSquareDiamond[YPoint[DistanceTmp[j].YPos].Rec].strDiamondPoint[YPoint[DistanceTmp[j].YPos].Diamond],
						XPoint[DistanceTmp[j].XPos].Point.x,YPoint[DistanceTmp[j].YPos].Rec);
					point[k].pt_val.y  = AdjustYRec_Y(&DiamondBuf->strYSquareDiamond[YPoint[DistanceTmp[k].YPos].Rec].strDiamondPoint[YPoint[DistanceTmp[k].YPos].Diamond],
						XPoint[DistanceTmp[k].XPos].Point.x,YPoint[DistanceTmp[j].YPos].Rec);
				}
			}
		}
	}
#endif

CALC_POINT_END:
	CalcPointID(point,&PointNumTmp );
	return PointNumTmp;
}

static signed int CalcPointID(struct PT_BUF *point,signed int* num)
{
	unsigned int i,j;
	unsigned int NeedContiune;

	unsigned int DistanceNUM;
	unsigned int DistanceNUM_t;
	CALC_DISTANCE DistanceTmp[MAX_POINT*MAX_POINT];
	POS_USE PosUse;

	DistanceNUM = 0;
	DistanceNUM_t = 0;
	memset(&PosUse,0,sizeof(POS_USE));
	if (*num > MAX_POINT)
	{
		*num = MAX_POINT ;
	}

	for (i=0;i<*num;i++)	//�����е�ID ����
	{
		point[i].id = 0;
	}

	for (j=0;j<MAX_POINT;j++)	//���е� ̧��һ��
	{
		if (g_PointStatus.LastPoint[j].id == 0)
		{
			continue;
		}
		g_PointStatus.LastPointUpTime[j]++;
	}
	if ((*num)<=0)
	{
		goto DETECT_TOUCH_UP;
	}
	for (j=0;j<MAX_POINT;j++)
	{
		if (g_PointStatus.LastPoint[j].id == 0)
		{
			continue;
		}
		for (i=0;i<*num;i++)
		{
			DistanceTmp[DistanceNUM].distance = CalclDistance_Short(&point[i].pt_val, &g_PointStatus.LastPoint[j].pt_val);
			DistanceTmp[DistanceNUM].XPos = j;	
			DistanceTmp[DistanceNUM].YPos = i;
			DistanceNUM++;
		}
	}

	shell_sort(DistanceTmp , DistanceNUM); 
	DistanceNUM_t = GetThresholdPos(DistanceTmp,DistanceNUM,DISTANCE_THRESHOLD_ID);

	for (i = 0;i<DistanceNUM_t;i++)
	{
		NeedContiune = 0;

		for (j=0;j<PosUse.PosNum;j++)
		{
			if ((DistanceTmp[i].XPos ==PosUse.XPosUse[j])||
				(DistanceTmp[i].YPos ==PosUse.YPosUse[j]))
			{
				NeedContiune =1;
				break;
			}
		}

		if (NeedContiune == 1)
		{
			continue;
		}

		point[DistanceTmp[i].YPos].id = g_PointStatus.LastPoint[DistanceTmp[i].XPos].id;
		point[DistanceTmp[i].YPos].tip = 1;
		point[DistanceTmp[i].YPos].valid = 1;
		g_PointStatus.LastPoint[DistanceTmp[i].XPos] = point[DistanceTmp[i].YPos];
		g_PointStatus.LastPointUpTime[DistanceTmp[i].XPos] = 0;	//̧���������
		PosUse.XPosUse[PosUse.PosNum]=DistanceTmp[i].XPos;
		PosUse.YPosUse[PosUse.PosNum]=DistanceTmp[i].YPos;
		PosUse.PosNum++;
	}
	//�����Ƿ����µ�	
DETECT_NEW_POINT:
	for (i=0;i<*num;i++)
	{
		if (point[i].id != 0)
		{
			continue;
		}
		for (j=0; j<MAX_POINT;j++)
		{
			if (g_PointStatus.LastPoint[j].id != 0)
			{
				continue;
			}
			point[i].id = j+1;
			point[i].tip = 1;
			point[i].valid = 1;
			g_PointStatus.LastPoint[j] = point[i];
			g_PointStatus.ulLastPointNum++ ;
			g_PointStatus.LastPointUpTime[j] = 0;	//̧���������
			break;
		}
	}

DETECT_TOUCH_UP:
	//����ǲ����еĵ��Ѿ����û������ ���û�����ݣ��ǾͰ�����㷢��һ��̧���¼�
	for (j=0;j<MAX_POINT;j++)
	{
		if (g_PointStatus.LastPoint[j].id == 0)
		{
			continue;
		}
		if (g_PointStatus.LastPointUpTime[j] >= TIMES_THRESHOLD_UP)
		{
			g_PointStatus.LastPoint[j].tip = 0;
			point[*num] = g_PointStatus.LastPoint[j];
			(*num) ++;
			g_PointStatus.LastPointUpTime[j] = 0;
			g_PointStatus.LastPoint[j].id=0;
			g_PointStatus.ulLastPointNum--;
		}
	}

	PRINTFF ("ulLastPointNum=%d \r\n",g_PointStatus.ulLastPointNum);
	return 0;
}

//����distance����������������
void ExcDistance(CALC_DISTANCE *dis,signed int src,signed int dst)
{

	CALC_DISTANCE dis_tmp;
	dis_tmp = dis[src];
	dis[src] = dis[dst];
	dis[dst] = dis_tmp;

}

void ForceDeleteAtDistance(CALC_DISTANCE *dis,unsigned int pos,signed int *len)
{
	signed int i;
	CALC_DISTANCE dis_tmp;
	if(pos>=(*len))
		return;
	(*len) -=1;
	dis_tmp = dis[pos];
	for (i=pos; i < (*len ) ;i++)
	{
		dis[i] = dis[i+1];
	}
	dis[i] = dis_tmp;
	*len = i;
}
//ɾ��distance��������ĳ��ֵ
void DeleteAtDistance(CALC_DISTANCE *dis,unsigned int pos,unsigned int *len)
{
	unsigned int i;
	CALC_DISTANCE dis_tmp;
	if(pos>=(*len))
		return;
	(*len) -=1;
	dis_tmp = dis[pos];
	for (i=pos; (i <*len)&&(dis[i].distance < DISTANCE_THRESHOLD_WC) ;i++)
	{
		dis[i] = dis[i+1];
	}
	dis[i] = dis_tmp;
	*len = i;
}

//ϣ������ 
void shell_sort(CALC_DISTANCE *arr, unsigned int len) 
{
	signed int gap, i, j;
	CALC_DISTANCE temp;
	for (gap = len >> 1; gap > 0; gap >>= 1)
	{
		for (i = gap; i < len; i++) 
		{
			temp = arr[i];
			for (j = i - gap; j >= 0 && arr[j].distance > temp.distance; j -= gap)
				arr[j + gap] = arr[j];
			arr[j + gap] = temp;
		}
	}
}

