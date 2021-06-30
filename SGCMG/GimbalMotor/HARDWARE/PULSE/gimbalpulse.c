#include "gimbalpulse.h"
#include "usart.h"

#define FixedSpeed

u16 TogglePulse = 5000;


//100rad/s^2
u16 accel100Table[37] = {1674, 1636, 1598, 1560, 1522, 1484, 1446, 1408, 1370, 1332, 1294, 1256, 1218, 1180, 1142, 1104, 1066, 1028, 990, 952, 914, 876, 838, 800, 762, 724, 686, 648, 610, 572, 534, 496, 458, 420, 382, 344, 279};

//50rad/s^2: 73 
u16 accel50Table[73] ={1674, 1655, 1636, 1617, 1598, 1579, 1560, 1541, 1522, 1503, 1484, 1465, 1446, 1427, 1408, 1389, 1370, 1351, 1332, 1313, 1294, 1275, 1256, 1237, 1218, 1199, 1180, 1161, 1142, 1123, 1104, 1085, 1066, 1047, 1028, 1009, 990, 971, 952, 933, 914, 895, 876, 857, 838, 819, 800, 781, 762, 743, 724, 705, 686, 667, 648, 
629, 610, 591, 572, 553, 534, 515, 496, 477, 458, 439, 420, 401, 382, 363, 344, 325, 279};

//20rad/s^2: 183 
u16 accel20Table[183] = {1674, 1667, 1660, 1653, 1646, 1639, 1632, 1625, 1618, 1611, 1604, 1597, 1590, 1583, 1576, 1569, 1562, 1555, 1548, 1541, 1534, 1527, 1520, 1513, 1506, 1499, 1492, 1485, 1478, 1471, 1464, 1457, 1450, 1443, 1436, 1429, 1422, 1415, 1408, 1401, 1394, 1387, 1380, 1373, 1366, 1359, 1352, 1345, 1338, 1331, 1324, 1317, 1310, 1303, 1296, 1289, 1282, 1275, 1268, 1261, 1254, 1247, 1240, 1233, 1226, 1219, 1212, 1205, 1198, 1191, 1184, 1177, 1170, 1163, 1156, 1149, 1142, 1135, 1128, 
1121, 1114, 1107, 1100, 1093, 1086, 1079, 1072, 1065, 1058, 1051, 1044, 1037, 1030, 1023, 1016, 1009, 1002, 995, 988, 981, 974, 967, 960, 953, 946, 939, 932, 925, 918, 911, 904, 897, 890, 883, 876, 869, 862, 855, 848, 841, 834, 827, 820, 813, 806, 799, 792, 785, 778, 771, 764, 757, 750, 743, 736, 729, 722, 715, 708, 701, 694, 
687, 680, 673, 666, 659, 652, 645, 638, 631, 624, 617, 610, 603, 596, 589, 582, 575, 568, 561, 554, 547, 540, 533, 526, 519, 512, 505, 498, 491, 484, 477, 470, 463, 456, 449, 442, 435, 428, 421, 414, 407, 279};
u8 tableIndex = 0;

TIM_HandleTypeDef TIM3_Handler = {0};
TIM_OC_InitTypeDef TIM3_CH_Handler = {0};

//speedRampData_t srd;
LinearRampData_t lrd;

void gimbalPulseInit(u16 arr, u16 psc) {
	TIM3_Pulse_Init(arr, psc);
	HAL_TIM_OC_Start_IT(&TIM3_Handler, TIM_CHANNEL_3);
	//stepperMotorMove(0, 66, 335);
	//calcMotorPara(1, 100);//335

}

