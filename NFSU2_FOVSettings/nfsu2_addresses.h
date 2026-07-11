#pragma once

#include <cstdint>

namespace NFSU2
{
    // Static FOV scale floats used by the gameplay projection matrix.
    inline constexpr std::uintptr_t FOV_HorizontalA = 0x007A27DC;
    inline constexpr std::uintptr_t FOV_HorizontalB = 0x007A27E0;
    inline constexpr std::uintptr_t FOV_Vertical = 0x007A27D8;

    // Default values shipped with the base game (before widescreen correction).
    inline constexpr float DefaultFOVHorizontal = 1.1f;
    inline constexpr float DefaultFOVVertical = 0.86956525f;

    // Display settings load/save (registry-backed, called from the Options menu).
    inline constexpr std::uintptr_t LoadDisplaySettings = 0x005BEA20;
    inline constexpr std::uintptr_t SaveDisplaySettings = 0x005BEEA0;

    inline constexpr wchar_t RegistryKey[] =
        L"Software\\EA Games\\Need for Speed Underground 2";
    inline constexpr wchar_t RegistryFOVValue[] = L"CustomFOVMultiplier";
}
