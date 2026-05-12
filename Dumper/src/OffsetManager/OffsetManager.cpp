#include "HOffsetManager.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include "../Utilis/HUtilis.h"
#include "../NetVarManager/HNetVarManager.h"
#include <iostream>

namespace Dumper
{
    namespace OffsetManager
    {
        void COffsetManager::Dump(void)
        {
            if (!pProcess->GetModuleByName("client.dll") || !pProcess->GetModuleByName("engine.dll")) {
                std::cout << "[-] Required modules not found!" << std::endl;
                return;
            }

            std::stringstream ss;
            ss << "- - - - - - Garry's Mod Offset Dumper - - - - - - " << std::endl;
            ss << "| -> " << Utilis::GetTime();
            ss << "- -" << std::endl << std::endl;

            // === NETVARS ===
            // Если NetVars не загрузились, здесь будут нули, но паттерны ниже найдутся
            DumpNetVar("DT_BasePlayer", "m_iHealth", 0x0, ss);
            DumpNetVar("DT_BasePlayer", "m_fFlags", 0x0, ss);
            DumpNetVar("DT_BasePlayer", "m_lifeState", 0x0, ss);
            DumpNetVar("DT_BasePlayer", "m_hActiveWeapon", 0x0, ss);
            DumpNetVar("DT_BasePlayer", "m_vecViewOffset[0]", 0x0, ss);
            DumpNetVar("DT_BasePlayer", "m_nTickBase", 0x0, ss);

            DumpNetVar("DT_BaseEntity", "m_vecOrigin", 0x0, ss);
            DumpNetVar("DT_BaseEntity", "m_iTeamNum", 0x0, ss);
            DumpNetVar("DT_BaseEntity", "m_bSpotted", 0x0, ss);

            // === PATTERNS (Актуальные для GMod x86) ===

            // LocalPlayer: Ищет указатель на локального игрока
            DumpPatternOffset("LocalPlayer", "dwLocalPlayer", "client.dll",
                "A1 ? ? ? ? 85 C0 74 07 05 ? ? ? ? EB 02 33 C0",
                Remote::SignatureType_t::READ | Remote::SignatureType_t::SUBTRACT,
                0x1, 0x0, ss);

            // EntityList: Ищет указатель на список сущностей
            DumpPatternOffset("EntityList", "dwEntityList", "client.dll",
                "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 8B 47 04",
                Remote::SignatureType_t::READ | Remote::SignatureType_t::SUBTRACT,
                0x2, 0x0, ss);

            // ViewMatrix: Ищет матрицу вида (для ESP) в engine.dll
            DumpPatternOffset("EngineRender", "dwViewMatrix", "engine.dll",
                "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9",
                Remote::SignatureType_t::READ | Remote::SignatureType_t::SUBTRACT,
                0x3, 0x0, ss);

            // ForceJump: Ищет адрес переменной +jump (для Bhop)
            DumpPatternOffset("Extra", "dwForceJump", "client.dll",
                "89 15 ? ? ? ? 8B 15 ? ? ? ? 83 FA 05 75 0B",
                Remote::SignatureType_t::READ | Remote::SignatureType_t::SUBTRACT,
                0x2, 0x0, ss);

            std::ofstream("GMod_OffsetManager.txt") << ss.str();
            std::cout << "[+] Offsets dumped to GMod_OffsetManager.txt" << std::endl;
        }

        void COffsetManager::DumpNetVar(const std::string& tablename, const std::string& varname, uintptr_t offset, std::stringstream& ss)
        {
            uintptr_t result = pNetVarManager->GetNetVar(tablename, varname) + offset;
            LogToStringStream(tablename, varname, result, ss);
        }

        void COffsetManager::DumpPatternOffset(const std::string& tablename, const std::string& varname, const std::string& module, const char* pattern, int type, uintptr_t pattern_offset, uintptr_t address_offset, std::stringstream& ss)
        {
            uintptr_t result = pProcess->FindPattern(module, pattern, type, pattern_offset, address_offset);
            LogToStringStream(tablename, varname, result, ss);
        }

        void COffsetManager::LogToStringStream(const std::string& tablename, const std::string& varname, uintptr_t offset, std::stringstream& ss)
        {
            ss << std::setw(48)
                << std::setfill('_')
                << std::left
                << tablename + " -> " + varname + ": "
                << std::right
                << std::hex
                << " 0x"
                << std::setw(8)
                << std::setfill('0')
                << std::uppercase
                << offset << std::endl;
        }

        COffsetManager* COffsetManager::Singleton(void)
        {
            static auto g_pOffsetManager = new COffsetManager();
            return g_pOffsetManager;
        }
    }
}