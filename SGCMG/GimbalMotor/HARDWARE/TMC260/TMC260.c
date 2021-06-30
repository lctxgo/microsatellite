#include "TMC260.h"
#include "usart.h"
#include "SPI.h"
#include "delay.h"

#define		CHOPCONF_INITIAL_VAL				0x000901B4
#define		SGCSCONF_INITIAL_VAL				0x000D001F	//Filtered mode enable
#define		DRVCONF_INITIAL_VAL					0x000E0010	//VSENSE = 0
#define		DRVCTRL_INITIAL_VAL					0x00000008	//MICROSTEP_1	
#define 	SMARTEN_INITIAL_VAL					0x000A0202	//1/2 CS, 0xA8202: 1/4 CS
/* 

TMC260 SPI transaction requres 20bits. If more than 20bits are sent, only the last 20bits received.

STM32 SPI can send 8bits or 16bits at a time. Here 8bits mode is used. 

So 4 times transmission are needed and 24bits in total.

*/
u32 tmc260SPITransmit(u32 writeData) {
	u32 receiveData = 0;
	
	CS_OUT = 0;
	delay_us(1);
	
    receiveData |= SPI_ReadWriteByte((writeData >> 16) & 0xFF);
	receiveData <<= 8;
	receiveData |= SPI_ReadWriteByte((writeData >> 8) & 0xFF);
	receiveData <<= 8;
	receiveData |= SPI_ReadWriteByte(writeData & 0xFF);
	
	delay_us(1);
	CS_OUT = 1;
	
	receiveData &= 0xFFFFF;
	
	printf("\r\n0x%x\r\n", receiveData);
	
	return receiveData;
}

/*

TMC260 SPI CS control use the STM32 GPIOC port and the number is 11.

0 or 1 state instruct the output state.

For TMC260 slave device, CS = 0 is valid.

*/
void tmc260SPICS(u8 state) {
	if (state) {
		PCout(11) = 1;
	}
	else {
		PCout(11) = 0;
	}
}
/*

TMC260 initialization

1. GPIO configuration includes STEP, DIR, ENABLE, CS

2. SPI_Init()

3. TMC260 register configuration
*/
void tmc260Init() {
	u32 rev;
	//1. GPIO configuration
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	


	GPIO_Initure.Pull = GPIO_PULLUP;
	GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
	
	
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pin = DIR_GPIO_PIN;
	HAL_GPIO_Init(DIR_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin = EN_GPIO_PIN;
	HAL_GPIO_Init(EN_GPIO_PORT, &GPIO_Initure);
	
	GPIO_Initure.Pin = CS_GPIO_PIN;
	HAL_GPIO_Init(CS_GPIO_PORT, &GPIO_Initure);
	
	//STEP_OUT = 0;
	DIR_OUT = 1;
	CS_OUT = 1;
	EN_OUT = 1;
	
	//2. SPI_Init()
	SPI_Init();
//	CS_OUT = 0;
	EN_OUT = 0;
	printf("\r\ntmc260Init\r\n");
	//3. TMC260 register configuration
	rev = tmc260SPITransmit(CHOPCONF_INITIAL_VAL);
	printf("CHOPCONF_INITIAL_VAL return:0x%x", rev);
	rev = tmc260SPITransmit(SGCSCONF_INITIAL_VAL);
	printf("SGCSCONF_INITIAL_VAL:0x%x", rev);
	rev = tmc260SPITransmit(DRVCONF_INITIAL_VAL);
	printf("DRVCONF_INITIAL_VAL:0x%x", rev);
	rev = tmc260SPITransmit(DRVCTRL_INITIAL_VAL);
	printf("DRVCTRL_INITIAL_VAL:0x%x", rev);
	rev = tmc260SPITransmit(SMARTEN_INITIAL_VAL);
	printf("SMARTEN_INITIAL_VAL:0x%x", rev);
	
//	
//	delay_us(1);
//	CS_OUT = 1;
//	
	
	
}

/*

TMC260 set microstep through DRVCTRL bit3-bit0

*/
void tmc260SetMicroStep(u8 micro) {
	u32 cmd = DRVCTRL & 0x11111100;
	u32 rev;
	cmd |= micro;
	printf("\r\ncmd: 0x%x\r\n", cmd);
	rev = tmc260SPITransmit(cmd);
	printf("\r\nMicroStep set:0x%x\r\n", micro);
	printf("\r\ntmc260SetMicroStep return:0x%x", rev);
	//return rev;
}

/*

TMC260 set current through SGCSCONF bit4-bit0

intput scale 1-32
*/
u32 tmc260SetCurrent(u8 current) {
	u8 temp = (current - 1) & 0x1F;
	u32 cmd = SGCSCONF | temp;
	u32 rev = tmc260SPITransmit(cmd);
	printf("Current set:%d\r\n", current);
	printf("tmc260SetCurrent return:0x%x\r\n", rev);
	return rev;
}
/*

readback StallGuard2 parameter which indicate the load measurement

*/
u32 readStallGuard2() {
	u32 rev = 0;
	u32 writedata = DRVCONF_INITIAL_VAL & 0x60;
	writedata |= READSTALLGUARD2;
	rev = tmc260SPITransmit(writedata);
	rev = tmc260SPITransmit(READCOMMAND);
	rev >>= 10;
	rev = (0x3FF - rev) * 100 / 0x3FF;
	printf("StallGuard2 Load Measurement is:%d%%", rev);
	return rev;
}