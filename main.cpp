#include <windows.h>
#include <shellapi.h>
#include "InputState.h"
#include "resource.h"

// =========================
// GLOBAL APP STATE
// =========================
bool g_enabled = true;
bool g_socd_enabled = true;
bool g_mouse_override_enabled = true;

// Axis-specific SOCD toggles
bool g_socd_x_enabled = true; // A / D
bool g_socd_y_enabled = true; // W / S

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

#define ID_TOGGLE_SOCD      2001
#define ID_TOGGLE_MOUSE     2002
#define ID_TOGGLE_SOCD_X    2007
#define ID_TOGGLE_SOCD_Y    2008
#define ID_HELP             2003
#define ID_UPDATE           2004
#define ID_VERSION          2005
#define ID_EXIT             2006

NOTIFYICONDATA nid = {};
HMENU hTrayMenu;
HINSTANCE g_hInst;

// =========================
// UPDATE TRAY MENU TEXT
// =========================
void UpdateMenuText() {

    ModifyMenu(
        hTrayMenu,
        ID_TOGGLE_SOCD,
        MF_BYCOMMAND | MF_STRING,
        ID_TOGGLE_SOCD,
        g_socd_enabled ? TEXT("Disable SnapTap") : TEXT("Enable SnapTap")
    );

    ModifyMenu(
        hTrayMenu,
        ID_TOGGLE_SOCD_X,
        MF_BYCOMMAND | MF_STRING,
        ID_TOGGLE_SOCD_X,
        g_socd_x_enabled ? TEXT("Disable X (A/D)") : TEXT("Enable X (A/D)")
    );

    ModifyMenu(
        hTrayMenu,
        ID_TOGGLE_SOCD_Y,
        MF_BYCOMMAND | MF_STRING,
        ID_TOGGLE_SOCD_Y,
        g_socd_y_enabled ? TEXT("Disable Y (W/S)") : TEXT("Enable Y (W/S)")
    );

    ModifyMenu(
        hTrayMenu,
        ID_TOGGLE_MOUSE,
        MF_BYCOMMAND | MF_STRING,
        ID_TOGGLE_MOUSE,
        g_mouse_override_enabled
            ? TEXT("Disable Mouse Override")
            : TEXT("Enable Mouse Override")
    );
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

            TrackPopupMenu(
                hTrayMenu,
                TPM_RIGHTBUTTON,
                pt.x,
                pt.y,
                0,
                hwnd,
                NULL
            );
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case ID_TOGGLE_SOCD:
            g_socd_enabled = !g_socd_enabled;
            break;

        case ID_TOGGLE_SOCD_X:
            g_socd_x_enabled = !g_socd_x_enabled;
            break;

        case ID_TOGGLE_SOCD_Y:
            g_socd_y_enabled = !g_socd_y_enabled;
            break;

        case ID_TOGGLE_MOUSE:
            g_mouse_override_enabled = !g_mouse_override_enabled;
            break;

        case ID_HELP:
            MessageBox(
                hwnd,
                TEXT("Email: soma27245@gmail.com"),
                TEXT("Get Help"),
                MB_OK | MB_ICONINFORMATION
            );
            break;

        case ID_UPDATE:
            ShellExecute(
                NULL,
                TEXT("open"),
                TEXT("https://github.com/SAPNXTDOOR/SnapKey-Pro/releases"),
                NULL,
                NULL,
                SW_SHOWNORMAL
            );
            break;

        case ID_VERSION:
            MessageBox(
                hwnd,
                TEXT(
                    "SnapKey Pro v1.2\n"
                    "Release Date: 15/1/26\n"
                    "Repository: github.com/SAPNXTDOOR/SnapKey-Pro\n\n"
                    "This app is made by Saptarshi Mallick."
                ),
                TEXT("Version Info"),
                MB_OK | MB_ICONINFORMATION
            );
            break;

        case ID_EXIT:
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
            break;
        }
        break;

    case WM_DESTROY:
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
    g_hInst = hInstance;

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("SnapKeyProTray");

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        TEXT("SnapKey Pro"),
        WS_OVERLAPPEDWINDOW,
        0, 0, 0, 0,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    // =========================
    // TRAY MENU
    // =========================
    hTrayMenu = CreatePopupMenu();

    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_SOCD,   TEXT("Disable SnapTap"));
    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_SOCD_X, TEXT("Disable X (A/D)"));
    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_SOCD_Y, TEXT("Disable Y (W/S)"));
    AppendMenu(hTrayMenu, MF_STRING, ID_TOGGLE_MOUSE,  TEXT("Disable Mouse Override"));
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hTrayMenu, MF_STRING, ID_HELP,    TEXT("Get Help"));
    AppendMenu(hTrayMenu, MF_STRING, ID_UPDATE,  TEXT("Check for updates"));
    AppendMenu(hTrayMenu, MF_STRING, ID_VERSION, TEXT("Version Info"));
    AppendMenu(hTrayMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hTrayMenu, MF_STRING, ID_EXIT, TEXT("Exit"));

    // =========================
    // TRAY ICON
    // =========================
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 1;
    nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON));
    lstrcpy(nid.szTip, TEXT("SnapKey Pro"));

    Shell_NotifyIcon(NIM_ADD, &nid);

    // =========================
    // INSTALL HOOKS
    // =========================
    InstallKeyboardHook();
    InstallMouseHook();

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
