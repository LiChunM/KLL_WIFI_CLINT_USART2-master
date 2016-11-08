#include "power_drv.h"
#include "sys.h"
#include "includes.h"
#include "delay.h"

u8 Config_Byte;

void power_drv_init(void)
{
     	GPIO_InitTypeDef  GPIO_InitStructure;
     	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_1;			
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);					
 	GPIO_ResetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_1);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_9|GPIO_Pin_1;			
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);					 
}


void ACC_Init(void)
{
    
}

void Sound_drv_init(void)
{
     GPIO_InitTypeDef  GPIO_InitStructure;
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_10);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 
}

u8 send_Alarm(u8 senddata)
{
	u8 i_counter = 0;
	GPIO_ResetBits(GPIOB,GPIO_Pin_10);
	delay_ms(20);
	for(i_counter=0;i_counter<8;i_counter++)
		{
			GPIO_SetBits(GPIOB,GPIO_Pin_10);
			if((senddata&0x01)==0x01)
			{
				delay_us(12);
				GPIO_ResetBits(GPIOB,GPIO_Pin_10);
				delay_us(4);
			}
			else
			{
				delay_us(4);
				GPIO_ResetBits(GPIOB,GPIO_Pin_10);
				delay_us(12);
			}
			senddata=senddata>>1;	
		}
	GPIO_SetBits(GPIOB,GPIO_Pin_10);
	return 0;
}

