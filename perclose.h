#pragma once
#include "cv.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


/* ����������ĸ��� */
#define FACE_FEATURE_POINT_NUM 68

/* ��������������ĸ���1-17 */
#define FACE_CONTOUR_POINT_NUM 17 
/* ��üë������ĸ���18-22*/
#define LEFT_EYE_BROW_POINT_NUM 5
/* ��üë������ĸ���23-27 */
#define RIGHT_EYE_BROW_POINT_NUM 5
/* ����������ĸ���28-36 */
#define NOSE_POINT_NUM 9
/* ����������ĸ���37-42 */
#define LEFT_EYE_POINT_NUM 6
/* ����������ĸ���43-48 */
#define RIGHT_EYE_POINT_NUM 6
/* ��������ĸ���49-68 */
#define MOUSE_POINT_NUM 20

/** ״̬���� */
typedef enum casSTATUS
{
	CAS_STATUS_CHECK_LICENSE_TIMEOUT = -3, /**< ��֤licenseʧ�� */
	CAS_STATUS_CHECK_LICENSE_FAILED = -2,  /**< ��֤licenseʧ�� */
	CAS_STATUS_READ_MAC_FAILED = -1,       /**< ��ȡMACʧ�� */
	CAS_STATUS_OK = 1,                     /**< �ɹ� */
	CAS_STATUS_SKIP,                       /**< ���� */
	CAS_STATUS_FALSE,                      /**< ��֧�� */
	CAS_STATUS_UNSUPPORT
} STATUS_E;

/* ѭ�����еĳ��� */
#define  MAX_BLINK_QUEUE_NUM 256
/** ����գ�۽ṹ */
typedef struct casBLINK_ELEM_S
{
	int          dist;                    /**< ���������ľ��� */
	unsigned int u32Used;                 /**< �Ƿ�ʹ�� */
}BLINK_ELEM_S;


/** ѭ�����У��洢գ�۾��룬�����ж�գ��Ƶ�� */
typedef struct casBLINK_QUEUE_S
{
	BLINK_ELEM_S astElems[MAX_BLINK_QUEUE_NUM];    /**< Ԫ�� */
	int          front;                            /**< ��ͷָ�� */
	int          rear;                             /**< ��βָ�� */
	bool         full;                             /**< ����������� */
}BLINK_QUEUE_S;

/**
ѭ������
������ָ��Q.ftont =MaxSiZe-1����ǰ��һ��λ�þ��Զ���0����������ó���ȡ������(%)��ʵ�֡�
��ʼʱ��Q.front=Q.rear=0
����ָ��� 1��Q.front=(Q.front+1)%MaxSize
��βָ��� 1��Q.rear=(Q.rear+1)%MaxSize
���г��ȣ�(Q.rear+MaxSize-Q.front)%MaxSize
�������ʱ��ָ�붼��˳ʱ�뷽���1 
*/
/** ��ʼ�� */
inline void InitBlinkQueue(BLINK_QUEUE_S &Q)
{
	Q.rear = Q.front = 0;  //��ʼ�����ס���βָ��
	Q.full = false;
}

/** �жӿ� */
inline bool isBlinkEmpty(BLINK_QUEUE_S Q) 
{
	if(Q.rear == Q.front) return true;  //�ӿ�����
	else return false;
}
/** �жӿ� */
inline void SetBlinkEmpty(BLINK_QUEUE_S &Q) 
{
	Q.rear = Q.front = 0;  //��ʼ�����ס���βָ��
	Q.full = false;
	memset(Q.astElems,0,sizeof(BLINK_ELEM_S)*MAX_BLINK_QUEUE_NUM);

}
/** ��ӣ�ÿ���������ݣ���βָ��++��ֱ�������󣬶�βָ������ָ����� */
inline bool EnBlinkQueue(BLINK_QUEUE_S &Q, BLINK_ELEM_S x)
{
	if((Q.rear + 1) % MAX_BLINK_QUEUE_NUM == Q.front) //����
	{
		Q.rear = Q.front; //�������βָ��ָ�����λ��
		Q.full = true;
	}
	Q.astElems[Q.rear] = x;
	Q.rear = (Q.rear + 1) % MAX_BLINK_QUEUE_NUM; //��βָ��� 1 ȡģ
	return true;
}
/** ����ʱ��ӵ�ǰ�����ȡcount������ */
inline void GetElemBlinkQueue(BLINK_QUEUE_S &Q,int count, BLINK_ELEM_S QCount[])
{
	for (int i = 0; i < count; i++)
	{
		QCount[i] = Q.astElems[(Q.rear-count+MAX_BLINK_QUEUE_NUM)% MAX_BLINK_QUEUE_NUM+i];
	}
}
/** ���� */
//inline bool DeBlinkQueue(BLINK_QUEUE_S &Q, BLINK_ELEM_S &x)
//{
//	if(Q.rear == Q.front) return false;  //�ӿգ�����
//	x = Q.astElems[Q.front];
//	Q.front= (Q.front + 1) % MAX_BLINK_QUEUE_NUM;  //��ͷָ��� 1 ȡģ
//	return true;
//}
/** ������Ԫ�صĸ�����(Q.rear-Q.front+MaxSize)%MaxSize */
//inline bool GetBlinkQueueLength(BLINK_QUEUE_S &Q)
//{
//	return (Q.rear - Q.front + MAX_BLINK_QUEUE_NUM) % MAX_BLINK_QUEUE_NUM;
//}


