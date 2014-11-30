#ifndef __TFT_H_
#define __TFT_H_

#include "ILI9341.h"

//��������ֱ���
#define TFT_SIZE_X 240
#define TFT_SIZE_Y 320

//5-6-5��ʽ��ɫ��
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F	//�Ϻ�ɫ��
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ  
#define GRAYBLUE       	 0X5458 //����ɫ
#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ
#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
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
