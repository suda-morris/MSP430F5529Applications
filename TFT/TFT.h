#ifndef __TFT_H_
#define __TFT_H_

#include "ILI9341.h"

//定义彩屏分辨率
#define TFT_SIZE_X 240
#define TFT_SIZE_Y 320

//5-6-5格式的色彩
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F	//紫红色的
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)
#define SKY_BLUE		 0x041f

void TFT_Write_Color(unsigned int c);
void TFT_CLRScreen(unsigned int bg_color);
void TFT_Box(unsigned char sx,unsigned int sy,unsigned char ex,unsigned int ey,unsigned int color);
void TFT_Point(unsigned char x, unsigned int y,unsigned int c);
void TFT_Line(unsigned char x1,unsigned int y1,unsigned char x2,unsigned int y2,unsigned int color);
void TFT_REC(unsigned char sx,unsigned int sy,unsigned char ex,unsigned int ey,unsigned int color);
void TFT_WriteChar16X24(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color, char ch);
void TFT_WriteChar8X16(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color, char ch);
void TFT_WriteString16X24(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,char *p);
void TFT_WriteString8X16(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,char *p);
void TFT_ChineseChar16(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color ,const unsigned char* hz,unsigned char c);
void TFT_ChineseChar24(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color ,const unsigned char* hz,unsigned char c);
void TFT_ChineseChar32(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color ,const unsigned char* hz,unsigned char c);
void TFT_ChineseString16(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,const unsigned char* hz,unsigned char words);
void TFT_ChineseString24(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,const unsigned char* hz,unsigned char words);
void TFT_ChineseString32(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,const unsigned char* hz,unsigned char words);
void TFT_Picture(unsigned char x,unsigned int y,unsigned char size_x,unsigned int size_y,const unsigned char *pic);
unsigned char TFT_WriteInt(unsigned char x, unsigned int y, unsigned int For_color,unsigned int Bk_color,signed long value);
void TFT_WriteDouble(unsigned char x,unsigned int y,unsigned int For_color,unsigned int Bk_color,double value,unsigned char dot);
void TFT_Circle(unsigned char x0,unsigned int y0,unsigned char r,unsigned int c);
void TFT_MenuConstruct(void);
#endif
