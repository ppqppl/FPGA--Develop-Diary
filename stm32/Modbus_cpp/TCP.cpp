#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>  
#include <math.h>
#include "stdint.h"
#define length_8 8    //����һ���꣬Ϊ����8λ16�������ĸ���
#define PORT 8002
#define SERVER_IP "123.56.90.74"
#define BUFFER_SIZE 4196

const char* kExitFlag = "exit";

/* ����ch�ַ���sign�����е���� */
int getIndexOfSigns(char ch)
{
	if(ch >= '0' && ch <= '9')
	{
		return ch - '0';
	}
	if(ch >= 'A' && ch <='F') 
	{
		return ch - 'A' + 10;
	}
	if(ch >= 'a' && ch <= 'f')
	{
		return ch - 'a' + 10;
	}
	return -1;
}
/* ʮ��������ת��Ϊʮ������ */
int hexToDec(char *source)
{
	int sum = 0;
	char low,high;
	for(int i=0,j=7;i<4;i++){
		//TODO
		high = (source[i] & 0xf0)>>4;
		low = source[i] & 0x0f;
		sum += high*pow(16,j--)+low*pow(16,j--); 
	}
	return sum;
}


unsigned char *fromhex(char *str)
{
	static unsigned char buf[512];
	size_t len = strlen(str) / 2;
	if (len > 512) len = 512;
	for (size_t i = 0; i < len; i++) {
		unsigned char c = 0;
		if (str[i * 2] >= '0' && str[i*2] <= '9') 
			c += (str[i * 2] - '0') << 4;
		if ((str[i * 2] & ~0x20) >= 'A' && (str[i*2] & ~0x20) <= 'F') 
			c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
		if (str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9') 
			c += (str[i * 2 + 1] - '0');
		if ((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
			c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
		buf[i] = c;
	}
	return buf;
}

uint16_t CRC_16(uint8_t *temp)
{
	uint8_t i,j;
	uint16_t CRC_1 = 0xFFFF;          //����CRC�Ĵ�����Ҳ���ǲ���1
	for(i = 0;i < 6;i++)       //�����forѭ��˵���ǲ���6�е��ظ����� 2 ������ 5
	{
		CRC_1 ^= temp[i]; //������ǲ���2�������������
		for(j = 0;j < 8;j++)         //����������ĵͰ�λȫ���Ƴ���forѭ��
		{
			if(CRC_1 & 0x01)         //�жϵͰ�λ�����һλ�Ƿ�Ϊ1��Ϊ1ʱִ��������䣬Ҳ���ǲ���3˵����λ�ж��벽��5˵������8��
			{
				/*һ��Ҫ����λ�������*/
				CRC_1 >>=1;          //��λ������򣬾��ǲ���4
				CRC_1 ^= 0xA001;     //0xA001Ϊ0x8005������
			}
			else                    //����Ϊ1����ֱ����λ��
			{
				CRC_1 >>=1;
			}
		}
	}
	
	//	CRC_1 = (((CRC_1 & 0xFF)<<8) + (CRC_1>>8));
	//	printf("%04x\r\n",CRC_1);     //���ڴ�ӡ���CRCУ����
	return(CRC_1);
}

int main() {
	printf("����TCP���ӣ�\n");
	// ��ʼ��socket dll��
	WORD winsock_version = MAKEWORD(2,2);
	//WSADATA�ṹ�����й�Windows Socketsʵ�ֵ���Ϣ��
	WSADATA wsa_data;
	//Winsock���г�ʼ��
	//���� WSAStartup ����������ʹ�� WS2 _32.dll
	//WSAStartup�� MAKEWORD (2��2) ����������ϵͳ�� Winsock �汾2.2 �����󣬲������ݵİ汾����Ϊ���÷�����ʹ�õ����°汾�� Windows �׽���֧��
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (iResult != 0) {
		printf("WSAStartup ʧ��!\n");
		WSACleanup();
		return 1;
	}
	
	// socket ���������󶨵��ض�
	//Ϊ����������һ��SOCKET�������ͻ�������
	//socket���������󶨵��ض���������ṩ�ߵ��׽��֡�
	//����1����ַ��淶
	//����2�����׽��ֵ����͹淶
	//����3��ʹ�õ�Э��
	SOCKET client_socket = INVALID_SOCKET;
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == INVALID_SOCKET) {
		printf("�׽��ִ���\n");
		WSACleanup();
		return 2;
	}
	
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	//�������ӵ�һ����ַ��ֱ��һ���ɹ�	
	if (connect(client_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		printf("Failed to connect server: %ld !\n", GetLastError());
		closesocket(client_socket);//�ر�һ���Ѵ��ڵ��׽��֡�
		client_socket = INVALID_SOCKET;
		return 3;
	}
	
	char recv_data[BUFFER_SIZE+1];
	while (true) {
		char *data = new char[length_8];	// Ҫ���������ָ������
		printf("����ָ�����ʽΪ0+��������ţ�1��2��3��4��5��+0300010002��������ɼ��������ı�ţ�0��ʾ�˳��ɼ�����\n");
		scanf("%s",data);
		if (strcmp(data,"0")==0) {	// �����˳�
			printf("Exit!\n");
			break;
		}
		uint16_t crc;	// CRC У������
		unsigned char * cmd;	// Ҫ��������Ŀ���̨shuju
		char crc1[8];
		cmd = fromhex(data);	// ��ȡ����
		crc = CRC_16(cmd);		// ���� CRC У��
		uint8_t a = 0xFF;
		for(int i=0;i<6;i++){
			//TODO
			crc1[i] = cmd[i];
		}
		crc1[6] = a & crc;	// ���������λΪ CRC У��λ
		crc1[7] = (crc >> 8) & a;
		
		if (send(client_socket, crc1, 8, 0) < 0) {
			printf("����ʧ��!\n");
			break;
		}
		
		int ret = recv(client_socket, recv_data, BUFFER_SIZE, 0);
		if (ret < 0) {
			printf("����ʧ��!\n");
			break;
		}
		recv_data[ret]=0; // ��ȷ�����յ����ַ���
		char yb[4],wd[4];
		for(int i=0;i<4;i++){
			//TODO
			yb[i] = recv_data[4+i];
			wd[i] = recv_data[8+i];
			
		}
		float mic = hexToDec(yb)/100.0;		// �������ǻ�ȡ����������ʮ�����ƣ�������Ҫ���н���ת��
		float strain_temp = hexToDec(wd)/100.0;
		printf("Ӧ�䣺%f\r\n",mic);
		printf("�¶ȣ�%f\r\n",strain_temp);
		
		
		//printf("Receive data from server: \"%x\"\n",recv_data);
	}
	
	closesocket(client_socket);
	WSACleanup();
	
	return 0;
}



