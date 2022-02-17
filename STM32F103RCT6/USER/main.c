/*
1.��Ŀ���ƣ������콢��-STM32�ۺϲ��԰�ָ��ģ����Գ���
2.��ʾģ�飺0.96������OLED��
3.����APP����
4.������λ������
5.��Ŀ��ɣ�0.96������OLED����AS608ָ��ģ�顢����STM32�ۺϲ��԰�
6.��Ŀ���ܣ���ӡ�ɾ������ָ֤�ơ���������뿴��ʾ��Ƶ
7.��Ҫԭ������ο�AS608ָ��ģ�������ֲ�
8.�����ַ��https://lssz.tmall.com ���Ա��������������콢�ꡱ
9.��ʾ��Ƶ����������ҳ��������ͷ���Ҫ��
10.��Ȩ�����������콢�����г��������������Ȩ�����뱾���Ʒ���׳��ۣ��벻Ҫ����������׷���䷨�����Σ�
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
  void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
        data=Res ;			 
     } 
}
int main(void)
{		 
	extern const u8 BMP1[];
	HZ= GB16_NUM();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();
	usart3_init(57600);
	uart_init(115200);
	KEY_Init();
	OLED_Init();
	OLED_Clear();
	OLED_ShowCH(0,0,"ָ��ģ����Գ���");
	OLED_ShowCH(16,2,"K1�����ָ��");
	OLED_ShowCH(16,4,"K3��ɾ��ָ��");
	OLED_ShowCH(16,6,"K5����ָ֤��");
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
			OLED_ShowCH(32,2,"�밴��ָ");
			press_FR();
		}	
	}
}

