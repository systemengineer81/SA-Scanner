
#pragma once

#ifndef __IO_PROVIDER_H__
#define __IO_PROVIDER_H__

//#include "Event.h"

class IIoProvider
{
public:
    typedef struct _DATA_INFO {
        LARGE_INTEGER ElapsedTime;
        PVOID         data;
        SIZE_T        size;
    } DATA_INFO, *PDATA_INFO;
public:
    //
    // events
    //
    Sharp::Event<DATA_INFO>   m_OnReadData;

    //
    // Connect to device
    //
    virtual BOOL Connect(const TCHAR *szSource) = 0;
    virtual BOOL Connect(const HANDLE hHandle) = 0;

    //
    // Disconnect from device
    //
    virtual VOID Disconnect(void) = 0;

    //
    // Read all available data from the input stream
    //
    virtual DWORD ReadAll(void **pData, SIZE_T *nSize, DWORD dwTimeout = INFINITE) = 0;

    virtual VOID  ReleaseData(void *pData) = 0;

    //
    // Read exact data size from the stream
    //
    virtual DWORD Read(void *pData, SIZE_T nSize, DWORD dwTimeout) = 0;

    //
    // Write to device
    //
    virtual DWORD Write(const void *pData, SIZE_T nSize, DWORD dwTimeout = INFINITE) = 0;

    //
    // purge subsystem
    virtual DWORD Purge( void ) = 0;
    // Set speed of connection, only for Serial
    virtual DWORD SetSpeed(DWORD dwBouds = CBR_9600) = 0;
    // Get state of work
    // virtual destructor
    virtual ~IIoProvider()
    {
    }

    //static BOOL CreateInstance(IIoProvider **instance);
};



#endif // __IO_PROVIDER_H__
