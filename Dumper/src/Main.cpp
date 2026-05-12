#include "HMain.h"
#include <iostream>

int main()
{
    std::cout << "[*] Starting GMod Dumper..." << std::endl;

    // Пробуем зацепиться за hl2.exe, если нет — за gmod.exe
    if (!pProcess->Attach("hl2.exe") && !pProcess->Attach("gmod.exe")) {
        std::cout << "[-] Garry's Mod process not found!" << std::endl;
        std::cout << "[*] Make sure the game is running." << std::endl;
        return -1;
    }

    std::cout << "[+] Attached to GMod!" << std::endl;

    if (!pNetVarManager->Load()) {
        std::cout << "[-] Failed to load NetVars! (Signature might be outdated)" << std::endl;
    }
    else {
        std::cout << "[+] NetVars loaded successfully." << std::endl;
    }

    pNetVarManager->Dump();
    pOffsetManager->Dump();

    // Очищаем память NetVars, но НЕ удаляем сами синглтоны
    pNetVarManager->Release();
    pProcess->Detach();

    std::cout << "[+] Done. Check the .txt files." << std::endl;
    std::cin.get();
    return 0;
}