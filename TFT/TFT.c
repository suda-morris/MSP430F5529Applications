#include <stdlib.h>
#include <math.h>
#include "TFT.h"
#include "char.h"
#include "chinese.h"

// Macros
#ifndef abs
#define abs(n)     (((n) < 0) ? -(n) : (n))
#endif

void TFT_Write_Color(unsigned int c)
{
	unsigned char vh,vl;
	vh = (unsigned char)(c>>8);
	vl = (unsigned char)c;
	TFT_Write_Data(vh);
	TFT_Write_Data(vl);
}

/************************************
清屏
入口参数： bg_color是背景颜色。
出口参数: 无
说明：使用背景颜色清除TFT模块屏幕的全部显示内容。
*************************************/
void TFT_CLRScreen(unsigned int bg_color)
{
	unsigned char i;
	unsigned int j;
	TFT_SetWindow(0,0,TFT_SIZE_X-1,TFT_SIZE_Y-1);
	for (i=0;i<TFT_SIZE_X;i++)
	{
		for(j=0;j<TFT_SIZE_Y;j++)
		{
		  	TFT_Write_Color(bg_color);
		}
	}

}

/*********************************************
画实心矩形
入口参数： (sx,sy)左上角顶点坐标, (ex,ey)右下角顶点坐标, color颜色
出口参数: 无
说明：在指定位置上画出实心矩形。
**********************************************/
void TFT_Box(unsigned char sx,unsigned int sy,unsigned char ex,unsigned int ey,unsigned int color)
{ 
	unsigned int temp;
	TFT_SetWindow(sx,sy,ex,ey); 
   	sx=ex-sx+1;
	sy=ey-sy+1;
	while(sx--)
	{
	 	temp=sy;
		while(temp--)
	 	{	
			TFT_Write_Color(color);
		}
	}
}

/*********************************************
画点
入口参数： (x，y)是点的坐标，color 是点的颜色。
出口参数: 无
说明：用指定的颜色在指定的坐标位置上画出一个点。
**********************************************/
void TFT_Point(unsigned char x, unsigned int y,unsigned int c)
{  
	TFT_SetXY(x,y);
	TFT_Write_Color(c);
}

/******************************************************************** 
画直线（可以画任意方向直线，包括横线、竖线、斜线）。
入口参数： (x1,y1)起点, (x2,y2)终点, color颜色。
出口参数: 无
说明：用指定的颜色在指定的两点间画出一条直线。
***********************************************************************/
void TFT_Line(unsigned char x1,unsigned int y1,unsigned char x2,unsigned int y2,unsigned int color)
{  
    unsigned int t;
    signed int xerr=0;
    signed int yerr=0;
    signed int delta_x;
    signed int delta_y,distance;
    signed char incx,incy;  
    unsigned char col;
    unsigned int row;
    delta_x = x2-x1;//计算坐标增量  
    delta_y = y2-y1;  
    col=x1;  
    row=y1;  
    if(delta_x > 0) 		//设置单步方向
	{
		incx=1; 	
	} 
    else if(delta_x == 0)   
    {  
		incx=0;			//垂直线    
    }
	else 
	{
		incx=-1;
		delta_x=-delta_x;
	}  
    if(delta_y > 0)
	{
		incy=1;
	}  
    else if(delta_y == 0)
	{
	  	incy=0;			//水平线 
	} 
    else 
	{
		incy=-1;
		delta_y=-delta_y;
	}    
    if(delta_x>delta_y) distance=delta_x;//选取基本增量坐标轴  
    else distance=delta_y;  

    for(t=0;t<=distance+1;t++)  
	{                                     //画线输出  
	    TFT_Point(col, row, color);
        xerr+=delta_x;  
        yerr+=delta_y;  
      	if(xerr>distance)  
        {  
            xerr-=distance;  
            col+=incx;  
        }  
        if(yerr>distance)  
        {  
            yerr-=distance;  
            row+=incy;  
        }  
    }  
}

/*********************************************
画空心矩形
入口参数： (sx,sy)左上角顶点坐标, (ex,ey)右下角顶点坐标, color颜色
出口参数: 无
说明：在指定位置上画出空心矩形。
**********************************************/
void TFT_REC(unsigned char sx,unsigned int sy,unsigned char ex,unsigned int ey,unsigned int color)
{
	TFT_Line(sx,sy,ex,sy,color);
	TFT_Line(sx,sy,sx,ey,color);
	TFT_Line(ex,sy,ex,ey,color);
	TFT_Line(sx,ey,ex,ey,color);
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void TFT_Circle(unsigned char x0,unsigned int y0,unsigned char r,unsigned int c)
{
	unsigned char a,b;
	signed char di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		TFT_Point(x0-b,y0-a,c);             //3           
		TFT_Point(x0+b,y0-a,c);             //0           
		TFT_Point(x0-a,y0+b,c);             //1       
		TFT_Point(x0-b,y0-a,c);             //7           
		TFT_Point(x0-a,y0-b,c);             //2             
		TFT_Point(x0+b,y0+a,c);             //4               
		TFT_Point(x0+a,y0-b,c);             //5
		TFT_Point(x0+a,y0+b,c);             //6 
		TFT_Point(x0-b,y0+a,c);             
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		TFT_Point(x0+a,y0+b,c);
	}
}


