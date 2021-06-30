#ifndef _GIMBALPULSE_H_
#define _GIMBALPULSE_H_

#include "sys.h"

//#define T_FREQ 					2000000								//定时器计数频率，psc = 72 - 1
//#define FSPR						200									//整步下一圈的步数
//#define MICRO_STEP				32									//微步细分数
//#define	SPR						(FSPR * MICRO_STEP)					//实际旋转一圈的脉冲数

//#define PI						((float)3.14159)		
//#define ALPHA					((float)(2 * PI / SPR))			
//#define A_T_x10					((float)(10 * ALPHA * T_FREQ))
//#define T_FREQ_148				((float)(T_FREQ * 0.676 / 10))
//#define A_SQ					((float)(2 * 100000 * ALPHA))
//#define A_x200					((float)(200 * ALPHA))

//typedef struct {
//	s32 step_delay;													//下一个脉冲间隔时间
//	s32 min_delay;													//最小脉冲的周期（与最大速度有关）
//	s32 max_s_lim;
//	u16 step_count;													//计算走了多少步
//	s32 rest;														//计算余数
//	s32	accel_count;												//加速计数值	
//	
//}speedRampData_t;

#define T_FREQ 					1000000								//定时器计数频率，psc = 72 - 1
#define FSPR					24									//整步下一圈的步数
#define MICRO_STEP				1									//微步细分数
#define	SPR						(FSPR * MICRO_STEP)					//实际旋转一圈的脉冲数

#define PI						((float)3.14159)		
#define ALPHA					((float)(2 * PI / SPR))	

typedef struct {
	u32 step_delay;													//下一个脉冲间隔时间
	u32 min_delay;													//最小脉冲的周期（与最大速度有关）
	u32 max_n;
	u16 step_count;													//计算走了多少步
	s32 rest;														//计算余数

	
}LinearRampData_t;														


					

					
extern LinearRampData_t lrd;
extern u16 TogglePulse;

void gimbalPulseInit(u16 arr, u16 psc);

void TIM3_Pulse_Init(u16 arr, u16 psc);

//void stepperMotorMove(u32 step, u32 accel, u32 speed);
void calcMotorPara(u32 time, u32 speed);

void changeFixedSpeed(u16 newSpeed);
#endif
