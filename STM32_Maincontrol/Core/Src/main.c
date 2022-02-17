/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define USER_MAIN_DEBUG
#ifdef USER_MAIN_DEBUG
#define user_main_printf(format, ...) printf(format "\r\n", ##__VA_ARGS__)
#define user_main_info(format, ...) printf("[\tmain]info:" format "\r\n", ##__VA_ARGS__)
#define user_main_debug(format, ...) printf("[\tmain]debug:" format "\r\n", ##__VA_ARGS__)
#define user_main_error(format, ...) printf("[\tmain]error:" format "\r\n", ##__VA_ARGS__)
#else
#define user_main_printf(format, ...)
#define user_main_info(format, ...)
#define user_main_debug(format, ...)
#define user_main_error(format, ...)
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//  void Printft();
uint8_t Curtain_Status = 0;       //默认窗帘关闭
uint8_t Light_Status = 0;         //默认有光
uint8_t Gas_Status = 1;           //默认空气优
uint8_t EntranceGuard_Status = 0; //门禁默认关闭
uint8_t RxBuffer1, RxBuffer2, RxBuffer3;
uint8_t mode;            //设置模式变量
uint8_t OpenValue = 0;   //人为对窗帘打开控制默认关闭
uint8_t CloseValue = 0;  //人为对窗帘关闭默认关闭
uint8_t ActionValue = 0; //动作标志位默认为0，即没有开关时即没有动作标志位

uint32_t Transfer_Value; //设置传入数据变量
uint8_t data[8] = {0};
uint32_t light_value;
uint32_t gas_value;
uint8_t AD_Value;
uint8_t curtain = 0; //默认自动模式
uint32_t ADC_Value[100];
uint8_t i;
uint32_t ad1, ad2;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void close_curtain(void);
void open_curtain(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
void open_engine_work(int16_t i);
void close_engine_work(int16_t i);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#include "stdio.h"
int fputc(int ch, FILE *f)
{
    uint8_t temp[1] = {ch};
    HAL_UART_Transmit(&huart2, temp, 1, 2); //huart1 根据配置修改
    return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */
    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */
    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_ADC1_Init();
    MX_USART3_UART_Init();
    MX_TIM2_Init();
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

    /* USER CODE BEGIN 2 */

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_Value, 100);
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxBuffer1, 1);
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&RxBuffer2, 1);
    HAL_UART_Receive_IT(&huart3, (uint8_t *)&RxBuffer3, 1); //对于串口3先不做处理

    data[0] = 'a';
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        /*空气质量*/
        //	  HAL_ADC_Start(&hadc2);//启动ADC装换
        //	 HAL_ADC_PollForConversion(&hadc2, 50);//等待转换完成，第二个参数表示超时时间，单位ms.
        //	 if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc2), HAL_ADC_STATE_REG_EOC)){
        //			AD_Value = HAL_ADC_GetValue(&hadc2);//读取ADC转换数据，数据为12位
        //			gas_value= (int)AD_Value*99.00/4096;
        //			data[5] = gas_value/10+ '0';
        //			data[6] = gas_value%10+ '0';
        //printf("[\tmain]info:v=%d\r\n",gas_value);//打印日志
			open_curtain();
        
        for (i = 0, ad1 = 0, ad2 = 0; i < 100;)
        {
            ad1 += ADC_Value[i++];
            ad2 += ADC_Value[i++];
        }
        ad1 /= 50;
        ad2 /= 50;
        light_value = 100-(int)ad1 * 100.0f / 4096;
        gas_value = (int)ad2 * 100.0f / 4096;
        if (light_value <30)
            Light_Status = 1; //光暗的时候
        else
            Light_Status = 0;

        //printf("[\tmain]info:AD1_value=%1.3fV\r\n", ad1*5.0f/4096);
        //printf("[\tmain]info:AD2_value=%1.3fV\r\n", ad2*5.0f/4096);

        //}
        data[1] = Curtain_Status + '0';
        data[2] = EntranceGuard_Status + '0';
        data[5] = gas_value / 10 + '0';
        data[6] = gas_value % 10 + '0';
        data[3] = light_value / 10 + '0';
        data[4] = light_value % 10 + '0';

        uartx_printf(huart1, "%s", data);
        uartx_printf(huart3, "%s", data);   //发消息给蓝牙
        HAL_Delay(1500);

        //		/*窗帘控制*/

        if (curtain == 0) //手动模式
        {
            if (OpenValue == 1 && Curtain_Status == 0 && ActionValue == 0) //窗帘想要打开且窗帘是关闭的时候
            {
                open_curtain();
                ActionValue++;
                OpenValue = 0;
                Curtain_Status = 1;
            }
            if (CloseValue == 1 && Curtain_Status == 1 && ActionValue == 0) //窗帘想要关闭且窗帘是打开的时候
            {
                close_curtain();
                ActionValue++;
                CloseValue = 0;
                Curtain_Status = 0;
            }
        }
        else //自动模式
        {
            if (OpenValue == 0 && CloseValue == 0)
            {
                if (Light_Status == 1 && Curtain_Status == 0 && ActionValue == 0) //光暗窗帘关闭的时候
                {
                    open_curtain();
                    ActionValue++;
                    OpenValue = 0;
                    Curtain_Status = 1;
                }
                if (Light_Status == 0 && Curtain_Status == 1 && ActionValue == 0)
                {
                    close_curtain();
                    ActionValue++;
                    CloseValue = 0;
                    Curtain_Status = 0;
                }
            }
            else if (OpenValue == 1)
            {
                if (Curtain_Status == 0 && ActionValue == 0)
                {
                    open_curtain();
                    ActionValue++;
                    OpenValue = 0;
                    Curtain_Status = 1;
                }
            }
            else if (CloseValue == 1)
            {
                if (Curtain_Status == 1 && ActionValue == 0)
                {
                    close_curtain();
                    ActionValue++;
                    CloseValue = 0;
                    Curtain_Status = 0;
                }
            }
        }

        ActionValue = 0;

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }

    //void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)//串口中断
    //{
    //	__HAL_UART_CLEAR_PEFLAG(huart);//串口接收清空错误标志
    //	if(huart2->Instance==USART1);//如果是蓝牙串口发来消息

    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */
