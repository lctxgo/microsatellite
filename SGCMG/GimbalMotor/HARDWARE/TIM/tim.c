#include "tim.h"

//TIM_HandleTypeDef TIM3_Handler;
//TIM_OC_InitTypeDef TIM3_CH_Handler;
TIM_HandleTypeDef TIM1_Handler;
TIM_OC_InitTypeDef TIM1_CH_Handler;


/*

TIM3 Initialization : gimbal pwm

arr: 自动重装值    ;    psc: 时钟预分频数

定时器溢出时间：Tout = ((arr + 1) * (psc + 1)) / Ft us

Ft = 定时器工作频率 MHz

*/

void TIM3_Init(u16 arr, u16 psc) {
	TIM3_Handler.Instance = TIM3;								//TIM3
	TIM3_Handler.Init.Prescaler = psc;							//预分频系数
	TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;			//向上计数
	TIM3_Handler.Init.Period = arr;								//自动装载值
	TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	//时钟分频因子
//	HAL_TIM_Base_Init(&TIM_Handler);							
//	HAL_TIM_Base_Start_IT(&TIM_Handler);						//使能并更新中断
	HAL_TIM_PWM_Init(&TIM3_Handler);								//初始化PWM
	
	TIM3_CH_Handler.OCMode = TIM_OCMODE_PWM1;					//OC模式为PWM1
	TIM3_CH_Handler.Pulse = arr / 2;								//默认占空比为50%
	TIM3_CH_Handler.OCPolarity = TIM_OCNPOLARITY_HIGH;			//输出比较极性为低
	HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH_Handler, TIM_CHANNEL_3);
																//配置TIM3输出通道3
	HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_3);
}

/*

TIM1 Initialization : wheel pwm

*/
void TIM1_Init(u16 arr, u16 psc) {
	TIM1_Handler.Instance = TIM1;								//TIM1
	TIM1_Handler.Init.Prescaler = psc;							//预分频系数
	TIM1_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;			//向上计数
	TIM1_Handler.Init.Period = arr;								//自动装载值
	TIM1_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	//时钟分频因子
//	HAL_TIM_Base_Init(&TIM_Handler);							
//	HAL_TIM_Base_Start_IT(&TIM_Handler);						//使能并更新中断
	HAL_TIM_PWM_Init(&TIM1_Handler);								//初始化PWM
	
	TIM1_CH_Handler.OCMode = TIM_OCMODE_PWM1;					//OC模式为PWM1
	TIM1_CH_Handler.Pulse = arr;								//默认占空比为50%
	TIM1_CH_Handler.OCPolarity = TIM_OCNPOLARITY_HIGH;				//输出比较极性为高
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler, &TIM1_CH_Handler, TIM_CHANNEL_4);
																//配置TIM1输出通道4
	HAL_TIM_PWM_Start(&TIM1_Handler, TIM_CHANNEL_4);
}


/*

定时器底层驱动函数，开启时钟，设置中断优先级
此函数会被HAL_TIM_Base_Init()函数调用

*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim) {
	
	if (htim->Instance == TIM3) {
		//__HAL_RCC_TIM3_CLK_ENABLE();

//		HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);					//抢占优先级1，子优先级3
//		HAL_NVIC_EnableIRQ(TIM3_IRQn);							//开启中断
	}
	if (htim->Instance == TIM1) {
		//__HAL_RCC_TIM1_CLK_ENABLE();

	}
}

/*

TIM3中断服务函数

*/
//void TIM3_IRQHandler(void) {
//	HAL_TIM_IRQHandler(&TIM3_Handler);
//}

/*

回调函数
此函数会被中断服务函数调用


*/
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
//	if (htim->Instance == TIM3) {
//		//
//	}

//}
/*

定时器底层驱动函数，开启时钟，引脚配置
此函数会被HAL_TIM_PWM_Init()函数调用


*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim) {
	GPIO_InitTypeDef GPIO_Initure;
	if (htim->Instance == TIM3) {
		__HAL_RCC_TIM3_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		
		GPIO_Initure.Pin = GPIO_PIN_0;
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;
		GPIO_Initure.Pull = GPIO_PULLUP;
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOB, &GPIO_Initure);
	}
	if (htim->Instance == TIM1) {
		__HAL_RCC_TIM1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		GPIO_Initure.Pin = GPIO_PIN_11;
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;
		GPIO_Initure.Pull = GPIO_PULLUP;
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	}
}
/*

设置TIM3CH3占空比

*/
void TIM_SetTIM3CH3CCR(u32 compare) {
	TIM3->CCR3 = compare;
}

/*

设置TIM1CH4占空比

*/
void TIM_SetTIM1CH4CCR(u32 compare) {
	TIM1->CCR4 = compare;
}
/*

设置TIM3CH3频率

*/


