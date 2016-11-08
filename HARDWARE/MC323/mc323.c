#include "sys.h"
#include "mc323.h"
#include "usart2.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"    
#include "includes.h"
#include "analysis.h"
#include "command.h"
#include "usart3.h"

const u8 *modetbl[2]={"UDP","TCP"};

volatile u8 InterCurLine=0;
volatile u8 IPCONNCET=0;
volatile u8 RtuSendIdentifier=0;
volatile u8 RtuSendBufNUll=0;
volatile u8 RtuSendFinish=0;
volatile u8 DataSensoCheck=0;

const u8* wifiap_ssid="KLL-ESP8266";	
const u8* wifiap_password="12345678"; 



void atk_8266_reset(void)
{
	if(SystemDebug==2)printf("atk_8266_reset\r\n");
	DRV_WIFI_RST_ONE;
	delay_ms(10);
	DRV_WIFI_RST_ZERO;
	delay_ms(20);
	DRV_WIFI_RST_ONE;
	delay_ms(10);
}


u8* atk_8266_check_cmd(u8 *str)
{
	char *strx=0;
	if(USART2_RX_STA&0X8000)		
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;
		strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;
}

void atk_8266_at_response(u8 mode)
{
	if(USART2_RX_STA&0X8000)		
	{ 
		USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;
		if(SystemDebug==2)printf("%s\r\n",USART2_RX_BUF);
		if(mode)USART2_RX_STA=0;
	} 
}






u8 atk_8266_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0;
	u8 buf[100]={0};
	OS_CPU_SR cpu_sr=0;
	USART2_RX_STA=0;
	sprintf((char*)buf,"%s\r\n",cmd);
	if(SystemDebug==2)printf("%s",buf);
	OS_ENTER_CRITICAL();
	USART2_CMD(buf);
	OS_EXIT_CRITICAL();
	if(ack&&waittime)		
	{
		while(--waittime)	
		{
			delay_ms(10);
			if(USART2_RX_STA&0X8000)
			{
				if(atk_8266_check_cmd(ack))break;
				USART2_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}


u8 atk_8266_quit_trans(void)
{
	while((USART2->SR&0X40)==0);	
	USART2->DR='+';      
	delay_ms(15);					
	while((USART2->SR&0X40)==0);	
	USART2->DR='+';      
	delay_ms(15);					
	while((USART2->SR&0X40)==0);	
	USART2->DR='+';      
	delay_ms(500);					
	return atk_8266_send_cmd("AT","OK",20);
}

u8 atk_8266_consta_check(void)
{
	u8 *p;
	u8 res;
	if(atk_8266_quit_trans())return 0;			
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);
	p=atk_8266_check_cmd("+CIPSTATUS:");
	atk_8266_at_response(1);
	res=*p;		
	return res;
}


u8 atk_8266_apsta_check(void)
{
	if(atk_8266_quit_trans())return 0;			
	atk_8266_send_cmd("AT+CIPSTATUS",":",50);	
	if(atk_8266_check_cmd("+CIPSTATUS:0")&&
		 atk_8266_check_cmd("+CIPSTATUS:1")&&
		 atk_8266_check_cmd("+CIPSTATUS:2")&&
		 atk_8266_check_cmd("+CIPSTATUS:4"))
		return 0;
	else return 1;
}


void atk_8266_get_wanip(u8* ipbuf)
{
	u8 *p,*p1;
		if(atk_8266_send_cmd("AT+CIFSR","OK",50))
		{
			ipbuf[0]=0;
			return;
		}
		p=atk_8266_check_cmd("\"");
		p1=(u8*)strstr((const char*)(p+1),"\"");
		*p1=0;
		sprintf((char*)ipbuf,"%s",p+1);	
}

void atk_8266_wifiap_init(void)
{
	u8 tcent;
	u8 p[100];
	delay_ms(1000);
	tcent=20;
	while(atk_8266_send_cmd("AT","OK",20))
		{
			tcent--;
			delay_ms(300);
			atk_8266_quit_trans();
			atk_8266_send_cmd("AT+CIPMODE=0","OK",200); 
			atk_8266_at_response(1);
			if(tcent==0)
				{
					tcent=20;
					break;
				}
		}
	atk_8266_at_response(1);
	while(atk_8266_send_cmd("ATE0","OK",20));
	atk_8266_at_response(1);
	atk_8266_send_cmd("AT+CWMODE=2","OK",20);
	atk_8266_at_response(1);
	atk_8266_send_cmd("AT+RST","OK",20);
	atk_8266_at_response(1);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",1,4",wifiap_ssid,wifiap_password); 
	atk_8266_send_cmd(p,"OK",1000);
	atk_8266_at_response(1);
}

