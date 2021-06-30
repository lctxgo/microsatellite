#include "sys.h"
#include "delay.h"
#include "usart.h"
//#include "led.h"
//#include "key.h"
#include "usmart.h"
#include "TMC260.h"

#include "wheelpwm.h"
#include "gimbalpulse.h"
/************************************************
 ALIENTEK 战舰STM32F103开发板 实验4
 串口实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/

int main(void)
{
	u8 len;	
	//u16 times=0;
	u32 cnt = 0;
	
    HAL_Init();                    	 	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//设置时钟,72M
	delay_init(72);               		//初始化延时函数
	uart_init(115200);					//初始化串口
	
	usmart_dev.init(84);
	
	

	delay_ms(5000);
	

	wheelPWMInit(1000 - 1, 72 - 1);
	
	
	
//	DIR_OUT = 1;
//	delay_ms(5000);						//等待TMC260上电后再执行后续代码
//	delay_ms(5000);
//	delay_ms(5000);

//	tmc260Init();
//	
//	
//	DIR_OUT = 0;


	delay_us(1);
	gimbalPulseInit(0xffff, 72 - 1);
	//delay_us(1);
	
	
	
	
    while(1)
    {

			
//		printf("马老师，到底发生甚么事了\r\n");
//		delay_ms(1000);
			;
//       if(USART_RX_STA&0x8000)
//		{					   
//			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
//			printf("\r\nSTM32 receive:\r\n");
//			HAL_UART_Transmit(&UART1_Handler,(uint8_t*)USART_RX_BUF,len,1000);	//发送接收到的数据
//			while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//等待发送结束
//			printf("\r\n\r\n");//插入换行
//			USART_RX_STA=0;
//		}else
//		{
//			printf("Waiting for input...\r\n");  
//			
//			delay_ms(2000);   
//		} 
		
//		DIR_OUT = 1;
//		delay_ms(500);
//		DIR_OUT = 0;
//		delay_ms(500);
		//printf("-------\r\n");
//		cnt = TIM3->CNT;
//		printf("cnt:%d\r\n", cnt);
    }
}
