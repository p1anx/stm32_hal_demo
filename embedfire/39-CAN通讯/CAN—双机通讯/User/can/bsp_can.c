/**
  ******************************************************************************
  * @file    bsp_debug_usart.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   can����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ��  STM32 F103-�Ե� ������  
  * ��̳    :http://www.firebbs.cn
  * �Ա�    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./can/bsp_can.h"

CAN_HandleTypeDef Can_Handle;
CanTxMsgTypeDef TxMessage;		//���ͻ�����
CanRxMsgTypeDef RxMessage;		//���ջ�����

/*
 * ��������CAN_GPIO_Config
 * ����  ��CAN��GPIO ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_GPIO_Config(void)
{
 	GPIO_InitTypeDef GPIO_InitStruct;   	
  
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
	
	  __HAL_RCC_AFIO_CLK_ENABLE();

    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_CAN1_2();
}

/*
 * ��������CAN_NVIC_Config
 * ����  ��CAN��NVIC ����,��1���ȼ��飬0��0���ȼ�
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_NVIC_Config(void)
{
   /* ������ռ���ȼ��ķ��� */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_1);
	/*�ж����ã���ռ���ȼ�0�������ȼ�Ϊ0*/
	HAL_NVIC_SetPriority(CAN_RX_IRQ, 0 ,0);
	HAL_NVIC_EnableIRQ(CAN_RX_IRQ);
}

/*
 * ��������CAN_Mode_Config
 * ����  ��CAN��ģʽ ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Mode_Config(void)
{
	/************************CANͨ�Ų�������**********************************/
	/* ʹ��CANʱ�� */
  CAN_CLK_ENABLE();
  
  Can_Handle.Instance = CANx;
  Can_Handle.pTxMsg = &TxMessage;
  Can_Handle.pRxMsg = &RxMessage;
	/* CAN��Ԫ��ʼ�� */
	Can_Handle.Init.TTCM=DISABLE;			   //MCR-TTCM  �ر�ʱ�䴥��ͨ��ģʽʹ��
	Can_Handle.Init.ABOM=ENABLE;			   //MCR-ABOM  �Զ����߹��� 
	Can_Handle.Init.AWUM=ENABLE;			   //MCR-AWUM  ʹ���Զ�����ģʽ
	Can_Handle.Init.NART=DISABLE;			   //MCR-NART  ��ֹ�����Զ��ش�	  DISABLE-�Զ��ش�
	Can_Handle.Init.RFLM=DISABLE;			   //MCR-RFLM  ����FIFO ����ģʽ  DISABLE-���ʱ�±��ĻḲ��ԭ�б���  
	Can_Handle.Init.TXFP=DISABLE;			   //MCR-TXFP  ����FIFO���ȼ� DISABLE-���ȼ�ȡ���ڱ��ı�ʾ�� 
	Can_Handle.Init.Mode = CAN_MODE_NORMAL;  //��������ģʽ
	Can_Handle.Init.SJW=CAN_SJW_1TQ;		   //BTR-SJW ����ͬ����Ծ��� 2��ʱ�䵥Ԫ
	 
	/* ss=1 bs1=5 bs2=3 λʱ����Ϊ(1+5+3) �����ʼ�Ϊʱ������tq*(1+3+5)  */
	Can_Handle.Init.BS1=CAN_BS1_5TQ;		   //BTR-TS1 ʱ���1 ռ����5��ʱ�䵥Ԫ
	Can_Handle.Init.BS2=CAN_BS2_3TQ;		   //BTR-TS1 ʱ���2 ռ����3��ʱ�䵥Ԫ	
	
	/* CAN Baudrate = 1 MBps (1MBps��Ϊstm32��CAN�������) (CAN ʱ��Ƶ��Ϊ APB 1 = 36 MHz) */
	Can_Handle.Init.Prescaler =4;		   ////BTR-BRP �����ʷ�Ƶ��  ������ʱ�䵥Ԫ��ʱ�䳤�� 36/(1+5+3)/4=1 Mbps
	HAL_CAN_Init(&Can_Handle);
}

/*
 * ��������CAN_Filter_Config
 * ����  ��CAN�Ĺ����� ����
 * ����  ����
 * ���  : ��
 * ����  ���ڲ�����
 */
