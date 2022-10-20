#include "stm32f10x_usart.h"
 
#define Fck 100000000			//����ʱ��Ƶ��
 
char RX_BUFF[50]={0};
u8 Usart1_flag = 0;
 
//A9 TX	A10 RX
//PA9 ������� �ٶ�25MHZ
//PA10��������
	
void Usart1_Init(u32 bound)
{
		
	RCC->APB2ENR |= 1<<4;   //ʹ�ܴ���1ʱ��
	RCC->AHB1ENR |= 1<<0;   //ʹ��A��ʱ��
	
	/*��������*/
	GPIOA->MODER &= ~(0xf <<9*2 );	//��0
	GPIOA->MODER |= (0xa <<9*2 ); //���� 1010
	
	GPIOA->OTYPER &=~( 1<<9);     //A9����
	GPIOA->OSPEEDR &= ~(3 << 9*2);
	GPIOA->OSPEEDR |=(1 << 9*2);  //A9 25MHZ
	GPIOA->PUPDR &= ~( 3<<2*10);
	GPIOA->PUPDR |=( 1<<2*10);     //A10���� 01
	
	GPIOA->AFR[1] |= (0x77 << 4);   //0111 0111��λ  ����ΪAF7 usart1
	
	
	USART1->BRR =  Fck/bound;		//������
	
	USART1->CR1 |= 3 << 2;				//ʹ�ܷ��ͽ��չ���
	USART1->CR1 |= (1 << 13) |(1 << 5); 	//ʹ�� ���������ж�
 
	
//	NVIC_SetPriorityGrouping(7-3);//��ռռ3λ��0~7������Ӧ1λ��0~1��//���ȼ��������ã�������������
	NVIC_EnableIRQ((IRQn_Type)37);    //ʹ�ܴ����ж�
	NVIC_SetPriority((IRQn_Type)37, NVIC_EncodePriority (7-2, 0, 1));//������ռ0����Ӧ���ȼ�1
	
	
}
 
//����һ���ֽ�����
void Usart1_SendData(char ch)
{
	while( !(USART1->SR & (1<<6)));	//�ȴ����ݷ������
	USART1->DR = ch;
}
 
//�����ַ���
void Usart1_Send_Srting(char *str)
{
	char i=0;
	while( *(str + i)!= '\0')
	{
		Usart1_SendData(*(str + i));
		i++;
	}
}
 
 
//��ӳ��fputc����
//����ֱ��ʹ��C��printf����
//��Ҫ���<stdio.h>ͷ�ļ�
//����ѡMDK��Use MicroLIBѡ�ħ�����д򿪣�
int fputc(int ch,FILE *stream)
{
	Usart1_SendData(ch);
	return 0;
}
 
 
 
//����һ���ֽ�����
char Usart1_ReceiveData(void)
{
	char ch=0;
	if( USART1->SR & (1<<5))	//���յ�����
	{
		ch = USART1->DR;
		return ch;
	}
	return 0;
}
 
 
 
//�жϽ��պ���
void USART1_IRQHandler(void)
{
	char ch = 0;
	static u8 count = 0;
	
	if( USART1->SR & (1<<5))	//���յ�����
	{
		ch = USART1->DR;
		if( (ch != '\r') && (ch != '\n') )  //���ǻس�����δ������������ʱ��Ҫ��ӻس�
		{
			RX_BUFF[count++] = ch;
		}
		else 
		{
			RX_BUFF[count] = '\0'; //��ӽ�����
			Usart1_flag = 1;	//��ǽ��ս���
			count = 0;
		}
	}
}
 
 int main(void){
	 
 }