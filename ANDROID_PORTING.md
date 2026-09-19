# TH08 Android porting — audit + starter patch

This documents what was found while scoping an Android port of th08-modern,
what this patch already does, and what is left. It follows the same
strategy the Linux port uses: keep the reconstructed game source untouched,
and swap the platform-facing shim layer underneath it.

## What already works in our favor

The Linux port (`src/modern/linux/`) does **not** implement a new engine —
it emulates just enough Win32 + Direct3D8 + DirectSound + DirectInput for
the original game source to compile and run, on top of SDL2 + desktop
OpenGL. That emulation strategy transfers to Android almost unchanged:

- **SDL2 has a mature, official Android backend** (NativeActivity + Java
  glue, `SDLActivity`). Window creation, the GL context, keyboard/touch
  events, and audio (via SDL2's own OpenSL ES/AAudio backend) are handled by
  SDL2 itself — this is *not* something we need to write by hand.
- **Audio needs almost no new code.** `linux_compat.cpp`'s `IDirectSound8`
  shim (`LinuxDirectSound`, `LinuxSoundBuffer`) is built on `SDL_OpenAudio`/
  `SDL_QueueAudio`. SDL2's Android backend implements the same SDL audio API
  on top of AAudio/OpenSL ES internally, so this file should work with no
  changes.
- **32-bit is fine.** `CMakeLists.txt` already requires
  `CMAKE_SIZEOF_VOID_P EQUAL 4` (matches the original i386 target's ABI
  assumptions). The NDK still fully supports `armeabi-v7a` (32-bit ARM), so
  this is not a blocker — just pick that ABI, don't try `arm64-v8a` yet.
- **The exact-reconstruction linker script does not apply.**
  `src/modern/linux/th08-layout.ld` pins specific globals to the *original
  i386 executable's* virtual addresses, purely so the project's byte-exact
  comparison tooling can validate the ELF against the original PE. That's
  meaningless on ARM and is simply not linked into the Android target.

## Real blockers found, and their status after this patch

| # | Issue | Where | Status |
|---|---|---|---|
| 1 | `glBegin/glEnd` immediate-mode drawing does not exist in OpenGL ES at all (never has, in any version). | 3 call sites in `d3d8_compat.cpp`: the two full-screen blit helpers (`FlushBackbuffer`, the FBO→window present) and `LinuxDirect3DDevice8::Draw()` — the main per-frame geometry path used by every sprite/bullet/background/UI draw. | **Fixed in this patch.** New `src/modern/gl_es_compat.hpp` replaces all three with client-side vertex arrays (`glVertexPointer`/`glColorPointer`/`glTexCoordPointer` + `glDrawArrays`), which are valid on both desktop GL 1.x (current Linux build) and GLES 1.1 Common profile. |
| 2 | `glPushAttrib(GL_ALL_ATTRIB_BITS)`/`glPopAttrib()` — not in GLES at all (no attribute stack in the ES spec, any version). | Same two blit helpers. | **Fixed.** Replaced with `GlStateGuard`, a small RAII helper that manually saves/restores exactly the handful of states those two call sites touch (verified against `PrepareState()`, which already unconditionally reprograms everything a real `Draw()` call needs — the attribute stack was only ever isolating the blits, not preserving state the rest of the renderer depended on). |
| 3 | **`fontconfig` does not exist on Android.** | `linux_compat.cpp` (search for `FcConfig`/`FcPattern`/`FcFontMatch`) and `CMakeLists.txt`'s `find_package(Fontconfig REQUIRED)`. | **Not yet fixed — next step.** Bundle a single `.ttf` in `assets/fonts/`, extract it to internal storage on first run (SDL_ttf's `TTF_OpenFont()` needs a real path, and Android's `AAsset` API doesn't hand one back), and hand that path to every fontconfig call site. Stub entry point is `th08_android_find_font()` in `android_compat.hpp`. |
| 4 | Game reads keyboard/joystick state via the DirectInput shim; there is no keyboard on a phone. | `linux_compat.cpp`'s `IDirectInputDevice8` shim. | **Not yet fixed — next step.** `android_compat.hpp` sketches the approach: a touch overlay (virtual D-pad + Shoot/Bomb/Focus/Pause) that writes into the *same* state buffer the DirectInput shim already exposes, so no game-logic code changes. Needs: hit-testing on `SDL_FINGERDOWN/MOTION/UP`, and an overlay draw call using the same `DrawTexturedQuad()` this patch added. |
| 5 | `execinfo.h` (`backtrace()`) used for crash reporting doesn't exist in Bionic. | `linux_runtime.cpp`. | **Stub added** (`android_runtime.cpp`) — logs to `adb logcat` via `__android_log_print`, no symbolized backtrace yet. |
| 6 | `add_executable(th08-modern ...)` is wrong for Android — `SDLActivity` (SDL2's Java glue) loads a JNI **shared library**, not an executable. | `CMakeLists.txt` top-level target declaration. | **Fixed** — Android branch now uses `add_library(th08-modern SHARED ...)` with `LIBRARY_OUTPUT_NAME "main"` (matches SDL2's default expectation of `libmain.so`). |
| 7 | `ANDROID` also satisfies CMake's `UNIX AND NOT APPLE` — an `elseif(ANDROID)` placed *after* that check is unreachable dead code. | `CMakeLists.txt` (both the target-declaration `if` chain and the compiler-flags `if` chain). | **Fixed** — Android branches now come first in both chains. This is an easy one to reintroduce if the CMake file is edited again later; watch for it. |
| 8 | SDL2/SDL2_image/SDL2_ttf aren't available via pkg-config when cross-compiling for Android. | `CMakeLists.txt`. | **Documented, not vendored.** The Android branch calls `find_package(SDL2/SDL2_image/SDL2_ttf REQUIRED)` and links the modern `SDL2::SDL2`-style targets; you need to actually bring those libraries into the build (FetchContent from their GitHub repos is the simplest — see the comment block in `CMakeLists.txt`; all three support being built as CMake subprojects for Android out of the box). |
| 9 | Storage: original code opens `th08.dat`/`thbgm.dat` by a user-chosen filesystem path. Modern Android (scoped storage) doesn't allow arbitrary path access from an app. | `linux_compat.cpp`'s file-access shims. | **Not addressed.** Needs a one-time Storage Access Framework picker (`ACTION_OPEN_DOCUMENT`) on the Java side to let the player select their legally-owned data files, then copy them into the app's private storage (`SDL_AndroidGetExternalStoragePath()`) where the existing file-reading code can open them by a normal path. |
| 10 | GLES **context version**: SDL2 must be told to request a GLES **1.x** context specifically. | New — not previously an issue since Linux always got desktop GL. | **Must set explicitly**, or the whole approach fails to link: `SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1); SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);` before `SDL_CreateWindow`. If this is left at SDL2's Android default, you get a GLES 2/3 context instead, which has **no** fixed-function pipeline at all — none of `glTexEnvi`, `glMatrixMode`, `glColor4ub`, etc. exist there, and this whole patch's approach (and the existing `d3d8_compat.cpp` texture-combiner emulation) would need a full shader rewrite instead. This is the single most important line to get right. |

