#pragma once
#include <iostream>
using namespace std;
class CControlServer
{
public:

	CControlServer(void);
	~CControlServer(void);

	void pressKey(char* buf, int bufLen);
	void OnLeftClick();
	void OnDoubleLClick();
	void OnRightClick();
	void OnLeftMouseDown();
	void OnLeftMouseUp();

private:
	void leftdown();
	void leftup();
	void rightdown();
	void rightup();
	void keydown(char* buf, int bufLen);
	void keyup(char* buf, int bufLen);
};