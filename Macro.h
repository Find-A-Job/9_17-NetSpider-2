#pragma once
#ifndef MACRO_H
#define MACRO_H

/*����*/
#define FILENAMESIZE	256		//�ļ������鳤��
#define URLSIZE			256		//URL���鳤��
#define MSGSIZE			256		//��Ϣ���鳤��
#define FILEPATHSIZE	512		//	
#define MSG512			512		//
#define MSG1024			1024	//
#define MSG2048			2048	//
#define SENDSIZE		1024	//�����ֽڴ�С
#define RECVSIZE		1024	//�����ֽڴ�С

#define PROTOCALTYPESIZE	16		//Э���������鳤��
#define DOMAINSIZE			128		//�������鳤��
#define IPSIZE				32		//IP��ַ���鳤��
#define PORTSIZE			8		//�˿����鳤��
#define DIRSIZE				256		//�ļ������鳤��
#define FILESIZE			256		//�ļ����鳤��
#define ARGVSIZE			256		//�������鳤��
#define ANCHORSIZE			256		//ê�����鳤��

#endif // !MACRO_H
/*
*�޸�ʱ�䣺
*���ߣ�
*������
*���ܣ�
*��ע��
*/

/*�ṹ��*/
struct StructURL
{
	TCHAR protocalType[PROTOCALTYPESIZE];
	TCHAR domain[DOMAINSIZE];
	TCHAR ip[IPSIZE];
	TCHAR port[PORTSIZE];
	TCHAR dirPath[DIRSIZE];
	TCHAR file[FILESIZE];
	TCHAR argv[ARGVSIZE];
	TCHAR anchor[ANCHORSIZE];
};
typedef StructURL *PStructURL;

struct StructStore
{
	TCHAR fileName[FILEPATHSIZE];
	TCHAR requestHead[FILEPATHSIZE];
	TCHAR requestBody[FILEPATHSIZE];
};
typedef StructStore *PStructStore;