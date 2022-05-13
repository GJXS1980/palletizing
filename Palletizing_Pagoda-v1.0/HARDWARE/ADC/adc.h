#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 

void Adc_Init(void);
float get_motor_encoder(u8 times);
u32 get_z_distance(u8 times);
extern volatile u8 BAT_Check[2];

#endif 
