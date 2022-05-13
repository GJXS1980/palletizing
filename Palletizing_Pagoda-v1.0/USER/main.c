#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "palletizing.h"
#include "pwm.h"
#include "key.h"
#include "iwdg.h"
#include "timer.h"
#include "adc.h"

u8 SUSPEND_STATUS = 0;  	//暂停状态

struct pall_tim
{
	u16 pall_x_arr;
	u16 pall_y_arr;
	u16 pall_z_arr;
	u16 pall_u_arr;	

	u16 pall_x_psc;
	u16 pall_y_psc;
	u16 pall_z_psc;
	u16 pall_u_psc;
	
}tim_set;	

/*

步进：3200
步距角： 360度/2000 = 0.18

转速：转速 = 脉冲频率 * 60 /((360/T)*x)
			T是步进电机步距角（0.1125°），x是驱动器细分倍数（16）
			脉冲频率= 时钟频率（72Mhz）/ ((arr+1)*(psc+1))
			
			Speed = 15r/min时，若确定arr为199，则psc为449	
			= s*60/(360/0.1125) = s*60 /2000 -> 
?/
控制接口：
1.速度调节 	 rad/min
2.坐标控制
3.角度控制

*/

//参数初始化
void pall_parameter_init(void)
{
	tim_set.pall_x_arr = 100-1;  				//84MHZ
	tim_set.pall_x_psc = 600-1;
	
	tim_set.pall_y_arr = 100-1;					//168MHZ	
	tim_set.pall_y_psc = 600*2-1;   		//840

	tim_set.pall_z_arr = 100-1;				
	tim_set.pall_z_psc = 90-1;
	
	tim_set.pall_u_arr = 100-1;				
	tim_set.pall_u_psc = 450*2-1;

	System_parameter_set.Talus_angle = 0.18;
	System_parameter_set.Talus_dis  = 0.0192;  //待测试 mm
	
	pall_temp_pos.set_clib = 200;
}	


void pall_device_init(void)
{
	pall_parameter_init();
	Photoelectric_IO_Init();   				//光电开关1
	electromagnetism_IO_Init();				//末端 LED BEEB 控制
	motor_dir();
	delay_ms(100);
	
	/* 步进电机转速 = 频率 * 60 /((360/T)*x) */	
	// 840 00000 /840 = 100000Hz 频率   T=1*arr/100000 -> f=100000/100=1000Hz   1ms 
	// w = 1000*60/(360/0.1125) = ? rad/min 一分钟30转	
	
	//Y轴初始化	
	TIM8_PWM_Init(tim_set.pall_y_arr,tim_set.pall_y_psc);    
	TIM8->CCR1=(u16)(tim_set.pall_y_arr/2);
	delay_ms(100);
		
	//X轴初始化 慢了
	TIM13_PWM_Init(tim_set.pall_x_arr,tim_set.pall_x_psc);    
	TIM13->CCR1=(u16)(tim_set.pall_x_arr/2);	
	delay_ms(100);
	//Z轴初始化
	TIM3_PWM_Init(tim_set.pall_z_arr,tim_set.pall_z_psc);	
	TIM3->CCR2=(u16)(tim_set.pall_z_arr/2);	
	//U轴初始化	
	delay_ms(100);
	TIM1_Init(tim_set.pall_u_arr,tim_set.pall_u_psc);	
	TIM1->CCR1=(u16)(tim_set.pall_u_arr/2);	
	
	delay_ms(10);
	Adc_Init();
	delay_ms(50);
	TIM10_Init(10000-1,168-1);  				//1us
}	

void PALL_Origin(u8 axis)
{
	if(axis == 1 && EXTI_X) 
	{
		pall_temp_pos.OriginX = 1;
		Photoelectric_IO.Pho_IO_X = 0;	
		X_Dir_Control(0);	
		TIM_Cmd(TIM13, ENABLE );
	}
	else if(axis == 2  && EXTI_Y)
	{
		pall_temp_pos.OriginY = 1;
		Photoelectric_IO.Pho_IO_Y = 0;
		Y_Dir_Control(0);
		TIM_Cmd(TIM8, ENABLE);	
	}	
	else if(axis == 3)
	{
		pall_temp_pos.OriginZ = 1;
		pall_temp_pos.pos_z = 0.0f;
		(get_motor_encoder(10)>0.0f)? Z_Dir_Control(0):Z_Dir_Control(1);   //回原点
		TIM_Cmd(TIM3, ENABLE);
			
	}	
	else if(axis == 4)
	{
		PALL_U_IO_Control(1);
	}		
	
}	

