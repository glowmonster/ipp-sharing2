// Proxy stub for combase.dll (Win8+ only DLL, doesn't exist on Win7)
// Provides CoIncrementMTAUsage (Win8+) and forwards CoCreateFreeThreadedMarshaler to ole32.dll
// Compiled without CRT

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <objbase.h>

// Stub: CoIncrementMTAUsage (Win8+, not in Win7's ole32)
HRESULT WINAPI CoIncrementMTAUsage(CO_MTA_USAGE_COOKIE *pCookie) {
    (void)pCookie;
    // Try to ensure COM is initialized (harmless if already done)
    // CoInitializeEx is ref-counted, safe to call multiple times
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (pCookie) {
        *pCookie = (CO_MTA_USAGE_COOKIE)(ULONG_PTR)1;
    }
    return S_OK;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    (void)lpvReserved;
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
    }
    return TRUE;
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return DllMain(hinstDLL, fdwReason, lpvReserved);
}
