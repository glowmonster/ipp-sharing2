// Stub DLL for api-ms-win-core-synch-l1-2-0.dll
// Provides WaitOnAddress, WakeByAddressAll, WakeByAddressSingle
// These are never actually called in ipp-sharing's single-threaded async model
// Compiled without CRT: no malloc, no stdio, pure Windows API

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Stub: always return FALSE (not supported)
BOOL WINAPI WaitOnAddress(
    volatile void *Address,
    void *CompareAddress,
    SIZE_T AddressSize,
    DWORD dwMilliseconds
) {
    (void)Address;
    (void)CompareAddress;
    (void)AddressSize;
    (void)dwMilliseconds;
    return FALSE;
}

// Stub: no-op
void WINAPI WakeByAddressAll(void *Address) {
    (void)Address;
}

// Stub: no-op
void WINAPI WakeByAddressSingle(void *Address) {
    (void)Address;
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
