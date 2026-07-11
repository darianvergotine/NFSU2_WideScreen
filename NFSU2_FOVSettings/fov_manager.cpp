#include "fov_manager.h"

#include "memory_utils.h"
#include "nfsu2_addresses.h"

#include <algorithm>
#include <cmath>
#include <filesystem>

FovManager& FovManager::Instance()
{
    static FovManager instance;
    return instance;
}

FovManager::FovManager()
    : ini_(ResolveIniPath())
{
    LoadSettings();
}

std::wstring FovManager::ResolveIniPath() const
{
    wchar_t modulePath[MAX_PATH] = {};
    GetModuleFileNameW(GetModuleHandleW(nullptr), modulePath, MAX_PATH);

    std::filesystem::path gameDirectory = std::filesystem::path(modulePath).parent_path();
    std::filesystem::path scriptsDirectory = gameDirectory / L"scripts";
    std::filesystem::path iniPath = scriptsDirectory / L"NFSU2_FOVSettings.ini";

    std::error_code errorCode;
    std::filesystem::create_directories(scriptsDirectory, errorCode);
    return iniPath.wstring();
}

void FovManager::LoadSettings()
{
    minMultiplier_ = ini_.ReadFloat(L"FOV", L"MinMultiplier", minMultiplier_);
    maxMultiplier_ = ini_.ReadFloat(L"FOV", L"MaxMultiplier", maxMultiplier_);
    step_ = ini_.ReadFloat(L"FOV", L"Step", step_);
    enableHotkeys_ = ini_.ReadInt(L"FOV", L"EnableHotkeys", 1) != 0;
    applyEveryFrame_ = ini_.ReadInt(L"FOV", L"ApplyEveryFrame", 1) != 0;

    const float iniMultiplier = ini_.ReadFloat(L"FOV", L"Multiplier", 1.0f);
    const float registryMultiplier = ReadRegistryMultiplier(iniMultiplier);
    multiplier_ = std::clamp(registryMultiplier, minMultiplier_, maxMultiplier_);
}

void FovManager::SaveSettings() const
{
    ini_.WriteFloat(L"FOV", L"Multiplier", multiplier_);
    SaveRegistry();
}

float FovManager::ReadRegistryMultiplier(float defaultValue) const
{
    HKEY key = nullptr;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, NFSU2::RegistryKey, 0, KEY_READ, &key) != ERROR_SUCCESS)
    {
        return defaultValue;
    }

    DWORD type = REG_NONE;
    DWORD size = sizeof(float);
    float value = defaultValue;
    const LSTATUS status = RegQueryValueExW(
        key,
        NFSU2::RegistryFOVValue,
        nullptr,
        &type,
        reinterpret_cast<LPBYTE>(&value),
        &size);
    RegCloseKey(key);

    if (status != ERROR_SUCCESS || type != REG_BINARY)
    {
        return defaultValue;
    }

    return value;
}

void FovManager::SaveRegistry() const
{
    HKEY key = nullptr;
    if (RegCreateKeyExW(
            HKEY_CURRENT_USER,
            NFSU2::RegistryKey,
            0,
            nullptr,
            REG_OPTION_NON_VOLATILE,
            KEY_SET_VALUE,
            nullptr,
            &key,
            nullptr) != ERROR_SUCCESS)
    {
        return;
    }

    const float value = multiplier_;
    RegSetValueExW(
        key,
        NFSU2::RegistryFOVValue,
        0,
        REG_BINARY,
        reinterpret_cast<const BYTE*>(&value),
        sizeof(value));
    RegCloseKey(key);
}

float FovManager::Multiplier() const
{
    return multiplier_;
}

void FovManager::SetMultiplier(float multiplier, bool persist)
{
    multiplier_ = std::clamp(multiplier, minMultiplier_, maxMultiplier_);
    ApplyNow();

    if (persist)
    {
        SaveSettings();
    }
}

void FovManager::ApplyNow() const
{
    const float horizontal = NFSU2::DefaultFOVHorizontal * multiplier_;
    const float vertical = NFSU2::DefaultFOVVertical * multiplier_;

    Memory::WriteValue(NFSU2::FOV_HorizontalA, horizontal);
    Memory::WriteValue(NFSU2::FOV_HorizontalB, horizontal);
    Memory::WriteValue(NFSU2::FOV_Vertical, vertical);
}

void FovManager::WorkerLoop()
{
    bool increaseWasDown = false;
    bool decreaseWasDown = false;
    bool resetWasDown = false;

    while (running_.load())
    {
        if (applyEveryFrame_)
        {
            ApplyNow();
        }

        if (enableHotkeys_)
        {
            const bool increaseDown =
				(GetAsyncKeyState(VK_ADD) & 0x8000) != 0; // Use the "+" key to increase the FOV multiplier
            const bool decreaseDown =
				(GetAsyncKeyState(VK_SUBTRACT) & 0x8000) != 0; // Use the "-" key to decrease the FOV multiplier
            const bool resetDown =
				(GetAsyncKeyState(VK_MULTIPLY) & 0x8000) != 0; // Use the "*" key to reset the FOV multiplier to 1.0

            if (increaseDown && !increaseWasDown)
            {
                SetMultiplier(multiplier_ + step_);
            }
            else if (decreaseDown && !decreaseWasDown)
            {
                SetMultiplier(multiplier_ - step_);
            }
            else if (resetDown && !resetWasDown)
            {
                SetMultiplier(1.0f);
            }

            increaseWasDown = increaseDown;
            decreaseWasDown = decreaseDown;
            resetWasDown = resetDown;
        }

        Sleep(16);
    }
}

void FovManager::Start()
{
    if (running_.exchange(true))
    {
        return;
    }

    ApplyNow();
    worker_ = std::thread(&FovManager::WorkerLoop, this);
}

void FovManager::Stop()
{
    if (!running_.exchange(false))
    {
        return;
    }

    if (worker_.joinable())
    {
        worker_.join();
    }
}
