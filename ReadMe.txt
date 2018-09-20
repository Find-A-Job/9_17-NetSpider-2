/////////////////////////////////////////////////////////////////////////////
2018-9-17
1.入口函数需要用int _tmain(int argc, TCHAR **argv)
2.
2018-9-20
1.项目-》属性-》配置属性-》常规-》-》字符集，UNICODE字符集和多字节字符集有区别，
	sizeof(TCHAR)和sizeof(char)两个表达式的值在两种字符集中不一样，
	UNICODE字符集：sizeof(TCHAR)==2,sizeof(char)==1
	多字节字符集：sizeof(TCHAR)==1,sizeof(char)==1
	所以自定义函数TcharToChar和CharToTchar不能使用
2.	*ADDRINFOA中地址和端口位置
	*在ai_addr->sa_data字符串中(14位长度)
	*前两位是端口80（内部存储顺序是 0x00 0x50）
	*接着四位是地址128.1.83.59（内部顺序是 0x80 0x01 0x53 0x3b）
	*后八位都是0
	for (int i = 0; i < 14; ++i) {
		printf("%u\t", (UCHAR)((pai->ai_addr->sa_data)[i]));
	}
	printf("\n");
	printf("%s\n", (pai->ai_addr->sa_data));
	//////////////////////////////////////////
	for (int i = 0; i < 4; ++i) {
		printf("%u\t", (UCHAR)(ip[i]));
	}
	/////////////////////////////////////////
	mbstowcs_s(&returnValue, psu->ip, _countof(psu->ip), ip, _countof(ip));
	printf("function:\n");
	for (int i = 0; i < 4; ++i) {
		printf("%u\t", (UCHAR)(ip[i]));
	}
	printf("\n");
3.getaddrinfo()和freeaddrinfo()配合使用
4.在计算数组下标时经常犯错误，多亏zeromemory()救了一命，填充了0,让字符串没有越界