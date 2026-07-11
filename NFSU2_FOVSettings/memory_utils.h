#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstring>

namespace Memory
{
    inline bool WriteProtectedMemory(void* address, const void* data, std::size_t size)
    {
        if (!address || !data || size == 0)
        {
            return false;
        }

        DWORD oldProtect = 0;
        if (!VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect))
        {
            return false;
        }

        std::memcpy(address, data, size);

        DWORD ignored = 0;
        VirtualProtect(address, size, oldProtect, &ignored);
        FlushInstructionCache(GetCurrentProcess(), address, size);
        return true;
    }

    template <typename T>
    inline bool WriteValue(std::uintptr_t address, const T& value)
    {
        return WriteProtectedMemory(reinterpret_cast<void*>(address), &value, sizeof(T));
    }

    template <typename T>
    inline T ReadValue(std::uintptr_t address)
    {
        return *reinterpret_cast<const T*>(address);
    }
}
