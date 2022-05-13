#include "key.h"
#include "delay.h" 

/*
X10_A	 PC12 	��ͣ 
X11_A	 PC10		����   -->������λ
X12_A	 PA15		��ͣ 
*/

void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOC, ENABLE);
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
 
} 


u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;
	if(mode)key_up=1;  		  
	if(key_up&&(START==0||PAUSE==0))
	{
		delay_ms(150); 
		key_up=0;
		if(PAUSE==0)    
			return PAUSE_PRES;   
		else if(START==0) 
			return START_PRES;  		//����

	}else if(START==1&&PAUSE==1)key_up=1; 
	
 	return 0;
}




















