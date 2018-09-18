#pragma once
#ifndef MACRO_H
#define MACRO_H

/*常量*/
#define FILENAMESIZE	256		//文件名数组长度
#define URLSIZE			256		//URL数组长度
#define MSGSIZE			256		//信息数组长度
#define FILEPATHSIZE	512		//	
#define MSG512			512		//
#define MSG1024			1024	//
#define MSG2048			2048	//
#define SENDSIZE		1024	//发送字节大小
#define RECVSIZE		1024	//接收字节大小

#define PROTOCALTYPESIZE	16		//协议类型数组长度
#define DOMAINSIZE			128		//域名数组长度
#define IPSIZE				32		//IP地址数组长度
#define PORTSIZE			8		//端口数组长度
#define DIRSIZE				256		//文件夹数组长度
#define FILESIZE			256		//文件数组长度
#define ARGVSIZE			256		//参数数组长度
#define ANCHORSIZE			256		//锚点数组长度

#endif // !MACRO_H
/*
*修改时间：
*作者：
*参数：
*功能：
*备注：
*/

/*结构体*/
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