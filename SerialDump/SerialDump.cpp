// SerialDump.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "stdafx.h"
#include <conio.h>

#include "LinBus.h"

void Dump(int nSpaces, void *pData, SIZE_T nDataSize);

int main()
{
    IIoProvider     *pIoProvider = 0;
    //ILinBus         *pPhysicalLayer = 0;
    PVOID            data = 0;
    SIZE_T           dataSize = 0;

    _tprintf(_T("[*] LibDump begin\n"));

    HANDLE hDumpFile = CreateFile(
        _T("dump.dmp"),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        /*CREATE_ALWAYS,*/ OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hDumpFile == INVALID_HANDLE_VALUE)
    {
        _tprintf(_T("[-] failed to create dump file\n"));
        return -1;
    }

    CRawReader::CreateInstance(&pIoProvider);

    pIoProvider->Connect(hDumpFile);

    /*if (CSerialIo::CreateInstance(&pPhysicalLayer) != TRUE)
    {
        _tprintf(_T("[-] Failed to create instance\n"));
        return -1;
    }*/


    _tprintf(_T("[+] LibDump begin to listen COM port\n"));

    pIoProvider->SetSpeed(CBR_19200);

    /*if (pIoProvider->Connect(_T("\\\\.\\COM14")) != TRUE)
    {
        _tprintf(_T("[-] failed to connect to port\n"));

        CloseHandle(hDumpFile);
        delete pPhysicalLayer;
        return -1;
    }*/


    do
    {

        //pPhysicalLayer->Write("1234|5678|5678|5678|1234|5678|5678|5678|1234\0", sizeof("1234|5678|5678|5678|1234|5678|5678|5678|1234\0") - 1);

        //UCHAR buffer[5];

        /*if (pPhysicalLayer->Read(buffer, 5, 1000) == ERROR_SUCCESS)
        {
   
            Dump(0, buffer, 5);

            //WriteFile(hDumpFile, data, (DWORD)dataSize, &dwWritten, NULL);

        }
        else
        {
            _tprintf(_T("[-] timeout\n"));
        }*/

        if (pIoProvider->ReadAll(&data, &dataSize, 1000) == ERROR_SUCCESS)
        {
            DWORD dwWritten;
   
            Dump(0, data, dataSize);

            WriteFile(hDumpFile, data, (DWORD)dataSize, &dwWritten, NULL);

            pIoProvider->ReleaseData(data);
        }
        else
        {
            _tprintf(_T("[-] timeout\n"));
        }

        if (_kbhit()) {

            break;
        }


    }
    while (true);

    _getch();

    pIoProvider->Disconnect();

    delete pIoProvider;

    CloseHandle(hDumpFile);


    return 0;
}

#define HIGH_ANSI(a) (TCHAR)((a & 0xF0) < 0xA0 ? ((a & 0xF0) >> 4) + 0x30 : ((a & 0xF0) >> 4) + 0x37)
#define LOW_ANSI(a) (TCHAR)((a & 0x0F) < 0xA ? (a & 0x0F) + 0x30 : (a & 0x0F) + 0x37)
#define PUT_CHAR(a) (TCHAR)((a > 0x21) ? a : '.')

void
Dump(int nSpaces, void *pData, SIZE_T nDataSize)
{
    TCHAR *szDebug_string =
        reinterpret_cast<TCHAR *>(malloc(1000));

    if (szDebug_string == NULL) return;

    unsigned int nSize = nSpaces + (16 * 3 + 3 + 17 + 3);

    unsigned char* pDataPointer = reinterpret_cast<unsigned char *>(pData);
    unsigned char* pDataEnd = pDataPointer + nDataSize;
    TCHAR* pStringPointer;

    for (unsigned int i = 0; i<nSize; i++)
        *(szDebug_string + i) = 0x20;

    _stprintf(szDebug_string, _T("DUMP[%d]\n"), (int)nDataSize);
    _tprintf(szDebug_string);

    while (pDataPointer < pDataEnd)
    {
        pStringPointer = szDebug_string;
        *(szDebug_string + nSpaces + 17 * 3) = _T('|');
        *(szDebug_string + nSpaces + 17 * 3 + 17) = 0x20;
        *(szDebug_string + nSpaces + 17 * 3 + 18) = _T('\n');
        *(szDebug_string + nSpaces + 17 * 3 + 19) = 0;

        for (int i = 0; i< 16; i++)
        {
            if (pDataPointer < pDataEnd)
            {
                *(szDebug_string + nSpaces + 16 * 3 + 5 + i) = PUT_CHAR(*pDataPointer);
                *(pStringPointer++) = HIGH_ANSI(*pDataPointer);
                *(pStringPointer++) = LOW_ANSI(*pDataPointer);
            }
            else
            {
                *(szDebug_string + nSpaces + 16 * 3 + 5 + i) = 0x20;
                *(pStringPointer++) = 0x20;
                *(pStringPointer++) = 0x20;
            }
            pDataPointer++;
            *(pStringPointer++) = 0x20;
        }

        _tprintf(szDebug_string);
    }

    free(szDebug_string);
}
