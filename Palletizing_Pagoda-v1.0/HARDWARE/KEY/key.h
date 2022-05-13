#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

/*
X10_A	 PC12 	��ͣ 
X11_A	 PC10		���� 
X12_A	 PA15		��ͣ 
*/

/*����ķ�ʽ��ͨ��ֱ�Ӳ����⺯����ʽ��ȡIO*/
#define START 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)	 
#define PAUSE 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) 

#define STOP_STATUS  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)
#define RESET_STATUS  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)

#define PAUSE_PRES 		1
#define START_PRES		2 

void KEY_Init(void);	//IO��ʼ��
u8 KEY_Scan(u8);  		//����ɨ�躯��	



#endif
