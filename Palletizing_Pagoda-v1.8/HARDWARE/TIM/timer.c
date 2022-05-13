#include "stm32f4xx.h"
#include "timer.h"
#include "adc.h"
#include "pwm.h"
#include "palletizing.h"
#include "usart.h"
#include "key.h"
#include "timer.h"
#include "iwdg.h"

volatile  u8 	 update_pos  = 0;
volatile  u8   RESET_COUNT = 0;
volatile  u8   GRAP_STOP   = 1;
static    u32  LED_COUNT   = 0;


//夹爪 ->U轴
void TIM1_Init(u32 arr,u32 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE); 
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
  TIM_TimeBaseInitStructure.TIM_Period = arr;
  TIM_TimeBaseInitStructure.TIM_Prescaler= psc;
  TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
  TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_TIM10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset ;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
	TIM_OC1Init(TIM1, &TIM_OCInitStructure); 

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  
	TIM_ARRPreloadConfig(TIM1,ENABLE); 

	TIM_CtrlPWMOutputs(TIM1,ENABLE); 						//高级定时器输出必须使能这句		
	TIM_Cmd(TIM1, DISABLE ); 										//不使能定时器
}

void PALL_U_IO_Control(u8 value)
{
		pall_temp_pos.pulse_u = 3000;
	  if(value == 1)
		{
			  DIR.Dir_U = 1;
				Grab_Up;
		}
		else if(value == 0)
		{
				//GRAP_STOP = 0;	
			  DIR.Dir_U = 0;
				Put_Down;		
		
		}	
		
		TIM_Cmd(TIM1, ENABLE ); 
}	

void TIM10_Init(u32 arr,u32 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_Period = arr; 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  
	TIM_TimeBaseInit(TIM10,&TIM_TimeBaseStructure);
		
	TIM_ClearITPendingBit(TIM10,TIM_IT_Update);			
	TIM_ITConfig(TIM10,TIM_IT_Update,ENABLE);			
	TIM_ARRPreloadConfig( TIM10,ENABLE );				
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	
	TIM_Cmd( TIM10, ENABLE );

}


// 中断服务程序
void TIM1_UP_TIM10_IRQHandler (void)
{
	if ( TIM_GetITStatus( TIM10, TIM_IT_Update ) != RESET ) 
	{		
			
			TIM_ClearITPendingBit(TIM10,TIM_IT_Update);	
			IWDG_Feed();
			//急停按下
			if(STOP_STATUS)
			{
				TIM_Cmd( TIM8, DISABLE);
				TIM_Cmd( TIM13, DISABLE );
		    TIM_Cmd( TIM3, DISABLE );
				RED_ON;GREEN_OFF; YELLOW_OFF;
				SUSPEND_STATUS = 0;
				return;
			}
			else //正在复位中
			{	
				if(pall_temp_pos.OriginX)  	 TIM_Cmd(TIM13, ENABLE);
     		if(	pall_temp_pos.OriginY )  TIM_Cmd(TIM8, ENABLE );
        if(	pall_temp_pos.OriginZ )  TIM_Cmd( TIM3, ENABLE ); 
				
				RED_OFF;
			}	
		
//Z轴控制
			pall_cur_pos.cur_pos_z = get_motor_encoder(5);
			if((pall_cur_pos.cur_pos_z >= tar_pos.tar_pos_z) && DIR.Dir_Z) 			//100 -> 200  下
			{
						TIM_Cmd( TIM3, DISABLE );	
            pall_temp_pos.OriginZ = 0;				
			}
			else if((pall_cur_pos.cur_pos_z <= tar_pos.tar_pos_z) && !DIR.Dir_Z )  //200 -> 100  上
			{
						TIM_Cmd( TIM3, DISABLE );	
            pall_temp_pos.OriginZ = 0;				
			}	

			
			if((pall_cur_pos.cur_pos_z <= -35.0f) && !DIR.Dir_Z)
					TIM_Cmd( TIM3, DISABLE );		
			else if((pall_cur_pos.cur_pos_z >= 850.0f ) && DIR.Dir_Z)
					TIM_Cmd( TIM3, DISABLE );					
			
			if(pall_cur_pos.cur_pos_z < 0)
					pall_cur_pos.cur_pos_z  = 0;

					
//X轴限位
			if(!LIMIT_XX && !DIR.Dir_X)
			{			   				
					TIM_Cmd(TIM13, DISABLE ); 
				
			}else if(!LIMIT_X && DIR.Dir_X)
			{				   
					TIM_Cmd(TIM13, DISABLE ); 
			}
			
//Y 轴限位			
			if(!LIMIT_YY && !DIR.Dir_Y)
			{			   
					TIM_Cmd(TIM8, DISABLE ); 						
					
			}else if(!LIMIT_Y && DIR.Dir_Y)
			{			   
					TIM_Cmd(TIM8, DISABLE ); 		
				
			}	
			
//U轴限位		
			if(!LIMIT_UU && !DIR.Dir_U && !GRAP_STOP)  //放
			{			   
				  GRAP_STOP = 0;	
					TIM_Cmd(TIM1, DISABLE ); 						
							
			}
			else if(!LIMIT_U && DIR.Dir_U && GRAP_STOP) //夹
			{			   
				  GRAP_STOP = 1;
					TIM_Cmd(TIM1, DISABLE ); 				
			}				
				
	 }
	
//U轴控制				
	if ( TIM_GetITStatus( TIM1, TIM_IT_Update ) != RESET ) 
	{		
			pall_temp_pos.pulse_u--;
		  if(pall_temp_pos.pulse_u<0)
			{ 
				TIM_Cmd(TIM1, DISABLE ); 	
				GRAP_STOP  = !GRAP_STOP;
		  }
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
	}		
	
}


