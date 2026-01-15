#include <windows.h>
#include <shellapi.h>
#include "InputState.h"
#include "resource.h"

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

wchar_t g_iniPath[MAX_PATH];

void InitIniPath() {
    GetModuleFileNameW(NULL, g_iniPath, MAX_PATH);
    PathRemoveFileSpecW(g_iniPath);
    wcscat_s(g_iniPath, L"\\settings.ini");
}


// =========================
// PORTABLE SETTINGS (INI)
// =========================
extern wchar_t g_iniPath[MAX_PATH];
const wchar_t* INI_SECTION = L"SnapKeyPro";


void SaveSettings() {
    WritePrivateProfileStringW(L"SnapKeyPro", L"SnapTap",
        g_socd_enabled ? L"1" : L"0", g_iniPath);

    WritePrivateProfileStringW(L"SnapKeyPro", L"SOCD_X",
        g_socd_x_enabled ? L"1" : L"0", g_iniPath);

    WritePrivateProfileStringW(L"SnapKeyPro", L"SOCD_Y",
        g_socd_y_enabled ? L"1" : L"0", g_iniPath);

    WritePrivateProfileStringW(L"SnapKeyPro", L"MouseOverride",
        g_mouse_override_enabled ? L"1" : L"0", g_iniPath);

    WritePrivateProfileStringW(L"SnapKeyPro", L"MouseOverride_X",
        g_mouse_override_x_enabled ? L"1" : L"0", g_iniPath);

    WritePrivateProfileStringW(L"SnapKeyPro", L"MouseOverride_Y",
        g_mouse_override_y_enabled ? L"1" : L"0", g_iniPath);
}



void LoadSettings() {
    g_socd_enabled = GetPrivateProfileIntW(
        L"SnapKeyPro", L"SnapTap", 1, g_iniPath);

    g_socd_x_enabled = GetPrivateProfileIntW(
        L"SnapKeyPro", L"SOCD_X", 1, g_iniPath);

    g_socd_y_enabled = GetPrivateProfileIntW(
        L"SnapKeyPro", L"SOCD_Y", 1, g_iniPath);

    g_mouse_override_enabled = GetPrivateProfileIntW(
        L"SnapKeyPro", L"MouseOverride", 1, g_iniPath);

    g_mouse_override_x_enabled = GetPrivateProfileIntW(
        L"SnapKeyPro", L"MouseOverride_X", 1, g_iniPath);

    g_mouse_override_y_enabled = GetPrivateProfileIntW(
        L"SnapKeyPro", L"MouseOverride_Y", 1, g_iniPath);
}




// =========================
// GLOBAL APP STATE
// =========================
bool g_enabled = true;

// SOCD toggles
bool g_socd_enabled = true;
bool g_socd_x_enabled = true;
bool g_socd_y_enabled = true;

// Mouse override toggles
bool g_mouse_override_enabled = true;
bool g_mouse_override_x_enabled = true;
bool g_mouse_override_y_enabled = true;

// =========================
// INPUT STATE DEFINITIONS
// =========================
bool pW=false, pA=false, pS=false, pD=false;
bool pFire=false;

bool sW=false, sA=false, sS=false, sD=false;
bool lW=false, lA=false, lS=false, lD=false;

bool prevW=false, prevA=false, prevS=false, prevD=false;

char lastH=0;
char lastV=0;

// =========================
// TRAY DEFINITIONS
// =========================
#define WM_TRAYICON (WM_USER + 1)

#define ID_TOGGLE_SOCD        2001
#define ID_TOGGLE_SOCD_X      2002
#define ID_TOGGLE_SOCD_Y      2003
#define ID_TOGGLE_MOUSE       2004
#define ID_TOGGLE_MOUSE_X     2005
#define ID_TOGGLE_MOUSE_Y     2006
#define ID_HELP               2007
#define ID_UPDATE             2008
#define ID_VERSION            2009
#define ID_EXIT               2010

NOTIFYICONDATA nid = {};
HMENU hTrayMenu;

