#include "CalcPoint.h"

PT_STATUS g_PointStatus;	//扫描点ID的状态


//获取菱形中心点
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

//获取两点间距
//为了节省计算量 不开平方 来表示点间距
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


// 打印所有菱形数据
//DiamondBuf: 菱形数据指针
//OutBuf： 输出的buff
//ScanCount: 当前数据编号
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

//计算触摸点
//DiamondBuf: 菱形数据指针	
//point: 得到触摸点数据缓存
//返回值：触摸点的个数
int CalcPoint(CALC_DIAMOND_BUF *DiamondBuf,struct PT_BUF *point)
{
	unsigned int i,j,k,xDiamondNum,yDiamondNum,disNUM,disNUM_f,PointNum;
	int xPointNum[SCAN_X_SQUARE_NUM],xPointNumSum;
	int yPointNum[SCAN_Y_SQUARE_NUM],yPointNumSum;
	int PointNumTmp;
	int PosTem;
	int xSure, ySure;
	CACL_DIAMOND_CENTER XPoint[MAX_POINT_REC*MAX_POINT_REC*SCAN_X_SQUARE_NUM];	//x方向菱形 中心点buff
	CACL_DIAMOND_CENTER YPoint[MAX_POINT_REC*MAX_POINT_REC*SCAN_Y_SQUARE_NUM]; //y方向菱形 中心点buff
	CALC_DISTANCE DistanceTmp[MAX_POINT_REC*MAX_POINT_REC*SCAN_X_SQUARE_NUM*MAX_POINT_REC*MAX_POINT_REC*SCAN_Y_SQUARE_NUM];	//中心点间距buff
	CACL_PROJECTION_COUNT XProjectionCount[SCAN_X_SQUARE_NUM];	//X方向每个矩形里面的每个三角里面形投影的个数
	CACL_PROJECTION_COUNT YProjectionCount[SCAN_Y_SQUARE_NUM];	//Y方向每个矩形里面的每个三角里面形投影的个数
	xDiamondNum = 0;	//x方向有效菱形 中心点个数
	yDiamondNum = 0;	//y方向有效菱形 中心点个数
	disNUM = 0;	//中心点间距个数
	disNUM_f = 0;
	PointNum = 0;	//最终点的个数
	xPointNumSum = 0;
	yPointNumSum = 0;
	xSure = 0;
	ySure = 0;
	PointNumTmp = 0; //根据中心点距离计算出来的触摸点个数， 可能存在假点或者少点
	memset(XProjectionCount,0xff,sizeof(XProjectionCount));	//x方向菱形数算出来的实际点个数
	memset(YProjectionCount,0xff,sizeof(YProjectionCount));	//y方向菱形计算出来的实际点个数

	//计算出所有X方向菱形中心点
	for(i = 0;i < SCAN_X_SQUARE_NUM; i++)
	{	
		if(DIAMOND_X_GET_C(DiamondBuf,i) ==0)
			continue;

		for(j=0;j<DIAMOND_X_GET_C(DiamondBuf,i);j++)
		{
			if(DIAMOND_X_GET_N(DiamondBuf,i,j) == 0)
				continue;
			//计算AD两个三角形内分别有几个投影
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
			//计算BC两个三角形内分别有几个投影
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
				
			if((DIAMOND_X_GET_P(DiamondBuf,i,j,1).y ==0) || (DIAMOND_X_GET_P(DiamondBuf,i,j,1).y ==TATAL_HEIGHT) ||			//三角形
				(DIAMOND_X_GET_P(DiamondBuf,i,j,2).y ==0)|| (DIAMOND_X_GET_P(DiamondBuf,i,j,2).y ==TATAL_HEIGHT))
			{
				if(DIAMOND_X_GET_P(DiamondBuf,i,j,0).x == DIAMOND_X_GET_P(DiamondBuf,i,j,3).x)	//边界三角形
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
				else	//矩形内的三角形
				{
					
				}
			}
			else
			{
				//菱形
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

	//计算出所有Y方向菱形中心点
	for(i = 0;i < SCAN_Y_SQUARE_NUM; i++)
	{
		if(DIAMOND_Y_GET_C(DiamondBuf,i) ==0)
			continue;
		for(j=0;j<DIAMOND_Y_GET_C(DiamondBuf,i);j++)
		{
			if(DIAMOND_Y_GET_N(DiamondBuf,i,j) == 0)
				continue;

			//计算AD两个三角形内分别有几个投影
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
			//计算BC两个三角形内分别有几个投影
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

			if((DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y ==0) || (DIAMOND_Y_GET_P(DiamondBuf,i,j,1).y ==TATAL_HEIGHT) ||			//三角形
				(DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y ==0)|| (DIAMOND_Y_GET_P(DiamondBuf,i,j,2).y ==TATAL_HEIGHT))
			{
				if(DIAMOND_Y_GET_P(DiamondBuf,i,j,0).x == DIAMOND_Y_GET_P(DiamondBuf,i,j,3).x)	//边界三角形
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
				else	//矩形内的三角形
				{

				}
			}
			else
			{
				//菱形
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
	//计算出所有中心点间距
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
	memset(xPointNum,0,sizeof(xPointNum));	//x方向菱形数算出来的实际点个数
	memset(yPointNum,0,sizeof(yPointNum));	//y方向菱形计算出来的实际点个数
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

	PointNum = ((xPointNumSum>yPointNumSum) ? xPointNumSum : yPointNumSum);	//最终是实际点数应该是两者中的 较大值

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
	
	shell_sort(DistanceTmp,disNUM);	//排序
	PointNumTmp = GetThresholdPos(DistanceTmp, disNUM);	//获取点个数
	PRINTFF("xPoint=%d yPoint=%d PointNum=%d PointNumTmp=%d\r\n",xPointNumSum,yPointNumSum,PointNum,PointNumTmp);
	/*3175-3375 多点*/
	/*2f75 少点且通过距离判断点错误 需要通过投影来确定*/ 
	/*2c75 少点通过且距离可以判断出正确点*/
	/*2575 菱形复用*/
RESTART_LESS:
	//采样误差造成，可能会少点 通过扩大阈值来 补充点
	if (PointNumTmp < PointNum)	
	{
		while ((DistanceTmp[PointNumTmp].distance < DISTANCE_THRESHOLD_WC) && (PointNumTmp < PointNum)) 
		{
			PointNumTmp++;	//确实少点 那就按照 少点来计算
		}
	}

	//memset(xPointNum,0,sizeof(xPointNum));	//x方向菱形数算出来的实际点个数
	//memset(yPointNum,0,sizeof(yPointNum));	//y方向菱形计算出来的实际点个数
	//for(i = 0;i < PointNumTmp; i++)	//求出每个矩形 拥有的点数
	//{
	//	xPointNum[XPoint[DistanceTmp[i].XPos].Rec]++ ;
	//	yPointNum[YPoint[DistanceTmp[i].YPos].Rec]++ ;
	//}

	for(i = 0;i < SCAN_X_SQUARE_NUM; i++)  
	{
		if(xPointNum[i] ==0)	//没有点 略过
			continue;
		//if ((PointNum == 3)&&(DIAMOND_X_GET_C(DiamondBuf,i)==4))//3点4个投影时 有可能存在公用菱形 也可能不存在默认存在 
		//{
		//	continue;
		//}
		if((xSure == 1)||(xPointNum[i] == 3))
		//if(xSure == 1)
		{

			if(((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2) > xPointNum[i]) ||// AD两个三角形投影个数大于点个数那么 这就是一个错误帧
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2) > xPointNum[i])) // BC两个三角形投影个数大于点个数 这是一个错误的数据帧
			{
				continue;
			}

			//3点的 田字形 2点的 口字型 不能有重复的菱形 和重复的投影
			if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2)==xPointNum[i])&&
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2)==xPointNum[i])) 
			{
				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//删除使用同一个菱形的数据
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

						//某些情况下，一个投影内只有一个点，必须删除同一个投影中重合的数据

						if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//如果不在同一个矩形里面 没有可比性
							continue;

						if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) & 0xffff0000) == //删除AD两个三角形里面重复使用投影的的点
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

						if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) &0x0000ffff) ==  //删除BC两个三角形里面重复使用投影的的点
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

			//3点的 日字形 2点的 以字型 不能有重复的菱形 AD不能重复的投影 BC 必须每个投影都有点
			if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2)==xPointNum[i])&&
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2) < xPointNum[i])) 
			{
				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//删除使用同一个菱形的数据
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

						//某些情况下，一个投影内只有一个点，必须删除同一个投影中重合的数据

						if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//如果不在同一个矩形里面 没有可比性
							continue;

						if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) & 0xffff0000) == //删除AD两个三角形里面重复使用投影的的点
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

						//每个投影都有点 。。。。。

					}
				}
			}

			//3点的 日字形 2点的 一字型 不能有重复的菱形 BC不能重复的投影 AD必须每个投影都有点
			if (((XProjectionCount[i].ACount + XProjectionCount[i].DCount + 2) < xPointNum[i])&&
				((XProjectionCount[i].BCount + XProjectionCount[i].CCount + 2)  == xPointNum[i]))
			{
				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//删除使用同一个菱形的数据
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

						//某些情况下，一个投影内只有一个点，必须删除同一个投影中重合的数据

						if((XPoint[DistanceTmp[j].XPos].Rec != i ) || (XPoint[DistanceTmp[k].XPos].Rec != i))	//如果不在同一个矩形里面 没有可比性
							continue;

						if ((DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond) &0x0000ffff) ==  //删除BC两个三角形里面重复使用投影的的点
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

						//每个投影都有点 。。。。。

					}
				}
			}
			//每个投影都有点
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

		//if ((PointNum == 3)&&(DIAMOND_Y_GET_C(DiamondBuf,i)==4))//3点4个投影时 有可能存在公用菱形 也可能不存在默认不存在 
		//{
		//	continue;
		//}
		if((ySure == 1)||(yPointNum[i] == 3))
		//if(ySure == 1)
		{

			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) > yPointNum[i])||
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) > yPointNum[i]))// AD或者BC两个三角形投影个数大于点个数那么这是一个错误帧
			{
				continue;
			}

			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) == yPointNum[i])&& // AD两个三角形投影个数等于点个数那么就不能存在投影复用的情况
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) == yPointNum[i]))// BC两个三角形投影个数等于点个数那么就不能存在投影复用的情况
			{
				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//删除使用同一个菱形的数据
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

						//某些情况下，一个投影内只有一个点，必须删除同一个投影中重合的数据
						if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//如果两个点不在一个矩形里面 不具备这个条件
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
			//3点的 日字形 2点的 以字型 不能有重复的菱形 AD不能重复的投影 BC 必须每个投影都有点
			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) == yPointNum[i])&& // AD两个三角形投影个数等于点个数那么就不能存在投影复用的情况
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) < yPointNum[i]))// BC两个三角形投影个数小于点个数那么每个菱形都有点
			{

				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//删除使用同一个菱形的数据
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

						//某些情况下，一个投影内只有一个点，必须删除同一个投影中重合的数据
						if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//如果两个点不在一个矩形里面 不具备这个条件
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
						//每个投影都有点 。。。。。

					}
				}
			}
			//3点的 日字形 2点的 一字型 不能有重复的菱形 BC不能重复的投影 AD必须每个投影都有点
			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) < yPointNum[i])&& 
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) == yPointNum[i]))
			{

				for (j = 0;j < PointNumTmp;j++)
				{
					for (k = j+1 ;k < PointNumTmp ;k++)
					{
						//删除使用同一个菱形的数据
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

						//某些情况下，一个投影内只有一个点，必须删除同一个投影中重合的数据
						if((YPoint[DistanceTmp[j].YPos].Rec != i)||(YPoint[DistanceTmp[k].YPos].Rec != i))	//如果两个点不在一个矩形里面 不具备这个条件
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
						//每个投影都有点 。。。。。
					}
				}
			}
			//每个投影都有点
			if (((YProjectionCount[i].ACount + YProjectionCount[i].DCount + 2) < yPointNum[i])&& 
				((YProjectionCount[i].BCount + YProjectionCount[i].CCount + 2) < yPointNum[i]))
			{
			}

		}
	}


	//可能存在边界点，菱形细长，需要特殊处理
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
		// 如何处理呢？
	}
