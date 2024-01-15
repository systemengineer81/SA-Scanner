#pragma once

#ifndef __SERIAL_IO__H__
#define __SERIAL_IO__H__

class CSerialIoListener;

class CSerialIo : public IIoProvider
{
private:
    HANDLE              m_hConnectionPort;
    HANDLE              m_hStartIoSemaphore;

    OVERLAPPED          m_OverlappedWrite;
    HANDLE              m_hCompleteEvent;
    DWORD               m_dwBoudRate;

    CIoQueue            m_IoReadQueue;

    DWORD               m_dwBuffersSize;
    PQUEUE_EVENT        m_CapturedEvent;

    CSerialIoListener*  m_IoListener[2];

    DWORD SetupConnection();
public:

    CSerialIo()
        : m_hConnectionPort(INVALID_HANDLE_VALUE)
        , m_dwBoudRate(CBR_9600)
        , m_dwBuffersSize(4096)
    {
    }

    //
    // Connect to device
    //
    virtual BOOL Connect(const TCHAR *szSource);
    virtual BOOL Connect(const HANDLE hHandle);

    //
    // Disconnect from device
    //
    virtual VOID Disconnect(void);

    //
    // Read all avalaible data from input stream
    //
    virtual DWORD ReadAll(void **ppData, SIZE_T *nSize, DWORD dwTimeout);

    //
    // Read exact data size from the stream
    //
    virtual DWORD Read(void *pData, SIZE_T nSize, DWORD dwTimeout);

    //
    // release data received
    //
    virtual VOID  ReleaseData(void *pData);


    //
    // Write to device
    //
    virtual DWORD Write(const void *pData, SIZE_T nSize, DWORD dwTimeout);

    //
    // Flush query
    //
    virtual DWORD Purge(void);

    //
    // Set speed of connection, only for Serial
    //
    virtual DWORD SetSpeed(DWORD nSpeed);

    virtual ~CSerialIo()
    {
    }

    static BOOL CreateInstance(IIoProvider **instance);
};

#endif // __SERIAL_IO__H__