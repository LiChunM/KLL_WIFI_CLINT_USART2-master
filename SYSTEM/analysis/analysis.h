#ifndef _ANALYSIS_H_
#define _ANALYSIS_H_
#include "sys.h"

#define isPrime(year) ((year%4==0&&year%100!=0)||(year%400==0))


typedef struct 
{
	u8 speed;
	u8 fangxiang;
	u8 frontled;     //������ٿ���
	u8 bhandled;    //��������
	u8 waningled;	//ɲ����
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
	u16 		machineID;		//������id
	u8		wifi_linkid;		//��ͨwifi���ID	0x01~0x04�������Ǹ�С�������ϱ��Ǹ�С����
	u8		Remote_ControlId;	//�ֱ������ʶ���롣
	u8		Pair_Success;	// ��Գɹ����Ƶ���˸����

	u32		wifi_conecting_counter; 	//��ң����ͨ�ż���
	u32 		circle_counter;	//ÿȦ������
	u32		last_circle_counter;	//���Ϸ���ÿȦ�ļ������ۻ�ʱ�� 4���ֽڣ���mS�����룩Ϊ��λ�����ۼ�

	u8 	io_counter_flag;
	u8	send_upinfo_flag;
	u16	speed;		//�����ٶ�  0-255;
	u16	speed2frenquence;	//�������ٶ�ת��ΪpwmƵ��ֵ
	u8	moto_stat ;		//�����״̬��ֹͣ����������

	u16	connect_default_timer;		//����Ĭ���豸ʱ��
	u16	connect_match_timer;			//����ר���豸ʱ��
/*
//�����Ƶ�״̬0x00---ǰ���𣬱�������
0x01---ǰ�Ƴ�������������
0x02---ǰ����˸��1������1���𣩣���������
0x03---ǰ�ƣ������ƾ���˸��1������1����
ע����4��״̬�ǵ�λ����ť��Ӧ��4��λ�á�
*/
	u8 	led_stat;		
/*
0x00-----ɲ������
0x01-----ɲ���Ƴ���
*/
	u8	led_stop_stat;		//ɲ����״̬��־
/*
0x01~0x64 ���ȼ���1~100
С���յ��󣬰���������������ٶȣ����磺0x64Ϊ����ٶȡ�

*/	
	u8	car_max_speed_rate;	//����ٶȿ��Ʊ���ֵ
/*
0x00--�ر�������ʾ��0x01--��������ʾ����ʱĬ��Ϊ01�����ֱ�����û������Ŀ��ؿ��ƣ�
*/	
	u8	music_switch;			//�������ƿ���
/*
0x00 ----- ���������� 
0x02 ----- ���ŵ�2����Ƶ �����ٶ�ֵ�����������20��������ʱ�����ŵ�2�Σ�
0x03 ----- ���ŵ�3����Ƶ�����ٶȵݼ�ʱ�����ŵ�3�Σ�
*/
	u8	music_section;			//�����������״̬
	u8	music_onoff;				//����״̬���ڲ��Ż��ǿ���
/*
0x00 -- ���ʱ������
0x01 - -���ʱ���١�
*/	
	u8	cruve_slowdown_switch;	//����Ƿ���ٿ���
/*
0x01~0x64 ���ȼ���1~100
С���յ��󣬰���������������ٶȣ����磺0x64Ϊ����ٶȡ�
*/
	u8	cruve_slowdown_rate;	//���ʱ�����ٶȱ���ֵ
/*
6���ֽڣ�������չ�á���Ĭ����00H��
*/
	u8	cruve_flag;		// ���״̬���Ƿ����������·��
	
	u8	backbuf[6];
	
	u8	run_direction;	//���з���
	u8	TRANSFERMODE;		//wifi����ģʽ
	u8	WIFI_CONNECT_FLAG;	//�����Ƿ�ɹ�


	u8	get_degree_flag;		// ��ȡ�Ƕȴ�������־λ
	u16 	acc_x,acc_y,acc_z,gy_x,gy_y,gy_z;		
	float x_degree,y_degree,z_degree;	//������ĽǶ�ֵ
	u8 Key_val;		//����ֵ
	u16	Key_filter;	//�˲�����

	//�����Ա�־λ��0x01 ��Ե���Ĭ�Ϸ����� 0x02, ��Ե��Ǳ�׼��������
	// 0xff�� ��һ����������û�����
	char match_flag;		//�Ƿ�������
	char	speed_flag;		//�ٶȱ�ʾ	1,�������ٶȣ�0 ����û���ٶ�
	char	speed_old_flag;		//�ٶȱ�ʾ
	u16 speed_min_couter;	//�ٶȱ�ʾ����
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
