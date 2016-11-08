#include "delay.h"
#include "sys.h"
#include "usart.h"	 
#include "includes.h"  
#include "flash_in.h"
#include "analysis.h"
#include "stm32f10x_adc.h"
#include "analysis.h"
#include "adc.h"
#include "rc522.h"


//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//任务堆栈	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);	
 			   
//USART1任务
//设置任务优先级
#define USART1_TASK_PRIO       			3 
//设置任务堆栈大小
#define USART1_STK_SIZE  		    		512
//任务堆栈
OS_STK USART1_TASK_STK[USART1_STK_SIZE];
//任务函数处理用户的随时的输入
void usart1_task(void *pdata);

//USART2任务
//设置任务优先级
#define USART2_TASK_PRIO       		 	6
//设置任务堆栈大小
#define USART2_STK_SIZE  				512
//任务堆栈	
OS_STK USART2_TASK_STK[USART2_STK_SIZE];
//任务函数用来处理CDMA数据
void usart2_task(void *pdata);

//UART5任务
//设置任务优先级
#define UART5_TASK_PRIO    			2 
//设置任务堆栈大小
#define UART5_STK_SIZE  		 		256
//任务堆栈	
OS_STK UART5_TASK_STK[UART5_STK_SIZE];
//任务函数用来接受GPS数据
void uart5_task(void *pdata);


//主任务
//设置任务优先级
#define MAIN_TASK_PRIO       			8
//设置任务堆栈大小
#define MAIN_STK_SIZE  					512
//任务堆栈	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//任务函数
void main_task(void *pdata);


//预留任务
//设置任务优先级
#define XXX_TASK_PRIO       			5 
//设置任务堆栈大小
#define XXX_STK_SIZE  					64
//任务堆栈	
OS_STK XXX_TASK_STK[XXX_STK_SIZE];
//任务函数什么也不做
void xxx_task(void *pdata);

//HAND 任务
//设置任务优先级
#define HAND_TASK_PRIO      			7 //进行握手任务操作
//设置任务堆栈大小
#define HAND_STK_SIZE  				128
//任务堆栈	
OS_STK HAND_TASK_STK[HAND_STK_SIZE];
//任务函数
void hand_task(void *pdata);	
 			   
//323任务
//设置任务优先级
#define  MC323_TASK_PRIO       			4
//设置任务堆栈大小
#define MC323_STK_SIZE  					256
//任务堆栈	
OS_STK MC323_TASK_STK[MC323_STK_SIZE];
//任务函数什么也不做
void MC323_task(void *pdata);

 			   
//323任务
//设置任务优先级
#define  MC323_reconnection_TASK_PRIO       			9
//设置任务堆栈大小
#define MC323_reconnection_STK_SIZE  					128
//任务堆栈	
OS_STK MC323_reconnection_TASK_STK[MC323_reconnection_STK_SIZE];
//任务函数什么也不做
void MC323_reconnection_task(void *pdata);




 int main(void)
 {	 
 	JTAG_Set(0x02);
  	power_drv_init();
 	delay_init();	    	
	NVIC_Configuration();
	uart_init(72,115200);
	USART3_Init(115200);
	USART2_Init(36,115200);
	PWM_Init();
	TIM1_PWM_init(1500,16);
	InitRc522();
	Sound_drv_init();
	IWDG_Init(4,625);
	SYS_Parameter_Init();
	OSInit();
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();
}							    
//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 	
	OSStatInit();					
 	OS_ENTER_CRITICAL();			    
 	OSTaskCreate(usart1_task,(void *)0,(OS_STK*)&USART1_TASK_STK[USART1_STK_SIZE-1],USART1_TASK_PRIO);						   
 	OSTaskCreate(usart2_task,(void *)0,(OS_STK*)&USART2_TASK_STK[USART2_STK_SIZE-1],USART2_TASK_PRIO);	 				   
 	OSTaskCreate(uart5_task,(void *)0,(OS_STK*)&UART5_TASK_STK[UART5_STK_SIZE-1],UART5_TASK_PRIO);
	OSTaskCreate(hand_task,(void *)0,(OS_STK*)&HAND_TASK_STK[HAND_STK_SIZE-1],HAND_TASK_PRIO);	 				   
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);	
	OSTaskCreate(MC323_task,(void *)0,(OS_STK*)&MC323_TASK_STK[MC323_STK_SIZE-1],MC323_TASK_PRIO);	 				   
 	OSTaskCreate(xxx_task,(void *)0,(OS_STK*)&XXX_TASK_STK[XXX_STK_SIZE-1],XXX_TASK_PRIO);
	OSTaskCreate(MC323_reconnection_task,(void *)0,(OS_STK*)&MC323_reconnection_TASK_STK[MC323_reconnection_STK_SIZE-1],MC323_reconnection_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);
	OS_EXIT_CRITICAL();
}
//usart1任务

