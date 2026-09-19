#pragma once
//
// android_compat.hpp — skeleton platform layer for TH08 on Android.
//
// This mirrors src/modern/linux/linux_compat.hpp's role: it is force-included
// ahead of every game translation unit (see the ANDROID branch added to
// CMakeLists.txt) and stands in for the Win32/DirectX headers the original
// source expects.
//
// STATUS: skeleton only. Reuses almost everything from the Linux compat
// layer (same D3D8/DirectSound/DirectInput shim strategy, same SDL2
// foundation) and only overrides the pieces that differ on Android:
//   1. Font loading — no fontconfig on Android; load a bundled .ttf asset.
//   2. Touch input — feed a virtual D-pad + buttons into the same
//      DirectInput state the game already reads, instead of a real
//      keyboard/joystick.
//   3. Storage paths — settings/scores/replays go under the app's private
//      external files dir (SDL_AndroidGetExternalStoragePath()), not a
//      user-chosen directory, and the original TH08 data files (th08.dat,
//      thbgm.dat) must be picked once via Android's Storage Access
//      Framework and copied/cached into that same directory (no direct
//      arbitrary filesystem path access on modern Android).
//
// TODO before this compiles as part of th08-modern:
//   - Pull in src/modern/linux/linux_compat.hpp's declarations for the
//     pieces that are unchanged (SDL2 window creation, D3D8/DSound/DInput
//     shims from d3d8_compat.cpp / linux_compat.cpp are reused as-is).
//   - Replace every fontconfig call site (search linux_compat.cpp for
//     "FcConfig"/"FcPattern") with th08_android_find_font() below.
//   - Wire th08_android_poll_virtual_input() into the same code path that
//     linux_compat.cpp's DirectInput shim uses to read key state each frame.
//
#include <jni.h>
#include <string>

// Returns a filesystem path to a bundled TTF the game can hand to SDL_ttf,
// replacing every fontconfig lookup (FcConfig/FcPattern/FcFontMatch calls)
// in linux_compat.cpp. Backing file lives at assets/fonts/<name>.ttf and is
// extracted once to SDL_AndroidGetInternalStoragePath() on first run
// (SDL's AAsset APIs are read-only and don't hand back a plain path, which
// SDL_ttf's TTF_OpenFont() needs).
const char *th08_android_find_font(const char *logicalName);

// One on-screen virtual button/stick. `code` matches the DirectInput
// scancode/button constant the game already polls (see linux_compat.cpp's
// DirectInput keyboard-state array), so the touch overlay reuses the exact
// same input plumbing as keyboard/joystick input — no changes needed in
// game logic (Player.cpp, GameManager.cpp, etc.).
struct TouchControl
{
    float x, y, radius;  // normalized [0,1] screen-space hit region
    int directInputCode;
    bool isDPadDirection;  // true for the 4 movement quadrants of the pad
};

// Called once at startup with the negotiated screen size to lay out the
// virtual D-pad (left side) and Shoot/Bomb/Focus/Pause buttons (right side).
void th08_android_init_touch_overlay(int screenWidth, int screenHeight);

// Called every SDL_FINGERDOWN/FINGERMOTION/FINGERUP event. Internally
// updates the same byte array linux_compat.cpp's IDirectInputDevice8
// shim returns from GetDeviceState(), so from the game's point of view
// touch input and keyboard input are indistinguishable.
void th08_android_handle_touch_event(const union SDL_Event *event);

// Optionally draws the semi-transparent overlay graphic for the virtual
// controls. Call once per frame after the game's own Present().
void th08_android_draw_touch_overlay();
