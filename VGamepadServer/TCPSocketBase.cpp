#include "pch.h"
#include "TCPSocketBase.h"
#include "ErrorCodes.h"
#include "defines.h"
#include "RecordScreen.h"
#include "Controller.h"
#include "string.h"
#define BACKLOG	10 
#define MAX_PACKET_SIZE 4096
CController* controller = NULL;

TCPSocketBase::TCPSocketBase()
{
	/*_isSocketConnected = false;
	isSocketCreated = false;
	_isReceiveThreadRunning = false;
	_isConnectThreadRunning = false;*/

	receiveLoop = NULL;
	_connectLoop = NULL;
	_sockFd = NULL;
	clientFd = NULL;
	_buffLen = NULL;
	_port = NULL;
	sin_size = NULL;

	controller = new CController();
}

TCPSocketBase::~TCPSocketBase()
{
}

int TCPSocketBase::connectSocket(int port)
{
	int err = 0;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (isSocketCreated == false)
	{
		isSocketCreated = true;
		_port = port;
		_connectLoop = AfxBeginThread(connectThreadMain, (void*)this);
	}
	else
	{
		return ERROR_SOCKET_ALREADY_CREATED;
	}

}

int TCPSocketBase::closeSocket(int socket)
{


	shutdown(socket, SD_BOTH);
	closesocket(socket);
	connectedClient[socket] = false;
	if (_isReceiveThreadRunning)
	{
		_isReceiveThreadRunning = false;
		DWORD dwRet = WaitForSingleObject(receiveLoop->m_hThread, 10);
		if (dwRet != WAIT_OBJECT_0 && dwRet != WAIT_FAILED) {
			TerminateThread(receiveLoop->m_hThread, 0);
		}
	}
	return OK;
}

int TCPSocketBase::closeSocket()
{
	return 0;
}

UINT TCPSocketBase::connectThreadMain(void* arg)
{
	TCPSocketBase* h = reinterpret_cast<TCPSocketBase*>(arg);
	int ret = h->connectInnerFunc();

	if (ret != OK)
	{
		h->isSocketCreated = false;
	}
	return 0;
}

int TCPSocketBase::connectInnerFunc()
{
	_sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int opt = 1;


	setsockopt(_sockFd, SOL_SOCKET, (SO_REUSEADDR), (char*)&opt, sizeof(int));


	int len, trysize, gotsize;
	len = sizeof(int);

	trysize = 512 * 1024;

	int err = setsockopt(_sockFd, SOL_SOCKET, SO_SNDBUF, (char*)&trysize, sizeof(int));
	err = setsockopt(_sockFd, SOL_SOCKET, SO_RCVBUF, (char*)&trysize, sizeof(int));

	int ret = connectAsServer();
	return ret;
}

UINT TCPSocketBase::receiveThreadMain(void* arg)
{
	TCPSocketBase* h = reinterpret_cast<TCPSocketBase*>(arg);
	h->receiveThreadLoop();
	return 0;
}

void TCPSocketBase::receiveThreadLoop()
{

	char* buf = new char[MAX_PACKET_SIZE];
	int res = 0;
	int temp = currentClient;
	connectedClient[temp] = true;
	while (connectedClient[temp])
	{
		UINT read_max_bytes = MAX_PACKET_SIZE;
		if (recv(clients[temp], buf, read_max_bytes, 0)) {
			controller->OnReceiveData(buf, temp);
		}
		else
			break;
	}
	SAFE_DELETE_ARRAY(buf);
	connectedClient[temp] = false;
	return;
}

int TCPSocketBase::connectAsServer(void)
{
	memset((char*)&_servAddr, 0, sizeof(_servAddr));
	_servAddr.sin_family = PF_INET;
	_servAddr.sin_addr.s_addr = INADDR_ANY;
	_servAddr.sin_port = htons(_port);

	int ret = bind(_sockFd, (struct sockaddr*) & _servAddr, sizeof(_servAddr));
	if (ret < 0)
	{
		return ERROR_CANNOT_BIND_SOCKET;
	}
	if (listen(_sockFd, BACKLOG) == -1)
	{
		return ERROR_CANNOT_LISTEN_SOCKET;
	}

	sin_size = sizeof _theirAddr;

	while (isSocketCreated)
	{
		clientFd = accept(_sockFd, (struct sockaddr*) & _theirAddr, &sin_size);

		if (clientFd == SOCKET_ERROR)
		{
			return ERROR_CANNOT_ACCEPT_CLIENT_SOCKET;
		}
		else
		{
			currentClient++;
			currentClient = currentClient % 10;
			if (currentClient != 0) {
				closesocket(clients[currentClient]);
			}
			clients[currentClient] = clientFd;
			CController* c = new CController();
			c->OnReceiveData("4", currentClient);
			receiveLoop = AfxBeginThread(receiveThreadMain, (void*)this);
		}
	}
	return OK;
}

int TCPSocketBase::sendSocket(int indexClient, char* buf, int len)
{
	char* data = new char[len + 4];

	data[0] = (len >> 24) & 0xFF;
	data[1] = (len >> 16) & 0xFF;
	data[2] = (len >> 8) & 0xFF;
	data[3] = len & 0xFF;
	memcpy(&data[4], buf, len);
	int ret = -1;
	ret = send(clients[indexClient], (const char*)data, len + 4, 0);
	return ret;
}
