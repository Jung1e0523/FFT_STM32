#include "led.h"
#include <stdio.h>
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include "math.h"
#include "fft.h"
#include "stm32_dsp.h"
#include "key.h"
#include "dac.h"

#define hpass_100 0
#define hpass_200 1
#define hpass_400 2

#define lpass_600 0
#define lpass_800 1
#define lpass_1000 2
#define lpass_1500 3
#define lpass_6000 4

u32 adc_1[NPT];
u32 fre_data_1[NPT];
u32 temp_1[22];
u32 temp_2[22];

void Transfer_array(u32* lBufMagArray)
{
			//赋值操作
		temp_2[0] = (u32)lBufMagArray[2] *400/512;
		temp_2[1] = (u32)lBufMagArray[3] *400/512;
		temp_2[2] = (u32)lBufMagArray[4] *400/512;
		temp_2[3] = (u32)lBufMagArray[5] *400/512;
		temp_2[4] = (u32)lBufMagArray[6] *400/512;
		temp_2[5] = (u32)lBufMagArray[7] *400/512;
		temp_2[6] = (u32)lBufMagArray[8] *400/512;
		temp_2[7] = (u32)lBufMagArray[9] *400/512;
		temp_2[8] = (u32)lBufMagArray[10] *400/512;
		temp_2[9] = (u32)lBufMagArray[11] *400/512;
		temp_2[10] = (u32)lBufMagArray[12] *400/512;
		temp_2[11] = (u32)lBufMagArray[14] *400/512;
		temp_2[12] = (u32)lBufMagArray[16] *400/512;
		temp_2[13] = (u32)lBufMagArray[18] *400/512;
		temp_2[14] = (u32)lBufMagArray[20] *400/512;
		temp_2[15] = (u32)lBufMagArray[24] *400/512;
		temp_2[16] = (u32)lBufMagArray[30] *400/512;
		temp_2[17] = (u32)lBufMagArray[40] *400/512;
		temp_2[18] = (u32)lBufMagArray[60] *400/512;
		temp_2[19] = (u32)lBufMagArray[80] *400/512;
		temp_2[20] = (u32)lBufMagArray[100] *400/512;
		temp_2[21] = (u32)lBufMagArray[120] *400/512;
}

int main()
{
	u8 t=0, x=0, y=0;
	int i=0, n=0;
	u32 fft_max = 0;
	float adc_max = 0.0;
	u16 fre[22] = { 100,150,200,250,300,350,400,450,500,550,600,700,800,900,1000,1200,1500,2000,3000,4000,5000,6000 };
	delay_init();	    	//延时函数初始化	  
	uart_init(9600);	 	//串口初始化为9600
	LCD_Init();
	adc_Init();		  		//ADC初始化
	LED_Init();
	KEY_Init(); 
	POINT_COLOR=GREEN;	//设置字体为绿色
	BACK_COLOR=BLACK;  	//设置背景颜色为黑色
	
	LCD_ShowString(0,0,30,24,24,"low");	
	LCD_ShowString(100,0,40,24,24,"high");
	LCD_ShowxNum(36,0,100,4,24,0);
	LCD_ShowxNum(150,0,600,4,24,0);
	for(i=0;i<22;i++)
	{
		LCD_ShowxNum(34*i,440,fre[i],4,16,0);
	}
	
	while(1)
	{
		t=KEY_Scan(0);
		switch(t)
		{				 
			case KEY0_PRES:
			{
				if(x==hpass_400)x=hpass_100;
				else x++;
				switch(x)
				{
					case hpass_100:PAout(1)=0;PAout(2)=0;LCD_ShowxNum(40,0,100,4,24,0); break;
					case hpass_200:PAout(1)=0;PAout(2)=1;LCD_ShowxNum(40,0,200,4,24,0); break;
					case hpass_400:PAout(1)=1;PAout(2)=0;LCD_ShowxNum(40,0,400,4,24,0); break;
					default:PAout(1)=0;PAout(2)=0; break;
				}
				break;
			}
			case KEY1_PRES:
				if(y==lpass_6000)y=lpass_600;
				else y++;
				switch(y)
					{
						case lpass_600:PAout(3)=0;PAout(4)=0;PAout(5)=0;LCD_ShowxNum(150,0,600,4,24,0); break;
						case lpass_800:PAout(3)=0;PAout(4)=0;PAout(5)=1;LCD_ShowxNum(150,0,800,4,24,0); break;
						case lpass_1000:PAout(3)=0;PAout(4)=1;PAout(5)=0;LCD_ShowxNum(150,0,1000,4,24,0); break;
						case lpass_1500:PAout(3)=0;PAout(4)=1;PAout(5)=1;LCD_ShowxNum(150,0,1500,4,24,0); break;
						case lpass_6000:PAout(3)=1;PAout(4)=0;PAout(5)=0;LCD_ShowxNum(150,0,6000,4,24,0); break;
						default:PAout(3)=0;PAout(4)=0;PAout(5)=0; break;
					}
				break;
			default:
				delay_ms(10);
		}
		adc_max = 0;
		for(i=0;i<NPT;i++)
		{
			adc_1[i] = Get_Adc1(ADC_Channel_0);
			if(adc_max < adc_1[i])
				adc_max = adc_1[i];
			delay_us(75);
		}
		adc_max /= 24;
		if(adc_max >= 100)	adc_max = 100;
		LCD_ShowxNum(360,0,(u32)adc_max,4,24,0);//显示fft最大频率
		
		cr4_fft_256_stm32(fre_data_1 , adc_1 ,NPT);
		
		for(i=0;i<NPT;i++)
		{
			lBufOutArray[i] = fre_data_1[i]<<16;
		}
		
		GetPowerMag();
		
		Transfer_array(lBufMagArray);
		for(i=0;i<22;i++)
		{
			if(temp_2[i] > 400) temp_2[i] = 400;
		}
		fft_max = temp_2[0];
		for(i=0;i<22;i++)
		{
			if(fft_max < temp_2[i])
			{
				fft_max = temp_2[i];
				n = i;
			}
		}
		LCD_ShowxNum(720,0,fre[n],4,24,0);//显示fft最大频率
		
		for(i=1;i<22;i++)
		{
			if (temp_2[i]>temp_1[i]) LCD_Fill(34*i+10,440-temp_2[i],34*i+30,440,GREEN);	//本轮数据比上一帧数据大，在上一帧的基础上增加画点；
			else	LCD_Fill(34*i+10,40,34*i+30,440-temp_2[i],BLACK);											//本轮数据比上一帧数据小，在上一帧的基础上画黑点覆盖，以求达到降低条形的效果；
		}
	
		for(i=0;i<22;i++)	temp_1[i] = temp_2[i];
		
		delay_ms(50);
	}
}
