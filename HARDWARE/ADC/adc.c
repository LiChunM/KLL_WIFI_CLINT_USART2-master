#include "delay.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
#include "adc.h"
#include "stm32f10x_adc.h"
#include "analysis.h"


_adc_values	adcvols;

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1, ENABLE );	  
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);   


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	
	ADC_DeInit(ADC1);  

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
	ADC_InitStructure.ADC_NbrOfChannel = 5;	
	ADC_Init(ADC1, &ADC_InitStructure);	

  
	ADC_Cmd(ADC1, ENABLE);	
	
	ADC_ResetCalibration(ADC1);	
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	
	
	ADC_StartCalibration(ADC1);	 
 
	while(ADC_GetCalibrationStatus(ADC1));	
 

}			


u16 Get_Adc(u8 ch)   
{
  	
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ch, 2, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ch, 3, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ch, 4, ADC_SampleTime_239Cycles5 );
	ADC_RegularChannelConfig(ADC1, ch, 5, ADC_SampleTime_239Cycles5 );
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));

	return ADC_GetConversionValue(ADC1);	
}


u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 

void Update_SysVol(void)
{
	u16 adcvalue;
	adcvalue=Get_Adc_Average(ADC_Channel_0, 10);
	adcvols.PA0VOL= adcvalue*ADC_BASE/4096;
	adcvalue=Get_Adc_Average(ADC_Channel_1, 10);
	adcvols.PA1VOL= adcvalue*ADC_BASE/4096;
	adcvalue=Get_Adc_Average(ADC_Channel_2, 10);
	adcvols.PA2VOL= adcvalue*ADC_BASE/4096;
	adcvalue=Get_Adc_Average(ADC_Channel_3, 10);
	adcvols.PA3VOL= adcvalue*ADC_BASE/4096;
	adcvalue=Get_Adc_Average(ADC_Channel_4, 10);
	adcvols.PA4VOL= adcvalue*ADC_BASE/4096;
}



void PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE );
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOB,GPIO_Pin_14);
}



void TIM1_PWM_init(u16 InitValue, u16 Pre_Divide)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//����TIM3�ṹ��
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
							RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO| 
							RCC_APB2Periph_TIM1, 
							ENABLE);
	
	//��ʱ����������
	TIM_TimeBaseStructure.TIM_Period =InitValue;//����ֵΪ1000
	TIM_TimeBaseStructure.TIM_Prescaler = Pre_Divide;//10��Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;//������Ƶ0
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

//pwm ����
 /* PWM1 Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//��ʱ������ΪPWM1ģʽ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;//ͨ��1���ʹ��
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;//ѡ�񻥲�����Ƚ�״̬
	TIM_OCInitStructure.TIM_Pulse = 0;    //���ô�װ�벶��ȽϼĴ���������ֵ 
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;//ѡ���������??TIM_OCInitStructure.TIM_OCNPolarity?=?TIM_OCNPolarity_High;???//ѡ�񻥲��������?		
//	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
//	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

	TIM_OC1Init(TIM1,&TIM_OCInitStructure); //����TIM_OCInitStruct��ָ���Ĳ�����ʼ��TIM3
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���

//	TIM_OC2Init(TIM1,&TIM_OCInitStructure);
//	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIMx��CCR1�ϵ�Ԥװ�ؼĴ���

	TIM_ARRPreloadConfig(TIM1, ENABLE);//ʹ��TIM3��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_Cmd(TIM1,ENABLE);
	TIM_SetCompare1(TIM1,2000);	
}


void moto_forward(void)
{	
	TIM_SetCompare1(TIM1,1000);	
	//TIM_SetCompare2(TIM1,sys_data.speed2frenquence);	
}

void moto_backward(void)
{
	//Ƶ��Խ���ٶ�Խ��

	if(sys_data.speed2frenquence>SPEED_MIN_FREQ)
	{
		TIM_CtrlPWMOutputs(TIM1,DISABLE);
		TIM_SetCompare1(TIM1,sys_data.speed2frenquence);
	}
	else
	{
		TIM_CtrlPWMOutputs(TIM1,ENABLE);
		TIM_SetCompare1(TIM1,sys_data.speed2frenquence);
	}
}

void moto_stop(void)
{
	TIM_SetCompare1(TIM1,0);	
	TIM_SetCompare2(TIM1,0);	
}

void moto_break(void)
{
	TIM_SetCompare1(TIM1,100);	
	TIM_SetCompare2(TIM1,100);	
}

u16	change_speed2frequency(u16 speed)
{
	u16 frenquency = 0;
	float	speedrate = 0;

	if(speed<30)frenquency = SPEED_MIN_FREQ+50;//�ٶ�Ϊ0 
	else
		{
			speed = speed + 130;
			if(speed>225)		
			{
				frenquency = 0;	//����ٶ�
			}	
			else
			{
				
				speedrate = SPEED_MIN_FREQ - SPEED_MAX_FREQ;
				speedrate = speedrate/200;
				
				speedrate = speedrate * speed;
				
				speedrate = SPEED_MIN_FREQ-speedrate ;
				frenquency = speedrate;
			}
		}
	return frenquency;
	
}

void moto_move(void)
{
//�ٶ�תΪƵ��
	sys_data.speed2frenquence = change_speed2frequency(sys_data.speed);	
//�жϷ���
	sys_data.run_direction = 0;
//������ж���	
	if(sys_data.run_direction == 1)
	{
		moto_forward();
	}
	else
		moto_backward();	
}


void Voice_Alarm_out(void)
{
	if((sys_data.music_switch ==1)&&(sys_data.music_section!=0))
	{
		DRV_5V_ON;
		send_Alarm(sys_data.music_section);
		sys_data.music_section = 0;
		DRV_5V_OFF;
	}
	else
	{
		sys_data.music_section = 0;
		DRV_5V_OFF;
	}
}

