###	岸吊/门吊
v1.3版本为最新版本

### v1.1 --> v1.8
```cpp
/*

Y0_A  PA6  TIM13_CH1  84MHz X轴
Y1_A  PC6  TIM8_CH1 168MHz Y轴
Y2_A  PC7  TIM3_CH2   84MHz Z轴

X0_A	PA0 			开关1  --->X轴正极限
X1_A	PA1 			开关2 
X2_A	PB15    		开关3  --->原点

X3_A	PD9 --> PA15    开关4  --->Y轴正极限
X4_A	PD10 -->PB3 	开关5  
X5_A	PC4 			开关6  --->原点

X6_A  PD1 --> PD6       夹爪限位 1
X7_A  PD0 --> PD5       夹爪限位 2 


X10_A	 PC12 --> PD10 	急停 
X11_A	 PC10 --> PC8	启动   -->长按复位
X12_A	 PA15	--> PC9	暂停 


//电机方向控制 
Y4_A	PD5 -->PB7		开漏输出    X轴方向控制
Y5_A	PD6 -->PB8		开漏输出	Y轴方向控制
Y6_A  PD7				开漏输出	Z轴方向控制
Y7_A  PB3 -->PD4    	开漏输出	U轴方向控制    


Y6_A        PD7  		Z轴启动/停止

Y13_A       PE0		    三色灯_红色
Y14_A 		PB7 -->PB9 	三色灯_黄色
Y15_A 		PB6 		三色灯_绿色
Y16_A 		PB5 		三色灯_蜂鸣器
*/

```







