#include "perclose.h"

/** �����󿪷� */
float IVA_sqrt(float number)
{ 
	long i;
	float x, y;
	const float f = 1.5F;
	x = number * 0.5F;
	y = number;
	i = * ( long * ) &y;
	i = 0x5f3759df - ( i >> 1 );

	y = * ( float * ) &i;
	y = y * ( f - ( x * y * y ) );
	y = y * ( f - ( x * y * y ) );
	return number * y;
}
int DistofTowPoint(CvPoint p1,CvPoint p2)
{
	//return (sqrt((p2.y-p1.y)*(p2.y-p1.y)+(p2.x-p1.x)*(p2.x-p1.x)));
	//ȡ����ֵ
	int dist = (int)IVA_sqrt((p2.y-p1.y)*(p2.y-p1.y)+(p2.x-p1.x)*(p2.x-p1.x));
	return dist;
}
CPerClose::CPerClose(void)
{

}


CPerClose::~CPerClose(void)
{

}


/* �����ڴ� */
STATUS_E CPerClose::Create()
{
	m_frameNum = 0;
	m_fatigueFlag = false;

	SetBlinkEmpty(m_leftEyeBlinkQueue);
	SetBlinkEmpty(m_rightEyeBlinkQueue);

	return CAS_STATUS_OK;
}
/* �������� */
STATUS_E CPerClose::Config(PERCLOSE_PARA para)
{
	m_para = para;
	return CAS_STATUS_OK;
}
/* ������ */
STATUS_E CPerClose::Process(CvPoint featurePoint[FACE_FEATURE_POINT_NUM])
{	
	FeaturePointCopy(featurePoint); /* ���������ݸ�ֵ */

	BlinkDetect();                  /* գ��ͳ�� */
	
	YawningDetect();                /* �����ͳ�� */
	
	NodDetect();                    /* ��ͷͳ�� */
	
	BlinkFrequency();               /* գ��Ƶ��ͳ�� */
	
	FatigueJudgment();              /* ƣ���ж� */

	m_frameNum++;                   //֡������

	return CAS_STATUS_OK;
}
/* �ͷ��ڴ� */
STATUS_E CPerClose::Release()
{
	return CAS_STATUS_OK;
}

/* ���ݸ��� */
STATUS_E CPerClose::FeaturePointCopy(CvPoint featurePoint[])
{
	int i,j;
	//���ݵ�����У��ֱ�ֵ��m_faceFeaturePoint�����ڶ�����m_faceFeaturePoint
	for (i= 0,j = 0;j < FACE_CONTOUR_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.faceContour[j] = featurePoint[i];
	}
	for (j = 0;j < LEFT_EYE_BROW_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.leftBrow[j] = featurePoint[i];
	}
	for (j = 0;j < RIGHT_EYE_BROW_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.rightBrow[j] = featurePoint[i];
	}
	for (j = 0;j < NOSE_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.nose[j] = featurePoint[i];
	}
	for (j = 0;j < LEFT_EYE_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.leftEye[j] = featurePoint[i];
	}
	for (j = 0;j < RIGHT_EYE_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.rightEye[j] = featurePoint[i];
	}
	for (j = 0;j < MOUSE_POINT_NUM; i++,j++)
	{
		m_faceFeaturePoint.mouse[j] = featurePoint[i];
	}

	return CAS_STATUS_OK;
}
/** 
����գ�ۼ�� 
����������ĸ���37-42
   38 39
37       40
   42 41
   1  2 
0       3
   5  4
��ⷽ����
1��di=abs(38+39-41-42)/2
2����di������У�ͳ��di
*/
STATUS_E CPerClose::LeftEyeBlinkDetect()
{
	BLINK_ELEM_S blinkElem;
	CvPoint *leftEyePoints = m_faceFeaturePoint.leftEye;

	blinkElem.dist = (DistofTowPoint(leftEyePoints[1],leftEyePoints[5])
	                 +DistofTowPoint(leftEyePoints[2],leftEyePoints[4])
					 ) >>1;
	EnBlinkQueue(m_leftEyeBlinkQueue,blinkElem);

	return CAS_STATUS_OK;
}
/** 
����գ�ۼ�� 
����������ĸ���43-48
   44 45
43       46
   48 47
   1  2 
0       3
   5  4
*/
STATUS_E CPerClose::RightEyeBlinkDetect()
{
	BLINK_ELEM_S blinkElem;

	CvPoint *rightEyePoints = m_faceFeaturePoint.rightEye;

	blinkElem.dist = (DistofTowPoint(rightEyePoints[1],rightEyePoints[5])
		             +DistofTowPoint(rightEyePoints[2],rightEyePoints[4])
		              ) >>1;

	EnBlinkQueue(m_rightEyeBlinkQueue,blinkElem);

	//ͳ�ƶ����е�����
	if (m_rightEyeBlinkQueue.full)//�������˺�ʼͳ��
	{
		//��ֵ
		int mean = 0;
		for (int i = 0; i < MAX_BLINK_QUEUE_NUM; i++)
		{
			mean += m_rightEyeBlinkQueue.astElems[(m_rightEyeBlinkQueue.rear+i)% MAX_BLINK_QUEUE_NUM].dist;
		}
		mean /= MAX_BLINK_QUEUE_NUM;
		
		m_leftEyeStatistics.mean = mean;
	}
	

	return CAS_STATUS_OK;
}

/* գ�ۼ�� */
STATUS_E CPerClose::BlinkDetect()
{
	LeftEyeBlinkDetect();
	RightEyeBlinkDetect();

	//�Զ��н���ͳ��
	//MAX_BLINK_QUEUE_NUM

	return CAS_STATUS_OK;
}
/* �������� */
STATUS_E CPerClose::YawningDetect()
{
	return CAS_STATUS_OK;
}
/* ��ͷ��� */
STATUS_E CPerClose::NodDetect()
{
	return CAS_STATUS_OK;
}
/* գ��Ƶ��ͳ�� */
STATUS_E CPerClose::BlinkFrequency()
{
	return CAS_STATUS_OK;
}
/* ƣ���ж� */
STATUS_E CPerClose::FatigueJudgment()
{
	return CAS_STATUS_OK;
}