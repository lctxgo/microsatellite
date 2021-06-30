#ifndef _TMC260_H_
#define _TMC260_H_

#include "SPI.h"
#include "sys.h"

/* 

Macro Register 

*/
#define		DRVCTRL				0x00000004	//microstep 16
#define		CHOPCONF			0x00080000
#define		SMARTEN				0x000A0000
#define		SGCSCONF			0x000C0000
#define		DRVCONF				0x000E0000


#define		READCOMMAND			0xFFFFFFFF
/* 

Microstep resolution defination in the mode of step/dir while SDOFF = 0 

SDOFF is bit7 in register DRVCONF

bit3-bit0 in DRVCONF

*/
#define		MICROSTEP_256		0x00
#define		MICROSTEP_128		0x01
#define		MICROSTEP_64		0x02
#define		MICROSTEP_32		0x03
#define		MICROSTEP_16		0x04
#define		MICROSTEP_8			0x05
#define		MICROSTEP_4			0x06
#define		MICROSTEP_2			0x07
#define		MICROSTEP_1			0x08

/*

readback format

*/
#define		READMICROSTEP		0x00
#define		READSTALLGUARD2		0x10
#define		READSTALLCOOL		0x20

/*

GPIO defination and initialization

SPI 3 wires including SCK, MISO, MOSI is configured in SPI_Init()

STEP, DIR, ENABLE, CS configuration

*/
//STEP
#define		STEP_GPIO_PORT		GPIOB
#define		STEP_GPIO_PIN		GPIO_PIN_0
#define		STEP_OUT			PBout(0)
//DIR
#define		DIR_GPIO_PORT		GPIOB
#define		DIR_GPIO_PIN		GPIO_PIN_5
#define		DIR_OUT				PBout(5)
//EN high: disable all MOSFETs
#define		EN_GPIO_PORT		GPIOC
#define		EN_GPIO_PIN			GPIO_PIN_5
#define		EN_OUT				PCout(5)
//CS
#define		CS_GPIO_PORT		GPIOC
#define		CS_GPIO_PIN			GPIO_PIN_11
#define		CS_OUT				PCout(11)
/*

PC11	----	SPI_CS

*/
u32 tmc260SPITransmit(u32 writeData);
void tmc260SPICS(u8 state);
void tmc260Init();
void tmc260SetMicroStep(u8 micro);
u32 tmc260SetCurrent(u8 current);

#endif
