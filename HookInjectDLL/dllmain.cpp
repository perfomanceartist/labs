
#include "pch.h"
#include "detours.h"
#include <iostream>
#include "hiding.h"
#pragma warning(disable  : 4996)

HANDLE pipe;
char FunctionName[512];

LPVOID DynamicTarget = NULL;
void notify_monitor()
{
	DWORD		 NumberReadBytes;
	DWORD	     NumberWriteBytes;
	DWORD		 NumberWrittenBytes;

	CHAR MessageToSend[1000];
	memset(MessageToSend, 0, 1000);
	strcpy(MessageToSend, FunctionName);
	strcat(MessageToSend, " is called");
	//MessageToSend[strlen()] = '\0';
	WriteFile(pipe, MessageToSend, strlen(MessageToSend) + 1, &NumberWrittenBytes, NULL);

	/*SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	sprintf_s(DataDate, "%02d:%02d:%02d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
	if (Timer == NULL || strcmp(DataDate, Timer))
	{
		if (Timer == NULL)
			Timer = (char*)calloc(10000, sizeof(char));
		sprintf(Timer, "%02d:%02d:%02d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
		sprintf_s(MessageToSend, "%s()\tTime: %s", FunctionName, Timer);
		NumberWriteBytes = (lstrlenA(MessageToSend) + 1) * sizeof(CHAR);
		WriteFile(pipe, MessageToSend, NumberWriteBytes, &NumberWrittenBytes, NULL);
	}*/

}






void __declspec(naked) HookingRoutine32() {
	__asm {
		PUSHFD
		PUSHAD
	}
	notify_monitor();
	__asm
	{
		// we set everything back to normal
		POPAD
		POPFD
		mov eax, DynamicTarget;
		push eax;
		ret;
	}

}



bool monitoring() {
	char data[512];
	DWORD numBytesWritten;
	DynamicTarget = (PDWORD)DetourFindFunction((LPCSTR)"kernel32.dll", (LPCSTR)FunctionName); //находим функцию в стандартной библиотеке
	if (DynamicTarget == NULL) {
		memset(data, 0, 512);
		strcpy(data, "error in DetourFindFunction");
		WriteFile(pipe, data, strlen(data), &numBytesWritten, NULL);
		return FALSE;
	}

	DetourTransactionBegin(); //(вызывается перед установкой захвата)
	DetourUpdateThread(GetCurrentThread()); //(инициализирует захват для указанного потока)
	DetourAttach(&(PVOID&)DynamicTarget, HookingRoutine32); //привязываем к функции в стандартной библиотеке свою
	DetourTransactionCommit();

	return true;
}

bool hiding() {
	size_t pos_slash = FileName.rfind('\\');

	FullPath = FileName;
	if ((int)pos_slash != -1)
		FullPath = FileName.substr(pos_slash + 1);
	WFull_Path = std::wstring(FullPath.begin(), FullPath.end());
	

	//аналогично вызову с подменой 
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pCreateFileA, MyCreateFileA_withHide);
	LONG err = DetourTransactionCommit();
	if (err != NO_ERROR)
		return false;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pCreateFileW, MyCreateFileW_withHide);
	err = DetourTransactionCommit();
	if (err != NO_ERROR)
		return false;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pFindFirstFileW, MyFindFirstFileW_withHide);
	err = DetourTransactionCommit();
	if (err != NO_ERROR)
		return false;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pFindFirstFileA, MyFindFirstFileA_withHide);
	err = DetourTransactionCommit();
	if (err != NO_ERROR)
		return false;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pFindNextFileW, MyFindNextFileW_withHide);
	err = DetourTransactionCommit();
	if (err != NO_ERROR)
		return false;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pFindNextFileA, MyFindNextFileA_withHide);
	err = DetourTransactionCommit();
	if (err != NO_ERROR)
		return false;
	
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pFindFirstFileExW, MyFindFirstFileExW_withHide);
	err = DetourTransactionCommit();
	if (err != NO_ERROR)
		return false;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)pFindFirstFileExA, MyFindFirstFileExA_withHide);
	err = DetourTransactionCommit();
	if (err != NO_ERROR)
		return false;

	return true;
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: {
		pipe = CreateFile(
			L"\\\\.\\pipe\\monitor_pipe",
			GENERIC_READ | GENERIC_WRITE, // only need read access
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
		if (pipe == INVALID_HANDLE_VALUE) {
			std::wcout << "Failed to connect to pipe." << std::endl;
			// look up error code here using GetLastError()
			system("pause");
			return false;
		}

		// The read operation will block until there is data to read
		char buffer[512];
		DWORD numBytesRead = 0;
		BOOL result = ReadFile(pipe, buffer, 512 * sizeof(char), &numBytesRead, NULL);
		if (result) buffer[numBytesRead] = '\0'; // null terminate the string
		else return false;

		if (numBytesRead > 6) {
			if (!strncmp(buffer, "-func ", 6)) {
				memset(FunctionName, 0, 512);
				strncpy(FunctionName, buffer + 6, numBytesRead - 6);
				return monitoring();
			}
			else if (!strncmp(buffer, "-hide ", 6)) {
				
				FileName = buffer;
				FileName = FileName.substr(6);
				return hiding();
			}
		}

	}
						   break;
	case DLL_PROCESS_DETACH: {
		//CloseHandle(pipe);
	}
						   break;
	default:
		break;
	}

	return TRUE;
}


