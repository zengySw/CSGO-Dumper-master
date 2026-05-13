#include "HMain.h"
#include <iostream>
#include <TlHelp32.h>

// Функция для вывода списка процессов для отладки
void ListGModProcesses()
{
    std::cout << "[*] Searching for GMod processes..." << std::endl;
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        std::cout << "[-] Failed to create process snapshot!" << std::endl;
        return;
    }

    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
    bool found = false;
    
    if (Process32First(hSnapshot, &pe32) == TRUE) {
        do {
            std::string name = pe32.szExeFile;
            // Ищем любые процессы, связанные с GMod/Source
            if (name.find("hl2") != std::string::npos || 
                name.find("gmod") != std::string::npos ||
                name.find("source") != std::string::npos) {
                std::cout << "[+] Found: " << name << " (PID: " << pe32.th32ProcessID << ")" << std::endl;
                found = true;
            }
        } while (Process32Next(hSnapshot, &pe32) == TRUE);
    }
    
    CloseHandle(hSnapshot);
    
    if (!found) {
        std::cout << "[-] No GMod/Source processes found!" << std::endl;
        std::cout << "[*] Make sure Garry's Mod is running." << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    SetConsoleOutputCP(CP_UTF8); // Поддержка UTF-8 для русского текста
    
    std::cout << "[*] Starting GMod Dumper..." << std::endl;
    std::cout << "[*] Version: 1.0 (Debug Mode)" << std::endl << std::endl;
    
    // Показываем список процессов для отладки
    ListGModProcesses();
    
    // Пробуем зацепиться за hl2.exe, если нет — за gmod.exe
    std::cout << "[*] Attempting to attach to hl2.exe..." << std::endl;
    if (!pProcess->Attach("hl2.exe")) {
        std::cout << "[-] Failed to attach to hl2.exe" << std::endl;
        std::cout << "[*] Trying gmod.exe..." << std::endl;
        
        if (!pProcess->Attach("gmod.exe")) {
            std::cout << "[-] Garry's Mod process not found!" << std::endl;
            std::cout << "[*] Make sure the game is running." << std::endl;
            std::cout << "[*] Press Enter to exit..." << std::endl;
            std::cin.get();
            return -1;
        }
    }

    std::cout << "[+] Successfully attached to GMod!" << std::endl;
    std::cout << "[*] Process: " << pProcess->GetProcessName() << std::endl;
    std::cout << "[*] PID: " << pProcess->GetProcessID() << std::endl << std::endl;

    std::cout << "[*] Loading NetVars..." << std::endl;
    if (!pNetVarManager->Load()) {
        std::cout << "[-] Failed to load NetVars! (Signature might be outdated)" << std::endl;
        std::cout << "[*] Check if your GMod version is supported." << std::endl;
    }
    else {
        std::cout << "[+] NetVars loaded successfully." << std::endl;
        std::cout << "[*] Found " << pNetVarManager->GetTableCount() << " tables." << std::endl;
    }

    std::cout << "[*] Dumping NetVars..." << std::endl;
    pNetVarManager->Dump();
    
    std::cout << "[*] Dumping Offsets..." << std::endl;
    pOffsetManager->Dump();

    // Очищаем память NetVars, но НЕ удаляем сами синглтоны
    pNetVarManager->Release();
    pProcess->Detach();

    std::cout << "[+] Done. Check the .txt files." << std::endl;
    std::cout << "[*] Press Enter to exit..." << std::endl;
    std::cin.get();
    return 0;
}