//各个轴回原点  
void Origin_Init(void)
{
	Photoelectric_IO_T_Init();  			//光电开关2
	delay_ms(50);
	PALL_Origin(3);
	
	PALL_U_IO_Control(0);
	delay_ms(10);
	
	PALL_Origin(1);
	delay_ms(100);
	PALL_Origin(2);
		
	while(1)
	{		
		//if(!LIMIT_U) pall_temp_pos.pulse_u = 50;
		
		if(!EXTI_X||Photoelectric_IO.Pho_IO_X)
		{
			TIM_Cmd( TIM13, DISABLE );	
			Photoelectric_IO.Pho_IO_X = 1;
			pall_temp_pos.OriginX = 0;	
		}
			
		if(!EXTI_Y||Photoelectric_IO.Pho_IO_Y)
		{
			TIM_Cmd(TIM8, DISABLE ); 
			Photoelectric_IO.Pho_IO_Y = 1;
			pall_temp_pos.OriginY = 0;					
		}
			
		if(!LIMIT_XX)
		{
			X_Dir_Control(1);	
			TIM_Cmd(TIM13, ENABLE );
		} 
		if(!LIMIT_YY)
		{
			Y_Dir_Control(1);	
			TIM_Cmd(TIM8, ENABLE );
		}  
		if(!LIMIT_UU)
		{
			TIM_Cmd(TIM1, DISABLE );
			GRAP_STOP = 0;
		}
		if(Photoelectric_IO.Pho_IO_X&&Photoelectric_IO.Pho_IO_Y)
		{
			//归零完成
			pall_cur_pos.cur_pos_x = 0.0;
			pall_cur_pos.cur_pos_y = 0.0;
			pall_cur_pos.cur_pos_z = 0.0;		  						
			TIM_Cmd(TIM1, DISABLE );
			TIM_Cmd(TIM8, DISABLE );
			TIM_Cmd( TIM13, DISABLE );					
			break;
		}	
	}	
	
}	


void send_data(u8 *data,u8 len)
{ 	
	u8 *p = data;
	while(len--)
	{
		while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
		USART1->DR = *p; 
		p++;
	}
}

//回复上位机消息
/*
	帧头    1字节
	确认码  2字节
	校验位  1字节
	帧尾    1字节
*/
void ack_send(void)
{
	u8 send[6]={0};
	u8 i = 0;
	int check = 0;
	send[0] = FRAME_HEADER;
	send[1] = recei_pall_data.cmd_type;
	send[2] = 0x00;
	for(i=0; i<3; i++)
	{
		check=send[i]^check; //异或，用于检测数据是否出错
	}
	send[3] = check;
	send[4] = FRAME_TAIL;
	send_data(send,5);

}

void error_ack_send(void)
{
	u8 send[6]={0};
	u8 i = 0;
	int check = 0;
	send[0] = FRAME_HEADER;
	send[1] = 0xff;
	send[2] = 0xff;
	for(i=0; i<3; i++)
	{
		check=send[i]^check; //异或，用于检测数据是否出错
	}
	send[3] = check;
	send[4] = FRAME_TAIL;
	send_data(send,5);
}	

u8 crc_check(u8 *addr,u8 len)
{
	int i;
	u8 crc=0;
	u8 *p=addr;
	for(i = 0; i<len; i++)
	{
		crc ^= *p++;
	}
	return crc;
}

