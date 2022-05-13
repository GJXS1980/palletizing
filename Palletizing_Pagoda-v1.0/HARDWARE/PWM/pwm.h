#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"

void TIM13_PWM_Init(u32 arr,u32 psc);
void TIM8_PWM_Init(u32 arr,u32 psc);
void TIM3_PWM_Init(u32 arr,u32 psc);	

typedef struct System_parameter
{
	
		float Talus_angle; 		 	//步距角
		float Talus_dis;	 			//步行程	
		u16 TIM_frequency;		//系统频率
	
	
}System_parameter_t;


//目标角度
typedef struct angle
{
	float angle_x;
	float angle_y;
	float angle_z;
}angle_t;	

//目标位置
typedef struct tar_pos
{
	float tar_pos_x;
	float tar_pos_y;
	float tar_pos_z;
}tar_pos_t;

//临时脉冲
typedef struct pos
{
	float pos_x;
	float pos_y;
	float pos_z;
	u32 pulse_x;
	u32 pulse_y;
	u32 pulse_z;
	int pulse_u;	
	int set_clib;
	u8 OriginX;
	u8 OriginY;
	u8 OriginZ;
	u8 OriginU;	
	int arrive;
	
}pos_t;

//当前所在位置
typedef struct new_pos
{
	float  cur_pos_x;
	float  cur_pos_y;
	float  cur_pos_z;	
	float  cur_pos_u;	
}cur_pos_t;


extern tar_pos_t tar_pos;
extern pos_t pall_temp_pos;
extern cur_pos_t pall_cur_pos;
extern System_parameter_t System_parameter_set;
#endif
