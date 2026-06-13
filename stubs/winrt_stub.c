// Stub DLL for api-ms-win-core-winrt-l1-1-0.dll
// Provides RoGetActivationFactory — returns E_NOTIMPL
// Compiled without CRT: no malloc, no stdio, pure Windows API

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// WinRT types we need to define ourselves (no WinRT headers needed for stubs)
typedef struct HSTRING__ { int unused; } *HSTRING;

// Stub: always return "not implemented"
HRESULT WINAPI RoGetActivationFactory(
    HSTRING activatableClassId,
    REFIID iid,
    void **factory
) {
    (void)activatableClassId;
    (void)iid;
    if (factory) *factory = NULL;
    return E_NOTIMPL;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    (void)hinstDLL;
    (void)lpvReserved;
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
    }
    return TRUE;
}

// Entry point that bypasses CRT initialization
BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return DllMain(hinstDLL, fdwReason, lpvReserved);
}
