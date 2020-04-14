#pragma once

#include <windows.h>
#include <queue>

using namespace std;

class CBuffer
{
public:
	CBuffer(void);
	~CBuffer(void);

public:
	int setMem(unsigned char* pBuf, unsigned int len, unsigned long pts);
	int	getMem(unsigned char*& pBuf, unsigned int& len, unsigned long &pts);
	void deleteMem(unsigned char*& pBuf);
	void clearBuffer();
	int getNumBuffer();
	void SetBufferingCount(int n);
	void setNumBufferMax(int n);
	int getNumBufferMax();
	int GetReservedCount();
private:
	queue<unsigned char*> qBuffer;
	queue<int> qSize; 
	queue<unsigned long> qPts;
	CRITICAL_SECTION CS;
	bool isAvailable;	
	bool isSend;
	int numBuffering;
	int nBuffMax;
	CRITICAL_SECTION CSReservedCount;
	int	reservedCount;

	void IncreaseReservedCount();
	void DecreaseReservedCount();
};
