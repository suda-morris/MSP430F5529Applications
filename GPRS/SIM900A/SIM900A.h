#ifndef SIM900A_H_
#define SIM900A_H_

/*
 * SIM900A主要引脚介绍：
 * 	4号：Ring，振铃，当有来电的时候，会产生下降沿并且保持低电平，直到用户接通或者挂断才变成高电平,有短信来的话ring引脚会产生120ms的低电平
 *可以使用外部中断下降沿触发，在中断中延时120ms，如果该引脚还是低电平说明此时接收到了来电
 */
#include <stdlib.h>
#include <string.h>
#include "MyF5529.h"

#define RING_NONE				0
#define RING_PHONE				1
#define RING_MSG				2

#define SIM900A_Enter			1
#define SIM900A_NoEnter			0
#define SIM900A_FeedBack_MAX	100
#define SIM900A_InfoMAX         40

extern void SIM900A_Init(void);		//设备初始化
extern void SIM900A_Send_Command(char * cmd,char addEnter);//利用串口发送指令
extern unsigned char SIM900A_CheckDevice();//测试设备AT指令
extern unsigned char* SIM900A_GetDeviceInfo();//返回设备信息
extern unsigned char SIM900A_CheckNet();	//测试运营商
extern unsigned char* SIM900A_GetNetInfo();//返回运营商的信息
extern unsigned char SIM900A_CheckSignal();//测试天线信号强度
extern unsigned char* SIM900A_GetSignalInfo();//返回天线信号强度
extern unsigned char SIM900A_CheckSerialNo();//测试设备序列号
extern unsigned char* SIM900A_GetSerialInfo();//返回设备序列号
extern unsigned char SIM900A_Dial(char* telNo);	//打电话
extern unsigned char* SIM900A_GetDialInfo();//返回打电话的信息
extern unsigned char SIM900A_Hang();			//挂电话
extern unsigned char SIM900A_Listen();			//接电话
extern unsigned char SIM900A_MessageConfig();	//发短信前的配置
extern unsigned char SIM900A_SendMessage(const char* telNo,unsigned char tel_len,const char* content,unsigned char content_len);//发送PDU短信
extern unsigned char* SIM900A_GetMSGSender();	//获取短信发送者
extern unsigned char* SIM900A_GetMSGContent();	//获取短信内容
#endif /* SIM900A_H_ */
