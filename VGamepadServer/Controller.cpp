#include "pch.h"
#include "defines.h"
#include "Controller.h"
#include "ControlServer.h"
CController::CController(void)
{
}

CController::~CController(void)
{
}

void CController::StartProgram(int port)
{
	if (!CSocketController::GetInstance())
		return;
	CSocketController::GetInstance()->StartSocketController(port);
}

void CController::StopProgram()
{
	CSocketController::GetInstance()->StopSocketController();
}

void CController::OnReceiveData(char type, char* buf, int bufLen)
{
	switch (type) {
	case KEYBOARD: {
		controlServer.pressKey(buf, bufLen);
	}
	case MOUSE: {
	}
	}
}
