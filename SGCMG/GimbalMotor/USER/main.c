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
 ALIENTEK ս��STM32F103������ ʵ��4
 ����ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

int main(void)
{
	u8 len;	
	//u16 times=0;
	u32 cnt = 0;
	
    HAL_Init();                    	 	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9);   	//����ʱ��,72M
	delay_init(72);               		//��ʼ����ʱ����
	uart_init(115200);					//��ʼ������
	
	usmart_dev.init(84);
	
	

	delay_ms(5000);
	

	wheelPWMInit(1000 - 1, 72 - 1);
	
	
	
//	DIR_OUT = 1;
//	delay_ms(5000);						//�ȴ�TMC260�ϵ����ִ�к�������
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

			
//		printf("����ʦ�����׷�����ô����\r\n");
//		delay_ms(1000);
			;
//       if(USART_RX_STA&0x8000)
//		{					   
//			len=USART_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
//			printf("\r\nSTM32 receive:\r\n");
//			HAL_UART_Transmit(&UART1_Handler,(uint8_t*)USART_RX_BUF,len,1000);	//���ͽ��յ�������
//			while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
//			printf("\r\n\r\n");//���뻻��
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
