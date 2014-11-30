#include "SIM900A.h"

unsigned char ring = RING_NONE;

//volatile unsigned char * SIM900A_FeedBack = NULL;
volatile unsigned char SIM900A_FeedBack[SIM900A_FeedBack_MAX] = {0};
volatile unsigned char SIM900A_FBPointer = 0;		//SIM900A_FeedBack堆栈的指针,外部变量
unsigned char* SIM_Info = NULL;						//保存用户需要的信息,是SIM900A_FeedBack的变体

void SIM900A_Init(void)
{
//	SIM900A_FeedBack = (unsigned char*)calloc(SIM900A_FeedBack_MAX,sizeof(char));
	SIM_Info = (unsigned char*)calloc(SIM900A_InfoMAX,sizeof(char));
	SIM900A_FBPointer = 0;
	USCIA1_EnterIntoConfig();
	USCIA1_SetBaud(9600);
	USCIA1_UART_Config();
	USCIA1_ExitConfig();
	USCIA1_Enable_RXInterrupt(YES);		//打开接收中断
}

void SIM900A_Send_Command(char * cmd,char addEnter)
{
	unsigned char length = strlen((const char*)cmd);
	unsigned char i = 0;
	SIM900A_FBPointer = 0;
	for(i =0;i<length;i++)
	{
		USCIA1_UART_SendByte(cmd[i]);
	}
	if(addEnter)
	{
		USCIA1_UART_SendByte(0x0d);					//回车
	}
}

unsigned char SIM900A_CheckDevice()
{
	SIM900A_Send_Command("ATI",SIM900A_Enter);
	delayms(1000);
	if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')	//仅仅表示指令正确发送
		return 1;
	else
		return 0;
}

unsigned char* SIM900A_GetDeviceInfo()
{
	unsigned char i = 0;
	SIM_Info[13] = '\0';
	for(i=0;i<13;i++)
	{
		SIM_Info[i] = SIM900A_FeedBack[i+6];
	}
	SIM900A_FBPointer = 0;
	return SIM_Info;
}

unsigned char SIM900A_CheckNet()
{
	SIM900A_Send_Command("AT+COPS?",SIM900A_Enter);
	delayms(1000);
	if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
		return 1;
	else
		return 0;
}

unsigned char* SIM900A_GetNetInfo()
{
	unsigned char i = 0;
	if(SIM900A_FBPointer<30)			//没有收到关于运营商的信息
	{
		SIM_Info[0] = 'N';
		SIM_Info[1] = 'o';
		SIM_Info[2] = ' ';
		SIM_Info[3] = 'S';
		SIM_Info[4] = 'I';
		SIM_Info[5] = 'M';
		SIM_Info[6] = '\0';
	}
	else
	{
		SIM_Info[12] = '\0';
		for(i=0;i<12;i++)
		{
			SIM_Info[i] = SIM900A_FeedBack[i+23];
		}
	}
	SIM900A_FBPointer = 0;
	return SIM_Info;
}

unsigned char SIM900A_CheckSignal()
{
	SIM900A_Send_Command("AT+CSQ",SIM900A_Enter);
	delayms(1000);
	if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
		return 1;
	else
		return 0;
}

unsigned char* SIM900A_GetSignalInfo()		//返回信号强度
{
	unsigned char signal;
	if(SIM900A_FeedBack[16] != ',')
	{
		signal = ((SIM900A_FeedBack[15]-0x30)*10 + (SIM900A_FeedBack[16]-0x30))*100/31;
	}
	else
	{
		signal = (SIM900A_FeedBack[15]-0x30)*100/31;
	}
	SIM_Info[0] = signal/10 + '0';
	SIM_Info[1] = signal%10 + '0';
	SIM_Info[2] = '%';
	SIM_Info[3] = '\0';
	SIM900A_FBPointer = 0;
	return SIM_Info;
}

unsigned char SIM900A_CheckSerialNo()
{
	SIM900A_Send_Command("AT+CGSN",SIM900A_Enter);
	delayms(1000);
	if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
		return 1;
	else
		return 0;
}

