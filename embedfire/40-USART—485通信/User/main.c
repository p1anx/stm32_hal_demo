/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� F103-�Ե� STM32 ������ 
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include "./led/bsp_led.h"  
#include "./485/bsp_485.h"
#include ".\key\bsp_key.h" 

static void Delay(__IO uint32_t nCount);
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{
	char *pbuf;
	uint16_t len;
    
  /* ����ϵͳʱ��Ϊ72MHz */
  SystemClock_Config();

  /* ��ʼ��RGB�ʵ� */
  LED_GPIO_Config();

  /* ��ʼ��USART1 ����ģʽΪ 115200 8-N-1 */
  DEBUG_USART_Config();
  /*��ʼ��485ʹ�õĴ��ڣ�ʹ���ж�ģʽ����*/
  _485_Config();
	
	Key_GPIO_Config();
	
	printf("\r\n ��ӭʹ��Ұ��  STM32 F103 �����塣\r\n");
  printf("\r\n Ұ��F103 485ͨѶʵ������\r\n");
	
	printf("\r\n ʵ�鲽�裺\r\n");

	printf("\r\n 1.ʹ�õ������Ӻ�����485ͨѶ�豸\r\n");
	printf("\r\n 2.ʹ������ñ���Ӻ�:5v --- C/4-5V,485-D --- PD5,485-R ---PD6 \r\n");
	printf("\r\n 3.��ʹ������Ұ�𿪷������ʵ�飬�����������嶼���ر����򼴿ɡ�\r\n");
	printf("\r\n 4.׼���ú󣬰�������һ���������KEY1������ʹ��485���ⷢ��0-255������ \r\n");
	printf("\r\n 5.���������485���յ�256���ֽ����ݣ����������16������ʽ��ӡ������ \r\n");
	
  while(1)
  {
		/*��һ�ΰ�������һ������*/
		if(	Key_Scan(KEY1_GPIO_PORT,KEY1_PIN) == KEY_ON)
		{
			uint16_t i;
			
			LED_BLUE;
			
			_485_TX_EN();
			
			for(i=0;i<=0xff;i++)
			{
				_485_SendByte(i);	 //��������
			}
			
			/*�Ӷ�����ʱ����֤485�����������*/
			Delay(0xFFF);
			_485_RX_EN();
			
			LED_GREEN;
			
			printf("\r\n�������ݳɹ���\r\n"); //ʹ�õ��Դ��ڴ�ӡ������Ϣ���ն�

		}
		else
		{		
			LED_BLUE;
			
			pbuf = get_rebuff(&len);
			if(len>=256)
			{
				LED_GREEN;
				printf("\r\n���յ�����Ϊ%d������\r\n",len);	
				_485_DEBUG_ARRAY((uint8_t*)pbuf,len);
				clean_rebuff();
			}
		}
  }

}
void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 72000000
  *            HCLK(Hz)                       = 72000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 2
  *            APB2 Prescaler                 = 1
  *            HSE Frequency(Hz)              = 8000000
  *            HSE PREDIV1                    = 2
  *            PLLMUL                         = 9
  *            Flash Latency(WS)              = 0
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1); 
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
