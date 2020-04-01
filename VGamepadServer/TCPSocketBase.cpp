#include "pch.h"
#include "TCPSocketBase.h"
#include "ErrorCodes.h"
#include "defines.h"

#define BACKLOG	10 
#define MAX_PACKET_SIZE 4096

CController* controller = NULL;

TCPSocketBase::TCPSocketBase()
{
	_isSocketConnected = false;
	_isSocketCreated = false;
	_isReceiveThreadRunning = false;
	_isConnectThreadRunning = false;

	_receiveLoop = NULL;
	_connectLoop = NULL;
	_sockFd = NULL;
	_clientFd = NULL;
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
	if (err != 0)
	{
		return ERROR_UNKNOWN;
	}

	if (_isSocketCreated == false)
	{
		_isSocketCreated = true;
		_port = port;
		_connectLoop = AfxBeginThread(connectThreadMain, (void*)this);
	}
	else
	{
		return ERROR_SOCKET_ALREADY_CREATED;
	}

}

int TCPSocketBase::closeSocket()
{

	_isSocketCreated = false;
	shutdown(_clientFd, SD_BOTH);
	closesocket(_clientFd);
	
	_isSocketConnected = false;

	if (_isReceiveThreadRunning)
	{
		_isReceiveThreadRunning = false;
		DWORD dwRet = WaitForSingleObject(_receiveLoop->m_hThread, _port);
		if (dwRet != WAIT_OBJECT_0 && dwRet != WAIT_FAILED) {
			TerminateThread(_receiveLoop->m_hThread, 0);
		}
	}
	return OK;
}

int TCPSocketBase::sendSocket(char* buf, int len)
{
	return send(_clientFd, (const char*)buf, len, 0);
}

UINT TCPSocketBase::connectThreadMain(void* arg)
{
	TCPSocketBase* h = reinterpret_cast<TCPSocketBase*>(arg);
	int ret = h->connectInnerFunc();

	if (ret != OK)
	{
		h->_isSocketCreated = false;
	}
	return 0;
}

int TCPSocketBase::connectInnerFunc()
{
	_sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	return connectAsServer();
}

UINT TCPSocketBase::receiveThreadMain(void* arg)
{
	TCPSocketBase* h = reinterpret_cast<TCPSocketBase*>(arg);
	h->receiveThreadLoop();
	return 0;
}

void TCPSocketBase::receiveThreadLoop()
{
	_isReceiveThreadRunning = true;
	char* buf = new char[MAX_PACKET_SIZE];
	int res = 0;
	while (_isReceiveThreadRunning)
	{
		res = receiveData(buf);
		break;
	}
	SAFE_DELETE_ARRAY(buf);
	_isReceiveThreadRunning = false;
	return;
}

int TCPSocketBase::receiveData(char* buf)
{
	UINT numbytes = 0;
	UINT read_max_bytes = MAX_PACKET_SIZE;

	numbytes = recv(_clientFd, buf, read_max_bytes, 0);
	_buffLen = strlen(buf);
	if (numbytes == -1)
	{
		_isSocketConnected = false;
		return ERROR_RECV_FAILED;
	}
	else if (numbytes > 0)
	{
		controller->OnReceiveData(buf[0],buf, _buffLen);
		return OK;
	}
	else
	{
		return ERROR_SOCKET_CLOSED;
	}

	return OK;
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

	_isConnectThreadRunning = TRUE;

	while (_isConnectThreadRunning)
	{
		_clientFd = accept(_sockFd, (struct sockaddr*) & _theirAddr, &sin_size);

		if (_clientFd == SOCKET_ERROR)
		{
			return ERROR_CANNOT_ACCEPT_CLIENT_SOCKET;
		}
		else
		{
			_receiveLoop = AfxBeginThread(receiveThreadMain, (void*)this);
			_isSocketConnected = TRUE;
		}
	}
	_isConnectThreadRunning = FALSE;
	return OK;
}
