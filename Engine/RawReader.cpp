#include "pch.h"
#include "IoProvider.h"
#include "RawReader.h"

BOOL
CRawReader::CreateInstance(IIoProvider **instance)
{
    CRawReader *pNewInstance = new CRawReader();

    if (pNewInstance != NULL)
    {
        *instance = pNewInstance;
        return TRUE;
    }

    return FALSE;

}

BOOL
CRawReader::Connect(const TCHAR *)
{
    return FALSE;
}

BOOL
CRawReader::Connect(const HANDLE hHandle)
{

    /*HANDLE hFile = CreateFile(
        szSource,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);*/

    m_hFile = hHandle;

    return TRUE;
}

VOID
CRawReader::Disconnect(void)
{

    CloseHandle(m_hFile);
}

DWORD
CRawReader::ReadAll(void **pData, SIZE_T *nSize, DWORD dwTimeout)
{
    return 0;
}

VOID
CRawReader::ReleaseData(void *pData)
{
    delete[] pData;
}

DWORD
CRawReader::Read(void *pData, SIZE_T nSize, DWORD dwTimeout)
{
    return 0;
}

DWORD
CRawReader::Write(const void *pData, SIZE_T nSize, DWORD dwTimeout)
{
    return 0;
}


DWORD
CRawReader::Purge(void)
{
    return 0;
}

// Set speed of connection, only for Serial
DWORD
CRawReader::SetSpeed(DWORD dwBouds)
{
    DWORD oldSpeed;
    oldSpeed = m_dwBoudRate;
    m_dwBoudRate = dwBouds;
    return oldSpeed;
}

