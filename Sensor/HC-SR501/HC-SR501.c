#include "HC-SR501.h"

/*
 * �������͵紫����HC-SR501ģ��
 * 		��Դ��5V
 * 		out���ӵ�Ƭ��P2.3����
 * 		������ߵĻ�����������˳ʱ�룬��������ߣ�����Ӧ��������
 * 		�����ұߵĻ�����������˳ʱ�룬�ӳ�ʱ������
 */
void HC_SR501_Init(void)
{
	GPIO_Set_Direction_In(2,3,GPIO_IN_UP);		//P2.3�������룬�����Ӧ�������
	GPIO_ExInt_Enable(2,3,GPIO_INT_EDGE_UP);	//P2.3�����ش����ж�
}
