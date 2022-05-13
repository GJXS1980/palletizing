#ifndef __Palletizing_GPIO_H
#define __Palletizing_GPIO_H
#include "sys.h"

//3����翪�� �ⲿ�ж� �½���
/**************************************

X0_A	PA0 		����1  --->X��������
X1_A	PA1 		����2 
X2_A	PB15    ����3  --->ԭ��

X3_A	PD9     ����4  --->Y��������
X4_A	PD10  	����5  
X5_A	PC4 		����6  --->ԭ��


X10_A	 PC12 	��ͣ 
X11_A	 PC10		����  	 //��λ
X12_A	 PA15		��ͣ

Y6_A       PD7 			Z������/ֹͣ

Y13_A 		 PE0			��ɫ��_��ɫ
Y14_A 		 PB7 			��ɫ��_��ɫ
Y15_A 		 PB6 			��ɫ��_��ɫ
Y16_A 		 PB5			��ɫ��_������
***********************************/


extern void Photoelectric_IO_Init(void);
extern void electromagnetism_IO_Init(void);
extern void motor_dir(void);
extern void X_Dir_Control(u8 dir);
extern void Y_Dir_Control(u8 dir);
extern void Z_Dir_Control(u8 dir);

extern void PALL_Z_IO_Control(u8 value);

#define MOTO_X_Reversal  	GPIO_ResetBits(GPIOB,GPIO_Pin_7)  
#define MOTO_X_Forward 	GPIO_SetBits(GPIOB,GPIO_Pin_7)

#define MOTO_Y_Reversal  	GPIO_ResetBits(GPIOB,GPIO_Pin_8)  
#define MOTO_Y_Forward 	GPIO_SetBits(GPIOB,GPIO_Pin_8)

#define MOTO_Z_Reversal  	GPIO_ResetBits(GPIOD,GPIO_Pin_7)  
#define MOTO_Z_Forward 	GPIO_SetBits(GPIOD,GPIO_Pin_7)

#define Grab_Up 	GPIO_SetBits(GPIOD,GPIO_Pin_4)  
#define Put_Down 	GPIO_ResetBits(GPIOD,GPIO_Pin_4)  


#define LIMIT_X 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define LIMIT_XX 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)
#define LIMIT_Y 	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_15)
#define LIMIT_YY 	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define LIMIT_U 	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_6)   //X06
#define LIMIT_UU 	GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_5)   //X07

//ԭ��
#define EXTI_X 	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_15)
#define EXTI_Y 	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_4)
#define EXTI_Z 	GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)

//ָʾ��
#define RED_ON  			GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define RED_OFF  			GPIO_ResetBits(GPIOE,GPIO_Pin_0)
#define YELLOW_ON  		GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define YELLOW_OFF  	GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define GREEN_ON  		GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define GREEN_OFF  		GPIO_ResetBits(GPIOB,GPIO_Pin_6)

#define BEEN_ON  			GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define BEEN_OFF  		GPIO_ResetBits(GPIOB,GPIO_Pin_5)

#define GRAB_STATUS  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)

typedef struct Pho_IO_T
{
	u8 Pho_IO_X;
	u8 Pho_IO_XX;
	u8 Pho_IO_Y;
	u8 Pho_IO_YY;	
	u8 Pho_IO_Z;
	u8 Pho_IO_ZZ;	

}Pho_IO;	

typedef struct dir_status_t
{
	u8 Dir_X;
	u8 Dir_Y;
	u8 Dir_Z;
	u8 Dir_U;

}dir_status;	


extern Pho_IO Photoelectric_IO;
extern dir_status DIR;
extern void Photoelectric_IO_T_Init(void);

#endif