/************************************************
**显示16X24格式的字符
**x,y是要显示的字符的起始位置，For_color为前景色，Bk_color位背景色，ch位要显示的字符
************************************************/
void TFT_WriteChar16X24(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color, char ch)
{       
   unsigned char temp,Bytes;
   unsigned char pos,t;
   unsigned char CHAR_W,CHAR_H;

	CHAR_W = 16;         //16*24
   	CHAR_H = 24;

   if(x>(TFT_SIZE_X-CHAR_W)||y>(TFT_SIZE_Y-CHAR_H))
   return;
   
   TFT_SetWindow(x, y, x+CHAR_W-1, y+CHAR_H-1);
   ch = ch-32;                       //按照ASCII编码顺序得到相应字母的编码
   Bytes = (CHAR_W*CHAR_H)>>3;		//除以8
   for(pos=0;pos<Bytes;pos++)  //CHAR_H 	
   {
	  temp= Font16x24[ch][pos];
      
      for(t=0;t<8;t++) //CHAR_W
      {                 
	    if(temp&0x80)
	    TFT_Write_Color(For_color);  //textcolor
	    else 
	    TFT_Write_Color(Bk_color);  //backcolor
        temp<<=1; 
      }
	  
   }
}  

/************************************************
**显示8X16格式的字符
**x,y是要显示的字符的起始位置，For_color为前景色，Bk_color位背景色，ch位要显示的字符
************************************************/
void TFT_WriteChar8X16(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color, char ch)
{       
   unsigned char temp,Bytes;
   unsigned char pos,t;
   unsigned char CHAR_W,CHAR_H;

   CHAR_W = 8;         //8*16
   CHAR_H = 16;

   if(x>(TFT_SIZE_X-CHAR_W)||y>(TFT_SIZE_Y-CHAR_H))
   return;
   
   TFT_SetWindow(x, y, x+CHAR_W-1, y+CHAR_H-1);
   ch = ch-32;                       //按照ASCII编码顺序得到相应字母的编码
   Bytes = (CHAR_W*CHAR_H)>>3;		//除以8
   for(pos=0;pos<Bytes;pos++)  //CHAR_H 	
   {
	temp= Font8x16[ch][pos];
      
      for(t=0;t<8;t++) //CHAR_W
      {                 
	    if(temp&0x80)
	    TFT_Write_Color(For_color);  //textcolor
	    else 
	    TFT_Write_Color(Bk_color);  //backcolor
        temp<<=1; 
      }
	  
   }
}  

void TFT_WriteString16X24(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,char *p)
{         
   while(*p!='\0')
   {       
     TFT_WriteChar16X24(x,y,For_color,Bk_color,*p);
	 x+=16;
     p++;
   }
}

void TFT_WriteString8X16(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,char *p)
{         
   while(*p!='\0')
   {       
     TFT_WriteChar8X16(x,y,For_color,Bk_color,*p);
     x+=8;
     p++;
   }
}

//打印汉字，size=16X16
void TFT_ChineseChar16(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color ,const unsigned char* hz,unsigned char c)
{
   unsigned char i,j;
   unsigned char ed;
   
   unsigned char *pchar = (unsigned char*)(hz + ((c-1)<<5));	 	//打印第几个字

   TFT_SetWindow(x,y,x+15,y+15);
   
   for(i=0;i<32;i++)
   {
     ed=*pchar++;
	 for(j=0;j<8;j++) //CHAR_W
      {                 
	    if(ed&0x01)
	    TFT_Write_Color(For_color);  //textcolor
	    else 
	    TFT_Write_Color(Bk_color);  //backcolor
        ed>>=1; 
      }
  }
}

//打印汉字，size=24X24
void TFT_ChineseChar24(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color ,const unsigned char* hz,unsigned char c)
{
   unsigned char i,j;
   unsigned char ed;

   unsigned char *pchar = (unsigned char*)(hz + (72*(c-1)));	 	//打印第几个字
    

   TFT_SetWindow(x,y,x+23,y+23);
   
   for(i=0;i<72;i++)
   {
     ed=*pchar++;
	 for(j=0;j<8;j++) //CHAR_W
      {                 
	    if(ed&0x01)
	    TFT_Write_Color(For_color);  //textcolor
	    else 
	    TFT_Write_Color(Bk_color);  //backcolor
        ed>>=1; 
      }
  }
}

//打印汉字，size=32X32
void TFT_ChineseChar32(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color ,const unsigned char*hz,unsigned char c)
{
   unsigned char i,j;
   unsigned char ed;

   unsigned char *pchar = (unsigned char*)(hz + (132*(c-1)));	 	//打印第几个字
   
   TFT_SetWindow(x,y,x+31,y+31);
   
   for(i=0;i<132;i++)
   {
     ed=*pchar++;
	 for(j=0;j<8;j++) //CHAR_W
      {                 
	    if(ed&0x01)
	    TFT_Write_Color(For_color);  //textcolor
	    else 
	    TFT_Write_Color(Bk_color);  //backcolor
        ed>>=1; 
      }
  }
}

