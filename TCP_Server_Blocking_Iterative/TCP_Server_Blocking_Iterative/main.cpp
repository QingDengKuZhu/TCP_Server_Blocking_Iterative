//阻塞I/O模型迭代回显服务器
#include "my_function.h"
#include <WinSock2.h>
#include <iostream>

#pragma  comment(lib, "ws2_32.lib")

using std::cout;
using std::endl;

/*服务器主题函数*/
int main(void)
{	
	//初始化Winsock库
	WSADATA wsa_data = {0};
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsa_data))
	{
		cout << "WSAStartup error : " << WSAGetLastError() << endl;
		return EXIT_FAILURE;
	}

	//调用服务器主体函数
	DoWork();
	
	//卸载Winsock库
	WSACleanup();

	return EXIT_SUCCESS;
}