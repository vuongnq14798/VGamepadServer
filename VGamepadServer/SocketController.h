#pragma once
#include "TCPSocketBase.h"

class CSocketController
{
public:
	CSocketController(void);
	~CSocketController(void);

	static CSocketController* getInstance();

	void StartSocketController(int port);
	void StopSocketController();

	BOOL StartMainControl(int port);
	void StopMainControl();
private:
	//TCPSocketBase* tcpServerSocket;
	BOOL			tcpServerConnected;
};
