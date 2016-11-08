#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_
#include "sys.h"

#define isPrime(year) ((year%4==0&&year%100!=0)||(year%400==0))


typedef struct 
{
	u8 speed;
	u8 fangxiang;
	u8 frontled;     //弯道减速开关
	u8 bhandled;    //正反运行
	u8 waningled;	//刹车灯
	u8 maxsepeed;
	u8 jiansepeed;
	u8 voice;
	u8 biandao;
	u8 ledstatu;
	u8 back[6];
}Core_data;


extern Core_data my_core_data;

extern u8 Hand_Data[60];

typedef struct 
{
	u8 MyYear;
	u8 MyMonth;
	u8 MyData;
}Mydatastrcut;

typedef	struct	SYS_PARA
{
	u8		configval;
	u16 		machineID;		//本机的id
	u8		wifi_linkid;		//联通wifi后的ID	0x01~0x04，存在那个小车，就上报那个小车。
	u8		Remote_ControlId;	//手柄的身份识别码。
	u8		Pair_Success;	// 配对成功，灯的闪烁次数

	u32		wifi_conecting_counter; 	//与遥控器通信计数
	u32 		circle_counter;	//每圈计数器
	u32		last_circle_counter;	//向上发送每圈的计数器累积时间 4个字节，以mS（毫秒）为单位进行累加

	u8 	io_counter_flag;
	u8	send_upinfo_flag;
	u16	speed;		//车的速度  0-255;
	u16	speed2frenquence;	//将车的速度转换为pwm频率值
	u8	moto_stat ;		//电机的状态，停止还是在运行

	u16	connect_default_timer;		//连接默认设备时间
	u16	connect_match_timer;			//连接专用设备时间
/*
//三个灯的状态0x00---前灯灭，报警灯灭
0x01---前灯常亮，报警灯灭
0x02---前灯闪烁（1秒亮，1秒灭），报警灯灭
0x03---前灯，报警灯均闪烁（1秒亮，1秒灭）
注：此4种状态是电位器旋钮对应的4个位置。
*/
	u8 	led_stat;		
/*
0x00-----刹车灯灭
0x01-----刹车灯长亮
*/
	u8	led_stop_stat;		//刹车灯状态标志
/*
0x01~0x64 ，等级从1~100
小车收到后，按比例来限制最高速度，例如：0x64为最高速度。

*/	
	u8	car_max_speed_rate;	//最高速度控制比例值
/*
0x00--关闭声音提示，0x01--打开声音提示（暂时默认为01，因手柄上面没有这个的开关控制）
*/	
	u8	music_switch;			//声音控制开关
/*
0x00 ----- 不播放声音 
0x02 ----- 播放第2段音频 （当速度值增大比例大于20个数量级时，播放第2段）
0x03 ----- 播放第3段音频（当速度递减时，播放第3段）
*/
	u8	music_section;			//声音段码控制状态
	u8	music_onoff;				//声音状态，在播放还是空闲
/*
0x00 -- 弯道时不减速
0x01 - -弯道时减速。
*/	
	u8	cruve_slowdown_switch;	//弯道是否减速开关
/*
0x01~0x64 ，等级从1~100
小车收到后，按比例来限制最高速度，例如：0x64为最高速度。
*/
	u8	cruve_slowdown_rate;	//弯道时减速速度比例值
/*
6个字节，留做扩展用。（默认是00H）
*/
	u8	cruve_flag;		// 弯道状态，是否运行在弯道路段
	
	u8	backbuf[6];
	
	u8	run_direction;	//运行方向
	u8	TRANSFERMODE;		//wifi传输模式
	u8	WIFI_CONNECT_FLAG;	//连接是否成功


	u8	get_degree_flag;		// 读取角度传感器标志位
	u16 	acc_x,acc_y,acc_z,gy_x,gy_y,gy_z;		
	float x_degree,y_degree,z_degree;	//与三轴的角度值
	u8 Key_val;		//按键值
	u16	Key_filter;	//滤波按键

	//存放配对标志位，0x01 配对的是默认服务器 0x02, 配对的是标准服务器，
	// 0xff， 第一次启动，还没有配对
	char match_flag;		//是否配对完成
	char	speed_flag;		//速度标示	1,代表有速度，0 代表没有速度
	char	speed_old_flag;		//速度标示
	u16 speed_min_couter;	//速度标示计数
	u16 speed_max_couter;	


	u8 speed_buf[20];
	u8 spedd_counter;

}SYS_PARA_T;
extern SYS_PARA_T	sys_data;


void dispose_wifi_cmd(u8 *pRecStr);
u8 SaveUsnamePswd(u8 outtime);
void RePayUP_CarData(u8 *len);
void ReReg_CarData(u8 *len);
u8 RegSucess(u8 outtime);
void CheckSpeedBit(void);
extern volatile u8 DataBiteInfo;




#endif
