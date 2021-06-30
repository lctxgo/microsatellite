#include "wheelpwm.h"
TIM_HandleTypeDef TIM1_Handler;
TIM_OC_InitTypeDef TIM1_CH_Handler;

TIM_HandleTypeDef TIM2_Handler;
TIM_IC_InitTypeDef TIM2_CH_Handler;

PID_Para_t sptr;
u32 captureNum = 0;
u8 startFlag = 0;
u8 timeNum = 0;
u8 tempCount;
s32 para;
s32 CCRVal;
/*

wheel pwm

*/

void wheelPWMInit(u16 arr, u16 psc) {
	IO2Init();
	
	incPIDInit();
	TIM1_PWM_Init(arr, psc);

	startFlag = 1;
	TIM2_CH3_Capture_Init(arr, psc);
	
	HAL_TIM_PWM_Start(&TIM1_Handler, TIM_CHANNEL_4);
	
}

/*

TIM1 PWM1 Mode Initialization

arr: �Զ���װֵ    ;    psc: ʱ��Ԥ��Ƶ��

��ʱ�����ʱ�䣺Tout = ((arr + 1) * (psc + 1)) / Ft us

Ft = ��ʱ������Ƶ�� MHz

*/
void TIM1_PWM_Init(u16 arr, u16 psc) {
	TIM1_Handler.Instance = TIM1;								//TIM1
	TIM1_Handler.Init.Prescaler = psc;							//Ԥ��Ƶϵ��
	TIM1_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;			//���ϼ���
	TIM1_Handler.Init.Period = arr;								//�Զ�װ��ֵ
	TIM1_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	//ʱ�ӷ�Ƶ����

	HAL_TIM_PWM_Init(&TIM1_Handler);								//��ʼ��PWM
	
	TIM1_CH_Handler.OCMode = TIM_OCMODE_PWM1;					//OCģʽΪPWM1
	CCRVal = arr / 2;
	TIM1_CH_Handler.Pulse = CCRVal;								//Ĭ��ռ�ձ�Ϊ50%
	TIM1_CH_Handler.OCPolarity = TIM_OCNPOLARITY_HIGH;				//����Ƚϼ���Ϊ��
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler, &TIM1_CH_Handler, TIM_CHANNEL_4);
																//����TIM1���ͨ��4

}

/*

��ʱ���ײ���������������ʱ�ӣ���������
�˺����ᱻHAL_TIM_PWM_Init()��������


*/
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* htim) {
	GPIO_InitTypeDef GPIO_Initure;
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

����TIM1CH4ռ�ձ�

*/
void TIM_SetTIM1CH4CCR(u32 compare) {
	TIM1->CCR4 = compare;
}

/*
	PID
*/
void incPIDInit() {
	sptr.lastError = 0;			//Error[-1] = 0
	sptr.preError = 0;				//Error[-2] = 0
	sptr.proportion = P_DATA;
	sptr.integral = I_DATA;
	sptr.derivative = D_DATA;
	sptr.setPoint = 199;		//T = 200 ms. 10000rpm
}

s32 incPIDCalc(double nextPoint) {
	double iError;
	s32 iIncPID;
	iError = sptr.setPoint - nextPoint;
	iIncPID = sptr.proportion * iError - sptr.integral * sptr.lastError + sptr.derivative * sptr.preError;
	sptr.preError = sptr.lastError;
	sptr.lastError = iError;
	return iIncPID;
};


/*
	TIM2���벶���ʼ������
	TIM2 CH3�������벶�񣬶Ա����������������в���
*/
void TIM2_CH3_Capture_Init(u16 arr, u16 psc) {
	TIM2_Handler.Instance = TIM2;
	
	TIM2_Handler.Init.Prescaler = psc;
	TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM2_Handler.Init.Period = arr;
	TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	
	HAL_TIM_IC_Init(&TIM2_Handler);
	
	TIM2_CH_Handler.ICPolarity = TIM_ICPOLARITY_RISING;							//�����ز���
	TIM2_CH_Handler.ICSelection = TIM_ICSELECTION_DIRECTTI;						//ӳ�䵽����ͨ��TI��
	TIM2_CH_Handler.ICPrescaler = TIM_ICPSC_DIV1;								//���������Ƶ���˴�����Ƶ
	TIM2_CH_Handler.ICFilter = 0;												//���������˲������˴����˲�
	
	HAL_TIM_IC_ConfigChannel(&TIM2_Handler, &TIM2_CH_Handler, TIM_CHANNEL_3);	//��������ͨ��3
	
	HAL_TIM_IC_Start_IT(&TIM2_Handler, TIM_CHANNEL_3);							//ʹ������ͨ��3�����ҿ��������ж�
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}
/*

	��ʱ��2�ײ���������������ʱ�ӣ���������
	�˺����ᱻHAL_TIM_IC_Init()��������


*/
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim) {
	GPIO_InitTypeDef GPIO_Initure;
	if (htim->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		GPIO_Initure.Pin = GPIO_PIN_2;
		GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;									//��������
		GPIO_Initure.Pull = GPIO_PULLDOWN;										//����
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;								//����
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	}
	
}
/*
	TIM2�жϷ�����
*/
void TIM2_IRQHandler(void) {
	HAL_TIM_IRQHandler(&TIM2_Handler);
}
/*
	��ʱ���������벶���жϴ���ص�����������HAL_TIM_IRQHandler�б�����
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
	captureNum++;
	//printf("captured!\r\n");
}
/*
	�δ�ʱ���жϻص�����
*/
void HAL_SYSTICK_Callback(void) {
	//printf("dida!\r\n");
	
	//1ms����һ���ж�
	if (startFlag) {
		timeNum++;
		if (timeNum == 200) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
			//200ms ��һ����
			tempCount = captureNum;
			para = incPIDCalc(tempCount);
			if (para <= -1 || para >= 1) {
				CCRVal += para;
			}
			if (CCRVal > 1000) {
				CCRVal = 1000;
			}
			if (CCRVal < 0) {
				CCRVal = 0;
			}
			printf("����ʽPID������Ϊ:%d���µ�CCRֵΪ��%d\r\n", para, CCRVal);
			
			printf("�趨��Ŀ���ٶ�Ϊ��%.2f rpm\r\n", sptr.setPoint * 60 * 5 / 6 );
			
			printf("������������Ϊ��%d\r\n", tempCount);
			
			printf("��ǰ��ʵ��ת��Ϊ��%.2f rpm\r\n", (double)tempCount * 60 * 5 / 6);
			
			TIM_SetTIM1CH4CCR(CCRVal);
			timeNum = 0;
			captureNum = 0;
		}
	}
}
void IO2Init() {
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_Initure.Pin = GPIO_PIN_12;
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;									
	GPIO_Initure.Pull = GPIO_PULLUP;										
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;								
	HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
}
void setDesiredPoint(double x) {
	sptr.setPoint = x;
}

