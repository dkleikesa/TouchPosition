#include "CalcPoint.h"

PT_STATUS g_PointStatus;	//ɨ���ID��״̬


//��ȡ�������ĵ�
static __INLINE void CalclDiamondCentre(CALC_DIAMOND *a,CALC_POINT *b,int xOry,int index)
{
	if(xOry == 0)
	{
		b->x = CALCL_REL_TO_ABS(CALCL_DIAMOND_CENTRE_X(*a),index);
		b->y = CALCL_DIAMOND_CENTRE_Y(*a);
	}
	else
	{
		b->x = CALCL_DIAMOND_CENTRE_X(*a);
		b->y = CALCL_REL_TO_ABS(CALCL_DIAMOND_CENTRE_Y(*a),index);
	}

}

//��ȡ������
//Ϊ�˽�ʡ������ ����ƽ�� ����ʾ����
static __INLINE int CalclDistance(CALC_POINT *a,CALC_POINT *b)
{
	return CALCL_SQUAR((a->x) - (b->x)) + CALCL_SQUAR((a->y)-(b->y));
}

static __INLINE int CalclDistance_Short(POINT_C *a,POINT_C *b)
{
	return CALCL_SQUAR((a->x) - (b->x)) + CALCL_SQUAR((a->y)-(b->y));
}

static __INLINE int  AdjustXRec_X (CALC_DIAMOND* p,int y0,char rec)
{
	if ((((CALC_DIAMOND*)p)->strPoint0.y - ((CALC_DIAMOND*)p)->strPoint3.y) == 0)
	{
		return CALCL_REL_TO_ABS(CALCL_DIAMOND_CENTRE_X(*p),rec);
	}
	return CALCL_REL_TO_ABS(ADJUST_XREC_X_REL(p,y0),rec);
}
static __INLINE int  AdjustYRec_Y(CALC_DIAMOND* p,int x0,char rec) 
{
	if ((((CALC_DIAMOND*)p)->strPoint2.x - ((CALC_DIAMOND*)p)->strPoint1.x) == 0)
	{
		return  CALCL_REL_TO_ABS(CALCL_DIAMOND_CENTRE_Y(*p),rec);
	}
	return CALCL_REL_TO_ABS(ADJUST_YREC_Y_REL(p,x0),rec);
}


// ��ӡ������������
//DiamondBuf: ��������ָ��
//OutBuf�� �����buff
//ScanCount: ��ǰ���ݱ��
void PrintDiamond(CALC_DIAMOND_BUF *DiamondBuf,char *OutBuf,int ScanCount)
{
	int n=0;
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
	unsigned int i,j,k,xDiamondNum,yDiamondNum,disNUM,disNUM_f,PointNum;
	int xPointNum[SCAN_X_SQUARE_NUM],xPointNumSum;
	int yPointNum[SCAN_Y_SQUARE_NUM],yPointNumSum;
	int PointNumTmp;
	int PosTem;
	int xSure, ySure;
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
	PointNumTmp = 0; //�������ĵ�����������Ĵ���������� ���ܴ��ڼٵ�����ٵ�
	memset(XProjectionCount,0xff,sizeof(XProjectionCount));	//x�����������������ʵ�ʵ����
	memset(YProjectionCount,0xff,sizeof(YProjectionCount));	//y�������μ��������ʵ�ʵ����

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
						DIAMOND_X_GET_P(DiamondBuf,i,j,1).y = DIAMOND_X_GET_P(DiamondBuf,i,j,3).y;
					}
				}
				else	//�����ڵ�������
				{
					
				}
			}
			else
			{
				//����
			}
			CalclDiamondCentre(&(DiamondBuf->strXSquareDiamond[i].strDiamondPoint[j]),&XPoint[xDiamondNum].Point,0,i);
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

			if((DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y ==0) || (DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y ==TATAL_HEIGHT) ||			//������
				(DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y ==0)|| (DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y ==TATAL_HEIGHT))
			{
				if(DIAMOND_Y_GET_P(DiamondBuf,i,j,0).x == DIAMOND_Y_GET_P(DiamondBuf,i,j,3).x)	//�߽�������
				{

					if (DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y == DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y)
					{
						DIAMOND_Y_GET_P(DiamondBuf,i,j,2).x = DIAMOND_Y_GET_P(DiamondBuf,i,j,0).x;
						DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y = DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y;
					}
					else if(DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y == DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y)
					{
						DIAMOND_Y_GET_P(DiamondBuf,i,j,1).x = DIAMOND_Y_GET_P(DiamondBuf,i,j,0).x;
						DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y = DIAMOND_Y_GET_P(DiamondBuf,i,j,3).y;
					}
				}
				else	//�����ڵ�������
				{

				}
			}
			else
			{
				//����
			}

			CalclDiamondCentre(&(DiamondBuf->strYSquareDiamond[i].strDiamondPoint[j]),&YPoint[yDiamondNum].Point,1,i);
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

