#include "pch.h"
#include "defines.h"
#include "Controller.h"
#include "ControlServer.h"
#include "RecordScreen.h"
CController::CController(void)
{
}

CController::~CController(void)
{
}

void CController::StartProgram(int port)
{
	if (!CSocketController::getInstance())
		return;
	CSocketController::getInstance()->StartSocketController(port);
}

void CController::StopProgram()
{
	CSocketController::getInstance()->StopSocketController();
}

void CController::OnReceiveData(char type, char* buf, int bufLen)
{
	switch (type) {
	case 'a': {
		controlServer.pressKey(buf, bufLen);
	}
	case MOUSE: {
	}

	case XBOX: {

	}

	case ONSTREAM: {
		if (CRecordScreen::getInstance()->startRecord()) {
			//StartSendVideo();
		}
	}

	case OFFSTREAM: {

	}
	}
}
