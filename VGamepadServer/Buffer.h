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
	int setMem(unsigned char* pBuf, unsigned int len);
	int	getMem(unsigned char*& pBuf, unsigned int& len);
	void deleteMem(unsigned char*& pBuf);
	void clearBuffer();
	int getNumBuffer();
	void SetBufferingCount(int n);
	void setNumBufferMax(int n);
	int getNumBufferMax();

private:
	queue<unsigned char*>		qBuffer;
	queue<int>					qSize; 
	CRITICAL_SECTION			CS;
	bool						isAvailable;	
	int							numBuffer;
	int							nBuffMax;
};
