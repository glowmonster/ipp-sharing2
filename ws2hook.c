// ws2hook.c - Proxy DLL for ws2_32.dll with GetHostNameW workaround for Win7
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// Define GetHostNameW ourselves with dllexport to override the dllimport from SDK header
int WSAAPI GetHostNameW(PWCHAR name, int namelen) {
    char ansi_buf[256];
    int result;

    // gethostname exists on all Windows versions including Win7
    result = gethostname(ansi_buf, sizeof(ansi_buf));

    if (result == 0) {
        // Convert ANSI hostname to wide char
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
