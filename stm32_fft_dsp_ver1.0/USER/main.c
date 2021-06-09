#include "led.h"
#include <stdio.h>
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "adc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "math.h"
#include "fft.h"
#include "stm32_dsp.h"

#define PI 3.14

u32 adc_1[NPT+1];
u32 adc_2[NPT+1];
u32 adc_3[NPT+1];
u32 output_1[NPT];
u32 output_2[NPT];
u32 output_3[NPT];
u32 fre_data_1[NPT];
u32 fre_data_2[NPT];
u32 fre_data_3[NPT];
u32 temp_1[NPT];
u32 temp_2[NPT];

	int main(void)
	{
		int i=0 , j=0 , t=0 ;
		u32 fft_max = 0;
		float kernel[3] = {0.274069,0.451863,0.274069};

		
		delay_init();	    	//延时函数初始化	  
		uart_init(9600);	 	//串口初始化为9600
		LED_Init();		  		//初始化与LED连接的硬件接口
		LCD_Init();
		adc_Init();		  		//ADC初始化
		LCD_Clear(BLACK);		//设置背景为黑色
		LCD_Display_Dir(1);	//横屏
		POINT_COLOR=GREEN;	//设置字体为黄色
		BACK_COLOR=BLACK;  	//设置背景颜色为黑色
		
		
					
		while(1)
		{
			for(i=0;i<NPT+1;i++)
		{
			adc_1[i] = Get_Adc1(ADC_Channel_0);
			adc_2[i] = Get_Adc2(ADC_Channel_1);
			adc_3[i] = Get_Adc3(ADC_Channel_2);//adc输入
		}

		for(i=1;i<NPT;i++)
		{
			output_1[i] = (float)adc_1[i-1]* kernel[0] + (float)adc_1[i]* kernel[1] + (float)adc_1[i+1]* kernel[2];
			output_2[i] = (float)adc_2[i-1]* kernel[0] + (float)adc_2[i]* kernel[1] + (float)adc_2[i+1]* kernel[2];
			output_3[i] = (float)adc_3[i-1]* kernel[0] + (float)adc_3[i]* kernel[1] + (float)adc_3[i+1]* kernel[2];//高斯滤波
		}
		
			for(i=0;i<NPT;i++)
			{
				adc_1[i] = adc_1[i+1];
				adc_2[i] = adc_2[i+1];
				adc_3[i] = adc_3[i+1];
			}
		
			for(i=0;i<NPT-1;i++)
			{
				output_1[i] = output_1[i+1];
				output_2[i] = output_2[i+1];
				output_3[i] = output_3[i+1];
			}
		
			adc_1[256] = Get_Adc1(ADC_Channel_0);
			adc_2[256] = Get_Adc2(ADC_Channel_1);
			adc_3[256] = Get_Adc3(ADC_Channel_2);//adc输入
			
			output_1[255] = (float)adc_1[254]* kernel[0] + (float)adc_1[255]* kernel[1] + (float)adc_1[256]* kernel[2];
			output_2[255] = (float)adc_2[254]* kernel[0] + (float)adc_2[255]* kernel[1] + (float)adc_2[256]* kernel[2];
			output_3[255] = (float)adc_3[254]* kernel[0] + (float)adc_3[255]* kernel[1] + (float)adc_3[256]* kernel[2];//高斯滤波
			
//			for(i=0;i<NPT;i++)
//			{
//				LCD_Fast_DrawPoint(i,80-output_1[i]*60/4096,YELLOW);
//				LCD_Fast_DrawPoint(i,150-output_2[i]*60/4096,YELLOW);
//				LCD_Fast_DrawPoint(i,220-output_3[i]*60/4096,YELLOW);
//			}
			
			LCD_ShowxNum(256,20,output_1[255],4,16,0);//显示通道1滤波后adc值
			
			cr4_fft_256_stm32(fre_data_1 , (u32*)output_1 ,NPT);
			cr4_fft_256_stm32(fre_data_2 , (u32*)output_2 ,NPT);
			cr4_fft_256_stm32(fre_data_3 , (u32*)output_3 ,NPT);
			
			for(i=0;i<NPT;i++)
			{
				lBufOutArray[i] = ( fre_data_1[i]+fre_data_2[i]+fre_data_3[i] )<<16;
			}
			
			GetPowerMag();
			
			fft_max=lBufMagArray[0];
			
			for(i=1;i<NPT;i++)
			{
				if (lBufMagArray[i]>lBufMagArray[i-1])
				fft_max = lBufMagArray[i];
				else i = i;
			}
			
			LCD_ShowxNum(256,40,fft_max,4,16,0);//显示FFT后谐波幅值

			
			for(i=0;i<NPT;i++)
			{
				temp_2[i] = (u32)lBufMagArray[i]*100/500;
				if (temp_2[i] > 200)
					temp_2[i] = 200;
			}
			
			for(i=1;i<40;i++)
			{
				if (temp_2[i]>temp_1[i])
				{
					for(j=temp_1[i];j<temp_2[i];j++)
					LCD_Fast_DrawPoint(6*i,220-j,GREEN);
				}
				else if (temp_2[i] < temp_1[i])
				{
					for(j=temp_2[i];j<temp_1[i];j++)
					LCD_Fast_DrawPoint(6*i,220-j,BLACK);
				}
				else j = j;
			}
			
			for(i=0;i<256;i++)
			temp_1[i] = temp_2[i];
						
			t++;
			
			LCD_ShowxNum(256,0,t,4,16,0);//显示循环次数
			
			delay_ms(75);
		}
	}	
