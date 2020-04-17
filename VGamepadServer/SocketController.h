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
	BOOL StartVideo(int client, char* buf, int len);
	void StopVideo(int client, char* buf, int len);

private:
	//TCPSocketBase* tcpServerSocket;
	BOOL			tcpServerConnected;
};
