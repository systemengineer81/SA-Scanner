
#include "pch.h"

#include "IoQueue.h"
#include "IoProvider.h"
#include "SerialIo.h"
#include "LinDataLayer.h"

class CLinDataLayer : public ILinDataLayer
{
private:
    pfnOnReadCallback m_pfnOnReadCallback;

    IIoProvider      *m_pIoProvider;
public:

    CLinDataLayer()
    {
        m_pIoProvider = NULL;
    }

    virtual VOID SetOnReadCallback(pfnOnReadCallback callback, const void *context);
    //
    // Connect to device
    //
    virtual BOOL Connect(const TCHAR *szSource);

    //
    // Disconnect from device
    //
    virtual VOID Disconnect(void);

    // virtual destructor
    virtual ~CLinDataLayer()
    {
    }

    static BOOL CreateInstance(ILinDataLayer **instance);

};


BOOL CLinDataLayer::CreateInstance(ILinDataLayer **instance)
{
    BOOL  result = FALSE;

    CLinDataLayer *pLinDataLayer = new CLinDataLayer();

    result = CSerialIo::CreateInstance(&pLinDataLayer->m_pIoProvider);

    *instance = pLinDataLayer;

    return TRUE;

}