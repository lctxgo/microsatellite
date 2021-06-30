#ifndef _TIM_H_
#define _TIM_H_

#include "sys.h"

extern TIM_HandleTypeDef TIM3_Handler;
extern TIM_OC_InitTypeDef TIM3_CH_Handler;
extern TIM_HandleTypeDef TIM1_Handler;
extern TIM_OC_InitTypeDef TIM1_CH_Handler;


void TIM3_Init(u16 arr, u16 psc);
void TIM1_Init(u16 arr, u16 psc);
void TIM_SetTIM3CH3CCR(u32 compare);
void TIM_SetTIM1CH4CCR(u32 compare);


#endif
