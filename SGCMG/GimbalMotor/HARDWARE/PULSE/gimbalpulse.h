#ifndef _GIMBALPULSE_H_
#define _GIMBALPULSE_H_

#include "sys.h"

//#define T_FREQ 					2000000								//��ʱ������Ƶ�ʣ�psc = 72 - 1
//#define FSPR						200									//������һȦ�Ĳ���
//#define MICRO_STEP				32									//΢��ϸ����
//#define	SPR						(FSPR * MICRO_STEP)					//ʵ����תһȦ��������

//#define PI						((float)3.14159)		
//#define ALPHA					((float)(2 * PI / SPR))			
//#define A_T_x10					((float)(10 * ALPHA * T_FREQ))
//#define T_FREQ_148				((float)(T_FREQ * 0.676 / 10))
//#define A_SQ					((float)(2 * 100000 * ALPHA))
//#define A_x200					((float)(200 * ALPHA))

//typedef struct {
//	s32 step_delay;													//��һ��������ʱ��
//	s32 min_delay;													//��С��������ڣ�������ٶ��йأ�
//	s32 max_s_lim;
//	u16 step_count;													//�������˶��ٲ�
//	s32 rest;														//��������
//	s32	accel_count;												//���ټ���ֵ	
//	
//}speedRampData_t;

#define T_FREQ 					1000000								//��ʱ������Ƶ�ʣ�psc = 72 - 1
#define FSPR					24									//������һȦ�Ĳ���
#define MICRO_STEP				1									//΢��ϸ����
#define	SPR						(FSPR * MICRO_STEP)					//ʵ����תһȦ��������

#define PI						((float)3.14159)		
#define ALPHA					((float)(2 * PI / SPR))	

typedef struct {
	u32 step_delay;													//��һ��������ʱ��
	u32 min_delay;													//��С��������ڣ�������ٶ��йأ�
	u32 max_n;
	u16 step_count;													//�������˶��ٲ�
	s32 rest;														//��������

	
}LinearRampData_t;														


					

					
extern LinearRampData_t lrd;
extern u16 TogglePulse;

void gimbalPulseInit(u16 arr, u16 psc);

void TIM3_Pulse_Init(u16 arr, u16 psc);

//void stepperMotorMove(u32 step, u32 accel, u32 speed);
void calcMotorPara(u32 time, u32 speed);

void changeFixedSpeed(u16 newSpeed);
#endif
