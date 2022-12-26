#include <winsock2.h>	//����ͨ��
#include <ws2tcpip.h>	//���ڼ���ip��ַ���º����ͽṹ
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")//����ws2_32.lib�⣬��Ȼ���뱨��
#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define DEFAULT_IP "127.0.0.1"// ������Ϊ����
#define DEFAULT_PORT "27015" //Ĭ�϶˿�
#define DEFAULT_BUFLEN 512 	//�ַ�����������


int main() {
	
	printf("����������!\n");
	
	#pragma region 1. ��ʼ��
	
	WSADATA wsaData;	// ����һ���ṹ���Ա����ŵ��� Windows Socket ��ʼ����Ϣ
	//Winsock���г�ʼ��
	//���� WSAStartup ����������ʹ�� WS2 _32.dll
	int iResult;		// ������������
	//WSAStartup�� MAKEWORD (2��2) ����������ϵͳ�� Winsock �汾2.2 �����󣬲������ݵİ汾����Ϊ���÷�����ʹ�õ���汾�� Windows �׽���֧��
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);	// ��������������Ϊ 0 ��˵���ɹ�����
	
	if (iResult != 0) {	// ���ز�Ϊ 0 ����ʧ��
		printf("��ʼ��Winsock����: %d\n", iResult);
		return 1;
	}
	
	#pragma endregion 1. ��ʼ������
	
	
	#pragma region 2. Ϊ�����������׽���
	
	struct addrinfo* result = NULL,	* ptr = NULL, hints;
	
	ZeroMemory(&hints, sizeof(hints));	// ���ڴ������ݳ�ʼ��Ϊ��
	hints.ai_family = AF_INET; 			//AF _INET ����ָ�� IPv4 ��ַ��
	hints.ai_socktype = SOCK_STREAM;	// SOCK _STREAM ����ָ�����׽���
	hints.ai_protocol = IPPROTO_TCP;	// IPPROTO _TCP ����ָ�� tcp Э��
	hints.ai_flags = AI_PASSIVE;		// ָ�� getaddrinfo ������ʹ�õ�ѡ��ı�־��AI_PASSIVE��ʾ���׽��ֵ�ַ���ڵ��� bindfunction ʱʹ��
	
	// �ӱ����л�ȡ ip ��ַ����ϢΪ�� sockcet ʹ��
	//getaddrinfo �����ṩ�� ANSI ����������ַ�Ķ�����Э���ת����
	//����1�����ַ�������һ������(�ڵ�)���ƻ�һ������������ַ�ַ�����
	//����2����������˿ںš�
	// ����3��ָ�� addrinfo �ṹ��ָ�룬�ýṹ�ṩ�йص��÷�֧�ֵ��׽������͵���ʾ��
	//����4��ָ��һ����������������Ӧ��Ϣ�� addrinfo �ṹ�����ָ�롣
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("������ַ/�˿�ʧ��: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	
	// ����socket����ʹ�����������ͻ�������
	SOCKET ListenSocket = INVALID_SOCKET;
	// socket ���������󶨵��ض�
	//Ϊ����������һ��SOCKET�������ͻ�������
	//socket���������󶨵��ض���������ṩ�ߵ��׽��֡�
	//����1����ַ��淶
	//����2�����׽��ֵ����͹淶
	//����3��ʹ�õ�Э��
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {	//����Ƿ��д�����ȷ���׽���Ϊ��Ч���׽���
		printf("�׽��ִ���: %ld\n", WSAGetLastError());
		freeaddrinfo(result);	 //���� freeaddrinfo �������ͷ��� getaddrinfo ����Ϊ�˵�ַ��Ϣ������ڴ档
		WSACleanup();			//��ֹ WS2 _ 32 DLL ��ʹ��
		return 1;
	}
	
	#pragma endregion 2. �����׽��ֽ���
	
	#pragma region 3. ���׽���
	
	//Ҫʹ���������ܿͻ������ӣ����뽫��󶨵�ϵͳ�е������ַ��
	//Sockaddr�ṹ�����йص�ַ�塢IP ��ַ�Ͷ˿ںŵ���Ϣ��
	//bind���������ص�ַ���׽��ֹ�������������TCP�����׽���
	//����1����ʶδ���׽��ֵ���������
	//2��һ��ָ�򱾵ص�ַsockaddr�ṹ��ָ�룬���ڷ�����󶨵��׽��֡���������Sockaddr�ṹ
	//3����ָ��ֵ�ĳ���(���ֽ�Ϊ��λ)
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("����TCP�����׽���ʧ��: %d\n", WSAGetLastError());
		freeaddrinfo(result);		// ���� bind �����󣬲�����Ҫ��ַ��Ϣ �ͷ�
		closesocket(ListenSocket);	// �ر�һ���Ѵ��ڵ��׽���
		WSACleanup();
		return 1;
	}
	
	#pragma endregion 3. ���׽��ֽ���
	
	#pragma region 4. ���׽����ϼ����ͻ���(�����׽���)
	
	//���׽��ְ󶨵�ϵͳ��ip��ַ�Ͷ˿ں󣬷�����������IP��ַ�Ͷ˿��ϼ����������������
	//listen�������׽������������������ӵ�״̬��
	//����1����ʶ�Ѱ󶨵�δ�����׽��ֵ���������
	//2���������Ӷ��е���󳤶ȡ��������ΪSOMAXCONN�������׽��ֵĵײ�����ṩ�߽��Ѵ�����������Ϊ������ֵ
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		// SOMAXCONN�����˴��׽��������������
		printf("��������ʧ��: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);	// �ر�һ�������ӵ��׽���
		WSACleanup();
		return 1;
	}
	
	#pragma endregion 4. ���׽����ϼ����ͻ���(�����׽���)����
	
	#pragma region 5.�������Կͻ��˵����ӣ�Windows ��� 2��
	
	//���׽��ּ������Ӻ󣬳�����봦���׽����ϵ���������
	//������ʱ�׽��ֶ����Խ������Կͻ��˵�����
	SOCKET ClientSocket;
	
	//ͨ����������Ӧ�ó��򽫱����Ϊ�������Զ���ͻ��˵����ӡ� ���ڸ����ܷ�������ͨ��ʹ�ö���߳����������ͻ������ӡ� ���ʾ���Ƚϼ򵥣����ö��߳�
	
	ClientSocket = INVALID_SOCKET; //INVALID_SOCKET����������׽�����Ч
	//accept���������׽����ϵĴ������ӳ���
	//����1��һ����������������ʶһ���׽��֣����׽���ʹ��listen������������״̬������ʵ��������accept���ص��׽��ֽ����ġ�
	//2��һ�ֿ�ѡ��ָ�򻺳�����ָ�룬���ڽ���ͨ�Ų���֪������ʵ��ĵ�ַ��addr������ȷ�и�ʽ���ɵ�socket����soʱ�����ĵ�ַ�������
	//3:һ����ѡ��ָ�룬ָ��һ������������������addr������ָ��Ľṹ�ĳ��ȡ�
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("��������ʧ��: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	/*ע�⣺���ͻ������ӱ����ܺ󣬷�����Ӧ�ó���ͨ���Ὣ���ܵĿͻ����׽��ִ��� (ClientSocket ����) �������̻߳� i/o ��ɶ˿ڣ������������������ӡ�
	���ʾ��û�У����Բ鿴Microsoft Windows ����������߰� (SDK) ������ �߼� Winsock ʾ�� �н��������в��ֱ�̼�����ʾ���� 
	���ӣ�https://docs.microsoft.com/zh-cn/windows/win32/winsock/getting-started-with-winsock*/
	#pragma endregion 5.�������Կͻ��˵�����(Windows ��� 2)����
	
	#pragma region 6. �ڷ������Ͻ��պͷ�������
	
	char recvbuf[DEFAULT_BUFLEN]; 		//�ַ�����������
	int  iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;	//����ֵ
	
	do {
		//recv�����������ӵ��׽��ֻ��Ѱ󶨵��������׽��ֽ������ݡ�
		//����1���׽���������
		//����2��һ��ָ�򻺳�����ָ�룬�������մ�������ݡ�
		//����3������buf��ָ��Ļ������ĳ��ȣ����ֽ�Ϊ��λ��
		//����4��һ��Ӱ��˺�����Ϊ�ı�־
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("���յ��ֽ���: %d\n", iResult);
			//���������ش������ͷ�
			//����һ����ʼ������
			//send��������1����ʶ�������׽��ֵ���������
			//����2��ָ�����Ҫ���͵����ݵĻ�������ָ�롣����Ϊ�˼򵥽��ͻ��˷��͹�������Ϣ�ٷ��͸��ͻ��� 
			//����3������buf��ָ��Ļ����������ݵĳ���(���ֽ�Ϊ��λ)��strlen��ȡ�ַ�������
			//����4��ָ�����÷�ʽ��һ���־��
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("����ʧ��: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("�ֽڷ���: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("���ӹر�...\n");
		else {
			printf("����ʧ��: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}
	} while (iResult > 0);
	
	#pragma endregion 6. �ڷ������Ͻ��պͷ������ݽ���
	
	#pragma region 7. �Ͽ�����������
	
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("�ر�ʧ��: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	
	/*�ڶ��ֹرշ���
	ʹ�� Windows �׽��� DLL ��ɿͻ���Ӧ�ó���ʱ�������� WSACleanup �������ͷ���Դ��
	closesocket(ClientSocket);
	WSACleanup();*/
	
	#pragma endregion 7. �Ͽ����������ӽ���
	
	return 0;
}
