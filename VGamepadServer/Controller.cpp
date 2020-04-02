#include "pch.h"
#include "defines.h"
#include "Controller.h"
#include "ControlServer.h"
#include "RecordScreen.h"

CController::CController(void)
{
	controlServer = new CControlServer();
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

void CController::OnReceiveData(char* buf, int indexClient)
{
	int* input = new int[10];
	int len = 0;
	char* temp = strtok(buf, " ");
	while (temp)
	{
		input[len++] = atoi(temp);
		temp = strtok(NULL, " ");
	}

	switch (input[0]) {
	case KEYBOARD: {
		BOOL ret = controlServer->deleteXbox(indexClient);
		controlServer->pressKey(input, len - 1);
	}
	case MOUSE: {

	}

	case XBOX: {
		BOOL ret = controlServer->initXbox(indexClient);
		controlServer->updateXbox(indexClient, input, len - 1);
	}

	case ONSTREAM: {
		if (CRecordScreen::getInstance()->startRecord()) {
			//StartSendVideo();
		}
	}

	case OFFSTREAM: {

	}
	}
	
	SAFE_DELETE_ARRAY(input);
}
