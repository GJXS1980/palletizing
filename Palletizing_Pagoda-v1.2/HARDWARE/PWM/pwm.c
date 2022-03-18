#include "pwm.h"
#include "palletizing.h"
#include "usart.h"

tar_pos_t tar_pos;
pos_t pall_temp_pos; 
cur_pos_t pall_cur_pos;
System_parameter_t System_parameter_set;

//共阳接法 
 
//X轴
//Y0_A  PA6  TIM13_CH1  84MHz 
void TIM13_PWM_Init(u32 arr,u32 psc)
{
	
		GPIO_InitTypeDef GPIO_InitStructure;
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
	
	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM13,ENABLE);  	    
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 		
		
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM13); 
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        
		GPIO_Init(GPIOA,&GPIO_InitStructure);              
			
		TIM_TimeBaseStructure.TIM_Prescaler=psc;  
		TIM_TimeBaseStructure.TIM_Period=arr;  
		TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
		TIM_TimeBaseInit(TIM13,&TIM_TimeBaseStructure);
		
		TIM_ClearFlag(TIM13,TIM_FLAG_Update);
		TIM_ITConfig(TIM13,TIM_IT_Update,ENABLE);
		
		NVIC_InitStructure.NVIC_IRQChannel=TIM8_UP_TIM13_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
		NVIC_Init(&NVIC_InitStructure);
		
		TIM_OCInitStructure.TIM_OCMode =TIM_OCMode_PWM1;// TIM_OCMode_PWM1;   //跳变前的电平为高电平
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;//TIM_OCPolarity_Low;   //跳变时的电平为低电平   负脉冲
		TIM_OC1Init(TIM13, &TIM_OCInitStructure); 


		TIM_OC1PreloadConfig(TIM13, TIM_OCPreload_Enable);  
		TIM_ARRPreloadConfig(TIM13,ENABLE); 
		
		TIM_Cmd(TIM13, DISABLE); 			

}	 

//Y轴
//Y1_A  PC6  TIM8_CH1 168MHz 
void TIM8_PWM_Init(u32 arr,u32 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE); 
	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
  TIM_TimeBaseInitStructure.TIM_Period = arr;
  TIM_TimeBaseInitStructure.TIM_Prescaler= psc;
  TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up;
  TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  TIM_TimeBaseInit(TIM8,&TIM_TimeBaseInitStructure);
	
	TIM_ClearFlag(TIM8, TIM_FLAG_Update);
  TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel=TIM8_UP_TIM13_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
  NVIC_Init(&NVIC_InitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
//TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset ;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
	TIM_OC1Init(TIM8, &TIM_OCInitStructure); 

	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);  
	TIM_ARRPreloadConfig(TIM8,ENABLE); 

	TIM_CtrlPWMOutputs(TIM8,ENABLE); 						//高级定时器输出必须使能这句		
	TIM_Cmd(TIM8, DISABLE ); 										//不使能定时器

}	


//Z轴 -> 当直流电机使用
//Y2_A  PC7  TIM3_CH2   84MHz 
 void TIM3_PWM_Init(u32 arr,u32 psc)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 使能TIM4时钟
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE );
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );
	
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource7,GPIO_AF_TIM3 );

	// 配置GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOC, &GPIO_InitStructure );

  // 定时器设置
  TIM_TimeBaseInitStructure.TIM_Period = arr; // 重装载值
  TIM_TimeBaseInitStructure.TIM_Prescaler = psc; // 预分频
  TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
  TIM_TimeBaseInit( TIM3, &TIM_TimeBaseInitStructure );
	
/*	 -> 当直流电机使用
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
  TIM_ClearFlag( TIM3, TIM_FLAG_Update );

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init( &NVIC_InitStructure );

*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OC2Init(TIM3, &TIM_OCInitStructure); 

	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  
	TIM_ARRPreloadConfig(TIM3,ENABLE); 
	
	TIM_Cmd( TIM3, DISABLE ); 

}	
 
void TIM8_UP_TIM13_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM13,TIM_IT_Update) == SET)
	{		
		
			if(!PBin(15))      //X轴原点
			{		
				 pall_cur_pos.cur_pos_x = 0.0;		
			}
							
			if(!pall_temp_pos.OriginX)
			{	  
				  if(DIR.Dir_X)
						pall_cur_pos.cur_pos_x += System_parameter_set.Talus_dis;
					else
         		pall_cur_pos.cur_pos_x -= System_parameter_set.Talus_dis;		
				
				  if((pall_cur_pos.cur_pos_x >= tar_pos.tar_pos_x && DIR.Dir_X) || (pall_cur_pos.cur_pos_x <= tar_pos.tar_pos_x && !DIR.Dir_X))
					{	
							TIM_Cmd( TIM13, DISABLE );
						  pall_temp_pos.arrive -=1;
					}
			}

		TIM_ClearFlag(TIM13,TIM_IT_Update);
	}

	if(TIM_GetITStatus(TIM8,TIM_IT_Update) == SET)
	{		
		
			if(!PCin(4))     //Y轴原点
			{		
					pall_cur_pos.cur_pos_y = 0.0;
      }	
			
			if(!pall_temp_pos.OriginY)
			{
				if(DIR.Dir_Y)
				  	pall_cur_pos.cur_pos_y += System_parameter_set.Talus_dis;
				else
         		pall_cur_pos.cur_pos_y -= System_parameter_set.Talus_dis;		
					
				if((pall_cur_pos.cur_pos_y >= tar_pos.tar_pos_y && DIR.Dir_Y) || (pall_cur_pos.cur_pos_y <=tar_pos.tar_pos_y && !DIR.Dir_Y))
				{				
					 TIM_Cmd(TIM8, DISABLE ); 
					 pall_temp_pos.arrive -=1;
		
				}
			}
		
		TIM_ClearFlag(TIM8,TIM_IT_Update);
	}
	
}


void TIM3_IRQHandler(void)
{
	float position=0.0f;
	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{			
			if(!pall_temp_pos.OriginZ)
			{
					pall_temp_pos.pulse_z++;
				
				  if(DIR.Dir_Z)
							pall_cur_pos.cur_pos_z += System_parameter_set.Talus_dis;
					else
							pall_cur_pos.cur_pos_z -= System_parameter_set.Talus_dis;		
					
					position = pall_temp_pos.pulse_z*System_parameter_set.Talus_dis;
				
					if(position >= pall_temp_pos.pos_z  )
					{	
//						printf("Z 脉冲数：%d 实际位置：%f 当前位置：%f \r\n ",	pall_temp_pos.pulse_z,pall_temp_pos.pos_z,pall_cur_pos.cur_pos_z);
						TIM_Cmd( TIM3, DISABLE ); 
						pall_temp_pos.pulse_z = 0;
						pall_temp_pos.arrive -=1;
					}						
			}	

		TIM_ClearFlag(TIM3,TIM_IT_Update);
	}

}



