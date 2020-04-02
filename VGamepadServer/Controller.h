#pragma once
#include "SocketController.h"
#include "ControlServer.h"
class CController
{
public:
	CController(void);
	~CController(void);

	void StartProgram(int port);
	void StopProgram();

	void OnReceiveData(char* buf, int indexClient);
private:
	CControlServer *controlServer;
};
