#ifndef DRV8834_H_
#define DRV8834_H_
/*
 * ˫·��ʽ�������������
 * M1 = 1, M0 = ���� ��ʹ����1/32ϸ��
 */
#define DRV8834_Direction_Right()		GPIO_SetBit(6,7)
#define DRV8834_Direction_Left()		GPIO_ResetBit(6,7)


extern unsigned char DRV8834_Caculate_Steps(unsigned int angle);//���ݽǶȼ���õ���ӦSTEP�ϵ������������
extern void DRV8834_Go(unsigned char steps);//��STEP�ܽ��ϸ���������
extern void DRV8834_Init();
extern void DRV8834_Degree(unsigned int degree);
#endif /* DRV8834_H_ */
