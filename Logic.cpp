#include "InputState.h"

// =========================
// EMIT ONLY ON CHANGE
// =========================
void EmitIfChanged() {
    if (lW != prevW) SendKey('W', lW);
    if (lA != prevA) SendKey('A', lA);
    if (lS != prevS) SendKey('S', lS);
    if (lD != prevD) SendKey('D', lD);

    prevW = lW;
    prevA = lA;
    prevS = lS;
    prevD = lD;
}

// =========================
// SOCD RESOLUTION (AXIS AWARE)
// =========================
void ResolveSOCD() {

    // -------- HORIZONTAL (A / D) --------
    if (g_socd_enabled && g_socd_x_enabled) {
        if (pA && pD) {
            sA = (lastH == 'A');
            sD = (lastH == 'D');
        } else {
            sA = pA;
            sD = pD;
        }
    } else {
        // SOCD disabled for X axis
        sA = pA;
        sD = pD;
    }

    // -------- VERTICAL (W / S) --------
    if (g_socd_enabled && g_socd_y_enabled) {
        if (pW && pS) {
            sW = (lastV == 'W');
            sS = (lastV == 'S');
        } else {
            sW = pW;
            sS = pS;
        }
    } else {
        // SOCD disabled for Y axis
        sW = pW;
        sS = pS;
    }
}

// =========================
// MAIN UPDATE
// =========================
void Update() {

    // Fully disabled = transparent
    if (!g_enabled) {
        lW = lA = lS = lD = false;
        EmitIfChanged();
        return;
    }

    ResolveSOCD();

    // -------- MOUSE OVERRIDE (UNCHANGED) --------
    if (g_mouse_override_enabled && pFire) {
        lW = lA = lS = lD = false;
    } else {
        lW = sW;
        lA = sA;
        lS = sS;
        lD = sD;
    }

    EmitIfChanged();
}