## What this patch does NOT attempt

- Does **not** vendor SDL2/SDL2_image/SDL2_ttf sources — you need to pull
  those in locally (FetchContent snippet is in the CMakeLists.txt comment).
- Does **not** implement the touch overlay, font-asset extraction, or the
  Storage Access Framework picker — only the header/hooks they'll plug into
  (`android_compat.hpp`).
- Does **not** produce a working `.apk`. There's no Android NDK, SDK, or
  device/emulator available in the environment this patch was written in
  (network access is restricted to a small allow-list of domains that does
  not include `dl.google.com`), so the actual NDK build has to happen on
  your machine with Android Studio / command-line tools installed.
- Vertex fog (`D3DRS_FOGENABLE` + `D3DRS_FOGVERTEXMODE = D3DFOG_LINEAR`)
  stops being applied on **both** platforms after this patch — see the
  comment in `gl_es_compat.hpp`. It was already a silent no-op on any GLES
  target before this patch (the `GL_EXT_fog_coord` entry point simply isn't
  resolvable there), so this only changes behavior on the existing desktop
  Linux build. Worth checking the effects/fog usage ledger before merging.

## Verification done so far

- `gl_es_compat.hpp` was compiled standalone against real desktop GL headers
  (`-std=c++17 -Wall -Wextra`, zero warnings) to confirm the API usage
  (`glIsEnabled`, `glGetTexEnviv`, `glVertexPointer`, etc.) is well-formed —
  all of it is also valid GLES 1.1 Common-profile API.
- The three call-site patches in `d3d8_compat.cpp` were checked by hand
  against the original texture-coordinate winding (the two blit helpers use
  opposite Y-flips — `FlushBackbuffer` samples top-down pixel data,
  the FBO→window present samples a bottom-up render target — both are
  preserved exactly in the `DrawTexturedQuad()` call arguments).
- A full compile of the patched `d3d8_compat.cpp` against the project's
  actual i386 cross-toolchain was **not** completed — the sandbox this was
  written in hit unrelated Ubuntu multiarch/`i386` dependency conflicts
  installing `libsdl2-dev:i386` and friends. Run
  `scripts/setup-modern-linux.sh` locally and rebuild to confirm the Linux
  target still builds clean with this patch before relying on it; that's a
  much cheaper sanity check than a full NDK build and exercises the same
  changed code paths.

## Suggested build order

1. Apply this patch, rebuild the **existing Linux target** locally
   (`scripts/setup-modern-linux.sh`) and play through a stage. This
   validates items 1, 2, and 6-7's CMake logic changes cheaply, on hardware
   you already have working.
2. Bring in SDL2/SDL2_image/SDL2_ttf for Android (FetchContent) and get a
   `libmain.so` to **link** for `armeabi-v7a` — don't worry about it running
   correctly yet, just get through the linker.
3. Fix item 10 (force a GLES1.1 context) and get the window to open and
   clear to a color on a device/emulator.
4. Fix item 3 (fontconfig → bundled asset) — UI text should start rendering.
5. Fix item 4 (touch overlay → DirectInput state) — the game should become
   playable with a data file selected via a temporary hardcoded path for
   testing.
6. Fix item 9 (Storage Access Framework picker) last, once everything else
   is confirmed working end-to-end.
