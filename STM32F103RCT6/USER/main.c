/*
1.项目名称：绿深旗舰店-STM32综合测试板指纹模块测试程序
2.显示模块：0.96寸绿深OLED屏
3.配套APP：无
4.配套上位机：无
5.项目组成：0.96寸绿深OLED屏、AS608指纹模块、绿深STM32综合测试板
6.项目功能：添加、删除、验证指纹。具体操作请看演示视频
7.主要原理：具体参考AS608指纹模块数据手册
8.购买地址：https://lssz.tmall.com 或淘宝上搜索“绿深旗舰店”
9.演示视频：宝贝详情页面或购买后像客服索要！
10.版权声明：绿深旗舰店所有程序都申请软件著作权。均与本店产品配套出售，请不要传播，以免追究其法律责任！
*/
#include "sys.h"
#include "delay.h"
#include "oled_iic.h"
#include "stdio.h"
#include "key.h"
#include "timer.h"
#include "as608.h"
#include "usart3.h"
#include "usart.h"
  u8 data;
  void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
        data=Res ;			 
     } 
}
int main(void)
{		 
	extern const u8 BMP1[];
	HZ= GB16_NUM();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	delay_init();
	usart3_init(57600);
	uart_init(115200);
	KEY_Init();
	OLED_Init();
	OLED_Clear();
	OLED_ShowCH(0,0,"指纹模块测试程序");
	OLED_ShowCH(16,2,"K1键添加指纹");
	OLED_ShowCH(16,4,"K3键删除指纹");
	OLED_ShowCH(16,6,"K5键验证指纹");
	while(1)
	{		
		printf("%c",data);		
		if(data=='0')
			beep_alarm();
			
		key_num=KEY_Scan(0);
		if(key_num==1)
		{
			key_num=0;
			OLED_Clear();
			Add_FR();
		}
		if(key_num==3)
		{
			key_num=0;
			OLED_Clear();
			Del_FR();
		}
		if(key_num==5)
		{
			key_num=0;
			OLED_Clear();
			OLED_ShowCH(32,2,"请按手指");
			press_FR();
		}	
	}
}

