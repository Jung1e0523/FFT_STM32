 #include "adc.h"
 #include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK miniSTM32开发板
//ADC 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
	   
		   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   
void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 

void adc_Init(void)
{
	//定义结构体
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1|RCC_APB2Periph_ADC2|RCC_APB2Periph_ADC3, ENABLE);//使能所用功能时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//12MHZ
	
  RCC->APB2ENR|=1<<2;                      
  GPIOA->CRH&=0XFFFFF000;//PA0~2上拉输入
	
	 RCC->APB2ENR|=1<<9;      //使能ADC时钟
	 
	//ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult ;//ADC双重模式，同步规则转换 ADC1->CR1|=6<<16 双重模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//单通道转换 
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发ADC转换 ADC1->CR2|=7<<16
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//顺序进行规则转换的ADC 通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);//初始化ADC结构体
	ADC_Cmd(ADC1, ENABLE);

	//ADC_DeInit(ADC2);
	ADC_InitStructure.ADC_Mode = ADC_Mode_RegSimult ;//ADC双重模式，同步规则转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//单通道转换
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换h
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发ADC转换 ADC2->CR2|=7<<16
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//顺序进行规则转换的ADC 通道的数目
	ADC_Init(ADC2, &ADC_InitStructure);//初始化ADC结构体
	ADC_Cmd(ADC2, ENABLE);
	
	//ADC_DeInit(ADC3);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//ADC3独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//单通道转换
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//软件触发ADC转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;//顺序进行规则转换的ADC 通道的数目
	ADC_Init(ADC3, &ADC_InitStructure);//初始化ADC结构体
	ADC_Cmd(ADC3, ENABLE);
	
	//ADC校准
	ADC_ResetCalibration(ADC1);//重置指定的ADC 的校准寄存器
	ADC_ResetCalibration(ADC2);
	ADC_ResetCalibration(ADC3);
	while(ADC_GetResetCalibrationStatus(ADC1)&&ADC_GetResetCalibrationStatus(ADC2)&&ADC_GetResetCalibrationStatus(ADC3));//获取ADC 重置校准寄存器的状态
	ADC_StartCalibration(ADC1);//开始指定ADC 的校准状态
	ADC_StartCalibration(ADC2);
	ADC_StartCalibration(ADC3);
	while(ADC_GetCalibrationStatus(ADC1)&&ADC_GetCalibrationStatus(ADC2)&&ADC_GetCalibrationStatus(ADC3));//获取指定ADC 的校准程序
		
	//设置指定ADC 的规则组通道，设置它们的转化顺序和采样时间
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC2, ADC_Channel_1, 1,ADC_SampleTime_1Cycles5);
	ADC_RegularChannelConfig(ADC3, ADC_Channel_2, 1,ADC_SampleTime_1Cycles5);
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)&&ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC)&&ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC));//等待转换结果	  
	
	//使能指定的ADC 的软件转换启动功
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC2, ENABLE); 
	ADC_SoftwareStartConvCmd(ADC3, ENABLE);	
}

u16 Get_Adc1(u8 ch)
{
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc2(u8 ch)
{
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC2);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc3(u8 ch)
{
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC3);	//返回最近一次ADC1规则组的转换结果
}