// =========================
// UPDATE MENU TEXT
// =========================
void UpdateMenuText() {

    ModifyMenu(hTrayMenu, ID_TOGGLE_SOCD,
        MF_BYCOMMAND | MF_STRING, ID_TOGGLE_SOCD,
        g_socd_enabled ? TEXT("Disable SnapTap") : TEXT("Enable SnapTap"));

    ModifyMenu(hTrayMenu, ID_TOGGLE_SOCD_X,
        MF_BYCOMMAND | MF_STRING, ID_TOGGLE_SOCD_X,
        g_socd_x_enabled ? TEXT("Disable SOCD X (A/D)") : TEXT("Enable SOCD X (A/D)"));

    ModifyMenu(hTrayMenu, ID_TOGGLE_SOCD_Y,
        MF_BYCOMMAND | MF_STRING, ID_TOGGLE_SOCD_Y,
        g_socd_y_enabled ? TEXT("Disable SOCD Y (W/S)") : TEXT("Enable SOCD Y (W/S)"));
    ModifyMenu(hTrayMenu, ID_TOGGLE_MOUSE,
        MF_BYCOMMAND | MF_STRING, ID_TOGGLE_MOUSE,
        g_mouse_override_enabled ? TEXT("Disable Mouse Override") : TEXT("Enable Mouse Override"));

    ModifyMenu(hTrayMenu, ID_TOGGLE_MOUSE_X,
        MF_BYCOMMAND | MF_STRING, ID_TOGGLE_MOUSE_X,
        g_mouse_override_x_enabled
            ? TEXT("Disable Mouse Override X (A/D)")
            : TEXT("Enable Mouse Override X (A/D)"));

    ModifyMenu(hTrayMenu, ID_TOGGLE_MOUSE_Y,
        MF_BYCOMMAND | MF_STRING, ID_TOGGLE_MOUSE_Y,
        g_mouse_override_y_enabled
            ? TEXT("Disable Mouse Override Y (W/S)")
            : TEXT("Enable Mouse Override Y (W/S)"));
}

// =========================
// WINDOW PROC
// =========================
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);
            UpdateMenuText();
            TrackPopupMenu(hTrayMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case ID_TOGGLE_SOCD:        g_socd_enabled = !g_socd_enabled; break;
        case ID_TOGGLE_SOCD_X:      g_socd_x_enabled = !g_socd_x_enabled; break;
        case ID_TOGGLE_SOCD_Y:      g_socd_y_enabled = !g_socd_y_enabled; break;

        case ID_TOGGLE_MOUSE:       g_mouse_override_enabled = !g_mouse_override_enabled; break;
        case ID_TOGGLE_MOUSE_X:     g_mouse_override_x_enabled = !g_mouse_override_x_enabled; break;
        case ID_TOGGLE_MOUSE_Y:     g_mouse_override_y_enabled = !g_mouse_override_y_enabled; break;

        case ID_HELP:
            MessageBox(hwnd, TEXT("Email: soma27245@gmail.com"),
                TEXT("Get Help"), MB_OK | MB_ICONINFORMATION);
            break;

        case ID_UPDATE:
            ShellExecute(NULL, TEXT("open"),
                TEXT("https://github.com/SAPNXTDOOR/SnapKey-Pro/releases"),
                NULL, NULL, SW_SHOWNORMAL);
            break;

        case ID_VERSION:
            MessageBox(hwnd,
                TEXT("SnapKey Pro v1.4.1\n"
                     "Release Date: 15/1/26\n"
                     "Repository: github.com/SAPNXTDOOR/SnapKey-Pro\n\n"
                     "This app is made by Saptarshi Mallick."),
                TEXT("Version Info"),
                MB_OK | MB_ICONINFORMATION);
            break;

        case ID_EXIT:
            SaveSettings();
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
        SaveSettings();
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// =========================
// WINMAIN
// =========================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("SnapKeyProTray");
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName,
        TEXT("SnapKey Pro"), WS_OVERLAPPEDWINDOW,
        0, 0, 0, 0, NULL, NULL, hInstance, NULL);

    hTrayMenu = CreatePopupMenu();

    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_SOCD, TEXT("Disable SnapTap"));
    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_SOCD_X, TEXT("Disable SOCD X (A/D)"));
    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_SOCD_Y, TEXT("Disable SOCD Y (W/S)"));
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);
    
    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_MOUSE, TEXT("Disable Mouse Override"));
    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_MOUSE_X, TEXT("Disable Mouse Override X (A/D)"));
    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_MOUSE_Y, TEXT("Disable Mouse Override Y (W/S)"));
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hTrayMenu, MF_STRING, ID_HELP, TEXT("Get Help"));
    AppendMenu(hTrayMenu, MF_STRING, ID_UPDATE, TEXT("Check for updates"));
    AppendMenu(hTrayMenu, MF_STRING, ID_VERSION, TEXT("Version Info"));
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hTrayMenu, MF_STRING, ID_EXIT, TEXT("Exit"));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    lstrcpy(nid.szTip, TEXT("SnapKey Pro"));

    Shell_NotifyIcon(NIM_ADD, &nid);

    InitIniPath();
    LoadSettings();
    InstallKeyboardHook();
    InstallMouseHook();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
