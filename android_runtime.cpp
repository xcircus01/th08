// android_runtime.cpp — Android counterpart to src/modern/linux/linux_runtime.cpp.
//
// Not reused as-is because linux_runtime.cpp installs a SIGSEGV handler that
// calls backtrace()/backtrace_symbols() from <execinfo.h>. Bionic (Android's
// libc) does not ship execinfo.h. Use android/log.h + a minimal signal
// handler instead, or link a small unwinder (e.g. libunwindstack, already
// present on-device) if symbolized backtraces are wanted later.
//
// STATUS: skeleton — currently only wires up logging so crashes are at
// least visible in `adb logcat`, matching linux_runtime.cpp's baseline
// behavior before its backtrace printing was added.

#include "modern/windows_runtime.hpp"

#include <android/log.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

#define TH08_LOG_TAG "th08-modern"

namespace
{
    void CrashSignalHandler(int signal)
    {
        __android_log_print(ANDROID_LOG_ERROR, TH08_LOG_TAG,
                             "Fatal signal %d (%s) — see linux_runtime.cpp's handler "
                             "for the backtrace-printing behavior to port here.",
                             signal, strsignal(signal));
        _exit(1);
    }
}

// TODO: call this from wherever linux_runtime.cpp's equivalent
// installation point is invoked from th08-modern's startup path.
void th08_android_install_crash_handler()
{
    signal(SIGSEGV, CrashSignalHandler);
    signal(SIGABRT, CrashSignalHandler);
    signal(SIGBUS, CrashSignalHandler);
}