unsigned char* SIM900A_GetSerialInfo()
{
	unsigned char i = 0;
	SIM_Info[15] = '\0';
	for(i=0;i<15;i++)
	{
		SIM_Info[i] = SIM900A_FeedBack[i+10];
	}
	SIM900A_FBPointer = 0;
	return SIM_Info;
}

unsigned char SIM900A_Dial(char* telNo)
{
	char* cmd = calloc((5+strlen(telNo)),sizeof(char));
	strcpy(cmd,"ATD");
	cmd = strcat(cmd,telNo);
	cmd = strcat(cmd,";");
	SIM900A_Send_Command(cmd,SIM900A_Enter);
	delayms(1000);
	free(cmd);
	if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
		return 1;
	else
		return 0;
}

unsigned char* SIM900A_GetDialInfo()		//返回接听情况
{
	unsigned char pt = SIM900A_FBPointer;
	while((SIM900A_FeedBack[pt+3] != 'U') && (SIM900A_FeedBack[pt+3] != 'O'));	//拨号中
	if(SIM900A_FeedBack[pt+3] == 'U')
	{
		SIM_Info = (unsigned char*)"Busy.........";
	}
	else
	{
		SIM_Info = (unsigned char*)"No Carrier!!!";
	}
	SIM900A_FBPointer = 0;
	return SIM_Info;
}


unsigned char SIM900A_Hang()
{
	SIM900A_Send_Command("ATH",SIM900A_Enter);
	delayms(1000);
	if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
	{
		SIM900A_FBPointer = 0;
		return 1;
	}
	else
	{
		SIM900A_FBPointer = 0;
		return 0;
	}
}

unsigned char SIM900A_Listen()
{
	SIM900A_Send_Command("ATA",SIM900A_Enter);
	delayms(1000);
	if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
	{
		SIM900A_FBPointer = 0;
		return 1;
	}
	else
	{
		SIM900A_FBPointer = 0;
		return 0;
	}
}

unsigned char SIM900A_MessageConfig()
{
	char* cmd = (char*)calloc(15,sizeof(char));
	strcpy(cmd,"AT+CSCS=");
	cmd = strcat(cmd,"\"");
	cmd = strcat(cmd,"UCS2");
	cmd = strcat(cmd,"\"");
	SIM900A_Send_Command(cmd,SIM900A_Enter);
	delayms(1000);
	free(cmd);
	if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
	{
		SIM900A_Send_Command("AT+CNMI=2,2,0,1",SIM900A_Enter);
		delayms(1000);
		if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
		{
			SIM900A_Send_Command("AT+CMGF=0",SIM900A_Enter);
			delayms(1000);
			if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
			{
				SIM900A_Send_Command("AT+CLIP=1",SIM900A_Enter);
				delayms(1000);
				if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
				{
					SIM900A_FBPointer = 0;
					return 1;
				}
				else
				{
					SIM900A_FBPointer = 0;
					return 0;
				}
			}
			else
			{
				SIM900A_FBPointer = 0;
				return 0;
			}
		}
		else
		{
			SIM900A_FBPointer = 0;
			return 0;
		}
	}
	else
	{
		SIM900A_FBPointer = 0;
		return 0;
	}
}


