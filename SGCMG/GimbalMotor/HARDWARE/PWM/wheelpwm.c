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

arr: 自动重装值    ;    psc: 时钟预分频数

定时器溢出时间：Tout = ((arr + 1) * (psc + 1)) / Ft us

Ft = 定时器工作频率 MHz

*/
void TIM1_PWM_Init(u16 arr, u16 psc) {
	TIM1_Handler.Instance = TIM1;								//TIM1
	TIM1_Handler.Init.Prescaler = psc;							//预分频系数
	TIM1_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;			//向上计数
	TIM1_Handler.Init.Period = arr;								//自动装载值
	TIM1_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	//时钟分频因子

	HAL_TIM_PWM_Init(&TIM1_Handler);								//初始化PWM
	
	TIM1_CH_Handler.OCMode = TIM_OCMODE_PWM1;					//OC模式为PWM1
	CCRVal = arr / 2;
	TIM1_CH_Handler.Pulse = CCRVal;								//默认占空比为50%
	TIM1_CH_Handler.OCPolarity = TIM_OCNPOLARITY_HIGH;				//输出比较极性为高
	HAL_TIM_PWM_ConfigChannel(&TIM1_Handler, &TIM1_CH_Handler, TIM_CHANNEL_4);
																//配置TIM1输出通道4

}

/*

定时器底层驱动函数，开启时钟，引脚配置
此函数会被HAL_TIM_PWM_Init()函数调用


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

设置TIM1CH4占空比

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
	TIM2输入捕获初始化函数
	TIM2 CH3用作输入捕获，对编码器的输出脉冲进行捕获
*/
void TIM2_CH3_Capture_Init(u16 arr, u16 psc) {
	TIM2_Handler.Instance = TIM2;
	
	TIM2_Handler.Init.Prescaler = psc;
	TIM2_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;
	TIM2_Handler.Init.Period = arr;
	TIM2_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	
	HAL_TIM_IC_Init(&TIM2_Handler);
	
	TIM2_CH_Handler.ICPolarity = TIM_ICPOLARITY_RISING;							//上升沿捕获
	TIM2_CH_Handler.ICSelection = TIM_ICSELECTION_DIRECTTI;						//映射到输入通道TI上
	TIM2_CH_Handler.ICPrescaler = TIM_ICPSC_DIV1;								//配置输入分频，此处不分频
	TIM2_CH_Handler.ICFilter = 0;												//配置输入滤波器，此处不滤波
	
	HAL_TIM_IC_ConfigChannel(&TIM2_Handler, &TIM2_CH_Handler, TIM_CHANNEL_3);	//配置输入通道3
	
	HAL_TIM_IC_Start_IT(&TIM2_Handler, TIM_CHANNEL_3);							//使能输入通道3，并且开启捕获中断
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}
/*

	定时器2底层驱动函数，开启时钟，引脚配置
	此函数会被HAL_TIM_IC_Init()函数调用


*/
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef* htim) {
	GPIO_InitTypeDef GPIO_Initure;
	if (htim->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		
		GPIO_Initure.Pin = GPIO_PIN_2;
		GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;									//复用输入
		GPIO_Initure.Pull = GPIO_PULLDOWN;										//下拉
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;								//高速
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);
	}
	
}
/*
	TIM2中断服务函数
*/
void TIM2_IRQHandler(void) {
	HAL_TIM_IRQHandler(&TIM2_Handler);
}
/*
	定时器共用输入捕获中断处理回调函数，会在HAL_TIM_IRQHandler中被调用
*/
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
	captureNum++;
	//printf("captured!\r\n");
}
/*
	滴答定时器中断回调函数
*/
void HAL_SYSTICK_Callback(void) {
	//printf("dida!\r\n");
	
	//1ms进入一次中断
	if (startFlag) {
		timeNum++;
		if (timeNum == 200) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_12);
			//200ms 测一次速
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
			printf("增量式PID计算结果为:%d，新的CCR值为：%d\r\n", para, CCRVal);
			
			printf("设定的目标速度为：%.2f rpm\r\n", sptr.setPoint * 60 * 5 / 6 );
			
			printf("捕获的脉冲个数为：%d\r\n", tempCount);
			
			printf("当前的实际转速为：%.2f rpm\r\n", (double)tempCount * 60 * 5 / 6);
			
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

