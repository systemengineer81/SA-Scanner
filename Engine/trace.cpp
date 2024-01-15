
#include "pch.h"

#ifdef _DEBUG   // not include this in the release build

#define HIGH_ANSI(a) (TCHAR)((a & 0xF0) < 0xA0 ? ((a & 0xF0) >> 4) + 0x30 : ((a & 0xF0) >> 4) + 0x37)
#define LOW_ANSI(a) (TCHAR)((a & 0x0F) < 0xA ? (a & 0x0F) + 0x30 : (a & 0x0F) + 0x37)
#define PUT_CHAR(a) (TCHAR)((a > 0x21) ? a : '.')


void
Dump(int nSpaces, void *pData, unsigned int nDataSize)
{
    unsigned int nSize = nSpaces + (16 * 3 + 3 + 17 + 3);

    TCHAR* pString = reinterpret_cast<TCHAR *>(
        malloc(nSize * sizeof(TCHAR)));

    unsigned char* pDataPointer = reinterpret_cast<unsigned char *>(pData);
    unsigned char* pDataEnd = pDataPointer + nDataSize;
    TCHAR* pStringPointer;

    for (unsigned int i = 0; i<nSize; i++)
        *(pString + i) = 0x20;

    _stprintf(pString, _T("DUMP[%d]\n"), nDataSize);
    _DBG(pString);

    while (pDataPointer < pDataEnd)
    {
        pStringPointer = pString;
        *(pString + nSpaces + 17*3) = _T('|');
        *(pString + nSpaces + 17*3 + 17) = 0x20;
        *(pString + nSpaces + 17*3 + 18) = _T('\n');
        *(pString + nSpaces + 17*3 + 19) = 0;

        for (int i = 0; i< 16; i++)
        {
            if (pDataPointer < pDataEnd)
            {
                *(pString + nSpaces + 16 * 3 + 5 + i) = PUT_CHAR(*pDataPointer);
                *(pStringPointer++) = HIGH_ANSI(*pDataPointer);
                *(pStringPointer++) = LOW_ANSI(*pDataPointer);
            }
            else
            {
                *(pString + nSpaces + 16 * 3 + 5 + i) = 0x20;
                *(pStringPointer++) = 0x20;
                *(pStringPointer++) = 0x20;
            }

            pDataPointer++;
            *(pStringPointer++) = 0x20;
        }

        _DBG(pString);
    }

    free(pString);
}

#define MAX_MEMORY_ALLOC 0x1000

#ifdef _UNICODE

#define _TRACE(_x_) OutputDebugInfoW _x_

void
OutputDebugInfoW(const wchar_t *format, ...)
{
    va_list          vl;

    va_start(vl, format);

    wchar_t     *wcDebug_string = 
        reinterpret_cast<wchar_t*>(malloc(MAX_MEMORY_ALLOC));

    if (wcDebug_string == NULL) return;

    __try
    {

        SYSTEMTIME time;
        ::GetLocalTime(&time);

        swprintf(wcDebug_string, L"%02d:%02d:%04d | TID %04d > ", 
            time.wMinute, time.wSecond, time.wMilliseconds, GetCurrentThreadId());

        _vsnwprintf(wcDebug_string + wcslen(wcDebug_string), 
            MAX_MEMORY_ALLOC - sizeof(L"00:00:0000 | TID 0000 > \0\0\0\0") / sizeof(wchar_t),
            format, vl);

        ::OutputDebugStringW(wcDebug_string);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        ::OutputDebugStringW(L"DEBUG OVERFLOW!!!\r\n");
    }

    free(wcDebug_string);

    va_end(vl);
}

#else // _UNICODE

#define _TRACE(_x_) OutputDebugInfoA _x_

void OutputDebugInfoA(const char *format, ...)
{
    va_list          vl;

    va_start(vl, format);

    char *szDebug_string = 
        reinterpret_cast<char*>(malloc(MAX_MEMORY_ALLOC));

    if (szDebug_string == NULL) return;

    __try
    {
        SYSTEMTIME time;
        ::GetLocalTime(&time);

        sprintf(szDebug_string, "%02d:%02d:%04d | TID %04d > ", 
            time.wMinute, time.wSecond, time.wMilliseconds, GetCurrentThreadId());

        _vsnprintf(szDebug_string + strlen(szDebug_string), 
            MAX_MEMORY_ALLOC - sizeof("00:00:0000 | TID 0000 > \0\0\0\0"), 
            format, vl);

        ::OutputDebugStringA(szDebug_string);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        ::OutputDebugStringA("DEBUG OVERFLOW!!!\r\n");
    }

    free(wcDebug_string);

    va_end(vl);
}

#endif // _UNICODE

#endif // _DEBUG