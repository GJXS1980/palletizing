#include "sys.h"
#include "usart.h"
#include "palletizing.h"
#include "pwm.h"
#include "key.h"
#include "timer.h" 

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
	USART1->DR = (u8) ch;      
	return ch;
}
#endif
 
// 20字节 通信协议 
volatile  u8 ack_cmd = 0;
palletizing recei_pall_data;
palletizing send_pall_data;

/*
		帧头： 		  0X7A
		指令帧：     0,1
    数据帧： 	  X,Y,Z,U (16个字节)
		校验位：     check
		帧尾： 		  0X7B
*/

extern void ack_send(void);
void uart_init(u32 bound)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); 

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOA,&GPIO_InitStructure); 

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
	USART_Init(USART1, &USART_InitStructure); 

	 
	USART_ClearFlag(USART1, USART_FLAG_TC);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启串口空闲中断
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);	

	USART_Cmd(USART1, ENABLE); 

}

float my_abs(float value)
{			
	return value < 0.0f? -value:value;
}	
	
float Data_Analysis(u8 *buf,u8 axis)
{
	int i = 0;
	float data =  0.0f;
	
	if(axis == 1)          //X
	{
		for(i = 0; i<4;i++)
		{
				recei_pall_data.x_data.x_byte[i] = buf[2+i];
		}

		data = recei_pall_data.x_data.x;
	
	}
	else if(axis == 2)    //Y
  {
		for(i = 0; i<4;i++)
		{
				recei_pall_data.y_data.y_byte[i] = buf[6+i];
		}

		data = recei_pall_data.y_data.y;	
	
	
	}
	else if(axis == 3)    //Z
	{
		for(i = 0; i<4;i++)
		{
				recei_pall_data.z_data.z_byte[i] = buf[10+i];
		}

		data = recei_pall_data.z_data.z;	
	
	}
	else if(axis == 4)    //U
	{
		for(i = 0; i<4;i++)
		{
				recei_pall_data.u_data.u_byte[i] = buf[14+i];
		}

		data = recei_pall_data.u_data.u;	
	
	}

	return data;
}

//static float z_data = 0;
static 	u8 rxbuf[50]={0};

void USART1_IRQHandler(void)                	
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
	{
		u8 Usart_Receive;

		static u8 Count=0;
		int check=0,error=1,i;
		
		Usart_Receive = USART_ReceiveData(USART1);
    rxbuf[Count] = Usart_Receive;
			
    if(Usart_Receive == FRAME_HEADER||Count>0) Count++; else Count=0;			
		if (Count == 20)	
		{  					
				Count=0;
				if(rxbuf[19] == FRAME_TAIL ) 
				{
					
					for(i=0; i<18; i++)
					{
						check=rxbuf[i]^check;
					}
					if(check==rxbuf[18]) error=0; 
								
					if(error==0 && !STOP_STATUS)   //急停
				  {		
							ack_cmd = 1;
						  recei_pall_data.cmd_type = rxbuf[1];
							if(rxbuf[1] == 0x00)
							{
									pall_temp_pos.arrive = 2;	
									tar_pos.tar_pos_x = Data_Analysis(rxbuf,1);
									tar_pos.tar_pos_y = Data_Analysis(rxbuf,2);  
									tar_pos.tar_pos_z = Data_Analysis(rxbuf,3);
					
									if( tar_pos.tar_pos_z < 0 ) 	 tar_pos.tar_pos_z = 0; 
								
									(tar_pos.tar_pos_x - pall_cur_pos.cur_pos_x)>0.0f? X_Dir_Control(1):X_Dir_Control(0);	
                  //TIM_Cmd(TIM13, ENABLE ); 												
									(tar_pos.tar_pos_y - pall_cur_pos.cur_pos_y)>0.0f? Y_Dir_Control(1):Y_Dir_Control(0);
									//TIM_Cmd(TIM8, ENABLE );									
		
									pall_temp_pos.pos_z = tar_pos.tar_pos_z - pall_cur_pos.cur_pos_z;
									if( my_abs(pall_temp_pos.pos_z )<10 ) return;
								
									pall_temp_pos.pos_z>0.0f? Z_Dir_Control(1):Z_Dir_Control(0);	
									if(pall_temp_pos.pos_z>450) pall_temp_pos.pos_z = 450;
								  //TIM9->CNT = 0;
								  ODL_Z_POS = 0;
									TIM_Cmd(TIM3,  ENABLE );								
					
							}else if(rxbuf[1] == 0x01)
							{
								pall_temp_pos.set_clib = Data_Analysis(rxbuf,1);
							}
					 }
					 else recei_pall_data.cmd_type = 0xff;
					
			  }
		 }

		//清空标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}	
	
} 


	

