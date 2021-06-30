#ifndef _SPI_H_
#define _SPI_H_
#include "sys.h"
 

extern SPI_HandleTypeDef SPI_Handler;

void SPI_Init(void);
void SPI_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI_ReadWriteByte(u8 TxData);


#endif
