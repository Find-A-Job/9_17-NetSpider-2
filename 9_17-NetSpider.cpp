// 9_17-NetSpider.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Macro.h"
#include "func_zmx.h"

#pragma comment(lib, "ws2_32.lib")

/*主流程函数声明*/

int MainCommandLine(int , TCHAR **, TCHAR *);
int MainAnlyURL(TCHAR *, StructURL *);
int MainDomainToIp(StructURL *);
int MainMakeRequestHead(StructURL *, char *);
int MainConnectSendRecv(StructURL *, char *, StructStore *);

/*其他函数声明*/
int sendRecvSave(SOCKET, char *, StructStore *);			//


int _tmain(int argc, TCHAR **argv)
{
	/*函数内部变量*/
	TCHAR URL[URLSIZE] = { NULL };
	TCHAR msg_t[MSGSIZE] = { NULL };
	PStructURL psu=(PStructURL)malloc(sizeof(StructURL));		//申请
	PStructStore pss = (PStructStore)malloc(sizeof(StructStore));
	char requestHead[MSG2048] = { NULL };
	TCHAR fileName[FILEPATHSIZE] = { NULL };
	char msg[MSGSIZE] = { NULL };
	size_t returnValue = 0;
	char ip[IPSIZE] = { NULL };
	char port[PORTSIZE] = { NULL };

	/*处理命令行参数*/
	MainCommandLine(argc, argv, URL);

	/*解析URL*/
	MainAnlyURL(URL, psu);

	/*查询域名，转为ip地址*/
	MainDomainToIp(psu);

	/*制作请求头*/
	MainMakeRequestHead(psu, requestHead);

	wcstombs_s(&returnValue, ip, _countof(ip), psu->ip, _countof(ip));
	wcstombs_s(&returnValue, port, _countof(port), psu->port, _countof(port));
	wcstombs_s(&returnValue, msg, _countof(msg), psu->dirPath, _countof(msg));
	printf("main:\n");
	for (int i = 0; i < 4; ++i) {
		printf("%u\t", (UCHAR)ip[i]);
	}
	printf("\n");
	printf("main:\n");
	printf("%s\n", requestHead);
	//释放
	free(psu);
	free(pss);
	system("pause");
	return 0;

	/*连接，发送请求头，接收并分离出响应头和响应体*/
	MainConnectSendRecv(psu, requestHead, pss);

	//释放
	free(psu);
	free(pss);
	system("pause");
    return 0;
}

/*主流程函数定义*/

