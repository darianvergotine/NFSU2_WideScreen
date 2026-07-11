#include "fov_manager.h"

#include <Windows.h>

namespace
{
    void DisableThreadLibraryCallsForDll(HMODULE module)
    {
        ::DisableThreadLibraryCalls(module);
    }

    DWORD WINAPI InitThread(LPVOID)
    {
        // Wait until the main executable has finished its startup work.
        Sleep(3000);
        FovManager::Instance().Start();
        return 0;
    }
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCallsForDll(module);

        HANDLE thread = CreateThread(
            nullptr,
            0,
            InitThread,
            nullptr,
            0,
            nullptr);

        if (thread)
        {
            CloseHandle(thread);
        }
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        FovManager::Instance().Stop();
    }

    return TRUE;
}
