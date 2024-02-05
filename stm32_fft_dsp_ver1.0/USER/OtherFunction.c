#include "stdio.h"
#include "lcd.h"
#include "sys.h"	 
#include "stdlib.h"
#include "OtherFunction.h"

//void printnum(u16 x,u16 y,u8 *p)
//{
//	u16 width=16, height=16;
//	u8 size=4;
//	LCD_ShowString(x,y, width,height,size,p);
//}

void drawspectrum (u16 xend, u16 yend)
{
	int a=0;
	
	for(a=0;a<=220-yend;a++)
	{
		LCD_Fast_DrawPoint(xend,220-a,YELLOW);
	}
	for(a=1;a<=yend-20;a++)
	{
		LCD_Fast_DrawPoint(xend,yend-a,BLACK);
	}
}

void drawspectrum_clear(u16 xend, u16 yend)
{
	int a=0;
	
	for(a=0;a<=yend;a++)
	{
		LCD_Fast_DrawPoint(xend,220-a,BLACK);
	}
}
