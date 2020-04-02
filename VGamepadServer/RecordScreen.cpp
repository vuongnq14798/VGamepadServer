#include "pch.h"
#include "defines.h"
#include "RecordScreen.h"
#include "Util.h"

extern "C"
{
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <cstdint>
}

CRecordScreen* pRecordScreen = NULL;
CRecordScreen::CRecordScreen()
{
	//dosomethings
	capturingLoop = NULL;
	encodingLoop = NULL;

	isRecording = FALSE;

	pBufferRGB = new CBuffer();
	pBufferRGB->SetBufferingCount(1);
	pBufferRGB->setNumBufferMax(10);

	pBufferH264 = new CBuffer();
	pBufferH264->SetBufferingCount(1);
	pBufferH264->setNumBufferMax(100);
}

CRecordScreen::~CRecordScreen(void)
{
	//dosomethings
	SAFE_DELETE(pBufferRGB);
	SAFE_DELETE(pBufferH264);
}

CRecordScreen* CRecordScreen::getInstance()
{
	if (pRecordScreen == NULL)
		pRecordScreen = new CRecordScreen();
	return pRecordScreen;
}

BOOL CRecordScreen::startRecord()
{
	isRecording = TRUE;

	encodingLoop = AfxBeginThread(encodingThread, (void*)this);
	capturingLoop = AfxBeginThread(capturingThread, (void*)this);

	if (capturingLoop == NULL || encodingLoop == NULL)
	{
		stopRecord();
		return FALSE;
	}
	return TRUE;
}

void CRecordScreen::stopRecord()
{
	isRecording = FALSE;
	if (capturingLoop != NULL)
	{
		if (WaitForSingleObject(capturingLoop, 1000) != 0)
			TerminateThread(capturingLoop, 0);
		capturingLoop = NULL;
	}
	if (encodingLoop != NULL)
	{
		if (WaitForSingleObject(encodingLoop, 1000) != 0)
			TerminateThread(encodingLoop, 0);
		encodingLoop = NULL;
	}

	pBufferRGB->clearBuffer();
	pBufferH264->clearBuffer();

}

UINT CRecordScreen::capturingThread(void* arg)
{
	CRecordScreen* h = reinterpret_cast<CRecordScreen*>(arg);
	h->capture();
	return 0;
}

UINT CRecordScreen::encodingThread(void* arg)
{
	CRecordScreen* h = reinterpret_cast<CRecordScreen*>(arg);
	h->encodeImage();
	return 0;
}

void CRecordScreen::capture()
{
	while (isRecording)
	{
		static int width = 0;
		static int height = 0;

		HDC     hScreen = GetDC(NULL);
		if (width == 0 || height == 0) 
			Util::getResolution(width, height);
		HDC     hDC = CreateCompatibleDC(hScreen);
		HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, width, height);
		HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
		BOOL    bRet = BitBlt(hDC, 0, 0, width, height, hScreen, 0, 0, SRCCOPY | CAPTUREBLT);

		//// save bitmap to clipboard
		//OpenClipboard(NULL);
		//EmptyClipboard();
		//SetClipboardData(CF_BITMAP, hBitmap);
		//CloseClipboard();

		BITMAPINFOHEADER   bi;
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = -height;
		bi.biPlanes = 1;
		bi.biBitCount = 32;
		bi.biCompression = BI_RGB;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrUsed = 0;
		bi.biClrImportant = 0;

		DWORD dwBmpSize = ((width * bi.biBitCount + 31) / 32) * 4 * height;
		HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
		char* lpbitmap = (char*)GlobalLock(hDIB);
		GetDIBits(hScreen,
			hBitmap,
			0,
			height,
			lpbitmap,
			(BITMAPINFO*)&bi,
			DIB_RGB_COLORS);

		int ret = pBufferRGB->setMem(reinterpret_cast<BYTE*> (lpbitmap), dwBmpSize);
		if (ret > pBufferRGB->getNumBufferMax())
		{
			pBufferRGB->clearBuffer();
		}

		GlobalUnlock(hDIB);
		GlobalFree(hDIB);
		SelectObject(hDC, old_obj);
		DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);
		DeleteObject(hBitmap);
	}
}

int CRecordScreen::encodeImage()
{
	return 1;
}
