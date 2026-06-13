// Stub DLL for api-ms-win-core-winrt-error-l1-1-0.dll
// Provides RoOriginateErrorW — does nothing, returns success
// Compiled without CRT: no malloc, no stdio, pure Windows API

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Stub: just pretend the error was set successfully
BOOL WINAPI RoOriginateErrorW(
    HRESULT error,
    UINT cchMax,
    PCWSTR message
) {
    (void)error;
    (void)cchMax;
    (void)message;
    return TRUE; // "success"
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
