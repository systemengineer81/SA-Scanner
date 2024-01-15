#pragma once

#ifndef __RAWREADER__H__
#define __RAWREADER__H__

class CRawReader : public IIoProvider
{
private:
    HANDLE   m_hFile;
    DWORD    m_dwBoudRate;

public:
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
    // Read all available data from the input stream
    //
    virtual DWORD ReadAll(void **pData, SIZE_T *nSize, DWORD dwTimeout = INFINITE);

    virtual VOID  ReleaseData(void *pData);

    //
    // Read exact data size from the stream
    //
    virtual DWORD Read(void *pData, SIZE_T nSize, DWORD dwTimeout);

    //
    // Write to device
    //
    virtual DWORD Write(const void *pData, SIZE_T nSize, DWORD dwTimeout = INFINITE);

    //
    // purge subsystem
    virtual DWORD Purge(void);
    // Set speed of connection, only for Serial
    virtual DWORD SetSpeed(DWORD dwBouds = CBR_9600);
    // Get state of work
    // virtual destructor
    virtual ~CRawReader()
    {
    }

    static BOOL CreateInstance(IIoProvider **instance);

};


#endif // __RAWREADER__H__