/*
*修改时间：2018-9-18
*作者：zmx
*参数：argc:命令行参数个数，argv:命令行参数指针，URL:输出URL地址，'\0'结尾
*功能：从命令行中提取关键信息，目前是URL提取
*备注：二次修改时间：2018-9-20,修改内容：采用c语言自带函数进行宽字节转多字节
*/
int MainCommandLine(int argc, TCHAR **argv, TCHAR *URL) {
	/*内部变量*/
	char msg[MSGSIZE] = { NULL };
	size_t returnValue = 0;

	/*...*/
	switch (argc) {
	case 1: {
		printf("请输入URL参数\n");
	}break;
	case 2: {
		_stprintf_s(URL, URLSIZE, _T("%s"), argv[1]);
		wcstombs_s(&returnValue, msg, _countof(msg), URL, _countof(msg));		//宽字节转多字节
		printf("%s\n", msg);
	}break;
	default: {
		printf("未定义参数\n");
	}
	}

	printf("命令行参数分析完毕\n");
	return 0;
}
/*
*修改时间：2018-9-18
*作者：zmx
*参数：URL:接收URL字符串数组，'\0'结尾, psu:赋值结构体
*功能：分析URL，赋值给结构体psu
*备注：二次修改时间：2018-9-19，修改内容：1.增加提取各个部分的功能。2.
*/
int MainAnlyURL(TCHAR *URL, StructURL *psu) {
	/*内部变量*/
	TCHAR msg_t[URLSIZE] = { NULL };
	int coor_b = 0;							//关键字所在位置开始下标
	int coor_e = 0;							//关键字所在位置结束下标
	bool spec = FALSE;						//特殊情况标识

	/*...*/
	ZeroMemory(psu, sizeof(StructURL));		//初始化结构体内存

	/*提取协议部分*/
	coor_b = 0;
	coor_e = 0;
	for (int i = 0; URL[i] != NULL; ++i) {
		if (':' == URL[i] && '/' == URL[i + 1]) {		//判断依据：协议类型以://为分隔，http://www.baidu.com/
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
	else {								//未找到，则默认为http
		(psu->protocalType)[0] = 'h';
		(psu->protocalType)[1] = 't';
		(psu->protocalType)[2] = 't';
		(psu->protocalType)[3] = 'p';
		(psu->protocalType)[4] = '\0';
	}
	/*提取域名部分*/
	coor_b = 0;
	coor_e = 0;
	for (int i = 0; URL[i] != '\0'; ++i) {
		if ('/' == URL[i] && '/' == URL[i + 1]) {		//判断依据：域名部分以://为起始，':''/''\0'三种标识为结束，http://www.baidu.com/
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
	/*提取端口部分*/
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
	else {							//默认为80端口
		(psu->port)[0] = '8';
		(psu->port)[1] = '0';
		(psu->port)[2] = '\0';
	}
	/*提取文件夹部分*/
	coor_b = 0;
	coor_e = 0;
	spec = FALSE;
	for (int i = 0; URL[i] != '\0'; ++i) {
		if (0 == coor_b) {
			if ('/' == URL[i] && '/' != URL[i + 1] && '/' != URL[i - 1]) {		//判断依据：文件夹部分以/为起始，'/'为结束，http://www.baidu.com/
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
			if ('.' == URL[i] || '?' == URL[i] || '#' == URL[i]) {			//发现文件名部分, 参数部分， 锚部分，退出循环
				break;
			}
			if ('\0' == URL[i + 1]) {
				coor_e = i + 1;
				spec = TRUE;			//特殊情况，当
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
	/*提取文件名部分*/
	coor_b = 0;
	coor_e = 0;
	for (int i = 0; URL[i] != '\0'; ++i) {
		if ('/' == URL[i] && '/' != URL[i + 1]) {		//判断依据：文件名部分以/为起始, 带'.'，'?''#''\0'为结束，http://www.baidu.com/
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
	else {					//未发现起始标识
		(psu->file)[0] = '\0';
	}

	printf("URL分析完毕\n");
	return 0;
}

/*
*修改时间：2018-9-19
*作者：zmx
*参数： psu:赋值结构体
*功能：查询域名，转为ip地址
*备注：getaddrinfo()函数需要调用wsastartup()
*		2.printf注意它本身的隐式转换，最好自己显示转换一下。printf("%u\t", (UCHAR)ip[i]);
*/
int MainDomainToIp(StructURL *psu) {
	/*内部变量*/
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
		printf("WSAStartup启动失败：%d\n", WSAGetLastError());
		system("pause");
		exit(1);
	}
	wcstombs_s(&returnValue, domain, _countof(domain), psu->domain, _countof(domain));
	wcstombs_s(&returnValue, port, _countof(port), psu->port, _countof(port));

	ZeroMemory(&ai, sizeof(ai));
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_family = AF_INET;
	iFuncStat = getaddrinfo(domain, port, &ai, &pai);	//getaddrinfo()DNS查询服务
	if ( iFuncStat!= 0) {		
		printf("域名转ip失败, 错误代码：%d\n", iFuncStat);
		system("pause");
		exit(1);
	}
	CopyMemory(ip, &(pai->ai_addr->sa_data[2]), 4);
	ip[4] = '\0';
	mbstowcs_s(&returnValue, psu->ip, _countof(psu->ip), ip, _countof(psu->ip));		//

	freeaddrinfo(pai);						//释放由getaddrinfo()内部为PADDRINFOA动态分配的内存
	WSACleanup();
	return 0;
}

/*
*修改时间：2018-9-18
*作者：zmx
*参数：psu:URL数据结构体，requestHead:输出制作完成的请求头
*功能：制作请求头部
*备注：
*/
int MainMakeRequestHead(StructURL *psu, char *requestHead) {
	/*内部变量*/
	char send_head[MSGSIZE] = { NULL };			//请求头
	char send_host[MSGSIZE] = { NULL };			//host字段
	char send_agent[MSGSIZE] = { NULL };		//agent字段
	char send_language[MSGSIZE] = { NULL };		//language
	char send_encoding[MSGSIZE] = { NULL };		//压缩方式
	char send_connection[MSGSIZE] = { NULL };	//连接方式
	char send_cache[MSGSIZE] = { NULL };		//缓存
	char send_ending[MSGSIZE] = { NULL };		//结束标志

	char dir[DIRSIZE] = { NULL };
	char file[FILESIZE] = { NULL };
	char domain[DOMAINSIZE] = { NULL };

	/*...*/
	//TCHAR 转 char
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

	_tprintf(_T("请求头制作完毕完毕\n"));
	return 0;
}
/*
*修改时间：2018-9-18
*作者：zmx
*参数：psu:URL参数结构体,requestHead:请求头,pss:文件信息
*功能：启用socket进行通信，分析URL，发送请求头，分离响应数据
*备注：
*/
int MainConnectSendRecv(StructURL *psu, char *requestHead, StructStore *pss) {
	/*内部变量*/
	TCHAR msg_t[MSGSIZE] = { NULL };
	char ip[IPSIZE] = { NULL };
	char port[PORTSIZE] = { NULL };
	SOCKET cSock = NULL;

	/*...*/
	//socket初始化
	WORD sockVer = MAKEWORD(2, 2);
	WSADATA wsadata;
	if (WSAStartup(sockVer, &wsadata) != 0) {
		_stprintf_s(msg_t, 256, _T("WSAStartup error,GetLastError:%lu, errno:%d\n"), GetLastError(), errno);
		_tprintf_s(msg_t);
		system("pause");
		exit(1);
	}
	//创建socket
	cSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		//return和exit之前要关闭cSock
	if (INVALID_SOCKET == cSock) {
		_stprintf_s(msg_t, 256, _T("socket error, GetLastError:%lu, errno:%d\n"), GetLastError(), errno);
		_tprintf_s(msg_t);
		WSACleanup();
		system("pause");
		exit(1);
	}
	//建立连接  需要ULONG类型的ip和USHORT类型的port
	TcharToChar(psu->ip, _tcslen(psu->ip), ip);
	TcharToChar(psu->port, _tcslen(psu->port), port);
	sockaddr caddre;
	ZeroMemory(&caddre, sizeof(caddre));
	((sockaddr_in *)&caddre)->sin_addr.S_un.S_addr = *((ULONG *)ip[0]);		//字节序问题
	((sockaddr_in *)&caddre)->sin_port = *((USHORT *)port[0]);				//字节序问题
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

	//recv超时设置
	int time_out_recv = 2000;	//超时时间（毫秒）
	setsockopt(cSock, SOL_SOCKET, SO_RCVTIMEO, (char *)&time_out_recv, sizeof(int));		//设置接收超时返回


	/*收发存储分离*/
	sendRecvSave(cSock, requestHead, pss);

	return 0;
}
/*
*修改时间：2018-9-18
*作者：zmx
*参数：cSock:连接成功的套接字,requestHead:请求头信息,pss:文件信息
*功能：发送请求头，接收响应体，并将之分离为响应头和响应体
*备注：
*/
int sendRecvSave(SOCKET cSock, char *requestHead, StructStore *pss) {
	/*内部变量*/
	FILE *fileHead = NULL;
	FILE *fileBody = NULL;
	TCHAR msg_t[MSGSIZE] = { NULL };
	char recvmsg[RECVSIZE] = { NULL };		//p2存放接收的字节
	unsigned long long totalData = 0;		//接收总字节数
	unsigned long long actualData = 0;		//除去响应头的剩余部分
	unsigned long long bodyData = 0;		//响应体数据长度
	size_t sendLength = 0;					//p
	int eachRecvLength = 0;					//s实际接收字节数
	

	/*...*/
	_tfopen_s(&fileHead, pss->requestHead, _T("wb"));		//文件记录响应头
	_tfopen_s(&fileBody, pss->requestBody, _T("ab"));		//文件记录响应体
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
	sendLength = send(cSock, requestHead, (int)(strlen(requestHead)), 0);		//发送请求头

	if (strlen(requestHead) == sendLength) {									//判断所有信息是否发送正常
		eachRecvLength = recv(cSock, recvmsg, RECVSIZE, 0);						//接收响应
		if (-1 == eachRecvLength) {				//接收异常
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
		totalData += eachRecvLength;					//总量计数
		for (int i = 0; i < eachRecvLength; ++i) {
			if (0 == actualData) {						//区分是否响应头，0表示是响应头，非零表示响应头结束
				if ('\r' == recvmsg[i] && '\n' == recvmsg[i + 1] && '\r' == recvmsg[i + 2] && '\n' == recvmsg[i + 3]) {
					actualData += (eachRecvLength - 4 - i);											//接收总字节数-响应头字节数
					fwrite(recvmsg, sizeof(char), i + 3 + 1, fileHead);								//响应头写入文件
					fwrite(recvmsg, sizeof(char), eachRecvLength - (i + 3 + 1), fileBody);			//响应体部分写入文件
					break;
				}

			}
		}
		if (0 == actualData) {		//响应头超过RECVSIZE长度
			_stprintf_s(msg_t, _countof(msg_t), _T("响应头过长，暂未设置应对策略\n"));
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
		if (eachRecvLength == RECVSIZE) {				//接收长度与预期一致，可能还有后续数据
			eachRecvLength = recv(cSock, recvmsg, RECVSIZE, 0);						//接收响应
			while (-1 != eachRecvLength) {											//
				totalData += eachRecvLength;										//总量计数
				fwrite(recvmsg, sizeof(char), eachRecvLength, fileBody);			//响应体写入文件
			}
		}
	}
	//
	fclose(fileHead);
	fclose(fileBody);
	shutdown(cSock, SD_SEND);
	closesocket(cSock);
	WSACleanup();
	//
	return 0;
}