/*	for(i = 0; i<xDiamondNum ;i ++)
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
#if 1
		xPointNum[i] = max((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2),
			(XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2));
		xPointNumSum += xPointNum[i];
#else
		if(DIAMOND_X_GET_C(DiamondBuf,i) ==0)
			continue;
		for (k=1;k<=MAX_POINT_REC;k++)
		{
			if (DIAMOND_X_GET_C(DiamondBuf,i) <= (k*k))
			{
				xPointNum[i] = k;
				xPointNumSum += k;
				break;
			}
		}
#endif

	}
	for(i = 0;i < SCAN_Y_SQUARE_NUM; i++)
	{
#if 1
		yPointNum[i] =  max((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2),
			(YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2));
		yPointNumSum += yPointNum[i];
#else
		if(DIAMOND_Y_GET_C(DiamondBuf,i) ==0)
			continue;
		for (k=1;k<=MAX_POINT_REC;k++)
		{
			if (DIAMOND_Y_GET_C(DiamondBuf,i) <= (k*k))
			{
				yPointNum[i] = k;
				yPointNumSum += k;
				break;
			}
		}
#endif
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
	PointNumTmp = GetThresholdPos(DistanceTmp, disNUM);	//��ȡ�����
	PRINTFF("xPoint=%d yPoint=%d PointNum=%d PointNumTmp=%d\r\n",xPointNumSum,yPointNumSum,PointNum,PointNumTmp);
	/*3175-3375 ���*/
	/*2f75 �ٵ���ͨ�������жϵ���� ��Ҫͨ��ͶӰ��ȷ��*/ 
	/*2c75 �ٵ�ͨ���Ҿ�������жϳ���ȷ��*/
	/*2575 ���θ���*/
RESTART_LESS:
	//���������ɣ����ܻ��ٵ� ͨ��������ֵ�� �����
	if (PointNumTmp < PointNum)	
	{
		while ((DistanceTmp[PointNumTmp].distance < DISTANCE_THRESHOLD_WC) && (PointNumTmp < PointNum)) 
		{
			PointNumTmp++;	//ȷʵ�ٵ� �ǾͰ��� �ٵ�������
		}
	}

	//memset(xPointNum,0,sizeof(xPointNum));	//x�����������������ʵ�ʵ����
	//memset(yPointNum,0,sizeof(yPointNum));	//y�������μ��������ʵ�ʵ����
	//for(i = 0;i < PointNumTmp; i++)	//���ÿ������ ӵ�еĵ���
	//{
	//	xPointNum[XPoint[DistanceTmp[i].XPos].Rec]++ ;
	//	yPointNum[YPoint[DistanceTmp[i].YPos].Rec]++ ;
	//}

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


	//���ܴ��ڱ߽�㣬����ϸ������Ҫ���⴦��
	if (PointNumTmp < PointNum)
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
				}
				
			}
			//PRINTFF("Y ratio = %d \r\n",DIAMON_WRH_Y(DIAMOND_Y_GET_D(DiamondBuf,YPoint[DistanceTmp[i].YPos].Rec,YPoint[DistanceTmp[i].YPos].Diamond)));
			if(DIAMON_WRH_Y(DIAMOND_Y_GET_D(DiamondBuf,YPoint[DistanceTmp[i].YPos].Rec,YPoint[DistanceTmp[i].YPos].Diamond))>=DIAMON_RATIO)
			{
				//PRINTFF("Y new point %d\r\n",(XPoint[DistanceTmp[i].XPos].Point.y - YPoint[DistanceTmp[i].YPos].Point.y));
				if (abs(XPoint[DistanceTmp[i].XPos].Point.y - YPoint[DistanceTmp[i].YPos].Point.y) <= LONG_DIA_THRESHOLD)
				{
					PRINTFF("Y new point\r\n");
				}

			}
			

		}

		
	}
	if (PointNumTmp>MAX_POINT)
	{
		// ��δ����أ�
	}
COPY_POINT:
	//����������Чֵ
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