void atk_8266_wifiap_init2(void)
{
	u8 tcent;
	u8 p[100];
	delay_ms(1000);
	tcent=20;
	while(atk_8266_send_cmd("AT","OK",20))
		{
			tcent--;
			delay_ms(300);
			atk_8266_quit_trans();
			atk_8266_send_cmd("AT+CIPMODE=0","OK",200); 
			atk_8266_at_response(1);
			if(tcent==0)
				{
					tcent=20;
					break;
				}
		}
	atk_8266_at_response(1);
	while(atk_8266_send_cmd("ATE0","OK",20));
	atk_8266_at_response(1);
	atk_8266_send_cmd("AT+CWMODE=2","OK",20);
	atk_8266_at_response(1);
	atk_8266_send_cmd("AT+RST","OK",20);
	atk_8266_at_response(1);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	atk_8266_at_response(1);
	sprintf((char*)p,"AT+CWSAP=\"%s\",\"%s\",11,0",systemset.UserName,systemset.Passwd); 
	atk_8266_send_cmd(p,"OK",1000);
	atk_8266_at_response(1);
}


u8 atk_8266_wifiap_conncet(u8 mode,u8* ipaddr,u8* port)
{
	u8 p[40];
	u8 ipbuf[16];
	delay_ms(10); 
	atk_8266_send_cmd("ATE0","OK",20);
	atk_8266_at_response(1);
	if(mode==0)
		{
			atk_8266_send_cmd("AT+CIPMUX=0","OK",100);
			sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipaddr,(u8*)port); 
			atk_8266_send_cmd(p,"OK",500);
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);
		}
	if(mode==1)
		{
			atk_8266_send_cmd("AT+CIPMUX=0","OK",20); 
			sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipaddr,(u8*)port); 
			while(atk_8266_send_cmd(p,"OK",200))
				{
					
				}
			atk_8266_send_cmd("AT+CIPMODE=1","OK",200);
		}
	if(mode==2)
		{
			atk_8266_send_cmd("AT+CIPMUX=1","OK",20);
			atk_8266_at_response(1);
			sprintf((char*)p,"AT+CIPSERVER=1,%s",(u8*)port);
			atk_8266_send_cmd(p,"OK",20);
			atk_8266_at_response(1);
		}
	//atk_8266_get_wanip(ipbuf);
	//printf("ipbuf=%s\r\n",ipbuf);

	return 0;
}

u8 M35SendCmdCheckAck(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	u8 buf[100]={0};
	OS_CPU_SR cpu_sr=0;
	sprintf((char*)buf,"%s\r\n",cmd);
	OS_ENTER_CRITICAL();
	USART2_CMD(buf);
	OS_EXIT_CRITICAL();
	if(ack&&waittime)		
	{
		while(--waittime)	
		{
			OSTimeDlyHMSM(0,0,0,10);
			if(RtuSendIdentifier)break;
		}
		if(waittime==0)res=1; 
	}
	RtuSendIdentifier=0;
	return res;
}

u8 M35SendCmdCheckBufAck(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	u8 buf[100]={0};
	OS_CPU_SR cpu_sr=0;
	sprintf((char*)buf,"%s\r\n",cmd);
	OS_ENTER_CRITICAL();
	USART2_CMD(buf);
	OS_EXIT_CRITICAL();
	if(ack&&waittime)		
	{
		while(--waittime)	
		{
			OSTimeDlyHMSM(0,0,0,10);
			if(RtuSendBufNUll)break;
		}
		if(waittime==0)res=1; 
		if(RtuSendBufNUll==1)res=0;
		if(RtuSendBufNUll==2)res=2;
	}
	RtuSendBufNUll=0;
	return res;
}


u8 atk_8266_sendData(u8 *data,u16 length,u8 mode)
{
	u8 res=0; 
	u8 p[20];
 	if(mode==0)
 		{
 			//sprintf((char*)p,"AT+CIPSEND=%d",length);
 			//atk_8266_send_cmd(p,"OK",200); 
			//delay_ms(20);
			USART2_DATA(data,length);
 		}
	if(mode==2)
		{
			sprintf((char*)p,"AT+CIPSEND=0,%d",length);
			atk_8266_send_cmd(p,"OK",200); 
			//delay_ms(20);
			USART2_DATA(data,length);
		}
	//res=atk_8266DataCheckOK(400);
	return res;
}

u8 atk_8266DataCheckOK(u16 waittime)
{
	u8 res=0;
	while(--waittime)	
		{
			delay_ms(10);
			if(RtuSendFinish)break;
		}
	RtuSendFinish=0;
	if(waittime==0)res=1;
	return res;
}


