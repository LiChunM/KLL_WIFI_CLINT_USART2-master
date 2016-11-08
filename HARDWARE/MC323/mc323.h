#ifndef __MC323_H__
#define __MC323_H__	 
#include "sys.h"


extern volatile u8 IPCONNCET;
extern volatile u8 InterCurLine;
extern volatile u8 RtuSendIdentifier;
extern volatile u8 RtuSendFinish;
extern volatile u8 DataSensoCheck;
extern const u8 *modetbl[2];
u8 atk_8266DataCheckOK(u16 waittime);
void atk_8266_reset(void);
u8 atk_8266_CheckStatus(u32 timeout);
u8 Conecet2TheHandFromUdp(u8 *myusrname,u8 *mypasswd);
#endif
