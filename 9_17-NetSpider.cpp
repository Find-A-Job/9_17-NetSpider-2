// 9_17-NetSpider.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Macro.h"
#include "func_zmx.h"

#pragma comment(lib, "ws2_32.lib")

/*�����̺�������*/

int MainCommandLine(int , TCHAR **, TCHAR *);
int MainAnlyURL(TCHAR *, StructURL *, StructStore *);
int MainDomainToIp(StructURL *);
int MainFilePathSet(StructStore *);
int MainMakeRequestHead(StructURL *, char *);
int MainConnectSendRecv(StructURL *, char *, StructStore *);

/*������������*/
int sendRecvSave(SOCKET, char *, StructStore *);			//


int _tmain(int argc, TCHAR **argv)
{
	/*�����ڲ�����*/
	TCHAR URL[URLSIZE] = { NULL };
	TCHAR msg_t[MSGSIZE] = { NULL };
	PStructURL psu=(PStructURL)malloc(sizeof(StructURL));		//����
	PStructStore pss = (PStructStore)malloc(sizeof(StructStore));
	char requestHead[MSG2048] = { NULL };
	TCHAR fileName[FILEPATHSIZE] = { NULL };
	char msg[MSGSIZE] = { NULL };
	size_t returnValue = 0;
	char ip[IPSIZE] = { NULL };
	char port[PORTSIZE] = { NULL };

	/*...*/
	ZeroMemory(psu, sizeof(PStructURL));
	ZeroMemory(pss, sizeof(PStructStore));

	/*���������в���*/
	MainCommandLine(argc, argv, URL);

	/*����URL*/
	MainAnlyURL(URL, psu, pss);

	/*��ѯ������תΪip��ַ*/
	MainDomainToIp(psu);

	/*�ļ�·������*/
	MainFilePathSet(pss);

	/*��������ͷ*/
	MainMakeRequestHead(psu, requestHead);

	/*���ӣ���������ͷ�����ղ��������Ӧͷ����Ӧ��*/
	MainConnectSendRecv(psu, requestHead, pss);

	//�ͷ�
	free(psu);
	free(pss);
	system("pause");
    return 0;
}

/*�����̺�������*/

