#ifndef __CALC_POINT__
#define __CALC_POINT__

#include <stdio.h>
#include <math.h>

#define __INLINE __inline 

#ifndef  __CC_ARM
#define PRINTFF printf
#else
#define PRINTFF
#endif

#define MAX_POINT 5		//最大支持点数
#define MAX_POINT_C MAX_POINT*3	//计算点ID时候的缓存个数
#define MAX_POINT_REC 3	//每个矩形里面 支持的最大点数

#define SCAN_X_SQUARE_NUM	5
#define SCAN_Y_SQUARE_NUM	4

#define DISTANCE_THRESHOLD 20*20		//两个菱形相距20 以内 则认为是相交的
#define DISTANCE_THRESHOLD_WC	60*60	//特殊情况下可以延长到40
#define DISTANCE_THRESHOLD_ID		50*50	//两次两个点间距在50以内则认为这两个点ID相同
#define TIMES_THRESHOLD_UP		3	//连续检测到TIMES_THRESHOLD_UP 次这个点没数据就认为这个点已经抬起

#define DIAMON_RATIO		23	//菱形的长宽比超过一定程度就说明这是个 细长菱形，需要用特殊方法判定点
#define LONG_DIA_THRESHOLD  40	//长条菱形，通过一个方向的坐标距离来判定是否相交
#define SMALL_THRESHOLD_L	40	//如果有一个方向菱形很小，那么就用这个菱形来计算最终点ID 长小于30
#define SMALL_THRESHOLD_W	20	//如果有一个方向菱形很小，那么就用这个菱形来计算最终点ID 宽小于10

#define EXPOS_OFFSET 8
#define LENGTH_PRE		128
#define TOTAL_LENGTH	(LENGTH_PRE*SCAN_X_SQUARE_NUM +EXPOS_OFFSET*2)	//白板总宽度
#define TATAL_HEIGHT	(LENGTH_PRE*(SCAN_Y_SQUARE_NUM-1) + 72 + EXPOS_OFFSET*2) //白板总高度


#define NULL 0 

#ifndef  __CC_ARM

#pragma pack(push)
#pragma pack(1)
//点坐标结构
typedef struct CALC_POINT_S
{
    signed int  x;    /*X坐标*/
	signed int y;    /*Y坐标*/
}CALC_POINT;

//菱形坐标结构
typedef struct CALC_DIAMOND_S
{
    unsigned int ulDiamondNum;   /*菱形编号*/
    CALC_POINT strPoint0;        /*菱形顶点P0坐标*/
	CALC_POINT strPoint1;        /*菱形顶点P1坐标*/
    CALC_POINT strPoint2;        /*菱形顶点P2坐标*/
    CALC_POINT strPoint3;        /*菱形顶点P3坐标*/
}CALC_DIAMOND;

//输出菱形集合结构
typedef struct CALC_DIAMOND_INTERFACE_S
{
    unsigned int ulDiamondCount;		/*总共菱形个数*/
    CALC_DIAMOND strDiamondPoint[9];	/*菱形坐标数组*/
}CALC_DIAMOND_INTERFACE;

#endif
//一次扫描的所有数据
typedef struct CALC_DIAMOND_BUF_S
{
    CALC_DIAMOND_INTERFACE strXSquareDiamond[SCAN_X_SQUARE_NUM];
    CALC_DIAMOND_INTERFACE strYSquareDiamond[SCAN_Y_SQUARE_NUM];
	char cRev[6];
}CALC_DIAMOND_BUF;

//计算中心间距的结构体
typedef struct CALC_DISTANCE_S
{
	unsigned int distance;
	unsigned char XPos;
	unsigned char YPos;
}CALC_DISTANCE;

typedef struct CACL_DIAMOND_CENTER_S
{
	CALC_POINT Point;
	signed char Rec;
	signed char Diamond;
}CACL_DIAMOND_CENTER;

typedef struct CACL_PROJECTION_COUNT_S
{
	signed char ACount;
	signed char BCount;
	signed char CCount;
	signed char DCount;
	signed char APos[3];	
	signed char BPos[3];	
	signed char CPos[3];	
	signed char DPos[3];
}CACL_PROJECTION_COUNT;

typedef struct POS_USE_S
{
	unsigned int XPosUse[MAX_POINT_C];
	unsigned int PosNum;
	unsigned int YPosUse[MAX_POINT_C];
}POS_USE;

#ifndef  __CC_ARM
typedef struct POINT_C_S
{
	unsigned short x;
	unsigned short y;
}POINT_C;

struct PT_BUF
{
	unsigned short valid;
	unsigned short id;
	unsigned short tip;	 // tip for down and up events  2013.09.07
	POINT_C pt_val;
};
#endif

