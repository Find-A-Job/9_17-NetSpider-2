// 9_17-NetSpider.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

/*�����̺�������*/

int MainCommandLine(int , TCHAR **);
int MainAnlyURL(void);
int MainMakeRequestHead(void);
int MainConnectSendRecv(void);
int MainAnlyResponseHead(void);
int MainGetResponseBody(void);

/*������������*/



int _tmain(int argc, TCHAR **argv)
{
	/*�����ڲ�����*/
	TCHAR URL

	/*���������в���*/
	MainCommandLine(argc, argv);

	/*����URL*/
	MainAnlyURL();

	/*��������ͷ*/
	MainMakeRequestHead();

	/*���ӣ���������ͷ��������Ӧͷ��Ӧ��*/
	MainConnectSendRecv();

	/*������Ӧͷ*/
	MainAnlyResponseHead();

	/*�������Ӧ��*/
	MainGetResponseBody();

    return 0;
}

/*�����̺�������*/

int MainCommandLine(int argc, TCHAR **argv) {
	/*�ڲ�����*/

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