/*
*�޸�ʱ�䣺2018-9-18
*���ߣ�zmx
*������argc:�����в���������argv:�����в���ָ�룬URL:���URL��ַ��'\0'��β
*���ܣ�������������ȡ�ؼ���Ϣ��Ŀǰ��URL��ȡ
*��ע�������޸�ʱ�䣺2018-9-20,�޸����ݣ�����c�����Դ��������п��ֽ�ת���ֽ�
*/
int MainCommandLine(int argc, TCHAR **argv, TCHAR *URL) {
	/*�ڲ�����*/
	char msg[MSGSIZE] = { NULL };
	size_t returnValue = 0;

	/*...*/
	switch (argc) {
	case 1: {
		printf("������URL����\n");
	}break;
	case 2: {
		_stprintf_s(URL, URLSIZE, _T("%s"), argv[1]);
		wcstombs_s(&returnValue, msg, _countof(msg), URL, _countof(msg));		//���ֽ�ת���ֽ�
		printf("%s\n", msg);
	}break;
	default: {
		printf("δ�������\n");
	}
	}

	printf("�����в����������\n");
	return 0;
}
/*
*�޸�ʱ�䣺2018-9-18
*���ߣ�zmx
*������URL:����URL�ַ������飬'\0'��β, psu:��ֵ�ṹ��
*���ܣ�����URL����ֵ���ṹ��psu
*��ע�������޸�ʱ�䣺2018-9-19���޸����ݣ�1.������ȡ�������ֵĹ��ܡ�2.��ȡ������dir��û��ǰ׺'/'�������Ժ�׺'/'��β
*		�����޸�ʱ�䣺2018-9-21���޸����ݣ������β�pss���ж��ļ���׺��2.wcstombs_s()����ת����ֵ���ַ���ʱ�����׶�ʧ'\0'������֣���Ķ��ṹ��
*/
int MainAnlyURL(TCHAR *URL, StructURL *psu, StructStore *pss) {
	/*�ڲ�����*/
	TCHAR msg_t[URLSIZE] = { NULL };
	int coor_b = 0;							//�ؼ�������λ�ÿ�ʼ�±�
	int coor_e = 0;							//�ؼ�������λ�ý����±�
	bool spec = FALSE;						//���������ʶ

	/*...*/
	ZeroMemory(psu, sizeof(StructURL));		//��ʼ���ṹ���ڴ�
	ZeroMemory(pss, sizeof(PStructStore));

	/*��ȡЭ�鲿��*/
	coor_b = 0;
	coor_e = 0;
	for (int i = 0; URL[i] != NULL; ++i) {
		if (':' == URL[i] && '/' == URL[i + 1]) {		//�ж����ݣ�Э��������://Ϊ�ָ���http://www.baidu.com/
			coor_e = i;
			break;
		}
	}
	if (coor_e != 0) {
		for (int i = coor_b; i < coor_e; ++i) {
			(psu->protocalType)[i] = URL[i];
			if (i == coor_e - 1) {
				(psu->protocalType)[i + 1] = '\0';
			}
		}
	}
	else {								//δ�ҵ�����Ĭ��Ϊhttp
		(psu->protocalType)[0] = 'h';
		(psu->protocalType)[1] = 't';
		(psu->protocalType)[2] = 't';
		(psu->protocalType)[3] = 'p';
		(psu->protocalType)[4] = '\0';
	}
	/*��ȡ��������*/
	coor_b = 0;
	coor_e = 0;
	for (int i = 0; URL[i] != '\0'; ++i) {
		if ('/' == URL[i] && '/' == URL[i + 1]) {		//�ж����ݣ�����������://Ϊ��ʼ��':''/''\0'���ֱ�ʶΪ������http://www.baidu.com/
			coor_b = i + 2;
			i = i + 1;
			continue;
		}
		if (coor_b != 0) {
			if (':' == URL[i] || '/' == URL[i]) {
				coor_e = i;
				break;
			}
			if ('\0' == URL[i + 1]) {
				coor_e = i + 1;
				break;
			}
		}
	}
	if (coor_e != 0) {
		for (int i = coor_b; i < coor_e; ++i) {
			(psu->domain)[i - coor_b] = URL[i];
			if (i == coor_e - 1) {
				(psu->domain)[i - coor_b + 1] = '\0';
			}
		}
	}
	else {
		(psu->domain)[0] = '\0';
	}
	/*��ȡ�˿ڲ���*/
	coor_b = 0;
	coor_e = 0;
	for (int i = 0; URL[i] != '\0'; ++i) {
		if (':' == URL[i] && URL[i + 1] != '/') {
			coor_b = i + 1;
			continue;
		}
		if (coor_b != 0) {
			if ('/' == URL[i]) {
				coor_e = i;
				break;
			}
			if ('\0' == URL[i + 1]) {
				coor_e = i + 1;
				break;
			}
		}
	}
	if (coor_e != 0) {
		for (int i = coor_b; i < coor_e; ++i) {
			(psu->port)[i - coor_b] = URL[i];
			if (i == coor_e - 1) {
				(psu->port)[coor_e - coor_b] = '\0';
			}
		}
	}
	else {							//Ĭ��Ϊ80�˿�
		(psu->port)[0] = '8';
		(psu->port)[1] = '0';
		(psu->port)[2] = '\0';
	}
	/*��ȡ�ļ��в���*/
	coor_b = 0;
	coor_e = 0;
	spec = FALSE;
	for (int i = 0; URL[i] != '\0'; ++i) {
		if (0 == coor_b) {
			if ('/' == URL[i] && '/' != URL[i + 1] && '/' != URL[i - 1]) {		//�ж����ݣ��ļ��в�����/Ϊ��ʼ��'/'Ϊ������http://www.baidu.com/
				coor_b = i + 1;
				coor_e = i + 1;
				continue;
			}
		}
		else {
			if ('/' == URL[i] && '/' != URL[i + 1]) {
				coor_e = i;
				continue;
			}
			if ('.' == URL[i]) {							//��ȡ��׺
				for (int j = i + 1; URL[j] != '\0';++j) {
					pss->fileBodyType[j - i - 1] = URL[j];
					if ('\0' == URL[j + 1]) {
						pss->fileBodyType[j - i] = '\0';
						break;
					}
				}
				break;
			}
			if ('?' == URL[i] || '#' == URL[i]) {			//�����ļ�������, �������֣� ê���֣��˳�ѭ��
				break;
			}
			if ('\0' == URL[i + 1]) {
				coor_e = i + 1;
				spec = TRUE;			//�����������
				break;
			}
		}
	}
	if (0 != coor_b) {
		for (int i = coor_b; i <= coor_e; ++i) {
			(psu->dirPath)[i - coor_b] = URL[i];
			if (i == coor_e) {
				if (TRUE == spec) {
					(psu->dirPath)[i - coor_b] = '/';
					(psu->dirPath)[i - coor_b + 1] = '\0';
					spec = FALSE;
				}
				else {
					(psu->dirPath)[i - coor_b] = '\0';
				}
			}
		}
	}
	else {
		(psu->dirPath)[0] = '\0';
	}
	/*��ȡ�ļ�������*/
	coor_b = 0;
	coor_e = 0;
	for (int i = 0; URL[i] != '\0'; ++i) {
		if ('/' == URL[i] && '/' != URL[i + 1]) {		//�ж����ݣ��ļ���������/Ϊ��ʼ, ��'.'��'?''#''\0'Ϊ������http://www.baidu.com/
			coor_b = i + 1;
			continue;
		}
		if (coor_b != 0) {
			if ('?' == URL[i] || '#' == URL[i]) {
				coor_e = i;
				break;
			}
			if ('\0' == URL[i + 1]) {
				coor_e = i + 1;
				break;
			}
		}
	}
	if (coor_b != 0) {		
		if (coor_e != 0) {
			bool bfile = FALSE;
			for (int i = coor_b; i < coor_e; ++i) {
				(psu->file)[i - coor_b] = URL[i];
				if ('.' == URL[i]) {
					bfile = TRUE;
				}
				if (i == coor_e - 1) {
					(psu->file)[i - coor_b + 1] = '\0';
				}
			}
			if (FALSE == bfile) {
				(psu->file)[0] = '\0';
			}
		}
	}
	else {					//δ������ʼ��ʶ
		(psu->file)[0] = '\0';
	}
	//�ļ������ж�
	if ((psu->file)[0] != '\0') {
		_stprintf_s(pss->fileHeadType, _countof(pss->fileHeadType), _T("txt"));
	}
	else {
		_stprintf_s(pss->fileHeadType, _countof(pss->fileHeadType), _T("txt"));
		_stprintf_s(pss->fileBodyType, _countof(pss->fileBodyType), _T("html"));
	}

	printf("URL�������\n");
	return 0;
}

