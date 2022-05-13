#include "adc.h"
#include "delay.h"		 
#include "usart.h"

void  Adc_Init(void)
{    
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//读取电量 读取电池状态
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//一个通道不准 原因不明
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);	  //ADC1复位			
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);	//复位结束
		
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b; // 4096
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_Cmd(ADC1, ENABLE);//开启AD转换器	

	
}				  

u16 Get_Adc(u8 ch)   
{
	  	
	 //设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度			    
  
	ADC_SoftwareStartConv(ADC1);		//使能指定的ADC1的软件转换启动功能	
	 	
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);  //等待ADC1转换完成
			
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);											//清空标志位

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}



float get_motor_encoder(u8 times)
{
	u8 t;	
	u32 temp_val=0; 
	
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(7);
	}
	temp_val=temp_val/times;
	//printf("temp_val:%d\r\n",temp_val);
	
	return temp_val*0.400f -100.0f;
	
}	

u32 get_z_distance(u8 times)
{
	u8 t;	
	u32 temp_val=0; 
	
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(7);
	}
	temp_val=temp_val/times;
//	printf("temp_val:%d\r\n",temp_val);
	
	return temp_val;
}	


//u16 Get_Adc_Average(u8 ch,u8 times)
//{
//	u32 temp_val=0; u8 t;
//	float	BAT_ADC[2] = {0.0f};
//	
//	for(t=0;t<times;t++)
//	{
//		temp_val+=Get_Adc(ch);
//	}
//	temp_val=temp_val/times;
//	
//	BAT_ADC[0]=temp_val*3.315f/4096;
//	

//	 
//	return BAT_ADC[0]*100;
//} 


/*
5.65v -- 	1718  304.07   最下面
4.79  -- 	1458  304.38
3.77  --  1147  304.2
          265
*/



//下限位   1650   1600 	730mm      -->    570 
//                                 ------------ = 0.456mm
//上限位   265    350 	160mm      -->    1250    


//岸吊

//2.8   865    307                   
//4.45  1368                
//5.7   1750 

/*

662    1532      324 
338    856       676    0.4793

*/

