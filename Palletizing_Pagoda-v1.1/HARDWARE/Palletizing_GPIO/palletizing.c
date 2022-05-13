#include "palletizing.h"
#include "usart.h"
#include "pwm.h"
#include "delay.h"
#include "timer.h"

Pho_IO Photoelectric_IO;
dir_status DIR;	

//3����翪�� �ⲿ�ж� �½���
/*

X0_A	PA0 		����1  --->X��������
X1_A	PA1 		����2 
X2_A	PB15    ����3  --->ԭ��

X3_A	PD9     ����4  --->Y��������
X4_A	PD10  	����5  
X5_A	PC4 		����6  --->ԭ��

X6_A  PD1     ��צ��λ 1
X7_A  PD0     ��צ��λ 2 

X10_A	 PC12 	��ͣ 
X11_A	 PC10		����  	 //��λ
X12_A	 PA15		��ͣ

*/

//PC4 PB15 ԭ��
void Photoelectric_IO_T_Init(void)
{
  	GPIO_InitTypeDef 	GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
		
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOFʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);	//PB15
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource4);		//PC4
		
	/* ����EXTI_Line4,15 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line4 |EXTI_Line15;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//�ⲿ�ж�15
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;//�ⲿ�ж�4
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
		
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOFʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1|GPIO_Pin_9|GPIO_Pin_10 ;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);//PA0 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource1);//PA1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource9);//PD9 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource10);//PD10 
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource0);//PD0 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);//PD1 
	
	/* ����EXTI_Line0,1,9,10 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0|EXTI_Line1|EXTI_Line9|EXTI_Line10;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //�½��ش���
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//�ⲿ�ж�0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//�ⲿ�ж�1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//�ⲿ�ж�10
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

void EXTI0_IRQHandler(void)  
{

	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
			delay_ms(5);
			if(!PAin(0))
			{

				 TIM_Cmd( TIM13, DISABLE );
			}
			
			if(!PDin(0))
			{
				pall_temp_pos.pulse_u =250;		
				//TIM_Cmd( TIM1, DISABLE );				
			}	
			
			EXTI_ClearITPendingBit(EXTI_Line0);			
			//printf("x_1....\r\n");		
	}	

}

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
			
			if(!PDin(1))
			{
				 pall_temp_pos.pulse_u = pall_temp_pos.set_clib;	
			}	
			
			EXTI_ClearITPendingBit(EXTI_Line1);

	}
}  

void EXTI15_10_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)
  {
			delay_ms(5);
			if(!PBin(15))      //X��ԭ��
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
			EXTI_ClearITPendingBit(EXTI_Line15);

	}
	
	if(EXTI_GetITStatus(EXTI_Line10) != RESET)
  {
			delay_ms(5);
			if(!PDin(10))
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
			EXTI_ClearITPendingBit(EXTI_Line10);			
	}

}

void EXTI9_5_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)
  {
			delay_ms(5);
			if(!PDin(9))
			{		
				TIM_Cmd(TIM8, DISABLE );
				//printf("y_1....\r\n");			
			}
				EXTI_ClearITPendingBit(EXTI_Line9);		
	}

}

void EXTI4_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
			delay_ms(5);
			if(!PCin(4))     //Y��ԭ��
			{		
					Photoelectric_IO.Pho_IO_Y=1;
					pall_cur_pos.cur_pos_y = 0.0;
				
				 if(pall_temp_pos.OriginY)
				 {
					 TIM_Cmd( TIM8, DISABLE );	
				   pall_temp_pos.OriginY = 0;
				 }	
				
				
/*
					TIM_Cmd(TIM8, DISABLE ); 
					//printf("yy....\r\n");
*/
				
			}
			EXTI_ClearITPendingBit(EXTI_Line4);			
	}
}


//���������� 
//Y4_A	PD5 		  ��©���    X�᷽�����
//Y5_A	PD6 		  ��©���		 Y�᷽�����
//Y6_A  PD7				��©���		 Z�᷽�����
//Y7_A  PB3     	��©���		 U�᷽�����    
void motor_dir(void)
{
	
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD |RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 |GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
		
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	MOTO_X_Reversal;		 	 	
	MOTO_Y_Reversal;	
	
}	

//X������� 
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

//Y������� 
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

//Z������� 
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

Y6_A       PD7 			Z������/ֹͣ

Y13_A 		 PE0			��ɫ��_��ɫ
Y14_A 		 PB7 			��ɫ��_��ɫ
Y15_A 		 PB6 			��ɫ��_��ɫ
Y16_A 		 PB5			��ɫ��_������
*/


//ĩ�� LED BEEB ����
void electromagnetism_IO_Init(void)
{

  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7;
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



