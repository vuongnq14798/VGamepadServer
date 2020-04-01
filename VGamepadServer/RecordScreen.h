#pragma once
#include "Buffer.h"

class CRecordScreen {
public:
	CRecordScreen();
	~CRecordScreen();

	static CRecordScreen* getInstance();

	BOOL startRecord();
	void stopRecord();
	static UINT capturingThread(void* arg);
	static UINT encodingThread(void* arg);
	void capture();	
	int encodeImage();

private:
	HANDLE capturingLoop, encodingLoop;
	BOOL isRecording;
	CBuffer *pBufferRGB, *pBufferH264;
};
