#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

extern void TIM9_INIT(u16 arr,u16 psc);
void TIM1_Init(u32 arr,u32 psc);
void TIM10_Init(u32 arr,u32 psc);
void PALL_U_IO_Control(u8 value);
extern volatile u8 	update_pos;
extern volatile u8   GRAP_STOP;
extern float NEW_Z_POS,ODL_Z_POS;
extern  u8 	SUSPEND_STATUS;

#endif
