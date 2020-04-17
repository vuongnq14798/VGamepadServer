#include "pch.h"
#include "Buffer.h"
CBuffer::CBuffer(void)
{
    isAvailable = true;
    isSend = false;
    numBuffering = 1;
    nBuffMax = 100;
    reservedCount = 0;
    InitializeCriticalSection(&CS);
    InitializeCriticalSection(&CSReservedCount);
}

CBuffer::~CBuffer(void)
{
    isAvailable = false;
    clearBuffer();

    DeleteCriticalSection(&CS);
    DeleteCriticalSection(&CSReservedCount);
}

void CBuffer::clearBuffer()
{
    EnterCriticalSection(&CS);

    while (!qBuffer.empty())
    {
        delete[](qBuffer.front());
        qBuffer.pop();
        qSize.pop();
        qPts.pop();
        DecreaseReservedCount();
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

int CBuffer::setMem(unsigned char* pBuf, unsigned int len, unsigned long pts)
{
    if (!isAvailable)
        return -1;

    EnterCriticalSection(&CS);
    IncreaseReservedCount();
    unsigned char* pTemp = new unsigned char[len + 1];
    memcpy(pTemp, pBuf, len);
    qBuffer.push(pTemp);
    qSize.push(len);
    qPts.push(pts);

    int ret = qBuffer.size();

    LeaveCriticalSection(&CS);

    return ret;
}

int	CBuffer::getMem(unsigned char*& pBuf, unsigned int& len, unsigned long& pts)
{
    if (!isAvailable)
        return -1;
    EnterCriticalSection(&CS);
    int ret = qBuffer.size();

    if (ret >= numBuffering) {
        isSend = true;
    }
    else if (ret == 0) {
        isSend = false;
    }
    if (isSend)
    {
        pBuf = qBuffer.front();
        len = qSize.front();
        pts = qPts.front();

        qBuffer.pop();
        qSize.pop();
        qPts.pop();
        DecreaseReservedCount();
    }
    else
        ret = 0;

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
    numBuffering = n;
}

void CBuffer::setNumBufferMax(int n)
{
    nBuffMax = n;
}

int CBuffer::getNumBufferMax()
{
    return nBuffMax;
}

int CBuffer::GetReservedCount()
{
    EnterCriticalSection(&CSReservedCount);
    int nRet = reservedCount;
    LeaveCriticalSection(&CSReservedCount);
    return nRet;
}

void CBuffer::IncreaseReservedCount()
{
    EnterCriticalSection(&CSReservedCount);
    reservedCount++;
    LeaveCriticalSection(&CSReservedCount);
}

void CBuffer::DecreaseReservedCount()
{
    EnterCriticalSection(&CSReservedCount);
    reservedCount--;
    LeaveCriticalSection(&CSReservedCount);
}