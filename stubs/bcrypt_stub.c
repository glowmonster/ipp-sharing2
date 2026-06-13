// Proxy stub for bcryptprimitives.dll
// Provides ProcessPrng for Win7 using RtlGenRandom (SystemFunction036)
// All other functions forwarded to the real bcryptprimitives.dll (via .def)
// Compiled without CRT

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// RtlGenRandom aka SystemFunction036 from advapi32.dll
// Available since Windows XP, signature:
//   BOOLEAN NTAPI SystemFunction036(PVOID RandomBuffer, ULONG RandomBufferLength)
typedef BOOLEAN (WINAPI *RtlGenRandom_t)(PVOID, ULONG);

BOOL WINAPI ProcessPrng(PBYTE pbData, SIZE_T cbData) {
    HMODULE advapi = GetModuleHandleW(L"advapi32.dll");
    if (!advapi) return FALSE;
    
    RtlGenRandom_t fn = (RtlGenRandom_t)GetProcAddress(advapi, "SystemFunction036");
    if (!fn) return FALSE;
    
    // Win7 kernel with SystemFunction036 supported up to ~4GB, but in practice
    // Rust HashMap seeding requests are tiny (16-32 bytes), well within ULONG
    return fn(pbData, (ULONG)cbData);
}

// ProcessPrngGuid: the EXE doesn't import this but keep it for completeness
BOOL WINAPI ProcessPrngGuid(LPGUID lpGuid) {
    if (!lpGuid) return FALSE;
    return ProcessPrng((PBYTE)lpGuid, sizeof(GUID));
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
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
