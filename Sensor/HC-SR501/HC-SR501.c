#include "HC-SR501.h"

/*
 * 红外热释电传感器HC-SR501模块
 * 		电源：5V
 * 		out：接单片机P2.3引脚
 * 		调节左边的滑动变阻器：顺时针，灵敏度提高，即感应距离增大
 * 		调节右边的滑动变阻器：顺时针，延迟时间增加
 */
void HC_SR501_Init(void)
{
	GPIO_Set_Direction_In(2,3,GPIO_IN_UP);		//P2.3上拉输入，红外感应输出引脚
	GPIO_ExInt_Enable(2,3,GPIO_INT_EDGE_UP);	//P2.3上升沿触发中断
}