COPY_POINT:
	//返回所有有效值
	for(i=0;i<PointNumTmp;i++)
	{
		point[i].pt_val.x = XPoint[DistanceTmp[i].XPos].Point.x;
		point[i].pt_val.y = YPoint[DistanceTmp[i].YPos].Point.y;
	}
#if 1
	// 点坐标数据校正
	for (j = 0;j < PointNumTmp;j++)
	{
		for (k = j+1 ;k < PointNumTmp ;k++)
		{
			if(XPoint[DistanceTmp[j].XPos].Rec == XPoint[DistanceTmp[k].XPos].Rec)	//如果不在同一个矩形里面 没有可比性
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
			if(YPoint[DistanceTmp[j].YPos].Rec == YPoint[DistanceTmp[k].YPos].Rec)	//如果两个点不在一个矩形里面 不具备这个条件
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

	//第一次触摸开始
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
			if (min_distance < DISTANCE_THRESHOLD_ID)	//这个点是个老点
			{
				point[i].id = min_pos+1;
				point[i].tip = 1;
				point[i].valid = 1;
				g_PointStatus.LastPoint[min_pos] = point[i];
				g_PointStatus.LastPointUpTime[min_pos] = 0;	//抬起次数清零
			}
			else	//是个新点
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
		//检测是不是有的点已经多次没有数据 如果没有数据，那就把这个点发送一次抬起事件
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


//删除distance数组里面某个值
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

//获取当前小于距离阈值的位置
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

//希尔排序 
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

//插入排序法
void InsertSort(CALC_DISTANCE *a , int n)
{  
	int i,j;
	int ThresholdPos = 0;
	CALC_DISTANCE x;
	if(n <= 1)	//只有1个元素时没有办法排序
		return;
	if(a == NULL)
		return;

	for(i= 1; i<n; i++)
	{  
		if(a[i].distance < a[i-1].distance)	//若第i个元素大于i-1元素，直接插入。小于的话，移动有序表后插入  
		{               
			j= i-1;   
			x = a[i];        //复制为哨兵，即存储待排序元素  
			a[i] = a[i-1];           //先后移一个元素  
			while((j > 0) && (x.distance < a[j].distance))	//查找在有序表的插入位置  
			{  
				a[j+1] = a[j];  
				j--;         //元素后移  
			}  
			a[j+1] = x;      //插入到正确位置  
		}  
	}  

}

//int数组求和
static __INLINE int sum(int *a,int len)
{
	int i;
	int sum = 0;
	for(i=0;i<len;i++)
		sum += a[i];
	return sum;
}
//获取一个int类型 buff中的最小值
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
