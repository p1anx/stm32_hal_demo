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
#include "./sdio/bsp_sdio_sdcard.h"
#include "./sdio/sdio_test.h"
#include "./led/bsp_led.h"
#include "./FatFS/ff.h"
#include "./FatFS/ff_gen_drv.h"
#include "./FatFS/drivers/sd_diskio.h"
#include  "VS1053.h"
#include "string.h"
#include "flac.h"

static void printf_fatfs_error(FRESULT fresult);

uint8_t  buffer[BUFSIZE];

/**
  ******************************************************************************
  *                              �������
  ******************************************************************************
  */
char SDPath[4];                   /* SD���߼��豸·�� */
FATFS fs;													/* FatFs�ļ�ϵͳ���� */
FIL file;													/* �ļ����� */
FRESULT f_res;                    /* �ļ�������� */
UINT bw;

/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{        
  /* ����ϵͳʱ��Ϊ72 MHz */ 
  SystemClock_Config();
  /*��ʼ��USART ����ģʽΪ 115200 8-N-1���жϽ���*/
  DEBUG_USART_Config();
  
	 /* ��ʼ��LED�� */
  LED_GPIO_Config();
	LED_BLUE;	
	
  /*��ʼ��USART1*/
  DEBUG_USART_Config();
  
	/* ��ʼ�����Դ��ڣ�һ��Ϊ����1 */
	DEBUG_USART_Config();	
  printf("\r\r\n****** ����һ��SD�� �ļ�ϵͳʵ�� ******\r\r\n");

  /* ע��һ��FatFS�豸��SD�� */
  FATFS_LinkDriver(&SD_Driver, SDPath);

  //��SD�������ļ�ϵͳ���ļ�ϵͳ����ʱ���SD����ʼ��
  f_res = f_mount(&fs,(TCHAR const*)SDPath,1);
  printf_fatfs_error(f_res);
  VS_Init(); 
  printf("vs1053:%4X\n",VS_Ram_Test());
  HAL_Delay(100);
  VS_Sine_Test();	
  VS_HD_Reset();
  VS_Soft_Reset();
	
	while(1)
	{
		// ���Ը��������ⲿFLASH W25Q64��
		vs1053_player_song("music/TestFile.mp3");
		printf("MusicPlay End\n");
	}	
}
//���Ÿ���
void vs1053_player_song(uint8_t *filepath)
{
	uint16_t i=0;
	
	VS_Restart_Play();  					
	VS_Set_All();        							 
	VS_Reset_DecodeTime();
	
	if(strstr((const char*)filepath,".flac")||strstr((const char*)filepath,".FLAC"))
		VS_Load_Patch((uint16_t*)vs1053b_patch,VS1053B_PATCHLEN);
	
	f_res=f_open(&file,(const TCHAR*)filepath,FA_READ);

	if(f_res==0)
	{ 
		VS_SPI_SpeedHigh();				   
		while(1)
		{
			i=0;	
			f_res=f_read(&file,buffer,BUFSIZE,(UINT*)&bw);		
			do
			{  	
				if(VS_Send_MusicData(buffer+i)==0)
				{
					i+=32;
				}
			}while(i<bw);
			
			if(bw!=BUFSIZE||f_res!=0)
			{
				break;	  
			}
			LED2_TOGGLE;
		}
		 /* ע��һ��FatFS�豸��SD�� */
  FATFS_UnLinkDriver(SDPath);
	}	  					     	  
}   

/**
  * @brief  ��ӡ�����Ϣ
  * @param  ��
  * @retval ��
  */
static void printf_fatfs_error(FRESULT fresult)
{
  switch(fresult)
  {
    case FR_OK:                   
      printf("�������ɹ���\r\n");
    break;
    case FR_DISK_ERR:             
      printf("����Ӳ�����������������\r\n");
    break;
    case FR_INT_ERR:              
      printf("�������Դ���\r\n");
    break;
    case FR_NOT_READY:            
      printf("���������豸�޷�������\r\n");
    break;
    case FR_NO_FILE:              
      printf("�����޷��ҵ��ļ���\r\n");
    break;
    case FR_NO_PATH:             
      printf("�����޷��ҵ�·����\r\n");
    break;
    case FR_INVALID_NAME:         
      printf("������Ч��·������\r\n");
    break;
    case FR_DENIED:               
    case FR_EXIST:                
      printf("�����ܾ����ʡ�\r\n");
    break;
    case FR_INVALID_OBJECT:       
      printf("������Ч���ļ���·����\r\n");
    break;
    case FR_WRITE_PROTECTED:      
      printf("�����߼��豸д������\r\n");
    break;
    case FR_INVALID_DRIVE:        
      printf("������Ч���߼��豸��\r\n");
    break;
    case FR_NOT_ENABLED:          
      printf("������Ч�Ĺ�������\r\n");
    break;
    case FR_NO_FILESYSTEM:        
      printf("������Ч���ļ�ϵͳ��\r\n");
    break;
    case FR_MKFS_ABORTED:         
      printf("���������������⵼��f_mkfs��������ʧ�ܡ�\r\n");
    break;
    case FR_TIMEOUT:              
      printf("����������ʱ��\r\n");
    break;
    case FR_LOCKED:               
      printf("�����ļ���������\r\n");
    break;
    case FR_NOT_ENOUGH_CORE:      
      printf("�������ļ���֧�ֻ�ȡ�ѿռ�ʧ�ܡ�\r\n");
    break;
    case FR_TOO_MANY_OPEN_FILES:  
      printf("������̫���ļ���\r\n");
    break;
    case FR_INVALID_PARAMETER:    
      printf("����������Ч��\r\n");
    break;
  }
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
  *            HSE PREDIV1                    = 1
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
