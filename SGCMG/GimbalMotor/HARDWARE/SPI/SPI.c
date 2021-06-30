/*

STM32 SPI Driver using SPI1 Peripheral

SPI1_SCK	---- 	PA5
SPI1_MISO	----	PA6
SPI1_MOSI	----	PA7

*/
#include "SPI.h"

SPI_HandleTypeDef SPI_Handler;


/*SPIЭ���ʼ��*/
void SPI_Init(void) {
	SPI_Handler.Instance = SPI1;
	SPI_Handler.Init.Mode = SPI_MODE_MASTER;
	SPI_Handler.Init.Direction = SPI_DIRECTION_2LINES;
	SPI_Handler.Init.DataSize = SPI_DATASIZE_8BIT;						//SPI_DATASIZE_16BIT���ֿ�ѡ
	SPI_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH;					//����ͬ��ʱ�ӿ���״̬Ϊ�ߵ�ƽ
	SPI_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;						//ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
//	SPI_Handler.Init.CLKPolarity = SPI_POLARITY_LOW;
//	SPI_Handler.Init.CLKPhase = SPI_PHASE_1EDGE;
	
	SPI_Handler.Init.NSS = SPI_NSS_SOFT;								//NSSƬѡ�����������
	SPI_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	SPI_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;						//��MSB��ʼ����
	SPI_Handler.Init.TIMode = SPI_TIMODE_DISABLE;						//�ر�TIģʽ
	SPI_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPI_Handler.Init.CRCPolynomial = 7;									//CRCֵ�������ʽ
	
	HAL_SPI_Init(&SPI_Handler);
	__HAL_SPI_ENABLE(&SPI_Handler);
	
	//SPI_ReadWriteByte(0xFF);
}

/*SPI������ʼ�����ú����ᱻHAL_SPI_Init����*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {
//	GPIO_InitTypeDef GPIO_SPI2;
//	__HAL_RCC_GPIOB_CLK_ENABLE();
//	__HAL_RCC_SPI2_CLK_ENABLE();
//	
//	GPIO_SPI2.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
//	GPIO_SPI2.Mode = GPIO_MODE_AF_PP;
//	GPIO_SPI2.Pull = GPIO_PULLUP;
//	GPIO_SPI2.Speed = GPIO_SPEED_FREQ_HIGH;
//	
//	HAL_GPIO_Init(GPIOB, &GPIO_SPI2);

	GPIO_InitTypeDef GPIO_SPI1;
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_SPI1_CLK_ENABLE();
	
	GPIO_SPI1.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_SPI1.Mode = GPIO_MODE_AF_PP;
	GPIO_SPI1.Pull = GPIO_PULLUP;
	GPIO_SPI1.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(GPIOA, &GPIO_SPI1);
}

/*SPI�ٶ�����*/
void SPI_SetSpeed(u8 SPI_BaudRatePrescaler) {
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));		//�ж���Ч��
	__HAL_SPI_DISABLE(&SPI_Handler);
	SPI_Handler.Instance->CR1 &= 0xFFC7;
	SPI_Handler.Instance->CR1 |= SPI_BaudRatePrescaler;
	__HAL_SPI_ENABLE(&SPI_Handler);
}

/*SPI��д����*/
u8 SPI_ReadWriteByte(u8 TxData) {
	u8 RxData = 0x00;
	HAL_SPI_TransmitReceive(&SPI_Handler, &TxData, &RxData, 1, 1000);
	return RxData;
}

