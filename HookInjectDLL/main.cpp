#include "trspo1.h"

void printHelp() {
    cout << "Usage:" << endl;
    cout << "-pid 123 -func CreateFile" << endl;
    cout << "-pid 123 -hide C:\hello.txt" << endl;
    cout << "-name explorer.exe -func CreateFile" << endl;
    cout << "-name explorer.exe -hide C:\hello.txt" << endl;
    system("pause");
}


DWORD getProcessByName(const wchar_t* name) {
    HANDLE  hProcess = NULL; DWORD pid = 0;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    BOOL bProcess = Process32First(hTool32, &pe32);
    if (bProcess == TRUE)
    {
        while ((Process32Next(hTool32, &pe32)) == TRUE) {
            if (!wcscmp(pe32.szExeFile, name)) {
                pid = pe32.th32ProcessID;
                break;
            }
        }
    }
    CloseHandle(hTool32);
    return pid;
}
HANDLE getHandleByPID(DWORD pid) {
    HANDLE  hProcess = NULL;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    BOOL bProcess = Process32First(hTool32, &pe32);
    if (bProcess == TRUE)
    {
        while ((Process32Next(hTool32, &pe32)) == TRUE) {
            if (pe32.th32ProcessID == pid) {
                hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
                break;
            }
        }
    }
    CloseHandle(hTool32);
    return hProcess;
}


int main(int argc, char* argv[])
{
    if (argc < 5) {
        printHelp();
        return 1;
    }

    DWORD pID = 0;
    string process_id_type = string(argv[1]);
    string process_id = string(argv[2]);
    string action = string(argv[3]);
    string argument = string(argv[4]);

    if (process_id_type == "-pid") {
        pID = (DWORD)stoi(process_id);
        cout << "Waiting " << process_id << " for start...\n";
        HANDLE hProc = NULL;
        while (hProc == NULL) {
            hProc = OpenProcess(READ_CONTROL, FALSE, pID);
            Sleep(250);
        }
        CloseHandle(hProc);
    }
    else if (process_id_type == "-name") {
        wstring process_name = wstring(process_id.begin(), process_id.end());
        cout << "Waiting " << process_id << " for start...\n";
        while (pID == 0) {
            pID = getProcessByName(process_name.c_str());
            Sleep(250);
        }
    }
    else {
        printHelp();
        return 1;
    }
    
    if (action == "-func") {
        if(!HidingMonitor::monitoring(pID, argument)) cout << "Some error occured." << endl;
    }
    else if (action == "-hide") {
        if (!HidingMonitor::hiding(pID, argument)) cout << "Some error occured." << endl;
    }
    else {
        printHelp();
        return 1;
    }
    return 0;
}
