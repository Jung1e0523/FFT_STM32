	#include <stdio.h>
	#include <math.h>
	#include "stdlib.h"
	#include "GaussianFilter.h"
	
	///////////////gaussian�˲�/////////////////
	
	void GaussianFilter(int r ,int sigma ,int *y)
		{
			float *kernel ;
			int x;
			int m = r / 2 ;
			float sum = 0;
			int output;
			
			if (r <= 0 || sigma ==0 )
				return;
			
			//�����˹ģ��
			
			for (x = 0 ; x <= r ; x++ )
			{
				kernel [x] = (1 / (sigma * sqrt(2 * 3.14)) * exp(-(x-m)^2 / (2 * (sigma^2))));
				sum += kernel [x] ;
			}
			
			for (x = 0 ; x <= r ; x++ )
			{
				kernel[x] /= sum ;
			}
			
			//��˹ģ��
			
			for (x = 0 ; x <= r ; x++)
			{
				y[x] *= kernel[x];
				output += y[x];
			}
		}