static void CAN_Filter_Config(void)
{
	CAN_FilterConfTypeDef  CAN_FilterInitStructure;

	/*CANɸѡ����ʼ��*/
	CAN_FilterInitStructure.FilterNumber=0;						//ɸѡ����0
	CAN_FilterInitStructure.FilterMode=CAN_FILTERMODE_IDMASK;	//����������ģʽ
	CAN_FilterInitStructure.FilterScale=CAN_FILTERSCALE_32BIT;	//ɸѡ��λ��Ϊ����32λ��
	/* ʹ��ɸѡ�������ձ�־�����ݽ��бȶ�ɸѡ����չID�������µľ����������ǵĻ��������FIFO0�� */

	CAN_FilterInitStructure.FilterIdHigh= ((((uint32_t)0x1314<<3)|
										 CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF0000)>>16;		//Ҫɸѡ��ID��λ 
	CAN_FilterInitStructure.FilterIdLow= (((uint32_t)0x1314<<3)|
									     CAN_ID_EXT|CAN_RTR_DATA)&0xFFFF; //Ҫɸѡ��ID��λ 
	CAN_FilterInitStructure.FilterMaskIdHigh= 0x0;//FFFF;			//ɸѡ����16λÿλ����ƥ��
	CAN_FilterInitStructure.FilterMaskIdLow= 0x0;//FFFF;			//ɸѡ����16λÿλ����ƥ��
	CAN_FilterInitStructure.FilterFIFOAssignment=CAN_FILTER_FIFO0 ;	//ɸѡ����������FIFO0
	CAN_FilterInitStructure.FilterActivation=ENABLE;			//ʹ��ɸѡ��
	if(HAL_CAN_ConfigFilter(&Can_Handle,&CAN_FilterInitStructure) != HAL_OK)
	{
		printf("CAN ConfigFilter error!");
	}
}


/*
 * ��������CAN_Config
 * ����  ����������CAN�Ĺ���
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void CAN_Config(void)
{
  CAN_GPIO_Config();
  CAN_NVIC_Config();
  CAN_Mode_Config();
  CAN_Filter_Config();
  Init_RxMes(); 
  if(HAL_CAN_Receive_IT(&Can_Handle, CAN_FIFO0) != HAL_OK)
	{
		printf("CAN Receive IT error!");
	} 	  
}


/**
  * @brief  ��ʼ�� Rx Message���ݽṹ��
  * @param  RxMessage: ָ��Ҫ��ʼ�������ݽṹ��
  * @retval None
  */
void Init_RxMes(void)
{
  uint8_t ubCounter = 0;

  /*�ѽ��սṹ������*/
  Can_Handle.pRxMsg->StdId = 0x00;
  Can_Handle.pRxMsg->ExtId = 0x1314;
  Can_Handle.pRxMsg->IDE = CAN_ID_EXT;
  Can_Handle.pRxMsg->DLC = 0;
  Can_Handle.pRxMsg->FMI = 0;
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    Can_Handle.pRxMsg->Data[ubCounter] = 0x00;
  }
}

/*
 * ��������CAN_SetMsg
 * ����  ��CANͨ�ű�����������,����һ����������Ϊ0-7�����ݰ�
 * ����  �����ͱ��Ľṹ��
 * ���  : ��
 * ����  ���ⲿ����
 */	 
void CAN_SetMsg(void)
{	  
  uint8_t ubCounter = 0;
  Can_Handle.pTxMsg->StdId=0x00;						 
  Can_Handle.pTxMsg->ExtId=0x1314;					 //ʹ�õ���չID
  Can_Handle.pTxMsg->IDE=CAN_ID_EXT;				  //��չģʽ
  Can_Handle.pTxMsg->RTR=CAN_RTR_DATA;				 //���͵�������
  Can_Handle.pTxMsg->DLC=8;							 //���ݳ���Ϊ8�ֽ�
	
  /*����Ҫ���͵�����0-7*/
  for (ubCounter = 0; ubCounter < 8; ubCounter++)
  {
    Can_Handle.pTxMsg->Data[ubCounter] = ubCounter;
  }
}


/**************************END OF FILE************************************/











