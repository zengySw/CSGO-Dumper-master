#ifndef _HUTILIS_H_
#define _HUTILIS_H_
#pragma once
#include "..\Include\HWin.h"
#include <ctime>
#include <string>

namespace Dumper
{
    namespace Utilis
    {
        // Замени всю функцию GetTime() на:
        inline std::string GetTime()
        {
            std::time_t t = std::time(nullptr);
            std::tm tm{};
            localtime_s(&tm, &t); // Windows-safe
            char buf[32];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
            return std::string(buf);
        }
    }
}
#endif /* _HUTILIS_H_ */