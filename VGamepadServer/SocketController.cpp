#include "pch.h"
#include "defines.h"
#include "SocketController.h"

CSocketController* __pSocketController = NULL;
TCPSocketBase* __tcpMainServerSocket = NULL;
CSocketController::CSocketController(void)
{
	__tcpMainServerSocket = new TCPSocketBase();
	__tcpMainServerConnected = FALSE;
}

CSocketController::~CSocketController(void)
{
	SAFE_DELETE(__tcpMainServerSocket);
}

CSocketController* CSocketController::GetInstance()
{
	if (__pSocketController == NULL)
		__pSocketController = new CSocketController();
	return __pSocketController;
}

void CSocketController::StartSocketController(int port)
{
	StartMainControl(port);
}

void CSocketController::StopSocketController()
{
	StopMainControl();
}

BOOL CSocketController::StartMainControl(int port)
{
	int err = __tcpMainServerSocket->connectSocket(port);
	return err;
}

void CSocketController::StopMainControl()
{
	if (__tcpMainServerSocket)
		__tcpMainServerSocket->closeSocket();
}
