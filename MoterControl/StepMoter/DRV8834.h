#ifndef DRV8834_H_
#define DRV8834_H_
/*
 * 双路桥式步进电机驱动器
 * M1 = 1, M0 = 高阻 则使能了1/32细分
 */
#define DRV8834_Direction_Right()		GPIO_SetBit(6,7)
#define DRV8834_Direction_Left()		GPIO_ResetBit(6,7)


extern unsigned char DRV8834_Caculate_Steps(unsigned int angle);//根据角度计算得到对应STEP上的上升脉冲个数
extern void DRV8834_Go(unsigned char steps);//在STEP管脚上给出上升沿
extern void DRV8834_Init();
extern void DRV8834_Degree(unsigned int degree);
#endif /* DRV8834_H_ */