void close_curtain(void)
{
    close_engine_work(1);
}

void open_curtain(void)
{
    open_engine_work(1);
}
void close_engine_work(int16_t i)

{
    while (i--)
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 5);
        HAL_Delay(500);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 15);
    }
}
void open_engine_work(int16_t i)
{
    while (i--)
    {
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 5);
        HAL_Delay(1000);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 20);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t wifi_mode;
    __HAL_UART_CLEAR_PEFLAG(huart); //串口接收清空错误标志
                                    //	if(huart->Instance==USART3)//蓝牙串口发来消息
                                    //	{、
                                    ////		DBG("hello_world");
                                    //		mode = RxBuffer3;
                                    //	printf("%c",mode);
                                    //	switch(mode)
                                    //		{
                                    //		case 'C':
                                    //							curtain = 1;			//(自动模式)
                                    //

    //							 break;
    //		case 'D':
    //		//                                                                                                                                                                                                                                              DBG("hello_world");//Curtain_Off();//打开窗帘 ，函数运行超时了,串口无法打印
    //
    //							curtain = 0;			//（手动模式）
    //
    //							break;
    //		case 'A':
    //			OpenValue = 1;			///打开

    //			break;
    //		case 'B':
    //			CloseValue =1;		//关闭

    //
    //			break;
    //		}
    //		HAL_UART_Receive_IT(&huart3, (uint8_t*) &RxBuffer3, 1);//相当于手动清位,实测如果没有这个只能中断一次
    //	}
    if (huart->Instance == USART1) //接收esp8266通信数据
    {
        //	printf("hello");
        wifi_mode = RxBuffer1; //接收标志位数据并赋给全局变量
                               //printf("%c",wifi_mode);//检测接收的字符
        printf("%c", wifi_mode);
        switch (wifi_mode)
        {
        case 'C':
            curtain = 1; //改变状态(自动模式)

            //拉上窗帘，函数运行超时了,串口无法打印
            break;
        case 'D':
            //打开窗帘 ，函数运行超时了,串口无法打印

            curtain = 0; //改变状态（手动模式）
            //Curtain_Status= 1;//窗帘打开
            break;
        case 'A':
            OpenValue = 1; //open_curtain();

            break;
        case 'B':
            CloseValue = 1; //close_curtain();

            break;
        }
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxBuffer1, 1);
    }

    if (huart->Instance == USART2) //接收单片机门禁通信数据
    {
        EntranceGuard_Status = RxBuffer2 - '0';

        HAL_UART_Receive_IT(&huart2, (uint8_t *)&RxBuffer2, 1);
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
