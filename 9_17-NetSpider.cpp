// 9_17-NetSpider.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

/*主流程函数声明*/

int MainCommandLine(int , TCHAR **);
int MainAnlyURL(void);
int MainMakeRequestHead(void);
int MainConnectSendRecv(void);
int MainAnlyResponseHead(void);
int MainGetResponseBody(void);

/*其他函数声明*/



int _tmain(int argc, TCHAR **argv)
{
	/*函数内部变量*/
	TCHAR URL

	/*处理命令行参数*/
	MainCommandLine(argc, argv);

	/*解析URL*/
	MainAnlyURL();

	/*制作请求头*/
	MainMakeRequestHead();

	/*连接，发送请求头，接收响应头响应体*/
	MainConnectSendRecv();

	/*解析响应头*/
	MainAnlyResponseHead();

	/*分离出响应体*/
	MainGetResponseBody();

    return 0;
}

/*主流程函数定义*/

int MainCommandLine(int argc, TCHAR **argv) {
	/*内部变量*/

	/*...*/
	switch (argc) {
	case 0: {
		;
	}break;
	case 1: {
		;
	}break;
	default: {
		;
	}
	}

	return 0;
}
