#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <winsock2.h>  
#include <math.h>
#include "stdint.h"
#define length_8 8    //����һ���꣬Ϊ����8λ16�������ĸ���
#define PORT 8001
#define SERVER_IP "123.56.90.74"
#define  BUFFER_SIZE 4196

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



unsigned char *fromhex(const char *str)
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
	// ��ʼ��socket dll��
	WORD winsock_version = MAKEWORD(2,2);
	WSADATA wsa_data;
	if (WSAStartup(winsock_version, &wsa_data) != 0) {
		printf("Failed to init socket!\n");
		return 1;
	}
	
	SOCKET client_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_socket == INVALID_SOCKET) {
		printf("Failed to create server socket!\n");
		return 2;
	}
	
	char recv_data[BUFFER_SIZE+1];
	
	while (true) {
//		uint8_t data[length_8];
		char *data = new char[length_8];
		printf("����ָ�����ʽΪ0+��������ţ�1��2��3��4��5��0300010002");
		printf("������ɼ���������ָ� ����exit�˳���\r\n");
		scanf("%s",data);
		if (strcmp(data,kExitFlag)==0) {
			printf("Exit!\n");
			break;
		}
		uint16_t crc;
		unsigned char * cmd;
		char crc1[8];
		cmd = fromhex(data);
		crc = CRC_16(cmd);
		uint8_t a = 0xFF;
		for(int i=0;i<6;i++){
			//TODO
			crc1[i] = cmd[i];
		}
		crc1[6] = a & crc;
		crc1[7] = (crc >> 8) & a;
		
		struct sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(PORT);
		server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
		
		if (sendto(client_socket, crc1, 8, 0,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0) {
			printf("Failed to send data!\n");
			break;
		}
		
		int ret = recvfrom(client_socket, recv_data, BUFFER_SIZE, 0,NULL,NULL);
		if (ret < 0) {
			printf("Failed to receive data!\n");
			break;
		}
		recv_data[ret]=0; // correctly ends received string
		char var = cmd[5];
		if(var == 2){
			printf("�������������¶ȣ�ʪ��\r\n");
			//TODO
			char yb[4],wd[4];
			for(int i=0;i<4;i++){
				//TODO
				yb[i] = recv_data[4+i];
				wd[i] = recv_data[8+i];
				
			}
			float temp = hexToDec(yb)/100.0;
			float hum = hexToDec(wd);
			printf("�¶ȣ�%4.2f\r\n",temp);
			printf("ʪ�ȣ�%4.2f\r\n",hum);
		}else if(var == 1){
			//TODO
			printf("һ��������������ˮ׼��\r\n");
			char nd[4];
			for(int i=0;i<4;i++){
				//TODO
				nd[i] = recv_data[4+i];
			}
			float water_level = hexToDec(nd)*10.0;
			printf("�Ӷȣ�%6.2f\r\n",water_level);
		}
	}
	
	closesocket(client_socket);
	WSACleanup();
	
	return 0;
}

