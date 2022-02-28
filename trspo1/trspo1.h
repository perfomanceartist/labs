#ifndef TRSPO1_H
#define TRSPO1_H
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <io.h>
#include <string>
#include <thread>
#include <ctime>
using namespace std;

class HidingMonitor {
public:
    static bool hiding(DWORD pID, string FileName);
    static bool monitoring(DWORD pID, string function);
};

bool Inject(DWORD pID, char* path);
int injector(DWORD pID);
#endif