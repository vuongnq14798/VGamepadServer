#pragma once
#include <iostream>
#include <mutex>
#include <ViGEm\Client.h>
using namespace std;

typedef struct Xbox {
	PVIGEM_CLIENT client = NULL;
	PVIGEM_TARGET x360 = NULL;
}Clients;

class CControlServer
{
public:

	CControlServer(void);
	~CControlServer(void);

	void pressKey(int* input, int len);
	void OnLeftClick();
	void OnDoubleLClick();
	void OnRightClick();
	void OnLeftMouseDown();
	void OnLeftMouseUp();

	BOOL initXbox(int indexClient);
	void updateXbox(int indexClient, int* input, int len);
	BOOL deleteXbox(int indexClient);
	Xbox xbox[10];
	int currentClient;
	VIGEM_ERROR ret;
private:
	void leftdown();
	void leftup();
	void rightdown();
	void rightup();
	void keydown(int* input, int len);
	void keyup(int* input, int len);
};
