#ifndef __CALC_POINT__
#define __CALC_POINT__


#include <math.h>

#ifndef  __CC_ARM
#define __INLINE __inline 
#endif

#define MAX_POINT	5		//最大支持点数
#define MAX_POINT_REC 3	//每个矩形里面 支持的最大点数

#define SCAN_X_SQUARE_NUM	5
#define SCAN_Y_SQUARE_NUM	4

#define DISTANCE_THRESHOLD 20*20		//两个菱形相距20 以内 则认为是相交的
#define DISTANCE_THRESHOLD_WC	40*40	//特殊情况下可以延长到40
#define DISTANCE_THRESHOLD_ID		50*50	//两次两个点间距在50以内则认为这两个点ID相同
#define TIMES_THRESHOLD_UP		5	//连续检测到TIMES_THRESHOLD_UP 次这个点没数据就认为这个点已经抬起

#define TOTAL_LENGTH	128*SCAN_X_SQUARE_NUM	//白板总宽度
#define TATAL_HEIGHT	128*SCAN_Y_SQUARE_NUM + 72 //白板总高度


#define NULL 0 

#pragma pack(push)
#pragma pack(1)

//点坐标结构
typedef struct CALC_POINT_S
{
    int  x;    /*X坐标*/
	int y;    /*Y坐标*/
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
	int distance;
	char XPos;
	char YPos;
}CALC_DISTANCE;

typedef struct CACL_DIAMOND_CENTER_S
{
	CALC_POINT Point;
	char Rec;
	char Diamond;
}CACL_DIAMOND_CENTER;

typedef struct CACL_PROJECTION_COUNT_S
{
	char ACount;
	char BCount;
	char CCount;
	char DCount;
}CACL_PROJECTION_COUNT;

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

typedef struct PT_STATUS_S
{
	unsigned int ulLastPointNum;              /*实点状态0、1、2、3。。。*/
	struct PT_BUF LastPoint[MAX_POINT];   /*上次的实点信息*/
	char LastPointUpTime[MAX_POINT];	/*此点已经多少次没有扫描到信息*/
}PT_STATUS;
#pragma pack(pop)


#define CALCL_AVERAGE_TWO(a,b) ((int)(((int)(a)+(int)(b))/2))
#define CALCL_DIAMOND_CENTRE_X(a) CALCL_AVERAGE_TWO((a).strPoint0.x,((a)).strPoint3.x)
#define CALCL_DIAMOND_CENTRE_Y(a) CALCL_AVERAGE_TWO(((a)).strPoint1.y,((a)).strPoint2.y)
#define CALCL_REL_TO_ABS(a,b) ((a)+(b)*128)		


#define DIAMOND_X_GET(p,a,b,c)  (((CALC_DIAMOND_BUF*)p)->strXSquareDiamond[a].strDiamondPoint[b].strPoint##c)
#define DIAMOND_X_GET_N(p,a,b)  (((CALC_DIAMOND_BUF*)p)->strXSquareDiamond[a].strDiamondPoint[b].ulDiamondNum)
#define DIAMOND_X_GET_C(p,a)	(((CALC_DIAMOND_BUF*)p)->strXSquareDiamond[a].ulDiamondCount)

#define DIAMOND_Y_GET(p,a,b,c)  (((CALC_DIAMOND_BUF*)p)->strYSquareDiamond[a].strDiamondPoint[b].strPoint##c)
#define DIAMOND_Y_GET_N(p,a,b)  (((CALC_DIAMOND_BUF*)p)->strYSquareDiamond[a].strDiamondPoint[b].ulDiamondNum)
#define DIAMOND_Y_GET_C(p,a)	(((CALC_DIAMOND_BUF*)p)->strYSquareDiamond[a].ulDiamondCount)
#define CALCL_SQUAR(a) ((a)*(a)) 

#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))


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
#define ADJUST_YREC_Y(p,x3)  ((((CALC_DIAMOND*)p)->strPoint1.y - \
	((CALC_DIAMOND*)p)->strPoint2.y) * x3)/(((CALC_DIAMOND*)p)->strPoint1.x -\
	((CALC_DIAMOND*)p)->strPoint2.x) + (((CALC_DIAMOND*)p)->strPoint1.y - \
	((((CALC_DIAMOND*)p)->strPoint1.y - ((CALC_DIAMOND*)p)->strPoint2.y) * \
	((CALC_DIAMOND*)p)->strPoint1.x)/(((CALC_DIAMOND*)p)->strPoint1.x - \
	((CALC_DIAMOND*)p)->strPoint2.x))
#else
#define ADJUST_XREC_X_REL(p,y0) ((y0 - ((CALC_DIAMOND*)p)->strPoint3.y) * \
	(((CALC_DIAMOND*)p)->strPoint0.x - ((CALC_DIAMOND*)p)->strPoint3.x) / \
	(((CALC_DIAMOND*)p)->strPoint0.y - ((CALC_DIAMOND*)p)->strPoint3.y) + \
	((CALC_DIAMOND*)p)->strPoint3.x)
//x = (y - y1) * (x2 - x1) / (y2 - y1) + x1
//y = (x - x1) * (y2 - y1) / (x2 - x1) + y1
#define ADJUST_YREC_Y_REL(p,x0) ((x0 - ((CALC_DIAMOND*)p)->strPoint1.x) * \
	(((CALC_DIAMOND*)p)->strPoint2.y - ((CALC_DIAMOND*)p)->strPoint1.y) / \
	(((CALC_DIAMOND*)p)->strPoint2.x - ((CALC_DIAMOND*)p)->strPoint1.x) + \
	((CALC_DIAMOND*)p)->strPoint1.y)
#endif



void PrintDiamond(CALC_DIAMOND_BUF *DiamondBuf,char *OutBuf,int ScanCount);
int CalcPoint(CALC_DIAMOND_BUF *DiamondBuf,struct PT_BUF *point);
int GetMin(int* buf,int len,int *CurPos);
void InsertSort(CALC_DISTANCE *a, int n);
void shell_sort(CALC_DISTANCE *arr, int len) ;
void DeleteAtDistance(CALC_DISTANCE *dis,int pos,int len);
static int CalcPointID(struct PT_BUF *point,int *num);

extern PT_STATUS g_PointStatus;	

#endif