//打印汉字，size=16X16
void TFT_ChineseString16(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,const unsigned char*hz,unsigned char words)
{
	unsigned char i;
	for(i=1;i<=words;i++)
	{
		TFT_ChineseChar16(x+(i-1)<<4,y,For_color,Bk_color,hz,i);	
	}
}

//打印汉字，size=24X24
void TFT_ChineseString24(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,const unsigned char* hz,unsigned char words)
{
	unsigned char i;
	for(i=1;i<=words;i++)
	{
		TFT_ChineseChar24(x+(i-1)*24,y,For_color,Bk_color,hz,i);	
	}
}

//打印汉字，size=32X32
void TFT_ChineseString32(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,const unsigned char* hz,unsigned char words)
{
	unsigned char i;
	for(i=1;i<=words;i++)
	{
		TFT_ChineseChar32(x+(i-1)*32,y,For_color,Bk_color,hz,i);	
	}
}

//显示图片
void TFT_Picture(unsigned char x,unsigned int y,unsigned char size_x,unsigned int size_y,const unsigned char *pic)
{
	unsigned int i;
	if(((x+size_x)>TFT_SIZE_X) || ((y+size_y)>TFT_SIZE_Y))
	{
		TFT_WriteString16X24(x,y,BLACK,WHITE,"Too Large");
		return;
	}
	TFT_SetWindow(x,y,x+size_x-1,y+size_y-1);
	for(i=0;i<size_x*size_y;i++)
	{
		TFT_CS_Low();
		TFT_Write_Data(pic[2*i]);
		TFT_Write_Data(pic[2*i+1]);	  
		TFT_CS_High();
	}	
}

unsigned char TFT_WriteInt_Sign(unsigned char x, unsigned int y, unsigned int For_color,unsigned int Bk_color,signed long value,unsigned char sign)
{
	unsigned char count=0;
	signed char i=0;
	char* str = NULL;
	signed long temp = 0;
	if(sign == 0)
	{
		TFT_WriteChar8X16(x,y,For_color,Bk_color,'-');
		value = -value;
	}
	else
	{
		TFT_WriteChar8X16(x,y,For_color,Bk_color,'+');
	}
	temp = value;						//此时value非负
	if(value == 0)
	{
		count = 1;
	}
	while(temp)
	{
		count++;
		temp /= 10;
	}
	str = (char*)calloc(count+1,sizeof(char));
	str[count] = '\0';
	for(i=count-1;i>=0;i--)
	{
		str[i] = (value%10)+'0';
		value /= 10;
	}
	TFT_WriteString8X16(x+8,y,For_color,Bk_color,str);
	free(str);
	return count+1;									//‘1’表示符号位
}

unsigned char TFT_WriteInt(unsigned char x, unsigned int y, unsigned int For_color,unsigned int Bk_color,signed long value)
{
	unsigned char count=0;
	if(value < 0)
	{
		count = TFT_WriteInt_Sign(x,y,For_color,Bk_color,value,0);			//负整数
	}
	else
	{
		count = TFT_WriteInt_Sign(x,y,For_color,Bk_color,value,1);			//正整数
	}
	return count;
}


//在彩屏中写浮点数,dot表示待显示的小数点位数
void TFT_WriteDouble(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,double value,unsigned char dot)
{
	unsigned char dot1 = dot;
	signed long temp = (signed long)(value);
	unsigned char length = 0;
	char* str = (char*)calloc(dot+1,1);
	signed char i;
	if(value<0)
	{
		length = TFT_WriteInt_Sign(x,y,For_color,Bk_color,temp,0);
	}
	else
	{
		length = TFT_WriteInt_Sign(x,y,For_color,Bk_color,temp,1);
	}
	TFT_WriteChar8X16(x+length*8,y,For_color,Bk_color,'.');
	value = abs(value)-abs(temp);							//确保value是正数
	while(dot--)
	{
		value *= 10;
	}
	value = (value*10+5)/10;								//四舍五入
	dot = dot1;
	temp = (signed long)(value);
	str[dot] = '\0';
	for(i=dot-1;i>=0;i--)
	{
		str[i] = temp%10 + '0';
		temp /= 10;
	}
	TFT_WriteString8X16(x+8*(length+1),y,For_color,Bk_color,str);
	free(str);
}

void TFT_MenuConstruct(void)
{
	TFT_CLRScreen(SKY_BLUE);
	TFT_REC(0,0,TFT_SIZE_X-1,TFT_SIZE_Y-1,RED);
	TFT_WriteString8X16(TFT_SIZE_X-17,TFT_SIZE_Y-17,RED,BLACK,"P1");//右下角显示页码P1
	TFT_ChineseString24(72,0,RED,BLACK,hz24,4);						//第一行显示中文“苏州大学”
}
