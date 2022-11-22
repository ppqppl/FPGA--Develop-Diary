/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stm32f1xx_hal.h"
#include "includes.h"
#include "stdbool.h"
#include "oled.h"
#include "bmp.h"
#include "gui.h"
#include "AHT20.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* �������ȼ� */
#define START_TASK_PRIO		3
#define SHOW_ROLL_TASK_PRIO		4
#define SHOW_AHT20_TASK_PRIO	  5

/* �����ջ��С	*/
#define START_STK_SIZE 		64
#define SHOW_ROLL_STK_SIZE 		64
#define SHOW_AHT20_STK_SIZE 			64//����Ѵ�С����ᱨ���������Ÿ�Сһ��

/* ����ջ */	
CPU_STK START_TASK_STK[START_STK_SIZE];
CPU_STK SHOW_ROLL_TASK_STK[SHOW_ROLL_STK_SIZE];
CPU_STK SHOW_AHT20_TASK_STK[SHOW_AHT20_STK_SIZE];
/* ������ƿ� */
OS_TCB StartTaskTCB;
OS_TCB ShowrollTaskTCB;
OS_TCB Showaht20TaskTCB;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* ���������� */
void start_task(void *p_arg);
static  void  AppTaskCreate(void);
static  void  AppObjCreate(void);
static  void  show_roll(void *p_arg);
static  void  show_aht20(void *p_arg);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//����LED��ʾ���¶Ⱥ�ʪ��
uint32_t CT_data[2]={0,0};	// ���ö�ȡ�¶ȵĲ���
volatile int  c1,t1;
uint8_t temp[10];  
uint8_t hum[10];

