#pragma once
#include "WS2tcpip.h"
#include "Controller.h"

class TCPSocketBase
{
public:
	TCPSocketBase();
	~TCPSocketBase();

	int		connectSocket(int port);
	int 	closeSocket(int socket);
	int		closeSocket();
	int		sendSocket(char* buf, int len);
protected:
	static UINT connectThreadMain(void* arg);
	static UINT receiveThreadMain(void* arg);

	int connectInnerFunc();
	void receiveThreadLoop(void);
	int receiveData(char* buf);
	int connectAsServer(void);


private:
	int _sockFd, clientFd;
	CWinThread* receiveLoop;
	CWinThread* _connectLoop;

	int _port, _buffLen;
	bool _isSocketConnected, isSocketCreated,
		_isReceiveThreadRunning, _isConnectThreadRunning;
	struct sockaddr_storage _theirAddr;
	struct sockaddr_in _servAddr;
	socklen_t sin_size;
	int clients[10];
	int connectedClient[10];
	int currentClient = -1;
	
	//CController*controller;
};
