#include "my_function.h"
#include <iostream>

using std::endl;
using std::cout;

SOCKET BindListen(void)
{
	SOCKET hsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == hsock)
	{
		cout << "socket error : " << WSAGetLastError() << endl;
		return INVALID_SOCKET;
	}


	// 填充本地套接字地址 
	struct sockaddr_in serv_addr = {0};	/*本地套接字地址*/
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DEF_PORT);
	if(SOCKET_ERROR == bind(hsock, (const struct sockaddr *)&serv_addr, sizeof(struct sockaddr)))
	{
		cout << "bind error : " << WSAGetLastError() << endl;
		return INVALID_SOCKET;
	}

	/************************************************************************/
	/* 开始监听(将其设置为监听状态)                                                                     */
	/************************************************************************/
	if (SOCKET_ERROR == listen(hsock, SOMAXCONN))
	{
		cout << "listen error : " << WSAGetLastError() << endl;
		closesocket(hsock);
		return INVALID_SOCKET;
	}

	return hsock;
}

SOCKET AcceptConnection(SOCKET hListenSocket)
{
		struct sockaddr_in clien_addr = {0};
		int addr_len = sizeof(struct sockaddr_in);

		SOCKET hsock  = accept(hListenSocket, (struct sockaddr *)&clien_addr, &addr_len);
		if (INVALID_SOCKET == hsock)
		{
			cout << "accept error : " << WSAGetLastError() << endl;
			return INVALID_SOCKET;
		}

		return hsock;
}

bool ProcessConnection(SOCKET hClientSocket)
{
	char buf[BUFFER_SIZE] = {0};
	int result= 0;

	/*循环直到客户端关闭数据连接*/
	do 
	{
		/************************************************************************/
		/* 接搜数据端的数据.由于套接字hClientSock(默认)是阻塞模式,对其调用recv将会阻*/
		/* 塞,直到recv完成返回.当recv返回0时,表明客户端完成数据发送并且关闭了连接,此时*/
		/*就可以退出循环*/
		/************************************************************************/
		result = recv(hClientSocket, buf, BUFFER_SIZE, 0);
		if (SOCKET_ERROR == result)
		{
			printf("recv error : %d \n", WSAGetLastError());
			return false;
		}

		else
		{
			int nSent = 0;
			/*把数据原封不动发送回客户端,即回显*/
			while (nSent < result)
			{
				//这里的send也会阻塞,只有当sned返回后,程序才能继续执行
				int nTemp = send(hClientSocket, &buf[nSent], result-nSent, 0);
				if (nTemp > 0)
				{
					nSent += nTemp;
				}
				else if (SOCKET_ERROR == nTemp)
				{
					cout << "send error : " << WSAGetLastError() << endl;
					return false;
				}
				else
				{
					/*send返回0, 由于此时nSent<result,也就是说还有数据没有发送出去,所以连接是被客户端意外关闭的*/
					cout << "Connection closed unexpectedly by peer." << endl;
					return true;
				}
			}
		}
	} while (0 != result);

	cout << "Connection closed by peer." << endl;
	return true;
}

bool ShutdownConnection(SOCKET hClientSocket)
{
	char buff[512];
	int result = 0;

	if (SOCKET_ERROR == shutdown(hClientSocket,SD_SEND))
	{
		cout << "shutdown error : " << WSAGetLastError() << endl;
		return false;
	}

	do 
	{
		result = recv(hClientSocket, buff, sizeof(buff)/sizeof(char), 0);
		if (SOCKET_ERROR == result)
		{
			cout << "recv error : " << WSAGetLastError() << endl;			
			return false;
		}
		else
		{
			cout << result <<" unexpected bytes received." << endl;
		}
	} while (0 != result);

	/*关闭套接字*/
	if (SOCKET_ERROR == closesocket(hClientSocket))
	{
		printf("closesock error : %d \n", WSAGetLastError());
		return false;
	}

	return true;
}

void DoWork(void)
{
	/*获取监听套接字并进入监听状态*/
	SOCKET hListenSocket = BindListen();
	if (INVALID_SOCKET == hListenSocket)
	{
		return;
	}

	cout << "Server is Running..." << endl;

	while (true)
	{	
		/*第一个阶段,接受一个客户端连接*/
		SOCKET hClientSocket = AcceptConnection(hListenSocket);
		if (INVALID_SOCKET == hClientSocket)
		{
			break;
		}

		
		/*第二阶段,服务一个客户端连接*/
		if (false == ProcessConnection(hClientSocket))
		{
			break;
		}

		/*第三阶段, 关闭一个客户端连接*/
		if (false == ShutdownConnection(hClientSocket))
		{
			break;
		}
	}

	/*关闭监听套接字*/
	if(SOCKET_ERROR == closesocket(hListenSocket))
	{
		cout << "closesocket error : %d\n" << WSAGetLastError() << endl;
	}
	cout << "成功关闭客户端." << endl;
	return;
}