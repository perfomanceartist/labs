#include "trspo1.h"

bool FileExist(char* name)
{
    return _access(name, 0) != -1;
}

bool Inject(DWORD pID, char* path)
{
    HANDLE proc_handle;
    LPVOID RemoteString;
    LPCVOID LoadLibAddy;
    if (pID == 0)
        return false;
    proc_handle = OpenProcess(PROCESS_ALL_ACCESS, false, pID);
    if (proc_handle == 0)
        return false;
    LoadLibAddy = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    RemoteString = VirtualAllocEx(proc_handle, NULL, strlen(path), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    WriteProcessMemory(proc_handle, RemoteString, path, strlen(path), NULL);
    if (CreateRemoteThread(proc_handle, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, RemoteString, NULL, NULL) == NULL) {
        return false;
    }
    CloseHandle(proc_handle);
    return true;
}
int injector(DWORD pID) {
    char dll_name[32];
    char path[256];
    while (FileExist(path) == false)
    {
        strcpy_s(dll_name, "TRSPO1_DLL.dll");
        //printf("Enter DLL name: ");
        //scanf_s("%s", dll_name);
        GetFullPathNameA(dll_name, sizeof(path), path, NULL);
        if (FileExist(path))
        {
            cout << "Path:" << path << endl;
            printf("DLL found!\n");
            break;
        }
        else {
            cout << "Path:" << path << endl;
            printf("DLL not found!\n");
            Sleep(5000);
        }

    }
    printf("Preparing DLL for injection...\n");
    if (Inject(pID, path))
    {
        printf("DLL successfully injected!\n");
    }
    else
    {
        printf("CRITICAL ERROR! \nDestroying window...\n");
        Sleep(500);
        return -1;
    }
}

class HidingMonitor {
private:
    static HANDLE pipe;
    static bool InitPipe() {
        pipe = CreateNamedPipe(
            L"\\\\.\\pipe\\monitor_pipe", // name of the pipe
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_WAIT, // send data as a byte stream
            PIPE_UNLIMITED_INSTANCES, // only allow 1 instance of this pipe
            512, // no outbound buffer
            512, // no inbound buffer
            5000, // use default wait time
            NULL // use default security attributes
        );
        if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
            wcout << "Failed to create outbound pipe instance.";
            // look up error code here using GetLastError()
            system("pause");
            return false;
        }
    }
public:
    static bool monitoring(DWORD pID, string function) {
        if (!InitPipe()) return false;

        thread t1(injector, pID);
        wcout << "Waiting for a client to connect to the pipe..." << endl;
        // This call blocks until a client process connects to the pipe
        BOOL result = ConnectNamedPipe(pipe, NULL); //?
            //TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (!result) {
            wcout << "Failed to make connection on named pipe." << endl;
            cout << GetLastError() << endl;
            // look up error code here using GetLastError()

            CloseHandle(pipe); // close the pipe

            return false;
        }
        char data[512];
        strcpy(data, "-func ");
        strcat(data, function.c_str());
        wcout << "Sending data to pipe..." << endl;
        // This call blocks until a client process reads all the data
        //const wchar_t* data = L"OpenFile";
        DWORD numBytesWritten = 0;
        result = WriteFile(
            pipe, // handle to our outbound pipe
            data, // data to send
            strlen(data), // length of data to send (bytes)
            &numBytesWritten, // will store actual amount of data sent
            NULL // not using overlapped IO
        );
        if (result) {
            wcout << "Number of bytes sent: " << numBytesWritten << endl;
        }
        else {
            wcout << "Failed to send data." << endl;
            cout << "ERROR: " << GetLastError() << endl;
            return false;
            // look up error code here using GetLastError()
        }

        t1.detach();

        cout << "Function calls: " << endl;
        char buffer[1000]; DWORD numBytesRead;
        while (true) {
            result = ReadFile(
                pipe,
                buffer, // the data from the pipe will be put here
                1000, // number of bytes allocated
                &numBytesRead, // this will store number of bytes actually read
                NULL // not using overlapped IO
            );
            if (!result) {
                cout << "ReadFile returned FALSE" << endl;
                DisconnectNamedPipe(pipe);
                CloseHandle(pipe);
                //system("pause");
                return false;
            }

            buffer[numBytesRead] = '\0';
            time_t now = time(0);

            // convert now to string form
            char dt[256];
            ctime_s(dt, 256, &now);
            cout << buffer;
            cout << " " << dt;

        }


        // Close the pipe (automatically disconnects client too)
        CloseHandle(pipe);
        wcout << "Done." << endl;
        system("pause");
        return true;
    }
    static bool hiding(DWORD pID, string FileName) {

        if (!InitPipe()) return false;

        thread t1(injector, pID);
        wcout << "Waiting for a client to connect to the pipe..." << endl;
        // This call blocks until a client process connects to the pipe
        BOOL result = ConnectNamedPipe(pipe, NULL); //?
            //TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (!result) {
            wcout << "Failed to make connection on named pipe." << endl;
            cout << GetLastError() << endl;
            // look up error code here using GetLastError()

            CloseHandle(pipe); // close the pipe

            return false;
        }

        WCHAR wdata[512]; wstring WFileName = wstring(FileName.begin(), FileName.end());
        lstrcpyW(wdata, L"-hide ");
        lstrcatW(wdata, WFileName.c_str());
        char data[512];
        strcpy(data, "-hide ");
        strcat(data, FileName.c_str());
        wcout << "Sending data to pipe..." << endl;
        // This call blocks until a client process reads all the data
        //const wchar_t* data = L"OpenFile";
        DWORD numBytesWritten = 0;
        result = WriteFile(
            pipe, // handle to our outbound pipe
            (char*)data, // data to send
            FileName.size() + 6, // length of data to send (bytes)
            &numBytesWritten, // will store actual amount of data sent
            NULL // not using overlapped IO
        );
        if (result) {
            wcout << "Number of bytes sent: " << numBytesWritten << endl;
        }
        else {
            wcout << "Failed to send data." << endl;
            cout << "ERROR: " << GetLastError() << endl;
            return false;
            // look up error code here using GetLastError()
        }

        t1.detach();

        return true;
    }
};