/*
*�޸�ʱ�䣺2018-9-19
*���ߣ�zmx
*������ psu:��ֵ�ṹ��
*���ܣ���ѯ������תΪip��ַ
*��ע��getaddrinfo()������Ҫ����wsastartup()
*		2.printfע�����������ʽת��������Լ���ʾת��һ�¡�printf("%u\t", (UCHAR)ip[i]);
*/
int MainDomainToIp(StructURL *psu) {
	/*�ڲ�����*/
	TCHAR msg_t[MSGSIZE] = { NULL };
	char domain[DOMAINSIZE] = { NULL };
	char ip[IPSIZE] = { NULL };
	char port[PORTSIZE] = { NULL };
	char msg[MSGSIZE] = { NULL };
	size_t returnValue = 0;
	int iFuncStat = 0;
	WORD sockVer = 0;
	WSADATA wsadata = { NULL };
	ADDRINFOA ai = { NULL };
	PADDRINFOA pai = NULL;

	/*...*/
	sockVer = MAKEWORD(2, 2);
	if (WSAStartup(sockVer, &wsadata) != 0) {
		printf("WSAStartup����ʧ�ܣ�%d\n", WSAGetLastError());
		system("pause");
		exit(1);
	}
	wcstombs_s(&returnValue, domain, _countof(domain), psu->domain, _countof(domain));
	wcstombs_s(&returnValue, port, _countof(port), psu->port, _countof(port));
	ZeroMemory(&ai, sizeof(ai));
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_family = AF_INET;
	iFuncStat = getaddrinfo(domain, port, &ai, &pai);	//getaddrinfo()DNS��ѯ����
	if ( iFuncStat!= 0) {		
		printf("����תipʧ��, ������룺%d\n", iFuncStat);
		system("pause");
		exit(1);
	}
	psu->ul_ip = ((sockaddr_in *)(pai->ai_addr))->sin_addr.S_un.S_addr;
	psu->us_port = ((sockaddr_in *)(pai->ai_addr))->sin_port;;

	freeaddrinfo(pai);						//�ͷ���getaddrinfo()�ڲ�ΪPADDRINFOA��̬������ڴ�
	WSACleanup();
	printf("����תip���\n");
	return 0;
}

