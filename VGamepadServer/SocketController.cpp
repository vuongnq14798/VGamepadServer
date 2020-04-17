#include "pch.h"
#include "defines.h"
#include "SocketController.h"

CSocketController* pSocketController = NULL;
TCPSocketBase* tcpServerSocket = NULL;
CSocketController::CSocketController(void)
{
	tcpServerSocket = new TCPSocketBase();
	tcpServerConnected = FALSE;
}

CSocketController::~CSocketController(void)
{
	SAFE_DELETE(tcpServerSocket);
}

CSocketController* CSocketController::getInstance()
{
	if (pSocketController == NULL)
		pSocketController = new CSocketController();
	return pSocketController;
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
	int err = tcpServerSocket->connectSocket(port);
	return err;
}

void CSocketController::StopMainControl()
{
	if (tcpServerSocket)
		tcpServerSocket->closeSocket();
}

BOOL CSocketController::StartVideo(int indexClient, char* buf, int len) {
	return tcpServerSocket->sendSocket(indexClient, buf, len);
}

void CSocketController::StopVideo(int client, char* buf, int len)
{
}