void ProcessingTheirReply(u8 *RtuReplyBuf)
{
	u8 res=0;
	u8 *p1,*p2,*p3,*p4;
	p1=(u8*)strstr((const char *)RtuReplyBuf,":");
	if(p1!=NULL)
		{
			if(SystemFlow==10)
				{
					RtuSendBufNUll=1;
					DataSensoCheck=1;
				}

		}
	p2=(u8*)strstr((const char *)RtuReplyBuf,"SEND");
	if(p2!=NULL)
		{
			p3=(u8*)strstr((const char *)RtuReplyBuf,"SEND OK");
			if(p3!=NULL)RtuSendFinish=1;
		}

}



u8 waitforcenterrecall(u16 sdelaytime)
{	
	u8 res=0;
	while(--sdelaytime)	
		{
			delay_ms(10);
			if(DataSensoCheck)break;
		}
	DataSensoCheck=0;
	if(sdelaytime==0)res=1;
	return res;
}

void Get_phone_num(u8 *buf1,u8 *buf2)
{
	mid(buf2,buf1,11,3);
}


void Get_sys_ip(u8 *src,u8 *drc)
{
	u8 *p=NULL;
	p=(u8*)strstr((const char *)src,"!");
	if(p!=NULL)
		{
			while(*src!=0x20)src++;
			src++;
			while(*src!='!')
				{
					*drc=*src;
					src++;
					drc++;
				}
			*drc='\0';
		}
}



void send_mns_sucess(u8 *nums,u8 *data)
{
	
}


void atk_8266_recive_data(u8 *mc35data,u8 len)
{
	ProcessingTheirReply(mc35data);
}

u8 atk_8266ReviceCheckOK(u16 waittime)
{
	u8 res=0;
	while(--waittime)	
		{
			delay_ms(10);
			if(DataSensoCheck)break;
		}
	DataSensoCheck=0;
	if(waittime==0)res=1;
	return res;
}

u8 atk_8266ReviceInitOK(u16 waittime)
{
	u8 res=0;
	while(--waittime)	
		{
			delay_ms(10);
			if(RtuSendBufNUll)break;
		}
	RtuSendBufNUll=0;
	if(waittime==0)res=1;
	return res;
}


//////////////////////////////////////////////////////////////////////////////////////////////////


void atk_8266_wifista_init(u8 *usrname,u8 *pawassd)
{
	u8 tcent;
	u8 p[100];
	delay_ms(1000);
	tcent=20;
	while(atk_8266_send_cmd("AT","OK",20))
		{
			tcent--;
			delay_ms(300);
			atk_8266_quit_trans();
			atk_8266_send_cmd("AT+CIPMODE=0","OK",200); 
			atk_8266_at_response(1);
			if(tcent==0)
				{
					tcent=20;
					break;
				}
		}
	atk_8266_at_response(1);
	while(atk_8266_send_cmd("ATE0","OK",20));
	atk_8266_at_response(1);
	atk_8266_send_cmd("AT+RST","OK",20);
	atk_8266_at_response(1);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	atk_8266_at_response(1);
	atk_8266_send_cmd("AT+CWMODE=3","OK",20);
	atk_8266_at_response(1);
	atk_8266_send_cmd("AT+CWLAP","OK",20);
	atk_8266_at_response(1);
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",usrname,pawassd); 
	atk_8266_send_cmd(p,"OK",1000);
	atk_8266_at_response(1);
	atk_8266_send_cmd("ATE0","OK",20);
	atk_8266_at_response(1);
}




u8 Connect2Hand(u8* ipaddr,u8* port)
{
	u8 p[40];
	atk_8266_send_cmd("AT+CIPMUX=0","OK",100);
	atk_8266_at_response(1);
	sprintf((char*)p,"AT+CIPSTART=\"UDP\",\"%s\",%s",ipaddr,(u8*)port); 
	atk_8266_send_cmd(p,"OK",500);
	atk_8266_at_response(1);
	atk_8266_send_cmd("AT+CIPMODE=1","OK",200);
	atk_8266_at_response(1);
	
	return 0;
}


u8 Conecet2TheHandFromUdp(u8 *myusrname,u8 *mypasswd)
{
	IsOutOfData=0;
	atk_8266_wifista_init(myusrname,mypasswd);
	Connect2Hand("255.255.255.255","6060,6060,0");
	IsOutOfData=1;
	delay_ms(100);
	return 0;
	
}


u8 atk_8266_CheckStatus(u32 timeout)
{
	u8 res=0;
	u8 constate;
	IsOutOfData=0;
	while(--timeout)
		{
			delay_ms(10);
			constate=atk_8266_consta_check();
			if(constate=='+')break;
			else	delay_ms(300);
		}
	IsOutOfData=1;
	if(timeout==0)res=1;
	return res;
}
