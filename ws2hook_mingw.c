#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// No CRT startup code needed - this DLL only calls Windows API functions
// DllMainCRTStartup is the PE entry point (bypassed CRT init)

int WSAAPI GetHostNameW(PWCHAR name, int namelen) {
    char ansi_buf[256];
    int result = gethostname(ansi_buf, sizeof(ansi_buf));
    if (result == 0) {
        if (MultiByteToWideChar(CP_ACP, 0, ansi_buf, -1, name, namelen) == 0) {
            WSASetLastError(WSAEFAULT);
            return SOCKET_ERROR;
        }
    }
    return result;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    (void)hinstDLL;
    (void)lpvReserved;
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDLL);
            break;
        default:
            break;
    }
    return TRUE;
}

// Entry point that bypasses CRT initialization
BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    return DllMain(hinstDLL, fdwReason, lpvReserved);
}
