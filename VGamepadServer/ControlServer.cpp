  #include "pch.h"
#include "defines.h"
#include "ControlServer.h"
#include "ViGEm/Client.h"
#include <mutex>

static std::mutex m;

VOID CALLBACK notification(
	PVIGEM_CLIENT Client,
	PVIGEM_TARGET Target,
	UCHAR LargeMotor,
	UCHAR SmallMotor,
	UCHAR LedNumber
)
{
	m.lock();

	static int count = 1;

	std::cout.width(3);
	std::cout << count++ << " ";
	std::cout.width(3);
	std::cout << (int)LargeMotor << " ";
	std::cout.width(3);
	std::cout << (int)SmallMotor << std::endl;

	m.unlock();
}

CControlServer::CControlServer(void)
{
	currentClient = 0;
}

CControlServer::~CControlServer(void)
{
}

void CControlServer::pressKey(int* input, int len)
{
	keydown(input, len);
	keyup(input, len);
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
BOOL CControlServer::initXbox(int indexClient)
{
	if (xbox[indexClient].client == NULL && xbox[indexClient].x360 == NULL) {
		xbox[indexClient].client = vigem_alloc();
		ret = vigem_connect(xbox[indexClient].client);
		xbox[indexClient].x360 = vigem_target_x360_alloc();
		ret = vigem_target_add(xbox[indexClient].client, xbox[indexClient].x360);
		ret = vigem_target_x360_register_notification(xbox[indexClient].client, xbox[indexClient].x360, &notification);
		return true;
	}
	return false;
}
void CControlServer::updateXbox(int indexClient, int* input, int len)
{
	XUSB_REPORT report;
	XUSB_REPORT_INIT(&report);
	ret = vigem_target_x360_update(xbox[indexClient].client, xbox[indexClient].x360, report);
	if (input[1]) report.wButtons = input[1];
	if (input[2]) report.bLeftTrigger = input[2];
	if (input[3]) report.bRightTrigger = input[3];
	if (input[4]) report.sThumbLX = input[4];
	if (input[5]) report.sThumbLY = input[5];
	if (input[6]) report.sThumbRX = input[6];
	if (input[7]) report.sThumbRY = input[7];

	ret = vigem_target_x360_update(xbox[indexClient].client, xbox[indexClient].x360, report);
}
BOOL CControlServer::deleteXbox(int indexClient)
{
	if (xbox[indexClient].client != NULL && xbox[indexClient].x360 != NULL) {
		vigem_target_x360_unregister_notification(xbox[indexClient].x360);
		vigem_target_remove(xbox[indexClient].client, xbox[indexClient].x360);
		vigem_target_free(xbox[indexClient].x360);
		xbox[indexClient].client = NULL;
		xbox[indexClient].x360 = NULL;
		return true;
	}
	return false;
}
void CControlServer::keydown(int* input, int len)
{
	INPUT* _input = new INPUT[len];
	for (int i = 0; i < len; i++) {
		_input[i].type = INPUT_KEYBOARD;
		_input[i].ki.wScan = 0;
		_input[i].ki.time = 0;
		_input[i].ki.dwExtraInfo = 0;
		_input[i].ki.wVk = input[i + 1];
		_input[i].ki.dwFlags = 0;
	}
	SendInput(len, _input, sizeof(INPUT));
	SAFE_DELETE_ARRAY(_input);
	const auto client = vigem_alloc();
}
void CControlServer::keyup(int* input, int len)
{
	INPUT* _input = new INPUT[len];
	for (int i = 0; i < len; i++) {
		_input[i].type = INPUT_KEYBOARD;
		_input[i].ki.wScan = 0;
		_input[i].ki.time = 0;
		_input[i].ki.dwExtraInfo = 0;
		_input[i].ki.wVk = input[i + 1];
		_input[i].ki.dwFlags = KEYEVENTF_KEYUP;
	}
	SendInput(len, _input, sizeof(INPUT));
	SAFE_DELETE_ARRAY(_input);
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
