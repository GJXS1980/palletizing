#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 

/*
X10_A	 PC12 	急停 
X11_A	 PC10		启动 
X12_A	 PA15		暂停 
*/

/*下面的方式是通过直接操作库函数方式读取IO*/
#define START 		GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_10)	 
#define PAUSE 		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15) 

#define STOP_STATUS  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_12)
#define RESET_STATUS  GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)

#define PAUSE_PRES 		1
#define START_PRES		2 

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	



#endif