void getData(){	// ��ȡ�¶Ȳ�תΪ�ַ�����ʽ���� OLED ������ʾ
	//AHT20_Read_CTdata(CT_data);       //������CRCУ�飬ֱ�Ӷ�ȡAHT20���¶Ⱥ�ʪ������    �Ƽ�ÿ������1S��һ��
		AHT20_Read_CTdata_crc(CT_data);;  //crcУ��󣬶�ȡAHT20���¶Ⱥ�ʪ������ 
		c1 = CT_data[0]*1000/1024/1024;  //����õ�ʪ��ֵc1���Ŵ���10����
		t1 = CT_data[1]*2000/1024/1024-500;//����õ��¶�ֵt1���Ŵ���10����

		//תΪ�ַ���������ʾ
		temp[0]=t1/100+'0';
		temp[1]=(t1/10)%10+'0';
		temp[2]='.';
		temp[3]=t1%10+'0';
		temp[4]='\0';
		
		hum[0]=c1/100+'0';
		hum[1]=(c1/10)%10+'0';
		hum[2]='.';
		hum[3]=c1%10+'0';
		hum[4]=32;
		hum[5]='%';
		hum[6]='\0';
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	OS_ERR  err;
	OSInit(&err);
	
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */


	
	OLED_Init();	// OLED ��ʼ��

	OSTaskCreate((OS_TCB     *)&StartTaskTCB,                /* Create the start task                                */
				 (CPU_CHAR   *)"start task",
				 (OS_TASK_PTR ) start_task,
				 (void       *) 0,
				 (OS_PRIO     ) START_TASK_PRIO,
				 (CPU_STK    *)&START_TASK_STK[0],
				 (CPU_STK_SIZE) START_STK_SIZE/10,
				 (CPU_STK_SIZE) START_STK_SIZE,
				 (OS_MSG_QTY  ) 0,
				 (OS_TICK     ) 0,
				 (void       *) 0,
				 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				 (OS_ERR     *)&err);
	/* ����������ϵͳ������Ȩ����uC/OS-III */
	OSStart(&err);            /* Start multitasking (i.e. give control to uC/OS-III). */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
	
	/* YangJie add 2021.05.20*/
  BSP_Init();                                                   /* Initialize BSP functions */
  //CPU_Init();
  //Mem_Init();                                                 /* Initialize Memory Management Module */

#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  		//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN			//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  		//��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
	/* ����SHOW_ROLL���� */
	OSTaskCreate((OS_TCB 	* )&ShowrollTaskTCB,		
				 (CPU_CHAR	* )"show_roll", 		
                 (OS_TASK_PTR )show_roll, 			
                 (void		* )0,					
                 (OS_PRIO	  )SHOW_ROLL_TASK_PRIO,     
                 (CPU_STK   * )&SHOW_ROLL_TASK_STK[0],	
                 (CPU_STK_SIZE)SHOW_ROLL_STK_SIZE/10,	
                 (CPU_STK_SIZE)SHOW_ROLL_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	/* ����MSG���� */
	OSTaskCreate((OS_TCB 	* )&Showaht20TaskTCB,		
				 (CPU_CHAR	* )"show_aht20", 		
                 (OS_TASK_PTR )show_aht20, 			
                 (void		* )0,					
                 (OS_PRIO	  )SHOW_AHT20_TASK_PRIO,     	
                 (CPU_STK   * )&SHOW_AHT20_TASK_STK[0],	
                 (CPU_STK_SIZE)SHOW_AHT20_STK_SIZE/10,	
                 (CPU_STK_SIZE)SHOW_AHT20_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}
/**
  * ��������: �����������塣
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    ������
  */

// ʵ�ֺ���

static  void  show_roll (void *p_arg)
{
  OS_ERR      err;

  (void)p_arg;

  BSP_Init();                                                 /* Initialize BSP functions                             */
  CPU_Init();

  Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

  CPU_IntDisMeasMaxCurReset();

  AppTaskCreate();                                            /* Create Application Tasks                             */

  AppObjCreate();                                             /* Create Application Objects                           */

		// ��Ļ��ʾ
	OLED_Clear(0);	// �����Ļ
	OSTimeDlyHMSM(0, 0, 1, 0,OS_OPT_TIME_HMSM_STRICT,&err);
	OLED_WR_Byte(0x2E,OLED_CMD); //�رչ���
	OLED_WR_Byte(0x27,OLED_CMD); //ˮƽ��������ҹ��� 26/27
	OLED_WR_Byte(0x00,OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0x00,OLED_CMD); //��ʼҳ 0
	OLED_WR_Byte(0x07,OLED_CMD); //����ʱ����
	OLED_WR_Byte(0x02,OLED_CMD); //��ֹҳ 2
	OLED_WR_Byte(0x00,OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0xFF,OLED_CMD); //�����ֽ�
	GUI_ShowString(40,0,(uint8_t *)"ppqppl",16,1);
	//GUI_ShowCHinese(10,0,16,"���аٻ�������",1);
  while (DEF_TRUE)
  {
		OLED_WR_Byte(0x2F,OLED_CMD); //��������
		OSTimeDlyHMSM(0, 0, 30, 0,OS_OPT_TIME_HMSM_STRICT,&err);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
static  void  show_aht20 (void *p_arg)
{
  OS_ERR      err;

  (void)p_arg;

  BSP_Init();                                                 /* Initialize BSP functions                             */
  CPU_Init();

  Mem_Init();                                                 /* Initialize Memory Management Module                  */

#if OS_CFG_STAT_TASK_EN > 0u
  OSStatTaskCPUUsageInit(&err);                               /* Compute CPU capacity with no task running            */
#endif

  CPU_IntDisMeasMaxCurReset();

  AppTaskCreate();                                            /* Create Application Tasks                             */

  AppObjCreate();                                             /* Create Application Objects                           */
	
/***********************************************************************************/
/**///�ٸ��ϵ磬��ƷоƬ�ڲ�������Ҫʱ�䣬��ʱ100~500ms,����500ms
/***********************************************************************************/
/*	OSTimeDlyHMSM(0, 0, 0, 500,OS_OPT_TIME_HMSM_STRICT,&err);
	AHT20_Init();	// AHT20 ��ʼ��
		OSTimeDlyHMSM(0, 0, 0, 500,OS_OPT_TIME_HMSM_STRICT,&err);
	
  while (DEF_TRUE)
  {
	
			// ��ȡ�¶�
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);	// ʹ�� PC13 �����ϵİ���С���ݽ��в��ԡ���С������
		AHT20_Read_CTdata_crc(CT_data);       //����CRCУ�飬��ȡAHT20���¶Ⱥ�ʪ������    �Ƽ�ÿ������1S��һ��
		c1 = CT_data[0]*1000/1024/1024;  //����õ�ʪ��ֵc1���Ŵ���10����
		t1 = CT_data[1]*2000/1024/1024-500;//����õ��¶�ֵt1���Ŵ���10����
		
		
		
		getData();	// ��ȡ��ʪ��
		
		//��ʾ�¶�
		GUI_ShowCHinese(15,28,16,"�¶�",1);
		GUI_ShowString(47,28,":",16,1);
		GUI_ShowString(62,28,temp,16,1);
		GUI_ShowCHinese(96,28,16,"��",1);

		//��ʾʪ��
		GUI_ShowCHinese(15,48,16,"ʪ��",1);
		GUI_ShowString(47,48,":",16,1);
		GUI_ShowString(62,48,hum,16,1);
		
		OSTimeDlyHMSM(0, 0, 2, 0,OS_OPT_TIME_HMSM_STRICT,&err);
  }*/
  /* USER CODE END 3 */
}

/**
  * ��������: ����Ӧ������
  * �������: p_arg ���ڴ���������ʱ���ݵ��β�
  * �� �� ֵ: ��
  * ˵    ������
  */
static  void  AppTaskCreate (void)
{
  
}


/**
  * ��������: uCOSIII�ں˶��󴴽�
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  */
static  void  AppObjCreate (void)
{

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

#ifdef  USE_FULL_ASSERT
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
