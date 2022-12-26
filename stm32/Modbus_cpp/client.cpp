#include <winsock2.h>	//����ͨ��
#include <ws2tcpip.h>	//���ڼ���ip��ַ���º����ͽṹ
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")//����ws2_32.lib�⣬��Ȼ���뱨��
#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define DEFAULT_BUFLEN 512 	//�ַ�����������
#define DEFAULT_IP "127.0.0.1"// ������Ϊ����
#define DEFAULT_PORT "27015" // �����������Ķ˿�


int main() {
	printf("�����ͻ���\n");
	
	#pragma region 1. ��ʼ��
	
	//WSADATA�ṹ�����й�Windows Socketsʵ�ֵ���Ϣ��
	WSADATA wsaData;
	int iResult;	//���
	//Winsock���г�ʼ��
	//���� WSAStartup ����������ʹ�� WS2 _32.dll
	//WSAStartup�� MAKEWORD (2��2) ����������ϵͳ�� Winsock �汾2.2 �����󣬲������ݵİ汾����Ϊ���÷�����ʹ�õ����°汾�� Windows �׽���֧��
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	if (iResult != 0) {
		printf("WSAStartup ʧ��: %d\n", iResult);
		return 1;
	}
	
	#pragma endregion 1. ��ʼ������
	
	#pragma region 2. Ϊ�ͻ��˴����׽���
	
	//��ʼ��֮��ʵ���׽��ֶ��󹩿ͻ���ʹ��
	//�����׽���
	
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	
	// ZeroMemory ���������ڴ������ݳ�ʼ��Ϊ��
	ZeroMemory(&hints, sizeof(hints));
	//addrinfo��getaddrinfo()������ʹ�õĽṹ
	hints.ai_family = AF_INET; //AF _INET ����ָ�� IPv4 ��ַ��
	hints.ai_socktype = SOCK_STREAM;// SOCK _STREAM ����ָ�����׽���
	hints.ai_protocol = IPPROTO_TCP;// IPPROTO _TCP ����ָ�� tcp Э��
	hints.ai_flags = AI_PASSIVE;
	
	// �ӱ����л�ȡip��ַ����ϢΪ��sockcet ʹ��
	//������������ַ�Ͷ˿�
	//getaddrinfo�����ṩ��ANSI����������ַ�Ķ�����Э���ת����
	//����1�����ַ�������һ������(�ڵ�)���ƻ�һ������������ַ�ַ�����
	//����2����������˿ںš�
	// ����3��ָ��addrinfo�ṹ��ָ�룬�ýṹ�ṩ�йص��÷�֧�ֵ��׽������͵���ʾ��
	//����4��ָ��һ����������������Ӧ��Ϣ��addrinfo�ṹ�����ָ�롣
	iResult = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo ʧ��: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	SOCKET ConnectSocket  = INVALID_SOCKET;//�����׽��ֶ���
	
	//�������ӵ����صĵ�һ����ַ��
	ConnectSocket  = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//����Ƿ���ڴ�����ȷ���׽���Ϊ��Ч�׽��֡�
	if (ConnectSocket  == INVALID_SOCKET) {
		//WSAGetLastError�������ϴη����Ĵ���������Ĵ���š�
		printf("�׽��ִ���: %ld\n", WSAGetLastError());
		//���� freeaddrinfo �������ͷ��� getaddrinfo ����Ϊ�˵�ַ��Ϣ������ڴ�
		freeaddrinfo(result);
		WSACleanup();//������ֹ WS2 _ 32 DLL ��ʹ�á�
		return 1;
	}
	
	#pragma endregion 2. Ϊ�ͻ��˴����׽��ֽ���
	
	#pragma region 3. ���ӵ��׽���
	
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		//����getaddrinfo
		//�������ӵ�һ����ַ��ֱ��һ���ɹ�	
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		//����Ƿ���ڴ�����ȷ���׽���Ϊ��Ч�׽��֡�
		if (ConnectSocket == INVALID_SOCKET) {
			//WSAGetLastError�������ϴη����Ĵ���������Ĵ���š�
			printf("socket failed with error: %ld\n", WSAGetLastError());
			//���� freeaddrinfo �������ͷ��� getaddrinfo ����Ϊ�˵�ַ��Ϣ������ڴ�
			freeaddrinfo(result);
			WSACleanup();//������ֹ WS2 _ 32 DLL ��ʹ�á�
			return 1;
		}
		
		//���� connect ���������������׽��ֺ� sockaddr �ṹ��Ϊ�������ݡ�
		//connect����������ָ���׽��ֵ����ӡ�
		//����1����ʶδ�����׽��ֵ���������
		//����2��һ��ָ��Ҫ�������ӵ�sockaddr�ṹ��ָ�롣
		//����3��������ָ���sockaddr�ṹ�ĳ��ȣ����ֽ�Ϊ��λ
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);//�ر�һ���Ѵ��ڵ��׽��֡�
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	//Ӧ�ó���getaddrinfo���ص���һ����ַ,������ӵ���ʧ�ܡ�����������򵥵����ӣ�����ֻ���ͷ���Դ����getaddrinfo���ز���ӡһ��������Ϣ
	freeaddrinfo(result);//�ͷ��� getaddrinfo ����Ϊ�˵�ַ��Ϣ������ڴ�
	
	if (ConnectSocket == INVALID_SOCKET) {
		printf("�����ӵ�����������\n");
		WSACleanup();
		return 1;
	}
	
	#pragma endregion 3. ���ӵ��׽��ֽ���
	
	#pragma region 4.�ڿͻ����Ϸ��ͺͽ�������
	
	//����Ĵ�����ʾ�������Ӻ�ͻ���ʹ�õķ��ͺͽ��չ��ܡ�
	int recvbuflen = DEFAULT_BUFLEN;	//������
	
	const char* sendbuf = "Hello World";
	char recvbuf[DEFAULT_BUFLEN];
	//����һ����ʼ������
	//send��������1����ʶ�������׽��ֵ���������
	//����2��ָ�����Ҫ���͵����ݵĻ�������ָ�롣
	//����3������buf��ָ��Ļ����������ݵĳ���(���ֽ�Ϊ��λ)��strlen��ȡ�ַ�������
	//����4��ָ�����÷�ʽ��һ���־��
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		printf("����ʧ��: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);	//�ر��׽���
		WSACleanup();
		return 1;
	}
	printf("�ֽڷ���: %ld\n", iResult);
	
	//�ر����ڷ��͵����ӣ���Ϊ���ٷ�������
	//�ͻ�����Ȼ����ʹ��ConnectSocket����������
	//shutdown��ֹ�׽����ϵķ��ͻ���չ��ܡ�
	//����1���׽���������
	//����2���ر�������������1����رշ��Ͳ���
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("�ر�ʧ��: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);	//�ر��׽���
		WSACleanup();
		return 1;
	}
	
	//�������ݣ�ֱ���������ر�����
	do {
		//recv�����������ӵ��׽��ֻ��Ѱ󶨵������׽��ֽ������ݡ�
		//����1���׽���������
		//����2����ָ�򻺳�����ָ�룬�����մ������ݡ�
		//����3������buf��ָ��Ļ������ĳ��ȣ����ֽ�Ϊ��λ��
		//����4����Ӱ��˺���Ϊ�ı�־
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("���յ��ֽ���: %d\n", iResult);
		else if (iResult == 0)
			printf("���ӹر�\n");
		else
			printf("����ʧ�ܣ���: %d\n", WSAGetLastError());
	} while (iResult > 0);
	
	#pragma endregion 4.�ڿͻ����Ϸ��ͺͽ������ݽ���
	
	#pragma region 5. �Ͽ�����
	
	//���ַ����Ͽ��ͻ�������
	
	// ����ͷ������Ͽ�����д�����ͬ, �ͻ��˶Ͽ�����д�� ���ͺ� �� ����ǰ
	// shutdown(ConnectSocket, SD_SEND) SD_SEND��ʾsocket�ķ������ݶ���Ȼ�ر�(Ϊ�˷������ͷſͻ���������Դ), ������Ȼ�ܽ��շ���˵�����
	//shutdown��ֹ�׽����ϵķ��ͻ���չ��ܡ�
	//����1���׽���������
	//����2���ر�������������1����رշ��Ͳ���
	//ע�⣺��ʱ�ͻ���Ӧ�ó����Կ������׽����Ͻ������ݡ�
	//iResult = shutdown(ClientSocket, SD_SEND);
	//if (iResult == SOCKET_ERROR) {
	//  printf("shutdown failed: %d\n", WSAGetLastError());
	//  closesocket(ClientSocket);
	//  WSACleanup();
	//  return 1;
	//}
	closesocket(ConnectSocket);
	WSACleanup();
	
	#pragma region 5. �Ͽ����ӽ���
	
	return 0;
}