static int CalcPointID(struct PT_BUF *point,int* num)
{
	int i,j;
	int min_pos,first;
	int min_distance;
	int distance_tem;
	if (*num > MAX_POINT)
	{
		*num = MAX_POINT ;
	}

	//��һ�δ�����ʼ
	if (g_PointStatus.ulLastPointNum == 0)
	{
		for (i=0;i<*num;i++)
		{
			point[i].id = i+1;
			point[i].tip = 1;
			point[i].valid = 1;
			g_PointStatus.LastPoint[i] = point[i];
		}
		g_PointStatus.ulLastPointNum = *num;
		return 0;
	}
	if (g_PointStatus.ulLastPointNum > 0)
	{
		for (j=0;j<MAX_POINT;j++)
		{
			if (g_PointStatus.LastPoint[j].id == 0)
			{
				continue;
			}
			g_PointStatus.LastPointUpTime[j]++;
		}

		for (i=0;i<*num;i++)
		{

			first = 0;
			for (j=0;j<MAX_POINT;j++)
			{
				if (g_PointStatus.LastPoint[j].id == 0)
				{
					continue;
				}
				if (first == 0)
				{
					min_distance =  CalclDistance_Short(&point[i].pt_val, &g_PointStatus.LastPoint[j].pt_val);
					min_pos = j;
					first = 1;
					continue;
				}

				distance_tem = CalclDistance_Short(&point[i].pt_val, &g_PointStatus.LastPoint[j].pt_val);
				if (min_distance > distance_tem)
				{
					min_distance = distance_tem;
					min_pos = j;
				}

			}
			if (min_distance < DISTANCE_THRESHOLD_ID)	//������Ǹ��ϵ�
			{
				point[i].id = min_pos+1;
				point[i].tip = 1;
				point[i].valid = 1;
				g_PointStatus.LastPoint[min_pos] = point[i];
				g_PointStatus.LastPointUpTime[min_pos] = 0;	//̧���������
			}
			else	//�Ǹ��µ�
			{
				for (j=0;j<MAX_POINT;j++)
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
					break;
				}


			}
		}
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


	}
	PRINTFF ("ulLastPointNum=%d \r\n",g_PointStatus.ulLastPointNum);
	return 0;
}


//ɾ��distance��������ĳ��ֵ
void DeleteAtDistance(CALC_DISTANCE *dis,int pos,int *len)
{
	int i;
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

//��ȡ��ǰС�ھ�����ֵ��λ��
static __INLINE int GetThresholdPos(CALC_DISTANCE *arr, int len)
{
	int i = 0;
	for(i=0;i<len;i++)
	{
		if (arr[i].distance >= DISTANCE_THRESHOLD)
		{
			return i;
		}
	}
	return i;
}

//ϣ������ 
void shell_sort(CALC_DISTANCE *arr, int len) 
{
	int gap, i, j;
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

//��������
void InsertSort(CALC_DISTANCE *a , int n)
{  
	int i,j;
	int ThresholdPos = 0;
	CALC_DISTANCE x;
	if(n <= 1)	//ֻ��1��Ԫ��ʱû�а취����
		return;
	if(a == NULL)
		return;

	for(i= 1; i<n; i++)
	{  
		if(a[i].distance < a[i-1].distance)	//����i��Ԫ�ش���i-1Ԫ�أ�ֱ�Ӳ��롣С�ڵĻ����ƶ����������  
		{               
			j= i-1;   
			x = a[i];        //����Ϊ�ڱ������洢������Ԫ��  
			a[i] = a[i-1];           //�Ⱥ���һ��Ԫ��  
			while((j > 0) && (x.distance < a[j].distance))	//�����������Ĳ���λ��  
			{  
				a[j+1] = a[j];  
				j--;         //Ԫ�غ���  
			}  
			a[j+1] = x;      //���뵽��ȷλ��  
		}  
	}  

}

//int�������
static __INLINE int sum(int *a,int len)
{
	int i;
	int sum = 0;
	for(i=0;i<len;i++)
		sum += a[i];
	return sum;
}
//��ȡһ��int���� buff�е���Сֵ
int GetMin(int* buf,int len,int *CurPos)
{
	int i;
	int min ;
	if((len==0)||(buf == 0))
		return 0;
	min = buf[0];
	*CurPos = 0;
	for (i = 0;i<len;i++)
	{
		if (buf[i]< min)
		{
			min = buf[i];
			*CurPos = i;
		}
	}
	return min;
}