void usart1_task(void *pdata)
{
	u8 res,t;
	pdata = pdata;
	while(1)
		{
			delay_ms(10);
			Usart1CommandAnalysis();
		}
}
//usart2任务
void usart2_task(void *pdata)
{	
	u8 t=0;
	u8 res=0,length;
	pdata = pdata;
	while(1)
	{
		delay_ms(10);
		if(systemset.usrpwdsfg!=0x0A)
			{
				if(SystemFlow==0)
					{
SCANF:					
						atk_8266_reset();
						Conecet2TheHandFromUdp(systemset.DefUsName,systemset.DefPawd);
						res=SaveUsnamePswd(500);
						if(res)
							{
								if(SystemDebug==2)printf("Conecet2TheHandFromUdp Next\r\n");
								goto SCANF;
							}
						else
							{
								RePayUP_CarData(&length);
								atk_8266_sendData(Hand_Data,length,0);
								systemset.usrpwdsfg=0x0A;
								sysset_save_para(&systemset);
							}
						SystemFlow=1;
					}
				if(SystemFlow==1)
					{
ResetAtk:
						atk_8266_reset();
						Conecet2TheHandFromUdp(systemset.UserName,systemset.Passwd);
						res=atk_8266_CheckStatus(50);
						if(res)
							{
								if(SystemDebug==2)printf("Can not scan to wifi, continue scanning after reset\r\n");
								goto ResetAtk;
							}
						else
							{
REG:
								ReReg_CarData(&length);
								atk_8266_sendData(Hand_Data,length,0);
								res=RegSucess(300);
								if(res)
									{
										if(t>10)
											{
												if(SystemDebug==2)printf("Reg Time Out\r\n");
												t=0;
												goto ResetAtk;
											}
										if(SystemDebug==2)printf("RegError Try Next\r\n");
										t++;
										goto REG;
									}
								else
									{
										RePayUP_CarData(&length);
										atk_8266_sendData(Hand_Data,length,0);
									}
								
							}
						SystemFlow=2;
					}
			}
		else
			{
				if(systemset.SysSpeedBit)
					{
						if(SystemFlow==0)
							{
ResetAtk2:
								atk_8266_reset();
								Conecet2TheHandFromUdp(systemset.UserName,systemset.Passwd);
								res=atk_8266_CheckStatus(50);
								if(res)
									{
										if(systemset.SysSpeedBit)
										{
											systemset.SysSpeedBit=0;
											sysset_save_para(&systemset);
										}
										if(SystemDebug==2)printf("Can not scan to wifi, continue scanning after reset\r\n");
										goto ResetAtk2;
									}
								else
									{
										SystemFlow=2;
									}
							}
					}
				else
					{
						if(SystemFlow==0)
							{
								atk_8266_reset();
								Conecet2TheHandFromUdp(systemset.DefUsName,systemset.DefPawd);
								res=SaveUsnamePswd(500);
								if(res)
									{
										if(SystemDebug==2)printf("Conecet2TheHandFromUdp Error\r\n");
ResetAtk3:
										atk_8266_reset();
										Conecet2TheHandFromUdp(systemset.UserName,systemset.Passwd);
										res=atk_8266_CheckStatus(50);
										if(res)
											{
												if(systemset.SysSpeedBit)
													{
														systemset.SysSpeedBit=0;
														sysset_save_para(&systemset);
													}
												if(SystemDebug==2)printf("Can not scan to wifi, continue scanning after reset\r\n");
												goto ResetAtk2;
											}
										else
											{
												SystemFlow=2;
											}
									}
								else
									{
										RePayUP_CarData(&length);
										atk_8266_sendData(Hand_Data,length,0);
										systemset.usrpwdsfg=0x0A;
										sysset_save_para(&systemset);
										SystemFlow=1;
									}
							}
						if(SystemFlow==1)
							{
ResetAtk4:
								atk_8266_reset();
								Conecet2TheHandFromUdp(systemset.UserName,systemset.Passwd);
								res=atk_8266_CheckStatus(50);
								if(res)
									{
										if(SystemDebug==2)printf("Can not scan to wifi, continue scanning after reset\r\n");
										goto ResetAtk;
									}
								else
									{
REG4:
										ReReg_CarData(&length);
										atk_8266_sendData(Hand_Data,length,0);
										res=RegSucess(300);
										if(res)
											{
												if(t>10)
													{
														if(SystemDebug==2)printf("Reg Time Out\r\n");
														t=0;
														goto ResetAtk4;
													}
												if(SystemDebug==2)printf("RegError Try Next\r\n");
												t++;
												goto REG4;
											}
										else
											{
												RePayUP_CarData(&length);
												atk_8266_sendData(Hand_Data,length,0);
											}
										
									}
								SystemFlow=2;
							}

					}
			}
		
	}
}     
//gps任务
void uart5_task(void *pdata)
{
	pdata = pdata;
	while(1)
	{
		delay_ms(4);
		moto_move();
	}									 
}
//主任务
void main_task(void *pdata)
{	
 	while(1)
	{
		delay_ms(10);
		if(DataBiteInfo&0x08)
			{
				DataBiteInfo&=~(1<<3);
				if(SystemDebug==2)printf("Change Rail\r\n");
			}
		Voice_Alarm_out();	
		
	}
}	

void hand_task(void *pdata)
{
	pdata=pdata;
	while(1)
		{
			delay_ms(10);
			CheckSpeedBit();
		}
}
void MC323_task(void *pdata)
{
	
	pdata=pdata;
	while(1)
		{
			delay_ms(100);
		}
}
   		    
//监视任务
void xxx_task(void *pdata)
{	
	u8 i;
	pdata=pdata;
	while(1)
	{
		delay_ms(10);
	}
}

void MC323_reconnection_task(void *pdata)
{
	pdata=pdata;
	while(1)
		{
			delay_ms(20);
			IWDG_Feed();
		}
	
}


void HardFault_Handler(void)
{
	u32 temp;
	temp=SCB->CFSR;					
 	printf("CFSR:%8X\r\n",temp);	
	temp=SCB->HFSR;					
 	printf("HFSR:%8X\r\n",temp);	
 	temp=SCB->DFSR;					
 	printf("DFSR:%8X\r\n",temp);	
   	temp=SCB->AFSR;					
 	printf("AFSR:%8X\r\n",temp);
}