void current_location_send(void)
{
	u8 i = 0;
	send_pall_data.send_data[0] = FRAME_HEADER ;
	send_pall_data.send_data[1] = 0x01;
	
	send_pall_data.x_data.x = pall_cur_pos.cur_pos_x ;
	send_pall_data.y_data.y = pall_cur_pos.cur_pos_y;
	send_pall_data.z_data.z = pall_cur_pos.cur_pos_z;
	send_pall_data.u_data.u = pall_cur_pos.cur_pos_u;
	
	for(i=0 ;i<4; i++)
	{
		send_pall_data.send_data[2+i] = send_pall_data.x_data.x_byte[i]; //X
	}
	
	for(i=0 ;i<4; i++)
	{
		send_pall_data.send_data[6+i] = send_pall_data.y_data.y_byte[i]; //Y
	}
	
	for(i=0 ;i<4; i++)
	{
		send_pall_data.send_data[10+i] = send_pall_data.z_data.z_byte[i]; //Z
	}

	send_pall_data.send_data[14] = DIR.Dir_U;   	  //夹爪	
	send_pall_data.send_data[15] = STOP_STATUS; 		//急停
	send_pall_data.send_data[16] = 0;  							//预留
	send_pall_data.send_data[17] = 0;	 							//预留	
	
	send_pall_data.send_data[18] = crc_check(send_pall_data.send_data,18);
	send_pall_data.send_data[19] = FRAME_TAIL;
	
	send_data(send_pall_data.send_data,20);
}
	
void X_Y_Limit(void)
{
	//X轴限位
	if(!LIMIT_XX && !DIR.Dir_X)
	{			   				
		TIM_Cmd(TIM13, DISABLE ); 		
	}
	else if(!LIMIT_X && DIR.Dir_X)
	{				   
		TIM_Cmd(TIM13, DISABLE ); 
	}
			
	//Y 轴限位			
	if(!LIMIT_YY && !DIR.Dir_Y)
	{			   
		TIM_Cmd(TIM8, DISABLE ); 							
	}
	else if(!LIMIT_Y && DIR.Dir_Y)
	{			   
		TIM_Cmd(TIM8, DISABLE ); 			
	}				
}	

int main(void)
{ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);  
	uart_init(115200);
	KEY_Init();																
	pall_device_init();
		
	TIM9_INIT(2000-1,16800-1);//200ms
	Origin_Init(); //回原点
	delay_ms(100);
	IWDG_Init(4,625);
	
	while(1) 
	{		
		
		if(ack_cmd) 							//发送确认消息
		{
			switch(recei_pall_data.cmd_type)
			{
				case 0: 	if(!SUSPEND_STATUS) 					 //Z Y Z 轴控制 
							{
								TIM_Cmd(TIM13, ENABLE ); 
								TIM_Cmd(TIM8,  ENABLE );	
								X_Y_Limit();
							}
							else
							{
								TIM_Cmd(TIM3,  DISABLE );
							}
							break;   																		
					
				case 1: 	
							current_location_send();break;	//获取码垛当前位置
				
				case 2: 	
							TIM_Cmd(TIM8, ENABLE);
							TIM_Cmd( TIM13, ENABLE );
							TIM_Cmd(TIM3,  ENABLE );									
							ack_send();
							break;    						//正常操作指令  开始
				
				case 3: 	
							TIM_Cmd(TIM8, DISABLE);
							TIM_Cmd( TIM13, DISABLE );
							TIM_Cmd(TIM3,  DISABLE );
							ack_send();
							break;    						//暂停
				case 4: 	
							Origin_Init(); 
							ack_send();
							break;    						//复位
				
				case 5: 	
							PALL_U_IO_Control(1); 
							ack_send();
							break;    						//夹
				case 6: 	
							PALL_U_IO_Control(0); 
							ack_send();
							break;    						//放									
				case 7: 	
							PALL_Origin(1); 
							ack_send();
							break;    						//X回原点
				case 8: 		
							PALL_Origin(2);
							ack_send();
							break;    						//Y回原点									
				default : 
							error_ack_send();   					 //其它错误指令
			
			}
			
			ack_cmd = 0;
			
		}
		
		if(update_pos)
		{
			current_location_send();
			update_pos = 0;
		}
		
		switch(KEY_Scan(0))
		{
			case START_PRES: 
								TIM_Cmd(TIM8, ENABLE);TIM_Cmd( TIM13, ENABLE );TIM_Cmd(TIM3,  ENABLE );
								RED_OFF;GREEN_ON; YELLOW_OFF;
								SUSPEND_STATUS = 0;
								break;			//启动
			case PAUSE_PRES: 
								TIM_Cmd(TIM8, DISABLE);TIM_Cmd( TIM13, DISABLE );TIM_Cmd(TIM3,  DISABLE );
								RED_OFF;GREEN_OFF;YELLOW_ON;
								SUSPEND_STATUS = 1;
								break;		//暂停
			default :
								break;			
		
		}

	}
}
