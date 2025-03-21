/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   液晶显示
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx.h"
#include "./usart/bsp_debug_usart.h"
#include <stdlib.h>
#include "./led/bsp_led.h" 
#include "./lcd/bsp_nt35510_lcd.h"

static void LCD_Test(void);	
static void Delay ( __IO uint32_t nCount );
void Printf_Charater(void);

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{
  /* 系统时钟初始化成72MHz */
  SystemClock_Config();
  /* LED 端口初始化 */
  LED_GPIO_Config();	 
  /* 初始化串口 */
  DEBUG_USART_Config();

  NT35510_Init ();             //LCD 初始化
	
	printf("\r\n ********** 液晶屏中文显示程序（字库在SD卡）*********** \r\n"); 
	printf("\r\n 若汉字显示不正常，请阅读工程中的文件说明，根据要求给拷贝字模数据\r\n"); 

	//其中0、3、5、6 模式适合从左至右显示文字，
 //不推荐使用其它模式显示文字	其它模式显示文字会有镜像效果			
 //其中 6 模式为大部分液晶例程的默认显示方向  
  NT35510_GramScan( 6 );

	Printf_Charater();
	
	while ( 1 )
	{
		LCD_Test();
	}	
}


/*用于测试各种液晶的函数*/
void LCD_Test(void)
{
	/*演示显示变量*/
	static uint8_t testCNT = 0;	
	char dispBuff[100];
	
	testCNT++;	
	
	LCD_SetFont(&Font16x32);
	LCD_SetColors(RED,BLACK);

  NT35510_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */
	/********显示字符串示例*******/ 
  NT35510_DispStringLine_EN_CH(LINE(0),"野火4.3寸LCD参数：");
  NT35510_DispStringLine_EN_CH(LINE(2),"分辨率：480x800 px");
  NT35510_DispStringLine_EN_CH(LINE(3),"NT35510液晶驱动");
  NT35510_DispStringLine_EN_CH(LINE(4),"GT917S触摸屏驱动");

	/********显示变量示例*******/
	LCD_SetTextColor(GREEN);

	/*使用c标准库把变量转化成字符串*/
	sprintf(dispBuff,"显示变量计数 : %d ",testCNT);
  NT35510_ClearLine(LINE(7));	/* 清除单行文字 */
	
	/*然后显示该字符串即可，其它变量也是这样处理*/
	NT35510_DispStringLine_EN(LINE(7),dispBuff);

	/*******显示图形示例******/
  /* 画直线 */
  
  NT35510_ClearLine(LINE(7));/* 清除单行文字 */
	LCD_SetTextColor(BLUE);

  NT35510_DispStringLine_EN_CH(LINE(7),"画直线:");
  
	LCD_SetTextColor(RED);
  NT35510_DrawLine(50,270,420,275);  
  NT35510_DrawLine(50,300,420,375);
  
	LCD_SetTextColor(GREEN);
  NT35510_DrawLine(50,370,420,475);  
  NT35510_DrawLine(50,400,420,475);
	
	LCD_SetTextColor(BLUE);
  NT35510_DrawLine(50,420,420,325);  
  NT35510_DrawLine(50,450,420,395);
  
  Delay(0x2FFFFFF);
  
  NT35510_Clear(0,32*7,LCD_X_LENGTH,LCD_Y_LENGTH-32*7);	/* 清屏，显示全黑 */
  
  
  /*画矩形*/

  NT35510_ClearLine(LINE(7));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);

  NT35510_DispStringLine_EN_CH(LINE(7),"画矩形:");

	LCD_SetTextColor(RED);
  NT35510_DrawRectangle(50,300,200,100,1);
	
	LCD_SetTextColor(GREEN);
  NT35510_DrawRectangle(100,300,200,120,0);
	
	LCD_SetTextColor(BLUE);
  NT35510_DrawRectangle(250,300,200,150,1);
  
  
  Delay(0x2FFFFFF);
	
	NT35510_Clear(0,32*7,LCD_X_LENGTH,LCD_Y_LENGTH-32*7);	/* 清屏，显示全黑 */

  /* 画圆 */
  NT35510_ClearLine(LINE(7));	/* 清除单行文字 */
	LCD_SetTextColor(BLUE);
	
  NT35510_DispStringLine_EN_CH(LINE(7),"画圆:");

	LCD_SetTextColor(RED);
	NT35510_DrawCircle(150,400,60,1);

	LCD_SetTextColor(GREEN);
	NT35510_DrawCircle(250,400,60,0);

	LCD_SetTextColor(BLUE);
	NT35510_DrawCircle(350,400,60,1);

  Delay(0x2FFFFFF);
  
  NT35510_Clear(0,32*7,LCD_X_LENGTH,LCD_Y_LENGTH-32*7);	/* 清屏，显示全黑 */

}


/**
  * @brief  简单延时函数
  * @param  nCount ：延时计数值
  * @retval 无
  */	
static void Delay ( __IO uint32_t nCount )
{
  for ( ; nCount != 0; nCount -- );
	
}



/*"当"字符的字模16x16 */ 	  
unsigned char charater_matrix[] =    
{ /*"当",0*/
0x01,0x00,0x21,0x08,0x11,0x08,0x09,0x10,0x09,0x20,0x01,0x00,0x7F,0xF8,0x00,	0x08,
0x00,0x08,0x00,0x08,0x3F,0xF8,0x00,0x08,0x00,0x08,0x00,0x08,0x7F,0xF8,0x00,0x08,

};   
  
/**
  * @brief  使用串口在上位机打印字模
	*					演示字模显示原理
  * @retval 无
  */
void Printf_Charater(void)   
{   
	int i,j;   
	unsigned char kk; 
  
	/*i用作行计数*/
  for ( i=0;i<16;i++)   
	{   
		/*j用作一字节内数据的移位计数*/
		/*一行像素的第一个字节*/
    for(j=0; j<8; j++)   
    {      
			/*一个数据位一个数据位地处理*/
      kk = charater_matrix[2*i] << j ;  //左移J位          
			if( kk & 0x80)     
			{   
				printf("*"); //如果最高位为1，输出*号，表示笔迹
			}   
			else  
			{   
				printf(" "); //如果最高位为0，输出空格，表示空白  
			}   
    }   
		/*一行像素的第二个字节*/
    for(j=0; j<8; j++)   
		{              
      kk = charater_matrix[2*i+1] << j ;  //左移J位   
       
			if( kk & 0x80)         
			{   
				printf("*"); //如果最高位为1，输出*号，表示笔迹   
			}   
			else  
			{   
				printf(" "); //如果最高位为0，输出空格，表示空白 
			}   
		}         
    printf("\n");    //输出完一行像素，换行 
	}   
	printf("\n\n"); 		//一个字输出完毕
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