/*
*�޸�ʱ�䣺2018-9-21
*���ߣ�zmx
*������pss:�ļ�·���ṹ��
*���ܣ������ļ�·��
*��ע��
*/
int MainFilePathSet(StructStore *pss) {
	/*�ڲ�����*/
	time_t time_now = 0;
	char requestHead[FILEPATHSIZE] = { NULL };
	char requestBody[FILEPATHSIZE] = { NULL };

	/*...*/
	CreateDirectory(_T("Data"), NULL);
	time(&time_now);
	_stprintf_s(pss->requestHead, _countof(requestHead), _T("Data/%llu.%s"), time_now, pss->fileHeadType);
	_stprintf_s(pss->requestBody, _countof(requestBody), _T("Data/%llu.%s"), time_now, pss->fileBodyType);

	return 0;
}

/*
*�޸�ʱ�䣺2018-9-18
*���ߣ�zmx
*������psu:URL���ݽṹ�壬requestHead:���������ɵ�����ͷ
*���ܣ���������ͷ��
*��ע��
*/
int MainMakeRequestHead(StructURL *psu, char *requestHead) {
	/*�ڲ�����*/
	char send_head[MSGSIZE] = { NULL };			//����ͷ
	char send_host[MSGSIZE] = { NULL };			//host�ֶ�
	char send_agent[MSGSIZE] = { NULL };		//agent�ֶ�
	char send_language[MSGSIZE] = { NULL };		//language
	char send_encoding[MSGSIZE] = { NULL };		//ѹ����ʽ
	char send_connection[MSGSIZE] = { NULL };	//���ӷ�ʽ
	char send_cache[MSGSIZE] = { NULL };		//����
	char send_ending[MSGSIZE] = { NULL };		//������־

	char dir[DIRSIZE] = { NULL };
	char file[FILESIZE] = { NULL };
	char domain[DOMAINSIZE] = { NULL };

	/*...*/
	//TCHAR ת char
	TcharToChar(psu->dirPath, _tcslen(psu->dirPath), dir);
	TcharToChar(psu->file, _tcslen(psu->file), file);
	TcharToChar(psu->domain, _tcslen(psu->domain), domain);

	sprintf_s(send_head, _countof(send_head), "GET /%s%s HTTP/1.1\r\n", dir, file);
	sprintf_s(send_host, _countof(send_host), "Host: %s\r\n", domain);
	sprintf_s(send_agent, _countof(send_agent), "User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:60.0) Gecko/20100101 Firefox/60.0\r\nAccept: */*\r\n");
	sprintf_s(send_language, _countof(send_language), "Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2\r\n");
	sprintf_s(send_encoding, _countof(send_encoding), "Accept-Encoding:identity\r\n");
	sprintf_s(send_connection, _countof(send_connection), "Connection: keep-alive\r\n");
	sprintf_s(send_cache, _countof(send_cache), "Cache-Control: max-age=0\r\n");
	sprintf_s(send_ending, _countof(send_ending), "\r\n");

	sprintf_s(requestHead, MSG2048, "%s%s%s%s%s%s%s%s", send_head, send_host, send_agent, send_language, send_encoding, send_connection, send_cache, send_ending);

	printf("����ͷ�������\n");
	return 0;
}
/*
*�޸�ʱ�䣺2018-9-18
*���ߣ�zmx
*������psu:URL�����ṹ��,requestHead:����ͷ,pss:�ļ���Ϣ
*���ܣ�����socket����ͨ�ţ�����URL����������ͷ��������Ӧ����
*��ע��
*/
int MainConnectSendRecv(StructURL *psu, char *requestHead, StructStore *pss) {
	/*�ڲ�����*/
	TCHAR msg_t[MSGSIZE] = { NULL };
	char ip[IPSIZE] = { NULL };
	char port[PORTSIZE] = { NULL };
	SOCKET cSock = NULL;
	size_t returnValue = 0;
	sockaddr caddre;

	/*...*/
	//socket��ʼ��
	WORD sockVer = MAKEWORD(2, 2);
	WSADATA wsadata;
	if (WSAStartup(sockVer, &wsadata) != 0) {
		_stprintf_s(msg_t, 256, _T("WSAStartup error,GetLastError:%lu, errno:%d\n"), GetLastError(), errno);
		_tprintf_s(msg_t);
		system("pause");
		exit(1);
	}
	//����socket
	cSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		//return��exit֮ǰҪ�ر�cSock
	if (INVALID_SOCKET == cSock) {
		_stprintf_s(msg_t, 256, _T("socket error, GetLastError:%lu, errno:%d\n"), GetLastError(), errno);
		_tprintf_s(msg_t);
		WSACleanup();
		system("pause");
		exit(1);
	}
	//��������  ��ҪULONG���͵�ip��USHORT���͵�port
	ZeroMemory(&caddre, sizeof(caddre));
	((sockaddr_in *)&caddre)->sin_addr.S_un.S_addr = psu->ul_ip;		//�ֽ�������
	((sockaddr_in *)&caddre)->sin_port = psu->us_port;				//�ֽ�������
	caddre.sa_family = AF_INET;

	if (SOCKET_ERROR == connect(cSock, &caddre, sizeof(caddre))) {
		_stprintf_s(msg_t, 256, _T("connect error, GetLastError:%lu, errno:%d\n"), GetLastError(), errno);
		_tprintf_s(msg_t);
		//
		shutdown(cSock, SD_SEND);
		closesocket(cSock);
		WSACleanup();
		//
		system("pause");
		exit(1);
	}

	//recv��ʱ����
	int time_out_recv = 2000;	//��ʱʱ�䣨���룩
	setsockopt(cSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out_recv, sizeof(int));		//���ý��ճ�ʱ����

	/*�շ��洢����*/
	sendRecvSave(cSock, requestHead, pss);

	shutdown(cSock, SD_SEND);
	closesocket(cSock);
	WSACleanup();
	return 0;
}
/*
*�޸�ʱ�䣺2018-9-18
*���ߣ�zmx
*������cSock:���ӳɹ����׽���,requestHead:����ͷ��Ϣ,pss:�ļ���Ϣ
*���ܣ���������ͷ��������Ӧ�壬����֮����Ϊ��Ӧͷ����Ӧ��
*��ע��
*/
int sendRecvSave(SOCKET cSock, char *requestHead, StructStore *pss) {
	/*�ڲ�����*/
	FILE *fileHead = NULL;
	FILE *fileBody = NULL;
	TCHAR msg_t[MSGSIZE] = { NULL };
	char recvmsg[RECVSIZE] = { NULL };		//p2��Ž��յ��ֽ�
	unsigned long long totalData = 0;		//�������ֽ���
	unsigned long long actualData = 0;		//��ȥ��Ӧͷ��ʣ�ಿ��
	unsigned long long bodyData = 0;		//��Ӧ�����ݳ���
	size_t sendLength = 0;					//p
	int eachRecvLength = 0;					//sʵ�ʽ����ֽ���
	

	/*...*/
	_tfopen_s(&fileHead, pss->requestHead, _T("wb"));		//�ļ���¼��Ӧͷ
	_tfopen_s(&fileBody, pss->requestBody, _T("ab"));		//�ļ���¼��Ӧ��
	if (NULL == fileHead || NULL == fileBody) {
		_stprintf_s(msg_t, _countof(msg_t), _T("fopen err, GETLASTERROR:%lu, ERRNO:%d\n"), GetLastError(), errno);
		_tprintf(msg_t);
		//
		shutdown(cSock, SD_SEND);
		closesocket(cSock);
		WSACleanup();
		//
		system("pause");
		exit(1);
	}
	sendLength = send(cSock, requestHead, (int)(strlen(requestHead)), 0);		//��������ͷ

	if (strlen(requestHead) == sendLength) {									//�ж�������Ϣ�Ƿ�������
		eachRecvLength = recv(cSock, recvmsg, RECVSIZE, 0);						//������Ӧ
		printf("���ڽ���.");	//��ӡ'.'����ʾ���ڽ���
		if (-1 == eachRecvLength) {				//�����쳣
			_stprintf_s(msg_t, _countof(msg_t), _T("recv err, WSAGETLASTERROR:%d\n"), WSAGetLastError());
			_tprintf(msg_t);
			//
			shutdown(cSock, SD_SEND);
			closesocket(cSock);
			WSACleanup();
			//
			system("pause");
			exit(1);
		}
		totalData += eachRecvLength;					//��������
		for (int i = 0; i < eachRecvLength; ++i) {
			if (0 == actualData) {						//�����Ƿ���Ӧͷ��0��ʾ����Ӧͷ�������ʾ��Ӧͷ����
				if ('\r' == recvmsg[i] && '\n' == recvmsg[i + 1] && '\r' == recvmsg[i + 2] && '\n' == recvmsg[i + 3]) {
					actualData += (eachRecvLength - 4 - i);											//�������ֽ���-��Ӧͷ�ֽ���
					fwrite(recvmsg, sizeof(char), i + 3 + 1, fileHead);								//��Ӧͷд���ļ�
					fwrite(&recvmsg[i + 3 + 1], sizeof(char), eachRecvLength - (i + 3 + 1), fileBody);			//��Ӧ�岿��д���ļ�
					break;
				}

			}
		}
		if (0 == actualData) {		//��Ӧͷ����RECVSIZE����
			_stprintf_s(msg_t, _countof(msg_t), _T("��Ӧͷ��������δ����Ӧ�Բ���\n"));
			_tprintf(msg_t);
			//
			fclose(fileHead);
			fclose(fileBody);
			shutdown(cSock, SD_SEND);
			closesocket(cSock);
			WSACleanup();
			//
			system("pause");
			exit(1);
		}
		if (eachRecvLength == RECVSIZE) {				//���ճ�����Ԥ��һ�£����ܻ��к�������
			eachRecvLength = recv(cSock, recvmsg, RECVSIZE, 0);						//������Ӧ
			printf(".");	//��ӡ'.'����ʾ���ڽ���
			while (-1 != eachRecvLength) {											//
				if (0 == eachRecvLength) {
					break;
				}
				totalData += eachRecvLength;										//��������
				fwrite(recvmsg, sizeof(char), eachRecvLength, fileBody);			//��Ӧ��д���ļ�
				eachRecvLength = recv(cSock, recvmsg, RECVSIZE, 0);					//������Ӧ
			}
		}
	}
	
	//
	fclose(fileHead);
	fclose(fileBody);
	//
	printf("\n");
	printf("�������\n");
	return 0;
}