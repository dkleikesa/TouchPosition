#ifndef __CALC_POINT__
#define __CALC_POINT__


#include <math.h>

#ifndef  __CC_ARM
#define __INLINE __inline 
#endif

#define MAX_POINT	5		//���֧�ֵ���
#define MAX_POINT_REC 3	//ÿ���������� ֧�ֵ�������

#define SCAN_X_SQUARE_NUM	5
#define SCAN_Y_SQUARE_NUM	4

#define DISTANCE_THRESHOLD 20*20		//�����������20 ���� ����Ϊ���ཻ��
#define DISTANCE_THRESHOLD_WC	40*40	//��������¿����ӳ���40
#define DISTANCE_THRESHOLD_ID		50*50	//��������������50��������Ϊ��������ID��ͬ
#define TIMES_THRESHOLD_UP		5	//������⵽TIMES_THRESHOLD_UP �������û���ݾ���Ϊ������Ѿ�̧��

#define TOTAL_LENGTH	128*SCAN_X_SQUARE_NUM	//�װ��ܿ��
#define TATAL_HEIGHT	128*SCAN_Y_SQUARE_NUM + 72 //�װ��ܸ߶�


#define NULL 0 

#pragma pack(push)
#pragma pack(1)

//������ṹ
typedef struct CALC_POINT_S
{
    int  x;    /*X����*/
	int y;    /*Y����*/
}CALC_POINT;

//��������ṹ
typedef struct CALC_DIAMOND_S
{
    unsigned int ulDiamondNum;   /*���α��*/
    CALC_POINT strPoint0;        /*���ζ���P0����*/
	CALC_POINT strPoint1;        /*���ζ���P1����*/
    CALC_POINT strPoint2;        /*���ζ���P2����*/
    CALC_POINT strPoint3;        /*���ζ���P3����*/
}CALC_DIAMOND;

//������μ��Ͻṹ
typedef struct CALC_DIAMOND_INTERFACE_S
{
    unsigned int ulDiamondCount;		/*�ܹ����θ���*/
    CALC_DIAMOND strDiamondPoint[9];	/*������������*/
}CALC_DIAMOND_INTERFACE;

//һ��ɨ�����������
typedef struct CALC_DIAMOND_BUF_S
{
    CALC_DIAMOND_INTERFACE strXSquareDiamond[SCAN_X_SQUARE_NUM];
    CALC_DIAMOND_INTERFACE strYSquareDiamond[SCAN_Y_SQUARE_NUM];
	char cRev[6];
}CALC_DIAMOND_BUF;

//�������ļ��Ľṹ��
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
	unsigned int ulLastPointNum;              /*ʵ��״̬0��1��2��3������*/
	struct PT_BUF LastPoint[MAX_POINT];   /*�ϴε�ʵ����Ϣ*/
	char LastPointUpTime[MAX_POINT];	/*�˵��Ѿ����ٴ�û��ɨ�赽��Ϣ*/
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
//	�ⷽ��
//	k = (y0 - y1)/(x0 - x1)
//	b = y0 - k * x0 =  y0 - ((y0 - y1) * x0)/(x0 - x1)
//	
//	����x��y��ֵ��ʽ
//	y3 = k * x3 + b = ((y0 - y1) * x3)/(x0 - x1) + (y0 - ((y0 - y1) * x0)/(x0 - x1))
//	
//	����y��x��ֵ��ʽ
//	x3 = (y3 - b) / k = (y3 - (y0 - ((y0 - y1) * x0)/(x0 - x1))) * (x0 - x1) / (y0 - y1)
//-------------------------------------------------------------------------------------
//	(y-y1)/(x-x1)=(y2-y1)/(x2-x1)
//
//	����x��y
//	y = (x - x1) * (y2 - y1) / (x2 - x1) + y1
//
//	����y��x
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