/* �������� */
typedef struct PERCLOSE_PARA_S
{
	int threshBlinkFreq;                        /**< գ��Ƶ����ֵ���ж��Ƿ�ƣ�� */
}PERCLOSE_PARA;

/* գ������ͳ�ƵĲ��� */
typedef struct PERCLOSE_STATISTICS_S
{
	int mean;                                   /**< �����ֵ */
}PERCLOSE_STATISTICS_S;

/* ����������ֲ� */
typedef struct FACE_FEATURE_POINT_S 
{
	CvPoint faceContour[FACE_CONTOUR_POINT_NUM]; /**< ��������������ĸ��� */
	CvPoint leftBrow[LEFT_EYE_BROW_POINT_NUM];   /**< ��üë������ĸ��� */
	CvPoint rightBrow[RIGHT_EYE_BROW_POINT_NUM]; /**< ��üë������ĸ��� */
	CvPoint nose[NOSE_POINT_NUM];                /**< ����������ĸ��� */
	CvPoint leftEye[LEFT_EYE_POINT_NUM];         /**< ����������ĸ��� */
	CvPoint rightEye[RIGHT_EYE_POINT_NUM];       /**< ����������ĸ��� */
	CvPoint mouse[MOUSE_POINT_NUM];              /**< ��������ĸ��� */
		
}FACE_FEATURE_POINT_S;


/* գ��Ƶ��ͳ�� */
class CPerClose
{
public:
	PERCLOSE_PARA m_para;                        /**< perclose���Ĳ��� */
	FACE_FEATURE_POINT_S m_faceFeaturePoint;     /**< ���������� */
	float m_leftEyeBlinkFrq;                     /**< ����գ��Ƶ�� */
	float m_rightEyeBlinkFrq;                    /**< ����գ��Ƶ�� */
	float m_YawningFrq;                          /**< �����Ƶ�� */
	float m_NodFrq;                              /**< ��ͷƵ�� */


	BLINK_QUEUE_S m_leftEyeBlinkQueue;            /**< ����գ��ͳ��ѭ������ */
	BLINK_QUEUE_S m_rightEyeBlinkQueue;           /**< ����գ��ͳ��ѭ������ */

	PERCLOSE_STATISTICS_S m_leftEyeStatistics;    /**< ����գ��ͳ��ѭ������ */
	PERCLOSE_STATISTICS_S m_rightEyeStatistics;   /**< ����գ��ͳ��ѭ������ */
	
	long m_frameNum;                              /**< ֡��ͳ����������ʱ���Ƶ�� */
	int m_timePerFrame;                           /**< ÿ֡��ʱ�� */
	bool m_fatigueFlag;                           /**< �Ƿ�ƣ�͵ı�� */
public:

	/* �����ڴ� */
	STATUS_E Create();
	/* �������� */
	STATUS_E Config(PERCLOSE_PARA para);
	/* ������ */
	STATUS_E Process(CvPoint featurePoint[FACE_FEATURE_POINT_NUM]);
	/* �ͷ��ڴ� */
	STATUS_E Release();

	/* ���ݸ��� */
	STATUS_E FeaturePointCopy(CvPoint featurePoint[]);
	/* ����գ�ۼ�� */
	STATUS_E LeftEyeBlinkDetect();
	/* ����գ�ۼ�� */
	STATUS_E RightEyeBlinkDetect();
	/* գ�ۼ�� */
	STATUS_E BlinkDetect();
	/* �������� */
	STATUS_E YawningDetect();
	/* ��ͷ��� */
	STATUS_E NodDetect();
	/* գ��Ƶ��ͳ�� */
	STATUS_E BlinkFrequency();
	/* ƣ���ж� */
	STATUS_E FatigueJudgment();

	/* ���캯�� */
	CPerClose(void);
	/* �������� */
	~CPerClose(void);

	
};

