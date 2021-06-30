#include "SPI2.h"

SPI_HandleTypeDef SPI2_Handler;


/*SPI协议初始化*/
void SPI2_Init(void) {
	SPI2_Handler.Instance = SPI2;
	SPI2_Handler.Init.Mode = SPI_MODE_MASTER;
	SPI2_Handler.Init.Direction = SPI_DIRECTION_2LINES;
	SPI2_Handler.Init.DataSize = SPI_DATASIZE_8BIT;						//SPI_DATASIZE_16BIT两种可选
	SPI2_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH;					//串行同步时钟空闲状态为高电平
	SPI2_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;						//同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI2_Handler.Init.NSS = SPI_NSS_SOFT;								//NSS片选线由软件管理
	SPI2_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SPI2_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;						//从MSB开始传输
	SPI2_Handler.Init.TIMode = SPI_TIMODE_DISABLE;						//关闭TI模式
	SPI2_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI2_Handler.Init.CRCPolynomial = 7;								//CRC值计算多项式
	
	HAL_SPI_Init(&SPI2_Handler);
	__HAL_SPI_ENABLE(&SPI2_Handler);
	
	SPI2_ReadWriteByte(0xFF);
}

/*SPI电气初始化，该函数会被HAL_SPI_Init调用*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
	GPIO_InitTypeDef GPIO_SPI2;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_SPI2_CLK_ENABLE();
	
	GPIO_SPI2.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_SPI2.Mode = GPIO_MODE_AF_PP;
	GPIO_SPI2.Pull = GPIO_PULLUP;
	GPIO_SPI2.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOB, &GPIO_SPI2);
}

/*SPI速度设置*/
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler) {
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));		//判断有效性
	__HAL_SPI_DISABLE(&SPI2_Handler);
	SPI2_Handler.Instance->CR1 &= 0xFFC7;
	SPI2_Handler.Instance->CR1 |= SPI_BaudRatePrescaler;
	__HAL_SPI_ENABLE(&SPI2_Handler);
}

/*SPI读写函数*/
u8 SPI2_ReadWriteByte(u8 TxData) {
	u8 RxData = 0x00;
	HAL_SPI_TransmitReceive(&SPI2_Handler, &TxData, &RxData, 1, 1000);
	return RxData;
}