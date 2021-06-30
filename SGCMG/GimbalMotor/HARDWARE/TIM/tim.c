#include "tim.h"

//TIM_HandleTypeDef TIM3_Handler;
//TIM_OC_InitTypeDef TIM3_CH_Handler;
TIM_HandleTypeDef TIM1_Handler;
TIM_OC_InitTypeDef TIM1_CH_Handler;


/*

TIM3 Initialization : gimbal pwm

arr: �Զ���װֵ    ;    psc: ʱ��Ԥ��Ƶ��

��ʱ�����ʱ�䣺Tout = ((arr + 1) * (psc + 1)) / Ft us

Ft = ��ʱ������Ƶ�� MHz

*/

void TIM3_Init(u16 arr, u16 psc) {
	TIM3_Handler.Instance = TIM3;								//TIM3
	TIM3_Handler.Init.Prescaler = psc;							//Ԥ��Ƶϵ��
	TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;			//���ϼ���
	TIM3_Handler.Init.Period = arr;								//�Զ�װ��ֵ
	TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����
//	HAL_TIM_Base_Init(&TIM_Handler);							
//	HAL_TIM_Base_Start_IT(&TIM_Handler);						//ʹ�ܲ������ж�
	HAL_TIM_PWM_Init(&TIM3_Handler);								//��ʼ��PWM
	
	TIM3_CH_Handler.OCMode = TIM_OCMODE_PWM1;					//OCģʽΪPWM1
	TIM3_CH_Handler.Pulse = arr / 2;								//Ĭ��ռ�ձ�Ϊ50%
	TIM3_CH_Handler.OCPolarity = TIM_OCNPOLARITY_HIGH;			//����Ƚϼ���Ϊ��
	HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH_Handler, TIM_CHANNEL_3);
																//����TIM3���ͨ��3
	HAL_TIM_PWM_Start(&TIM3_Handler, TIM_CHANNEL_3);
}

/*

TIM1 Initialization : wheel pwm

*/
void TIM1_Init(u16 arr, u16 psc) {
	TIM1_Handler.Instance = TIM1;								//TIM1
	TIM1_Handler.Init.Prescaler = psc;							//Ԥ��Ƶϵ��
	TIM1_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;			//���ϼ���
	TIM1_Handler.Init.Period = arr;								//�Զ�װ��ֵ
	TIM1_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����
//	HAL_TIM_Base_Init(&TIM_Handler);							
//	HAL_TIM_Base_Start_IT(&TIM_Handler);						//ʹ�ܲ������ж�
	HAL_TIM_PWM_Init(&TIM1_Handler);								//��ʼ��PWM
	
	TIM1_CH_Handler.OCMode = TIM_OCMODE_PWM1;					//OCģʽΪPWM1
	TIM1_CH_Handler.Pulse = arr;								//Ĭ��ռ�ձ�Ϊ50%
	TIM1_CH_Handler.OCPolarity = TIM_OCNPOLARITY_HIGH;				//����Ƚϼ���Ϊ��
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler, &TIM1_CH_Handler, TIM_CHANNEL_4);
																//����TIM1���ͨ��4
	HAL_TIM_PWM_Start(&TIM1_Handler, TIM_CHANNEL_4);
}


/*

��ʱ���ײ���������������ʱ�ӣ������ж����ȼ�
�˺����ᱻHAL_TIM_Base_Init()��������

*/
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim) {
	
	if (htim->Instance == TIM3) {
		//__HAL_RCC_TIM3_CLK_ENABLE();

//		HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);					//��ռ���ȼ�1�������ȼ�3
//		HAL_NVIC_EnableIRQ(TIM3_IRQn);							//�����ж�
	}
	if (htim->Instance == TIM1) {
		//__HAL_RCC_TIM1_CLK_ENABLE();

	}
}

/*

TIM3�жϷ�����

*/
//void TIM3_IRQHandler(void) {
//	HAL_TIM_IRQHandler(&TIM3_Handler);
//}

/*

�ص�����
�˺����ᱻ�жϷ���������


*/
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
//	if (htim->Instance == TIM3) {
//		//
//	}

//}
/*

��ʱ���ײ���������������ʱ�ӣ���������
�˺����ᱻHAL_TIM_PWM_Init()��������


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

����TIM3CH3ռ�ձ�

*/
void TIM_SetTIM3CH3CCR(u32 compare) {
	TIM3->CCR3 = compare;
}

/*

����TIM1CH4ռ�ձ�

*/
void TIM_SetTIM1CH4CCR(u32 compare) {
	TIM1->CCR4 = compare;
}
/*

����TIM3CH3Ƶ��

*/


