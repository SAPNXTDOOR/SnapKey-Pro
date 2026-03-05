#include <windows.h>
#include "InputState.h"

HHOOK keyboardHook = NULL;
HHOOK mouseHook = NULL;

/* =========================
   KEYBOARD HOOK
   ========================= */
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* k = (KBDLLHOOKSTRUCT*)lParam;

        // Ignore injected input
        if (k->flags & LLKHF_INJECTED) {
            return CallNextHookEx(NULL, nCode, wParam, lParam);
        }

        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isKeyUp   = (wParam == WM_KEYUP   || wParam == WM_SYSKEYUP);

        if (isKeyDown || isKeyUp) {

            switch (k->vkCode) {

            case 'A':
                if (isKeyDown && !pA) { pA = true; lastH = 'A'; }
                else if (isKeyUp)      { pA = false; }
                break;

            case 'D':
                if (isKeyDown && !pD) { pD = true; lastH = 'D'; }
                else if (isKeyUp)      { pD = false; }
                break;

            case 'W':
                if (isKeyDown && !pW) { pW = true; lastV = 'W'; }
                else if (isKeyUp)      { pW = false; }
                break;

            case 'S':
                if (isKeyDown && !pS) { pS = true; lastV = 'S'; }
                else if (isKeyUp)      { pS = false; }
                break;

            default:
                break;
            }

            Update();

            // Block raw WASD ONLY when enabled
            if (g_enabled &&
                (k->vkCode == 'W' || k->vkCode == 'A' ||
                 k->vkCode == 'S' || k->vkCode == 'D')) {
                return 1;
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

/* =========================
   MOUSE HOOK
   ========================= */
LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_LBUTTONDOWN) {
            pFire = true;
            Update();
        } else if (wParam == WM_LBUTTONUP) {
            pFire = false;
            Update();
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

/* =========================
   INSTALL
   ========================= */
void InstallKeyboardHook() {
    keyboardHook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardProc,
        NULL,
        0
    );
}

void InstallMouseHook() {
    mouseHook = SetWindowsHookEx(
        WH_MOUSE_LL,
        MouseProc,
        NULL,
        0
    );
}
