#include "ini_reader.h"

#include <cwchar>

IniReader::IniReader(const std::wstring& filePath)
    : path_(filePath)
{
}

int IniReader::ReadInt(const wchar_t* section, const wchar_t* key, int defaultValue) const
{
    return static_cast<int>(GetPrivateProfileIntW(section, key, defaultValue, path_.c_str()));
}

float IniReader::ReadFloat(const wchar_t* section, const wchar_t* key, float defaultValue) const
{
    wchar_t buffer[64] = {};
    GetPrivateProfileStringW(
        section,
        key,
        L"",
        buffer,
        static_cast<DWORD>(std::size(buffer)),
        path_.c_str());

    if (buffer[0] == L'\0')
    {
        return defaultValue;
    }

    return static_cast<float>(_wtof(buffer));
}

bool IniReader::WriteFloat(const wchar_t* section, const wchar_t* key, float value) const
{
    wchar_t buffer[64] = {};
    swprintf_s(buffer, L"%.3f", value);
    return WritePrivateProfileStringW(section, key, buffer, path_.c_str()) != FALSE;
}
