#pragma once

#include "ini_reader.h"

#include <atomic>
#include <string>
#include <thread>

class FovManager
{
public:
    static FovManager& Instance();

    void Start();
    void Stop();

    float Multiplier() const;
    void SetMultiplier(float multiplier, bool persist = true);

    void ApplyNow() const;

private:
    FovManager();

    std::wstring ResolveIniPath() const;
    void LoadSettings();
    void SaveSettings() const;
    void SaveRegistry() const;
    float ReadRegistryMultiplier(float defaultValue) const;
    void WorkerLoop();

    IniReader ini_;
    std::atomic<bool> running_{ false };
    std::thread worker_;
    float multiplier_ = 1.0f;
    float minMultiplier_ = 0.50f;
    float maxMultiplier_ = 2.00f;
    float step_ = 0.05f;
    bool enableHotkeys_ = true;
    bool applyEveryFrame_ = true;
};
