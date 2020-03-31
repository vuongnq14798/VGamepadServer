#include "pch.h"
#include "Buffer.h"
CBuffer::CBuffer(void)
{
    isAvailable = true;
    numBuffer = 1;
    nBuffMax = 100;
    InitializeCriticalSection(&CS);

}

CBuffer::~CBuffer(void)
{
    isAvailable = false;
    clearBuffer();

    DeleteCriticalSection(&CS);
}

void CBuffer::clearBuffer()
{
    EnterCriticalSection(&CS);

    while (qBuffer.size() > 0)
    {
        delete[](qBuffer.front());
        qBuffer.pop();
        qSize.pop();
    }

    LeaveCriticalSection(&CS);
}

void CBuffer::deleteMem(unsigned char*& pBuf)
{
    EnterCriticalSection(&CS);

    if (pBuf)
    {
        delete[] pBuf;
        pBuf = NULL;
    }

    LeaveCriticalSection(&CS);
}

int CBuffer::setMem(unsigned char* pBuf, unsigned int len)
{
    if (!isAvailable)
        return -1;

    EnterCriticalSection(&CS);
    UCHAR* pTemp = new unsigned char[len + 1];
    memcpy(pTemp, pBuf, len);
    qBuffer.push(pTemp);
    qSize.push(len);

    int ret = qBuffer.size();

    LeaveCriticalSection(&CS);

    return ret;
}

int	CBuffer::getMem(unsigned char*& pBuf, unsigned int& len)
{
    if (!isAvailable)
        return -1;
    EnterCriticalSection(&CS);
    int ret = qBuffer.size();

    if (ret > 0)
    {
        pBuf = qBuffer.front();
        len = qSize.front();
        qBuffer.pop();
        qSize.pop();
    }

    LeaveCriticalSection(&CS);
    return ret;
}

int CBuffer::getNumBuffer()
{
    EnterCriticalSection(&CS);
    int nRet = qBuffer.size();
    LeaveCriticalSection(&CS);
    return nRet;
}

void CBuffer::SetBufferingCount(int n)
{
    numBuffer = n;
}

void CBuffer::setNumBufferMax(int n)
{
    nBuffMax = n;
}

int CBuffer::getNumBufferMax()
{
    return nBuffMax;
}
