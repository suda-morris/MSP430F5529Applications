#include "DRV8834.h"
#include "MyF5529.h"

const u16 DRV8834_StepAngle[129] =
{
		0,0,3,6,8,11,14,17,20,23,25,28,31,34,37,39,42,45,48,51,53,56,59,62,65,68,70,73,76,79,
		82,84,87,90,93,96,98,101,104,107,110,113,115,118,121,124,127,129,132,135,138,141,143,
		146,149,152,155,158,160,163,166,169,172,174,177,180,183,186,188,191,194,197,200,203,
		205,208,211,214,217,219,222,225,228,231,233,236,239,242,245,248,250,253,256,259,262,
		264,267,270,273,276,278,281,284,287,290,293,295,298,301,304,307,309,312,315,318,321,
		323,326,329,332,335,338,340,343,346,349,352,354,357
};//Electronical Angle corresponding with the rising edge on STEP pin

unsigned char DRV8834_Caculate_Steps(unsigned int angle)
{
	u8 i = 0;
	for(i=1;i<129;i++)
	{
		if(  angle == DRV8834_StepAngle[i] )
		{
			return i;
		}
		else if(angle > DRV8834_StepAngle[i] && angle < DRV8834_StepAngle[i+1])
		{
			if( (angle-DRV8834_StepAngle[i]) < (DRV8834_StepAngle[i+1]-angle) )
			{
				return i;
			}
			else
			{
				return i+1;
			}
		}
	}
	return 0;				//没有得到计算结果，不让电机转动
}
void DRV8834_Init()
{
	GPIO_Set_Direction_Out(6,6,HIGH);		//对用STEP引脚
	GPIO_Set_Direction_Out(6,7,HIGH);		//对应DIR引脚
}
void DRV8834_Go(unsigned char steps)
{
	u8 i = 0;
	for(i=0;i<steps;i++)
	{
		GPIO_ResetBit(6,6);
		delayus(500);
		GPIO_SetBit(6,6);				//制造上升沿
		delayus(500);					//STEP频率不大于250KHz
	}
}

void DRV8834_Degree(unsigned int degree)
{
	unsigned int i;
	degree = (unsigned int)((double)(degree)*1.125);
	for(i=0;i<degree;i++)
	{
		DRV8834_Go(4);
	}
}


