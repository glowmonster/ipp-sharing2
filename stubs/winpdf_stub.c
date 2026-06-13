// Stub DLL for windows.data.pdf.dll (Win8+ WinRT PDF API)
// Exports PdfCreateRenderer — returns E_NOTIMPL
// ipp-sharing uses Win32 print APIs, not WinRT PDF, so this is never called

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

HRESULT WINAPI PdfCreateRenderer(void) {
    return E_NOTIMPL;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    (void)lpvReserved;
    if (fdwReason == DLL_PROCESS_ATTACH) DisableThreadLibraryCalls(hinstDLL);
    return TRUE;
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return DllMain(hinstDLL, fdwReason, lpvReserved);
}
