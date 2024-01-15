#pragma once

#ifndef __DEBUG_H__
#define __DEBUG_H__


#ifdef _DEBUG

extern void Dump(int nSpaces, void *pData, unsigned int nDataSize);

#ifdef _UNICODE
extern void OutputDebugInfoW(const wchar_t *format, ...);
#define __TRACE(_x_) 
#define _TRACE(_x_)  OutputDebugInfoW _x_ 
#else // _UNICODE
extern void OutputDebugInfoA(const char *format, ...);
#define _TRACE(_x_) OutputDebugInfoA _x_
#endif // _UNICODE

#   define _DBG(_x_) ::OutputDebugString(_x_)
#   define _DMP(spaces, data, size) Dump(spaces, data, size)

#else // _DEBUG
#   define _DBG(_x_)
#   define _DMP(spaces, data, size)

#endif // _DEBUG

#endif // __DEBUG_H__