unsigned char SIM900A_SendMessage(const char* telNo,unsigned char tel_len,const char* content,unsigned char content_len)
{
	char* precmd = NULL;
	char* cmd = NULL;
	char SMSC_Addr[14];
	unsigned char i = 0;
	SIM900A_Send_Command("AT+CSCA?",SIM900A_Enter);	//获取服务中心号码
	delayms(1000);
	for(i=0;i<13;i++)
	{
		SMSC_Addr[i] = SIM900A_FeedBack[4*i+26];		//服务中心号码
	}
	SMSC_Addr[13] = '\0';

	precmd = (char*)calloc(11,sizeof(char));
	strcpy(precmd,"AT+CMGS=");
	precmd[8] = (8+(tel_len+1)/2+content_len)/10 + '0';
	precmd[9] = (8+(tel_len+1)/2+content_len)%10 + '0';
	precmd[10] = '\0';
	SIM900A_Send_Command(precmd,SIM900A_Enter);
	delayms(1000);
	free(precmd);

	if(SIM900A_FeedBack[13] == '>')			//是否收到'>'
	{
		cmd = (char*)calloc(19,sizeof(char));
		strcpy(cmd,"0891");
		for(i=0;i<14;i++)
		{
			if(SMSC_Addr[i] != '\0')
			{
				((i%2) == 1) ? (cmd[i+3] = SMSC_Addr[i]) : (cmd[i+5] = SMSC_Addr[i]);
			}
			else
			{
				((i%2) == 1) ? (cmd[i+3] = 'F') : (cmd[i+5] = 'F');
			}
		}
		cmd[18] = '\0';
		SIM900A_Send_Command(cmd,SIM900A_NoEnter);	//发送服务中心内容
		free(cmd);

		cmd = (char*)calloc(16+tel_len,sizeof(char));
		cmd = strcpy(cmd,"1100");
		cmd[4] = tel_len/16 + ((tel_len/16)>9? '7':'0');
		cmd[5] = tel_len%16 + ((tel_len%16)>9? '7':'0');
		cmd[6] = '\0';
		cmd = strcat(cmd,"91");
		for(i=0;i<14;i++)
		{
			if(telNo[i] != '\0')
			{
				((i%2) == 1)? (cmd[i+7] = telNo[i]) : (cmd[i+9] = telNo[i]);
			}
			else
			{
				((i%2) == 1)? (cmd[i+7] = 'F') : (cmd[i+9] = 'F');
			}
		}
		cmd[22] = '\0';
		cmd = strcat(cmd,"000800");
		SIM900A_Send_Command(cmd,SIM900A_NoEnter);
		free(cmd);

		cmd = (char*)calloc(3+2*content_len,sizeof(char));
		cmd[0] = content_len/16 + ((content_len/16)>9? '7':'0');
		cmd[1] = content_len%16 + ((content_len%16)>9? '7':'0');
		for(i=0;i<content_len;i++)
		{
			cmd[i*2+2] = content[i]/16 + ((content[i]/16)>9? '7':'0');
			cmd[i*2+3] = content[i]%16 + ((content[i]%16)>9? '7':'0');
		}
		cmd[2+2*content_len] = '\0';
		SIM900A_Send_Command(cmd,SIM900A_NoEnter);
		free(cmd);
		delayms(1000);
		SIM900A_FBPointer = 0;
		USCIA1_UART_SendByte(0x1A);
		delayms(8000);						//这里延时必须要长
		delayus(1);
		if(SIM900A_FeedBack[SIM900A_FBPointer-4] == 'O')
		{
			SIM900A_FBPointer = 0;
			return 1;
		}
		else
		{
			SIM900A_FBPointer = 0;
			return 0;
		}
	}
	else
	{
		SIM900A_FBPointer = 0;
		return 0;
	}
}

unsigned char* SIM900A_GetMSGSender()
{
	unsigned char i = 0;
	delayms(1000);
	for(i=39;i<=52;i++)
	{
		if(i%2 == 1)
		{
			SIM900A_FeedBack[i+1] = SIM900A_FeedBack[i]+SIM900A_FeedBack[i+1];
			SIM900A_FeedBack[i] = SIM900A_FeedBack[i+1]-SIM900A_FeedBack[i];
			SIM900A_FeedBack[i+1] = SIM900A_FeedBack[i+1]-SIM900A_FeedBack[i];
		}
	}
	for(i=0;i<14;i++)
	{
		SIM_Info[i] = SIM900A_FeedBack[i+39];
	}
	SIM_Info[13] = '\0';
	return SIM_Info;
}

unsigned char* SIM900A_GetMSGContent()
{
	unsigned char i = 0;
	for(i=73;i<SIM900A_FBPointer-2;i++)
	{
		SIM_Info[i-73] = SIM900A_FeedBack[i];
	}
	for(i=SIM900A_FBPointer-2-73;i<SIM900A_InfoMAX;i++)
	{
		SIM_Info[i] = '\0';
	}
	SIM_Info[SIM900A_FBPointer-2] = '\0';
	return SIM_Info;
}

