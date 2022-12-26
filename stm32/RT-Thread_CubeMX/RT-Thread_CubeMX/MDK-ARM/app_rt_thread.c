#include "rtthread.h"
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "stdio.h"
#include "AHT20.h"
 
struct rt_thread led1_thread;
struct rt_thread usart1_thread;
rt_uint8_t rt_led1_thread_stack[128];
rt_uint8_t rt_usart1_thread_stack[256];
void led1_task_entry(void *parameter);
void usart1_task_entry(void *parameter);

int fputc(int ch,FILE *f)
{
    HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);    
		//�ȴ����ͽ���	
		while(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_TC)!=SET){
		}		
    return ch;
}
 
//��ʼ���̺߳���
void MX_RT_Thread_Init(void)
{
	//��ʼ��LED1�߳�
	rt_thread_init(&led1_thread,"led1",led1_task_entry,RT_NULL,&rt_led1_thread_stack[0],sizeof(rt_led1_thread_stack),3,20);
	//�����̵߳���
	rt_thread_startup(&led1_thread);
	//��ʼ��USART1�߳�
	rt_thread_init(&usart1_thread,"usart1",usart1_task_entry,RT_NULL,&rt_usart1_thread_stack[0],sizeof(rt_usart1_thread_stack),3,20);
	//�����̵߳���
	rt_thread_startup(&usart1_thread);
}
 
//������
void MX_RT_Thread_Process(void)
{
	printf("Hello RT_Thread!!!\r\n");
	rt_thread_delay(100000);
}
 
//LED1����
void led1_task_entry(void *parameter)
{
	while(1)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_RESET);
		rt_thread_delay(500);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_SET);
		rt_thread_delay(500);
	}
}
//��ȡ�¶�����
void usart1_task_entry(void *parameter)
{
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
	uint32_t CT_data[2]={0,0};	//
	volatile int  c1,t1;
	rt_thread_delay(50);
	AHT20_Init();
	rt_thread_delay(2500);
	
	while(1)
	{
		AHT20_Read_CTdata_crc(CT_data);       //����CRCУ�飬��ȡAHT20���¶Ⱥ�ʪ������    �Ƽ�ÿ������1S��һ��
		c1 = CT_data[0]*1000/1024/1024;  //����õ�ʪ��ֵc1���Ŵ���10����
		t1 = CT_data[1]*2000/1024/1024-500;//����õ��¶�ֵt1���Ŵ���10����
		printf("���ڼ��");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		printf("\r\n");
		printf("�¶�:%d%d.%d",t1/100,(t1/10)%10,t1%10);	// ������Ҫ���¶Ƚ��м������ܵõ�������Ҫ���¶�ֵ
		printf("ʪ��:%d%d.%d",c1/100,(c1/10)%10,c1%10); // ����ͬ����Ҫ���ʶȽ��м���
		printf("\r\n");
		printf("�ȴ�");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		rt_thread_delay(100);
		printf(".");
		printf("\r\n");
	}
}
