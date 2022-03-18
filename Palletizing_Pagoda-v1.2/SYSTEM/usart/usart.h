#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 

#define FRAME_HEADER 	0X7B 
#define FRAME_TAIL 		0X0A

typedef union X_DATA
{
		float x;
		uint8_t x_byte[4];
		
}X_DATA;

typedef union Y_DATA
{
		float y;
		uint8_t y_byte[4];
}Y_DATA;

typedef union Z_DATA
{
		float z;
		uint8_t z_byte[4];
}Z_DATA;

typedef union U_DATA
{
		float u;
		uint8_t u_byte[4];
}U_DATA;



typedef struct palletizing
{
		unsigned char send_data[20];
		unsigned char cmd_type;
		X_DATA x_data;
		Y_DATA y_data;
		Z_DATA z_data;
		U_DATA u_data;
	
		u8 stop;
		u8 grab_status;
		u8 run_status;

}palletizing;


void uart_init(u32 bound);
float my_abs(float value);
extern volatile  u8 ack_cmd;
extern palletizing recei_pall_data;
extern palletizing send_pall_data;
#endif


