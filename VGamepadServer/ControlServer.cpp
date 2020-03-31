#include "pch.h"
#include "defines.h"
#include "ControlServer.h"

CControlServer::CControlServer(void)
{
}

CControlServer::~CControlServer(void)
{
}

void CControlServer::pressKey(char* buf, int bufLen)
{
	keydown(buf, bufLen);
	keyup(buf, bufLen);
}

void CControlServer::OnLeftClick()
{
	leftdown();
	leftup();
}

void CControlServer::OnDoubleLClick()
{
	leftdown();
	leftup();
	leftdown();
	leftup();
}

void CControlServer::OnRightClick()
{
	rightdown();
	rightup();
}

void CControlServer::OnLeftMouseDown()
{
	leftdown();
}

void CControlServer::OnLeftMouseUp()
{
	leftup();
}
void CControlServer::keydown(char* buf, int bufLen)
{
	INPUT* input = new INPUT[bufLen - 1];
	for (int i = 0; i < bufLen - 1; i++) {
		input[i].type = INPUT_KEYBOARD;
		input[i].ki.wScan = 0;
		input[i].ki.time = 0;
		input[i].ki.dwExtraInfo = 0;
		input[i].ki.wVk = buf[i + 1];
		input[i].ki.dwFlags = 0;
	}
	SendInput(bufLen - 1, input, sizeof(INPUT));
	SAFE_DELETE_ARRAY(input);
}
void CControlServer::keyup(char* buf, int bufLen)
{
	INPUT* input = new INPUT[bufLen - 1];
	for (int i = 0; i < bufLen - 1; i++) {
		input[i].type = INPUT_KEYBOARD;
		input[i].ki.wScan = 0;
		input[i].ki.time = 0;
		input[i].ki.dwExtraInfo = 0;
		input[i].ki.wVk = buf[i + 1];
		input[i].ki.dwFlags = KEYEVENTF_KEYUP;
	}
	SendInput(bufLen - 1, input, sizeof(INPUT));
	SAFE_DELETE_ARRAY(input);
}

void CControlServer::leftdown()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &input, sizeof(INPUT));
}

void CControlServer::leftup()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &input, sizeof(INPUT));
}

void CControlServer::rightdown()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &input, sizeof(INPUT));
}

void CControlServer::rightup()
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &input, sizeof(INPUT));
}
