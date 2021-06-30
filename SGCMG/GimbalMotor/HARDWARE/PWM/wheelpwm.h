#ifndef _WHEELPWM_H_
#define _WHEELPWM_H_

#include "sys.h"
#include "usart.h"

typedef struct {
	double 		setPoint;					//期望值
	double		proportion;					//比例系数
	double		integral;					//积分系数
	double		derivative;					//微分系数
	double		lastError;					//Error[-1]
	double		preError;					//Error[-2]
}PID_Para_t;


#define P_DATA		(double)3.2				//3.2
#define	I_DATA		(double)1.1			//1.1
#define D_DATA		(double)-0.15;				//-0.15

extern PID_Para_t sptr;
extern u8 startFlag;

void wheelPWMInit(u16 arr, u16 psc);

void TIM1_PWM_Init(u16 arr, u16 psc);

void incPIDInit();

s32 incPIDCalc(double nextPoint);

void TIM2_CH3_Capture_Init(u16 arr, u16 psc);

void IO2Init();
void setDesiredPoint(double x);
#endif
