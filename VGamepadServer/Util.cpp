#include "pch.h"
#include "Util.h"

char* Util::getHostName()
{
	WSADATA WSAData;
	char* hostName = NULL;
	if (!WSAStartup((2, 2), &WSAData)) {
		gethostname(hostName, 128);
		WSACleanup();
	}
	return hostName;
}

void Util::getResolution(int& width, int& height)
{
	HDC hScreenDC = GetDC(NULL);
	width = GetDeviceCaps(hScreenDC, HORZRES);
	height = GetDeviceCaps(hScreenDC, VERTRES);
	DeleteDC(hScreenDC);
}
