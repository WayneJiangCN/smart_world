#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"	 


#define KEY1 PCin(0)
#define KEY2 PCin(1) 
#define KEY3 PCin(2)
#define KEY4 PCin(3)
#define KEY5 PAin(0)

#define KEY1_PORT_CLK  RCC_APB2Periph_GPIOC
#define KEY1_PORT			 GPIOC
#define KEY1_PORT_PIN  GPIO_Pin_0

#define KEY2_PORT_CLK  RCC_APB2Periph_GPIOC
#define KEY2_PORT			 GPIOC
#define KEY2_PORT_PIN  GPIO_Pin_1

#define KEY3_PORT_CLK  RCC_APB2Periph_GPIOC
#define KEY3_PORT			 GPIOC
#define KEY3_PORT_PIN  GPIO_Pin_2

#define KEY4_PORT_CLK  RCC_APB2Periph_GPIOC
#define KEY4_PORT			 GPIOC
#define KEY4_PORT_PIN  GPIO_Pin_3

#define KEY5_PORT_CLK  RCC_APB2Periph_GPIOA
#define KEY5_PORT			 GPIOA
#define KEY5_PORT_PIN  GPIO_Pin_0

extern u8 key_num;

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);//按键扫描函数					    
#endif