void TIM9_INIT(u16 arr,u16 psc)
{
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);
	
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					//分频系数为8400-1	84M/8400 = 10000HZ		1/10000 = 0.0001S
	TIM_TimeBaseStructure.TIM_Period = arr;						  //预装载值为10-1  	10*0.0001S = 0.001S = 1ms
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE);					//溢出更新
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_BRK_TIM9_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM9在CCR2上的预装载寄存器
 
	TIM_ARRPreloadConfig(TIM9,ENABLE);//ARPE使能 
	
	TIM_Cmd(TIM9, ENABLE);  //使能TIM9 
}


float NEW_Z_POS = 0,ODL_Z_POS = 10.0;

void TIM1_BRK_TIM9_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM9,TIM_IT_Update) == SET)
	{	

		if((tar_pos.tar_pos_z > pall_cur_pos.cur_pos_z )  && DIR.Dir_Z)  //未到达目标点
		{
					NEW_Z_POS = get_z_distance(10);
			
					if((NEW_Z_POS-ODL_Z_POS) < 5.0f)
					{
	            TIM_Cmd( TIM3, DISABLE );
							DIR.Dir_Z = 0;
						  ODL_Z_POS = 10;
					}
					else
					{
						ODL_Z_POS = NEW_Z_POS;
					}
		}
						
		if((LED_COUNT%5 == 0) && !STOP_STATUS && !SUSPEND_STATUS) //200 ms 
		{
		   if(pall_temp_pos.OriginX || pall_temp_pos.OriginY ||pall_temp_pos.OriginZ){
					RED_OFF;GREEN_OFF; YELLOW_ON;}
			 else {
				 	RED_OFF;GREEN_ON; YELLOW_OFF;}
			 
			
		}
		else GREEN_OFF;
		
		
		if(!RESET_STATUS) 
			RESET_COUNT++;
		else
			RESET_COUNT = 0;
		
		if(RESET_COUNT>15)  //3s 
			NVIC_SystemReset();		
			
		
		LED_COUNT ++;
		update_pos = 1;
			
		TIM_ClearFlag(TIM9,TIM_IT_Update);
	}

}


