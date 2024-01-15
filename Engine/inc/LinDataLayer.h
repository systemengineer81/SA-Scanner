#pragma once
#ifndef __LINDATALAYER_IO__H__
#define __LINDATALAYER_IO__H__

class IIoProvider;

class ILinDataLayer
{
public:

    typedef VOID (* pfnOnReadCallback)(void *context, const void *data, SIZE_T dataSize);

    virtual VOID SetOnReadCallback(pfnOnReadCallback callback, const void *context) = 0;
    //
    // Connect to device
    //
    virtual BOOL Connect(const TCHAR *szSource) = 0;

    //
    // Disconnect from device
    //
    virtual VOID Disconnect(void) = 0;

    // virtual destructor
    virtual ~ILinDataLayer()
    {
    }

    static BOOL CreateInstance(ILinDataLayer **instance);

};


#endif // __LINDATALAYER_IO__H__