typedef struct PT_STATUS_L_S
{
	unsigned int ulLastPointNum;              /*实点状态0、1、2、3。。。*/
	struct PT_BUF LastPoint[MAX_POINT_C];   /*上次的实点信息*/
	char LastPointUpTime[MAX_POINT_C];	/*此点已经多少次没有扫描到信息*/
}PT_STATUS_L;

#ifndef  __CC_ARM
#pragma pack(pop)
#endif

#define CALCL_AVERAGE_TWO(a,b) ((int)(((int)(a)+(int)(b))/2))
#define CALCL_X_DIAMOND_CENTRE_X(a) CALCL_AVERAGE_TWO((a).strPoint0.x,((a)).strPoint3.x)
#define CALCL_X_DIAMOND_CENTRE_X_E(a) CALCL_AVERAGE_TWO((a).strPoint1.x,((a)).strPoint2.x)
#define CALCL_X_DIAMOND_CENTRE_Y(a) CALCL_AVERAGE_TWO(((a)).strPoint1.y,((a)).strPoint2.y)
#define CALCL_Y_DIAMOND_CENTRE_X(a) CALCL_AVERAGE_TWO((a).strPoint1.x,((a)).strPoint2.x)
#define CALCL_Y_DIAMOND_CENTRE_Y(a) CALCL_AVERAGE_TWO(((a)).strPoint0.y,((a)).strPoint3.y)
#define CALCL_Y_DIAMOND_CENTRE_Y_E(a) CALCL_AVERAGE_TWO(((a)).strPoint1.y,((a)).strPoint2.y)

#define CALCL_REL_TO_ABS(a,b) ((a)+(b)*128)		


#define DIAMOND_X_GET_P(p,a,b,c)  (((CALC_DIAMOND_BUF*)p)->strXSquareDiamond[a].strDiamondPoint[b].strPoint##c)
#define DIAMOND_X_GET_N(p,a,b)  (((CALC_DIAMOND_BUF*)p)->strXSquareDiamond[a].strDiamondPoint[b].ulDiamondNum)
#define DIAMOND_X_GET_C(p,a)	(((CALC_DIAMOND_BUF*)p)->strXSquareDiamond[a].ulDiamondCount)
#define DIAMOND_X_GET_D(p,a,b)  (((CALC_DIAMOND_BUF*)p)->strXSquareDiamond[a].strDiamondPoint[b])

#define DIAMOND_Y_GET_P(p,a,b,c)  (((CALC_DIAMOND_BUF*)p)->strYSquareDiamond[a].strDiamondPoint[b].strPoint##c)
#define DIAMOND_Y_GET_N(p,a,b)  (((CALC_DIAMOND_BUF*)p)->strYSquareDiamond[a].strDiamondPoint[b].ulDiamondNum)
#define DIAMOND_Y_GET_C(p,a)	(((CALC_DIAMOND_BUF*)p)->strYSquareDiamond[a].ulDiamondCount)
#define DIAMOND_Y_GET_D(p,a,b)  (((CALC_DIAMOND_BUF*)p)->strYSquareDiamond[a].strDiamondPoint[b])

#define CALCL_SQUAR(a) ((a)*(a)) 

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))

#define DIAMON_WRH_X(p) (((p).strPoint2.x - (p).strPoint1.x)==0) ? 0 : \
						(((p).strPoint0.y - (p).strPoint3.y) / \
						((p).strPoint2.x - (p).strPoint1.x))
#define DIAMON_WRH_Y(p) (((p).strPoint2.y - (p).strPoint1.y)==0) ? 0 : \
						(((p).strPoint0.x - (p).strPoint3.x) / \
						((p).strPoint2.y - (p).strPoint1.y))

