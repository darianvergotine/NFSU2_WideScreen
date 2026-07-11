#pragma once

#include <Windows.h>
#include <string>

class IniReader
{
public:
    explicit IniReader(const std::wstring& filePath);

    int ReadInt(const wchar_t* section, const wchar_t* key, int defaultValue) const;
    float ReadFloat(const wchar_t* section, const wchar_t* key, float defaultValue) const;
    bool WriteFloat(const wchar_t* section, const wchar_t* key, float value) const;

    const std::wstring& Path() const { return path_; }

private:
    std::wstring path_;
};
