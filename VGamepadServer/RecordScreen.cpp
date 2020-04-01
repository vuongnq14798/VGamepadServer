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
	//AVCodecContext* pCodecCtx;
	//AVCodec* pCodec;
	//AVPacket pkt;
	//uint8_t* picture_buf;
	//AVFrame* pFrame;
	//AVFrame* pFrameRGB;
	//int w_rgb, h_rgb;

	//int out_width = 1280;
	//int out_height = 720;
	//av_register_all();

	//Util::getResolution(w_rgb, h_rgb);

	//pCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
	//if (!pCodec) {
	//	printf("Can not find encoder! \n");
	//	return -1;
	//}

	//pCodecCtx = avcodec_alloc_context3(pCodec);
	//pCodecCtx->codec_id = AV_CODEC_ID_H264;
	//pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	//pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
	//pCodecCtx->width = out_width;
	//pCodecCtx->height = out_height;
	//pCodecCtx->time_base.num = 1;
	//pCodecCtx->time_base.den = 25;
	//pCodecCtx->bit_rate = 2000000;
	//pCodecCtx->gop_size = 12;
	//pCodecCtx->qmin = 2;
	//pCodecCtx->qmax = 10;

	////Optional Param
	//pCodecCtx->max_b_frames = 3;

	//AVDictionary* param = 0;
	//av_dict_set(&param, "preset", "ultrafast", 0);
	//av_dict_set(&param, "tune", "zerolatency", 0);

	//if (avcodec_open2(pCodecCtx, pCodec, &param) < 0) {

	//	return -1;
	//}

	//pFrame = av_frame_alloc();
	//pFrame->format = AV_PIX_FMT_YUV420P;
	//pFrame->width = pCodecCtx->width;
	//pFrame->height = pCodecCtx->height;

	//pFrameRGB = av_frame_alloc();

	//int picture_size;
	//picture_size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	//picture_buf = (uint8_t*)av_malloc(picture_size);

	//SwsContext* fooContext = sws_getContext(w_rgb, h_rgb, AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, NULL, NULL, NULL, NULL);

	//av_new_packet(&pkt, picture_size);

	//unsigned char* picture_buf_rgb = NULL;
	//unsigned int	picture_buff_size = 0;
	//unsigned long	picture_pts = 0;
	//int framecnt = 0;
	//while (TRUE)
	//{
	//	if (!isRecording	//stop recording
	//		&& pBufferRGB->GetReservedCount() == 0) //no buffer in queue
	//		break;
	//	do
	//	{
	//		if (pBufferRGB->GetReservedCount() == 0)
	//		{
	//			Sleep(1);
	//		}
	//		else
	//		{
	//			int qBufferCount = pBufferRGB->getMem(picture_buf_rgb, picture_buff_size);
	//			if (pBufferRGB->GetReservedCount() > 0)
	//			{

	//				pBufferRGB->deleteMem(picture_buf_rgb);
	//			}
	//			else
	//			{
	//				break;
	//			}
	//		}
	//	} while (encodingLoop);

	//	// convert rgb to yuv
	//	avpicture_fill((AVPicture*)pFrameRGB, picture_buf_rgb, AV_PIX_FMT_RGB32, w_rgb, h_rgb);
	//	avpicture_fill((AVPicture*)pFrame, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
	//	sws_scale(fooContext, pFrameRGB->data, pFrameRGB->linesize, 0, h_rgb, pFrame->data, pFrame->linesize);
	//	pBufferRGB->deleteMem(picture_buf_rgb);
	//	int got_picture = 0;
	//	//Encode
	//	int ret = avcodec_encode_video2(pCodecCtx, &pkt, pFrame, &got_picture);
	//	if (ret < 0)
	//	{
	//		continue;
	//	}
	//	if (got_picture == 1)
	//	{
	//		int cnt = pBufferH264->setMem(pkt.data, pkt.size);
	//		if (cnt > pBufferH264->getNumBufferMax())
	//		{
	//			pBufferH264->clearBuffer();
	//		}
	//		av_free_packet(&pkt);
	//	}
	//}

	////Clean
	//avcodec_close(pCodecCtx);
	//av_free(pFrame);
	//av_free(pFrameRGB);
	//av_free(picture_buf);

	return 1;
}