//-------------------------------------------------------------------------------------
//	|- y0 = k * x0 + b
//	|
//	|- y1 = k * x1 + b
//	解方程
//	k = (y0 - y1)/(x0 - x1)
//	b = y0 - k * x0 =  y0 - ((y0 - y1) * x0)/(x0 - x1)
//	
//	根据x求y的值公式
//	y3 = k * x3 + b = ((y0 - y1) * x3)/(x0 - x1) + (y0 - ((y0 - y1) * x0)/(x0 - x1))
//	
//	根据y求x的值公式
//	x3 = (y3 - b) / k = (y3 - (y0 - ((y0 - y1) * x0)/(x0 - x1))) * (x0 - x1) / (y0 - y1)
//-------------------------------------------------------------------------------------
//	(y-y1)/(x-x1)=(y2-y1)/(x2-x1)
//
//	根据x求y
//	y = (x - x1) * (y2 - y1) / (x2 - x1) + y1
//
//	根据y求x
//	x = (y - y1) * (x2 - x1) / (y2 - y1) + x1
//-------------------------------------------------------------------------------------
#if 0
#define ADJUST_XREC_X_REL(p,y3)  ((y3 - (((CALC_DIAMOND*)p)->strPoint0.y - \
	((((CALC_DIAMOND*)p)->strPoint0.y - ((CALC_DIAMOND*)p)->strPoint3.y) * \
	((CALC_DIAMOND*)p)->strPoint0.x)/(((CALC_DIAMOND*)p)->strPoint0.x - \
	((CALC_DIAMOND*)p)->strPoint3.x))) * (((CALC_DIAMOND*)p)->strPoint0.x - \
	((CALC_DIAMOND*)p)->strPoint3.x) / (((CALC_DIAMOND*)p)->strPoint0.y - \
	((CALC_DIAMOND*)p)->strPoint3.y))
#define ADJUST_YREC_Y(p,x3)  ((((CALC_DIAMOND*)p)->strPoint0.y - \
	((CALC_DIAMOND*)p)->strPoint3.y) * x3)/(((CALC_DIAMOND*)p)->strPoint0.x -\
	((CALC_DIAMOND*)p)->strPoint3.x) + (((CALC_DIAMOND*)p)->strPoint0.y - \
	((((CALC_DIAMOND*)p)->strPoint0.y - ((CALC_DIAMOND*)p)->strPoint3.y) * \
	((CALC_DIAMOND*)p)->strPoint0.x)/(((CALC_DIAMOND*)p)->strPoint0.x - \
	((CALC_DIAMOND*)p)->strPoint3.x))
#else
#define ADJUST_XREC_X_REL(p,y0) ((y0 - ((CALC_DIAMOND*)p)->strPoint3.y) * \
	(((CALC_DIAMOND*)p)->strPoint0.x - ((CALC_DIAMOND*)p)->strPoint3.x) / \
	(((CALC_DIAMOND*)p)->strPoint0.y - ((CALC_DIAMOND*)p)->strPoint3.y) + \
	((CALC_DIAMOND*)p)->strPoint3.x)
//x = (y - y1) * (x2 - x1) / (y2 - y1) + x1
//y = (x - x1) * (y2 - y1) / (x2 - x1) + y1
#define ADJUST_YREC_Y_REL(p,x0) ((x0 - ((CALC_DIAMOND*)p)->strPoint0.x) * \
	(((CALC_DIAMOND*)p)->strPoint3.y - ((CALC_DIAMOND*)p)->strPoint0.y) / \
	(((CALC_DIAMOND*)p)->strPoint3.x - ((CALC_DIAMOND*)p)->strPoint0.x) + \
	((CALC_DIAMOND*)p)->strPoint0.y)
#endif

#define ADD_LOST_POINT_X	\
for(m=PointNumTmp;m< PointNumTmp_t;m++)	\
{	\
	if(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[m].XPos].Rec,XPoint[DistanceTmp[m].XPos].Diamond)==DiamondNumTmp)	\
	{	\
		ExcDistance(DistanceTmp,PointNumTmp,m);	\
		PointNumTmp++;	\
		PRINTFF("X add lost point %08x no point\r\n",j,k,DiamondNumTmp);	\
		break;\
	}	\
}

#define ADD_LOST_POINT_Y	\
	for(m=PointNumTmp;m< PointNumTmp_t;m++)	\
{	\
	if(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[m].YPos].Rec,YPoint[DistanceTmp[m].YPos].Diamond)==DiamondNumTmp)	\
{	\
	ExcDistance(DistanceTmp,PointNumTmp,m);	\
	PointNumTmp++;	\
	PRINTFF("Y add lost point %08x no point\r\n",j,k,DiamondNumTmp);	\
	break;\
}	\
}

void PrintDiamond(CALC_DIAMOND_BUF *DiamondBuf,char *OutBuf,unsigned int ScanCount);
int CalcPoint(CALC_DIAMOND_BUF *DiamondBuf,struct PT_BUF *point);
void shell_sort(CALC_DISTANCE *arr,unsigned int len) ;
void DeleteAtDistance(CALC_DISTANCE *dis,unsigned int pos,unsigned int *len);
static signed int CalcPointID(struct PT_BUF *point,signed int *num);
void ExcDistance(CALC_DISTANCE *dis,signed int src,signed int dst);
void ForceDeleteAtDistance(CALC_DISTANCE *dis,unsigned int pos,signed int *len);
void ClearPointID(void);
extern PT_STATUS_L g_PointStatus;

#endif