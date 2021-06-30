#ifndef _SPI2_H_
#define _SPI2_H_
#include "sys.h"

extern SPI_HandleTypeDef SPI2_Handler;

void SPI2_Init(void);
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);


#endif