#include "palletizing.h"
#include "usart.h"
#include "pwm.h"
#include "delay.h"
#include "timer.h"

Pho_IO Photoelectric_IO;
dir_status DIR;	

//3个光电开关 外部中断 下降沿
/*

X0_A	PA0 		开关1  --->X轴正极限
X1_A	PA1 		开关2 
X2_A	PB15    开关3  --->原点

X3_A	PD9     开关4  --->Y轴正极限
X4_A	PD10  	开关5  
X5_A	PC4 		开关6  --->原点

X6_A  PD1     夹爪限位 1
X7_A  PD0     夹爪限位 2 

X10_A	 PC12 	急停 
X11_A	 PC10		启动  	 //复位
X12_A	 PA15		暂停

*/

//PC4 PB15 原点
void Photoelectric_IO_T_Init(void)
{
  GPIO_InitTypeDef 	 GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
		
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOF时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);	//PB15
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);		//PC4
		
	/* 配置EXTI_Line4,15 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4 | EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断15
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//外部中断4
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
}	
void Photoelectric_IO_Init(void)
{    
	
  GPIO_InitTypeDef 	 GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
		
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOF时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);//PA1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource15);//PD9 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);//PD10 
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource5);//PD0 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource6);//PD1 
	
	/* 配置EXTI_Line0,1,9,10 */
	//EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line1|EXTI_Line9|EXTI_Line10;
	EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line1|EXTI_Line3|EXTI_Line5|EXTI_Line6|EXTI_Line15;
	
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下降沿触发
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
  EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//外部中断1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//外部中断3
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;//外部中断5-6
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断15
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

//外部中断0
void EXTI0_IRQHandler(void)  
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
			delay_ms(5);
			if(!PAin(0))
			{
				 TIM_Cmd( TIM13, DISABLE );
			}
			
			EXTI_ClearITPendingBit(EXTI_Line0);			
			//printf("x_1....\r\n");		
	}	
}

//外部中断1
void EXTI1_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
			delay_ms(5);
			if(!PAin(1))
			{		
				TIM_Cmd( TIM13, DISABLE ); 
				Photoelectric_IO.Pho_IO_XX=1;
				if(pall_temp_pos.OriginX)
				{
					 X_Dir_Control(1);
					 TIM_Cmd( TIM13, ENABLE );	
				}
				//printf("x_2....\r\n");		
			}
			EXTI_ClearITPendingBit(EXTI_Line1);
	}
}  

//外部中断15
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
			delay_ms(5);
			if(!PBin(15))      //X轴原点
			{		
				 Photoelectric_IO.Pho_IO_X=1;
				 pall_cur_pos.cur_pos_x = 0.0;
				if(pall_temp_pos.OriginX)
				{
					TIM_Cmd( TIM13, DISABLE );	
				  pall_temp_pos.OriginX = 0;
					
				}	
/*				
				 //printf("xx....\r\n");
				 TIM_Cmd( TIM13, DISABLE );		
*/				
			}
			if(!PAin(15))
			{		
				TIM_Cmd(TIM8, DISABLE );
				//printf("y_1....\r\n");			
			}
			EXTI_ClearITPendingBit(EXTI_Line15);
	}
	
}
//外部中断5\6
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
			delay_ms(5);
			if(!PDin(5))
			{
				pall_temp_pos.pulse_u =250;		
				//TIM_Cmd( TIM1, DISABLE );				
			}	
			EXTI_ClearITPendingBit(EXTI_Line5);		
	}

	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
  {
			delay_ms(5);
			if(!PDin(6))
			{
				 pall_temp_pos.pulse_u = pall_temp_pos.set_clib;	
			}	
			EXTI_ClearITPendingBit(EXTI_Line6);		
	}	
	
}

//外部中断3
void EXTI3_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line3) != RESET)
	{
			delay_ms(5);
			if(!PBin(3))
			{		
					TIM_Cmd(TIM8, DISABLE );
				  Photoelectric_IO.Pho_IO_YY=1;				
				  if(pall_temp_pos.OriginY)
				 {
					  Y_Dir_Control(1);
					  TIM_Cmd( TIM8, ENABLE );	
				 }				
					//printf("y_2....\r\n");
			}
			EXTI_ClearITPendingBit(EXTI_Line3);			
	}
}


//外部中断4
void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
			delay_ms(5);
			if(!PCin(4))     //Y轴原点
			{		
					Photoelectric_IO.Pho_IO_Y=1;
					pall_cur_pos.cur_pos_y = 0.0;
				
				 if(pall_temp_pos.OriginY)
				 {
					 TIM_Cmd( TIM8, DISABLE );	
				   pall_temp_pos.OriginY = 0;
				 }	
			}
			EXTI_ClearITPendingBit(EXTI_Line4);			
	}
}


//电机方向控制 
//Y4_A	PD5 		  开漏输出    X轴方向控制
//Y5_A	PD6 		  开漏输出		 Y轴方向控制
//Y6_A  PD7				开漏输出		 Z轴方向控制
//Y7_A  PB3     	开漏输出		 U轴方向控制    
void motor_dir(void)
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD |RCC_AHB1Periph_GPIOB |RCC_AHB1Periph_GPIOA, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  //GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	//GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	MOTO_X_Reversal;		 	 	
	MOTO_Y_Reversal;	
	
}	

//X方向控制 
void X_Dir_Control(u8 dir)
{
	
	if(dir==1)
	{	
		DIR.Dir_X=1;
		MOTO_X_Forward;
		
	}
	else 	if(dir==0)
	{	
		DIR.Dir_X=0;
		MOTO_X_Reversal;
	}
				
}	

//Y方向控制 
void Y_Dir_Control(u8 dir)
{
	if(dir==1)
	{	
		DIR.Dir_Y=1;
		MOTO_Y_Forward;
	}
	else 	if(dir==0)
	{	
		DIR.Dir_Y=0;
		MOTO_Y_Reversal;
	}		
}	

//Z方向控制 
void Z_Dir_Control(u8 dir)
{
	if(dir==1)
	{	
		DIR.Dir_Z=1;
	  MOTO_Z_Reversal;
	}
	else 	if(dir==0)
	{	
		DIR.Dir_Z=0;
		MOTO_Z_Forward;
	}		
}	

/*

Y6_A       PD7 			Z轴启动/停止

Y13_A 		 PE0			三色灯_红色
Y14_A 		 PB7 			三色灯_黄色
Y15_A 		 PB6 			三色灯_绿色
Y16_A 		 PB5			三色灯_蜂鸣器
*/


//末端 LED BEEB 控制
void electromagnetism_IO_Init(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	
	GPIO_Init(GPIOE, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}	


void PALL_Z_IO_Control(u8 value)
{
	value?Grab_Up:Put_Down;
}	



