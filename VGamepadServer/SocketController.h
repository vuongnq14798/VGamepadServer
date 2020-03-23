#pragma once
#include "TCPSocketBase.h"

class CSocketController
{
public:
	CSocketController(void);
	~CSocketController(void);

	static CSocketController* GetInstance();

	void StartSocketController(int port);
	void StopSocketController();

	BOOL StartMainControl(int port);
	void StopMainControl();
private:
	//TCPSocketBase* __tcpMainServerSocket;
	BOOL			__tcpMainServerConnected;
};