void TIM3_Pulse_Init(u16 arr, u16 psc) {
	TIM3_Handler.Instance = TIM3;								//TIM3
	TIM3_Handler.Init.Prescaler = psc;							//预分频系数
	TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;			//向上计数
	TIM3_Handler.Init.Period = arr;								//自动装载值
	TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;	//时钟分频因子

	//HAL_TIM_Base_Init(&TIM3_Handler);
	//HAL_TIM_PWM_Init(&TIM3_Handler);	
	HAL_TIM_OC_Init(&TIM3_Handler);								//初始化输出比较
	
	TIM3_CH_Handler.OCMode = TIM_OCMODE_TOGGLE;					//OC模式为输出翻转
	TIM3_CH_Handler.Pulse = 0xFFFF;									//比较值为togglePulse
	TIM3_CH_Handler.OCPolarity = TIM_OCNPOLARITY_HIGH;			//输出比较极性为高
	//HAL_TIM_PWM_ConfigChannel(&TIM3_Handler, &TIM3_CH_Handler, TIM_CHANNEL_3);
																//配置TIM3输出通道3
	HAL_TIM_OC_ConfigChannel(&TIM3_Handler, &TIM3_CH_Handler, TIM_CHANNEL_3);
	
	
	

	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM3_IRQn);
	
		
}

void HAL_TIM_OC_MspInit(TIM_HandleTypeDef* htim) {
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

}


void TIM3_IRQHandler(void) {
	HAL_TIM_IRQHandler(&TIM3_Handler);
}



//void stepperMotorMove(u32 step, u32 accel, u32 speed) {
//	u16 count;
//	srd.min_delay = (u32)(A_T_x10 / speed);
//	srd.step_delay = (u32)(T_FREQ_148 * (sqrt(A_SQ / accel) / 10));
//	srd.max_s_lim = (u32)(speed * speed / (A_x200 * accel / 10));//计算多少步后达到最大速度
//	HAL_TIM_OC_Start_IT(&TIM3_Handler, TIM_CHANNEL_3);
//	count = __HAL_TIM_GET_COUNTER(&TIM3_Handler);
//		
//	__HAL_TIM_SET_COMPARE(&TIM3_Handler, TIM_CHANNEL_3, count + srd.step_delay);
//	srd.step_count = 0;
//	srd.accel_count = 0;
//}
void calcMotorPara(u32 time, u32 speed) {
//	u16 count;
//	u32 accel = speed / time;
//	lrd.max_n =(u32)(speed * time * time / (2 * ALPHA));
//	lrd.step_delay = T_FREQ * sqrt(2 * ALPHA / accel);//精度？
//	HAL_TIM_OC_Start_IT(&TIM3_Handler, TIM_CHANNEL_3);
//	count = __HAL_TIM_GET_COUNTER(&TIM3_Handler);
//		
//	__HAL_TIM_SET_COMPARE(&TIM3_Handler, TIM_CHANNEL_3, count + lrd.step_delay);
//	lrd.step_count = 0;
	lrd.max_n = 60;
	lrd.step_delay = 0xffff;
	lrd.step_count = 0;
	HAL_TIM_OC_Start_IT(&TIM3_Handler, TIM_CHANNEL_3);
}

u32 sum = 0;
u8 flag = 0;
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef* htim) {
	u16 tim_cnt;
#ifdef FixedSpeed
	tim_cnt = __HAL_TIM_GET_COUNTER(&TIM3_Handler);
	
	__HAL_TIM_SET_COMPARE(&TIM3_Handler, TIM_CHANNEL_3, tim_cnt + TogglePulse);
#else
	tim_cnt = __HAL_TIM_GET_COUNTER(&TIM3_Handler);
	
	__HAL_TIM_SET_COMPARE(&TIM3_Handler, TIM_CHANNEL_3, tim_cnt + accel20Table[tableIndex]);
	
	if (!flag) {
		//加速阶段
		if (tableIndex < 182)
		
		tableIndex++;
		else
			flag = 1;
	}
	else {
		if (tableIndex > 0)
			tableIndex--;
		else
			HAL_TIM_OC_Stop_IT(&TIM3_Handler, TIM_CHANNEL_3);
	}
		
	
#endif
}
void changeFixedSpeed(u16 newSpeed) {
	TogglePulse = newSpeed;
}
