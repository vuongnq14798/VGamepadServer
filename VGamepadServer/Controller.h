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
	BOOL StartSendVideo();
	static UINT SendingVideoThread(void* arg);
	void SendVideo();
	void StopSendVideo();
private:
	CControlServer *controlServer;
	CWinThread* sendingVideoLoop;
	BOOL isSendingVideo;
	int _indexClient;
};
