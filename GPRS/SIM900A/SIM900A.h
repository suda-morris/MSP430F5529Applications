#ifndef SIM900A_H_
#define SIM900A_H_

/*
 * SIM900A��Ҫ���Ž��ܣ�
 * 	4�ţ�Ring�����壬���������ʱ�򣬻�����½��ز��ұ��ֵ͵�ƽ��ֱ���û���ͨ���߹Ҷϲű�ɸߵ�ƽ,�ж������Ļ�ring���Ż����120ms�ĵ͵�ƽ
 *����ʹ���ⲿ�ж��½��ش��������ж�����ʱ120ms����������Ż��ǵ͵�ƽ˵����ʱ���յ�������
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

extern void SIM900A_Init(void);		//�豸��ʼ��
extern void SIM900A_Send_Command(char * cmd,char addEnter);//���ô��ڷ���ָ��
extern unsigned char SIM900A_CheckDevice();//�����豸ATָ��
extern unsigned char* SIM900A_GetDeviceInfo();//�����豸��Ϣ
extern unsigned char SIM900A_CheckNet();	//������Ӫ��
extern unsigned char* SIM900A_GetNetInfo();//������Ӫ�̵���Ϣ
extern unsigned char SIM900A_CheckSignal();//���������ź�ǿ��
extern unsigned char* SIM900A_GetSignalInfo();//���������ź�ǿ��
extern unsigned char SIM900A_CheckSerialNo();//�����豸���к�
extern unsigned char* SIM900A_GetSerialInfo();//�����豸���к�
extern unsigned char SIM900A_Dial(char* telNo);	//��绰
extern unsigned char* SIM900A_GetDialInfo();//���ش�绰����Ϣ
extern unsigned char SIM900A_Hang();			//�ҵ绰
extern unsigned char SIM900A_Listen();			//�ӵ绰
extern unsigned char SIM900A_MessageConfig();	//������ǰ������
extern unsigned char SIM900A_SendMessage(const char* telNo,unsigned char tel_len,const char* content,unsigned char content_len);//����PDU����
extern unsigned char* SIM900A_GetMSGSender();	//��ȡ���ŷ�����
extern unsigned char* SIM900A_GetMSGContent();	//��ȡ��������
#endif /* SIM900A_H_ */
