#pragma once
#include <Windows.h>
#include <string>

std::string	 FileName;
std::string	 FullPath;
std::wstring WFull_Path;

char charFullPath[512] = { 0 };
WCHAR wcharFullPath[512] = { 0 };

using namespace std;

HANDLE(WINAPI* pCreateFileA) (LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileA;
HANDLE(WINAPI* pCreateFileW) (LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = CreateFileW;
HANDLE(WINAPI* pFindFirstFileW) (LPCWSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData) = FindFirstFileW;
HANDLE(WINAPI* pFindFirstFileA)(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) = FindFirstFileA;
BOOL(WINAPI* pFindNextFileW) (HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData) = FindNextFileW;
BOOL(WINAPI* pFindNextFileA) (HANDLE hFindFile, LPWIN32_FIND_DATAA  lpFindFileData) = FindNextFileA;
HANDLE(WINAPI* pFindFirstFileExA) (LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExA;
HANDLE(WINAPI* pFindFirstFileExW) (LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId, LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp, LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExW;

HANDLE WINAPI MyCreateFileA_withHide(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    
    if (FullPath == lpFileName)
    {
        return INVALID_HANDLE_VALUE;
    }
    return pCreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI MyCreateFileW_withHide(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
    if (!lstrcmpW(lpFileName, WFull_Path.c_str()))
    {
        return INVALID_HANDLE_VALUE;
    }
    return pCreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI MyFindFirstFileA_withHide(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
    
    if (FullPath == lpFileName)
    {
        return INVALID_HANDLE_VALUE;
    }
    return pFindFirstFileA(lpFileName, lpFindFileData);
}

HANDLE WINAPI MyFindFirstFileW_withHide(LPCWSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
    if (!lstrcmpW(lpFileName, WFull_Path.c_str()))
    {
        return INVALID_HANDLE_VALUE;
    }
    return pFindFirstFileW(lpFileName, lpFindFileData);
}

BOOL WINAPI MyFindNextFileA_withHide(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData)
{
    bool ret = pFindNextFileA(hFindFile, lpFindFileData);
    if (lpFindFileData->cFileName == FullPath)
    {
        ret = pFindNextFileA(hFindFile, lpFindFileData);
    }
    return ret;
}

BOOL WINAPI MyFindNextFileW_withHide(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData)
{
    bool ret = pFindNextFileW(hFindFile, lpFindFileData);
    if (lpFindFileData->cFileName == WFull_Path)
    {
        ret = pFindNextFileW(hFindFile, lpFindFileData);
    }
    return ret;
}

HANDLE WINAPI MyFindFirstFileExW_withHide(LPCWSTR a0, FINDEX_INFO_LEVELS a1, LPWIN32_FIND_DATAW a2, FINDEX_SEARCH_OPS a3, LPVOID a4, DWORD a5)
{
    HANDLE ret = pFindFirstFileExW(a0, a1, a2, a3, a4, a5);
    if (a2->cFileName == WFull_Path)
    {
        ret = INVALID_HANDLE_VALUE;
    }
    return ret;
}

HANDLE WINAPI MyFindFirstFileExA_withHide(LPCSTR a0, FINDEX_INFO_LEVELS a1, LPWIN32_FIND_DATAA a2, FINDEX_SEARCH_OPS a3, LPVOID a4, DWORD a5)
{
    HANDLE ret = pFindFirstFileExA(a0, a1, a2, a3, a4, a5);
    if (a2->cFileName == FullPath)
    {
        ret = INVALID_HANDLE_VALUE;
        //pFindNextFileA(ret, a2);
    }
    return ret;
}

