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
	_indexClient = indexClient;
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
			StartSendVideo();
		}
	}

	case OFFSTREAM: {

	}
	}
	
	SAFE_DELETE_ARRAY(input);
}

BOOL CController::StartSendVideo( )
{
	sendingVideoLoop = AfxBeginThread(SendingVideoThread, (void*)this);
	if (sendingVideoLoop == NULL)
		return FALSE;
	isSendingVideo = TRUE;
	return TRUE;
}

UINT CController::SendingVideoThread(void* arg)
{
	CController* h = reinterpret_cast<CController*>(arg);
	h->SendVideo();
	return 0; 
}

void CController::SendVideo()
{
	unsigned char* ts_buff = NULL;
	unsigned int ts_buff_size = 0;
	unsigned long ts_pts = 0;

	CBuffer* qBuffer = CRecordScreen::getInstance()->getVideoBuffer();

	while (TRUE)
	{
		if (!isSendingVideo //stop sending
			&& qBuffer->GetReservedCount() == 0) //queue empty
			break;

		// get buffer from queue
		do
		{
			if (qBuffer->GetReservedCount() == 0)
			{
				Sleep(1);
			}
			else
			{
				int qBufferCount = qBuffer->getMem(ts_buff, ts_buff_size, ts_pts);
				if (qBuffer->GetReservedCount() > 0)
				{
					qBuffer->deleteMem(ts_buff);
				}
				else
				{
					break;
				}
			}
		} while (sendingVideoLoop);

		if (ts_buff != NULL)
		{
			////test code
			//static FILE* pFile = NULL;
			//if (!pFile) pFile = fopen("C:\\output.h264", "wb");
			//fwrite(ts_buff, 1, ts_buff_size, pFile);
			////end
			int ret = CSocketController::getInstance()->StartVideo(_indexClient, reinterpret_cast<char*> (ts_buff), ts_buff_size);
			
		}
		qBuffer->deleteMem(ts_buff);
	}
}

void CController::StopSendVideo()
{
	isSendingVideo = FALSE;
	if (sendingVideoLoop != NULL)
	{
		if (WaitForSingleObject(sendingVideoLoop, 1000) != 0)
			TerminateThread(sendingVideoLoop, 0);
	}
	CRecordScreen::getInstance()->stopRecord();
}