#ifndef __CALC_POINT__
#define __CALC_POINT__


#include <math.h>

#define __INLINE __inline 

#ifndef  __CC_ARM
#define PRINTFF printf
#else
#define PRINTFF
#endif

#define MAX_POINT	5		//���֧�ֵ���
#define MAX_POINT_REC 3	//ÿ���������� ֧�ֵ�������

#define SCAN_X_SQUARE_NUM	5
#define SCAN_Y_SQUARE_NUM	4

#define DISTANCE_THRESHOLD 20*20		//�����������20 ���� ����Ϊ���ཻ��
#define DISTANCE_THRESHOLD_WC	60*60	//��������¿����ӳ���40
#define DISTANCE_THRESHOLD_ID		50*50	//��������������50��������Ϊ��������ID��ͬ
#define TIMES_THRESHOLD_UP		3	//������⵽TIMES_THRESHOLD_UP �������û���ݾ���Ϊ������Ѿ�̧��

#define DIAMON_RATIO		23	//���εĳ���ȳ���һ���̶Ⱦ�˵�����Ǹ� ϸ�����Σ���Ҫ�����ⷽ���ж���
#define LONG_DIA_THRESHOLD  40	//�������Σ�ͨ��һ�����������������ж��Ƿ��ཻ

#define LENGTH_PRE		128
#define TOTAL_LENGTH	LENGTH_PRE*SCAN_X_SQUARE_NUM	//�װ��ܿ��
#define TATAL_HEIGHT	LENGTH_PRE*(SCAN_Y_SQUARE_NUM-1) + 72 //�װ��ܸ߶�


#define NULL 0 

#ifndef  __CC_ARM

#pragma pack(push)
#pragma pack(1)
//������ṹ
typedef struct CALC_POINT_S
{
    signed int  x;    /*X����*/
	signed int y;    /*Y����*/
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

#endif
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
	signed int distance;
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
	unsigned int ulLastPointNum;              /*ʵ��״̬0��1��2��3������*/
	struct PT_BUF LastPoint[MAX_POINT];   /*�ϴε�ʵ����Ϣ*/
	char LastPointUpTime[MAX_POINT];	/*�˵��Ѿ����ٴ�û��ɨ�赽��Ϣ*/
}PT_STATUS_L;

#ifndef  __CC_ARM
#pragma pack(pop)
#endif

#define CALCL_AVERAGE_TWO(a,b) ((int)(((int)(a)+(int)(b))/2))
#define CALCL_X_DIAMOND_CENTRE_X(a) CALCL_AVERAGE_TWO((a).strPoint0.x,((a)).strPoint3.x)
#define CALCL_X_DIAMOND_CENTRE_Y(a) CALCL_AVERAGE_TWO(((a)).strPoint1.y,((a)).strPoint2.y)
#define CALCL_Y_DIAMOND_CENTRE_X(a) CALCL_AVERAGE_TWO((a).strPoint1.x,((a)).strPoint2.x)
#define CALCL_Y_DIAMOND_CENTRE_Y(a) CALCL_AVERAGE_TWO(((a)).strPoint0.y,((a)).strPoint3.y)

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
for(j=PointNumTmp;j< PointNumTmp_t;j++)	\
{	\
	if(DIAMOND_X_GET_N(DiamondBuf,XPoint[DistanceTmp[j].XPos].Rec,XPoint[DistanceTmp[j].XPos].Diamond)==DiamondNumTmp)	\
	{	\
		ExcDistance(DistanceTmp,PointNumTmp,j);	\
		PointNumTmp++;	\
		PRINTFF("X add lost point %08x no point\r\n",j,k,DiamondNumTmp);	\
	}	\
}

#define ADD_LOST_POINT_Y	\
	for(j=PointNumTmp;j< PointNumTmp_t;j++)	\
{	\
	if(DIAMOND_Y_GET_N(DiamondBuf,YPoint[DistanceTmp[j].YPos].Rec,YPoint[DistanceTmp[j].YPos].Diamond)==DiamondNumTmp)	\
{	\
	ExcDistance(DistanceTmp,PointNumTmp,j);	\
	PointNumTmp++;	\
	PRINTFF("Y add lost point %08x no point\r\n",j,k,DiamondNumTmp);	\
}	\
}

void PrintDiamond(CALC_DIAMOND_BUF *DiamondBuf,char *OutBuf,int ScanCount);
int CalcPoint(CALC_DIAMOND_BUF *DiamondBuf,struct PT_BUF *point);
int GetMin(int* buf,int len,int *CurPos);
void InsertSort(CALC_DISTANCE *a, int n);
void shell_sort(CALC_DISTANCE *arr, int len) ;
void DeleteAtDistance(CALC_DISTANCE *dis,int pos,unsigned int *len);
static int CalcPointID(struct PT_BUF *point,int *num);
void ExcDistance(CALC_DISTANCE *dis,int src,int dst);
void ForceDeleteAtDistance(CALC_DISTANCE *dis,int pos,int *len);
void ClearPointID(void);
extern PT_STATUS_L g_PointStatus;	

#endif