cmake_minimum_required(VERSION 3.20)

project(th08-modern LANGUAGES CXX)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "TH08 currently requires a 32-bit toolchain.")
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter)

set(TH08_DX8_SDK_ROOT "${CMAKE_SOURCE_DIR}/scripts/prefix/mssdk" CACHE PATH
    "DirectX 8 SDK root containing include/ and lib/")
if(WIN32 AND NOT EXISTS "${TH08_DX8_SDK_ROOT}/include/d3dx8.h")
    message(FATAL_ERROR
        "DirectX 8 SDK headers were not found. Set TH08_DX8_SDK_ROOT or run the existing prefix setup first.")
endif()

set(TH08_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
add_custom_command(
    OUTPUT "${TH08_GENERATED_DIR}/i18n.hpp"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${TH08_GENERATED_DIR}"
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/scripts/generate_i18n.py"
            -o "${TH08_GENERATED_DIR}/i18n.hpp" --shift-jis
            "${CMAKE_SOURCE_DIR}/config/i18n.csv"
    DEPENDS
        "${CMAKE_SOURCE_DIR}/config/i18n.csv"
        "${CMAKE_SOURCE_DIR}/scripts/generate_i18n.py"
    VERBATIM
)
add_custom_target(th08-modern-generated DEPENDS "${TH08_GENERATED_DIR}/i18n.hpp")

set(TH08_GAME_SOURCES
    src/AnmManager.cpp
    src/AsciiManager.cpp
    src/AsciiManagerBossMarker.cpp
    src/AsciiManagerGauge.cpp
    src/AsciiManagerGuiMode.cpp
    src/AsciiManagerScale.cpp
    src/Background.cpp
    src/BulletManager.cpp
    src/EclDependencies.cpp
    src/EclExIns.cpp
    src/EclGlobals.cpp
    src/EclHelpers.cpp
    src/EclManager.cpp
    src/EclOperandsFloat.cpp
    src/EclOperandsInt.cpp
    src/EclRun.cpp
    src/EffectManager.cpp
    src/Ending.cpp
    src/EnemyManager.cpp
    src/EnemyManagerUpdate.cpp
    src/EnemyTimeline.cpp
    src/GameManager.cpp
    src/Global.cpp
    src/Gui.cpp
    src/ItemManager.cpp
    src/Midi.cpp
    src/MusicRoom.cpp
    src/Player.cpp
    src/PlayerBomb.cpp
    src/ReplayManager.cpp
    src/ResultScreen.cpp
    src/ScoreDat.cpp
    src/ScreenEffect.cpp
    src/SoundPlayer.cpp
    src/Spellcard.cpp
    src/Supervisor.cpp
    src/TextHelper.cpp
    src/TitleScreen.cpp
    src/main.cpp
    src/utils.cpp
    src/zwave.cpp
    src/pbg/Lzss.cpp
    src/pbg/PbgArchive.cpp
    src/pbg/PbgFile.cpp
)

if(WIN32)
    list(APPEND TH08_GAME_SOURCES src/modern/windows_runtime.cpp)
    add_executable(th08-modern WIN32 ${TH08_GAME_SOURCES})
elseif(ANDROID)
    # NOTE: ANDROID also satisfies "UNIX AND NOT APPLE" below, so this branch
    # MUST come first or CMake will silently build it as the Linux target
    # instead (wrong: pkg-config isn't available for cross builds, and a
    # plain executable isn't what SDLActivity loads).
    list(APPEND TH08_GAME_SOURCES
        src/modern/linux/d3d8_compat.cpp
        src/modern/linux/d3dx8_compat.cpp
        src/modern/linux/linux_compat.cpp
        # linux_runtime.cpp is intentionally NOT reused: it installs a
        # SIGSEGV backtrace handler via execinfo.h/backtrace(), which
        # bionic (Android's libc) does not provide. Android needs its own
        # small runtime.cpp (crash reporting via android/log.h instead).
        src/modern/android/android_runtime.cpp
    )
    # SDLActivity (SDL2's Java glue) loads this as a JNI shared library and
    # calls its own native entry point, which in turn calls SDL_main.
    add_library(th08-modern SHARED ${TH08_GAME_SOURCES})
elseif(UNIX AND NOT APPLE)
    list(APPEND TH08_GAME_SOURCES
        src/modern/linux/d3d8_compat.cpp
        src/modern/linux/d3dx8_compat.cpp
        src/modern/linux/linux_compat.cpp
        src/modern/linux/linux_runtime.cpp
    )
    add_executable(th08-modern ${TH08_GAME_SOURCES})
else()
    message(FATAL_ERROR "The modern target currently supports Windows, Linux, and Android.")
endif()
add_dependencies(th08-modern th08-modern-generated)
target_compile_features(th08-modern PRIVATE cxx_std_98)
target_compile_definitions(th08-modern PRIVATE
    TH08_MODERN_PORT
    WIN32_LEAN_AND_MEAN
)
target_include_directories(th08-modern PRIVATE
    "${CMAKE_SOURCE_DIR}/src"
    "${TH08_GENERATED_DIR}"
)

if(MINGW)
    # MinGW ships compatible D3D8/DirectInput/DirectSound headers and import
    # libraries, but not D3DX8. Search the legacy SDK only after the toolchain
    # headers so its old Windows SDK does not shadow modern system headers.
    set(TH08_DX8_SANITIZED_INCLUDE "${CMAKE_BINARY_DIR}/dx8-include")
    file(COPY "${TH08_DX8_SDK_ROOT}/include/" DESTINATION "${TH08_DX8_SANITIZED_INCLUDE}")
    file(GLOB TH08_DX8_HEADERS "${TH08_DX8_SANITIZED_INCLUDE}/*")
    string(ASCII 26 TH08_DOS_EOF)
    foreach(TH08_DX8_HEADER IN LISTS TH08_DX8_HEADERS)
        if(NOT IS_DIRECTORY "${TH08_DX8_HEADER}")
            file(READ "${TH08_DX8_HEADER}" TH08_DX8_HEADER_CONTENT)
            string(REPLACE "${TH08_DOS_EOF}" "" TH08_DX8_HEADER_CONTENT "${TH08_DX8_HEADER_CONTENT}")
            file(WRITE "${TH08_DX8_HEADER}" "${TH08_DX8_HEADER_CONTENT}")
        endif()
    endforeach()

    target_compile_options(th08-modern PRIVATE
        # The reconstructed source preserves the original VC7 /Od boundaries;
        # cross-TU inline bodies are not available for modern optimization.
        -O0
        -fpermissive
        -mms-bitfields
        -Wno-write-strings
        "-include${CMAKE_SOURCE_DIR}/src/modern/windows_compat.hpp"
        "-idirafter${TH08_DX8_SANITIZED_INCLUDE}"
    )
    target_link_options(th08-modern PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
        # Some reconstructed data references still use target absolute VAs.
        # Keep the original image base until every such owner is symbolic.
        -Wl,--disable-dynamicbase
    )
    target_link_libraries(th08-modern PRIVATE
        # The retail D3DX8 library is a static MSVC C++ archive and cannot be
        # mixed with MinGW's C++ ABI. The SDK's debug DLL exposes the same C
        # API through an import library, which is suitable for bring-up builds.
        "${TH08_DX8_SDK_ROOT}/lib/d3dx8d.lib"
        dxguid dinput8 dsound d3d8 winmm
        kernel32 user32 gdi32 advapi32 shell32 ole32 oleaut32 uuid dbghelp
    )
    set(TH08_D3DX8_DEBUG_DLL
        "${TH08_DX8_SDK_ROOT}/samples/multimedia/support/d3dx8d.dll")
    if(EXISTS "${TH08_D3DX8_DEBUG_DLL}")
        add_custom_command(TARGET th08-modern POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${TH08_D3DX8_DEBUG_DLL}" "$<TARGET_FILE_DIR:th08-modern>"
            VERBATIM
        )
    else()
        message(WARNING "d3dx8d.dll was not found; th08-modern.exe will build but needs that SDK DLL to start.")
    endif()
elseif(MSVC)
    target_include_directories(th08-modern SYSTEM PRIVATE "${TH08_DX8_SDK_ROOT}/include")
    target_link_directories(th08-modern PRIVATE "${TH08_DX8_SDK_ROOT}/lib")
    target_compile_options(th08-modern PRIVATE /W3)
    target_link_options(th08-modern PRIVATE /DYNAMICBASE:NO)
    target_link_libraries(th08-modern PRIVATE
        dxguid d3dx8 dinput8 dsound d3d8 winmm
        kernel32 user32 gdi32 advapi32 shell32 ole32 oleaut32 uuid dbghelp
    )
elseif(ANDROID)
    # Same ordering caveat as above: ANDROID also satisfies "UNIX AND NOT
    # APPLE", so this must be checked first here too.
    #
    # Built via the NDK's CMake toolchain, e.g.:
    #   cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
    #         -DANDROID_ABI=armeabi-v7a -DANDROID_PLATFORM=android-21 ..
    # armeabi-v7a (32-bit) is required today: see the CMAKE_SIZEOF_VOID_P
    # check at the top of this file. The reconstructed source assumes ILP32
    # (matches the original i386 target); a 64-bit ABI is out of scope until
    # that assumption is revisited project-wide, same as the x86_64 Linux
    # work on port/portable-64bit.
    #
    # SDL2/SDL2_image/SDL2_ttf are not available via pkg-config on Android.
    # Vendor them as CMake subprojects (FetchContent or a git submodule) and
    # link the targets they export instead of PkgConfig::*, e.g.:
    #   FetchContent_Declare(SDL2 GIT_REPOSITORY https://github.com/libsdl-org/SDL.git GIT_TAG release-2.30.x)
    #   FetchContent_Declare(SDL2_image GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git GIT_TAG release-2.8.x)
    #   FetchContent_Declare(SDL2_ttf GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git GIT_TAG release-2.22.x)
    #   FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_ttf)
    # then replace PkgConfig::SDL2 / PkgConfig::SDL2_IMAGE / PkgConfig::SDL2_TTF
    # below with SDL2::SDL2 / SDL2_image::SDL2_image / SDL2_ttf::SDL2_ttf.
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(OpenGL REQUIRED)  # resolves to libGLESv1_CM via the NDK toolchain file

    target_compile_definitions(th08-modern PRIVATE
        TH08_MODERN_ANDROID
        TH08_DESKTOP_GL=0
    )
    target_compile_options(th08-modern PRIVATE
        -O0
        -fpermissive
        -fdata-sections
        -fno-omit-frame-pointer
        -Wno-write-strings
        "-include${CMAKE_SOURCE_DIR}/src/modern/android/android_compat.hpp"
    )
    target_include_directories(th08-modern BEFORE PRIVATE
        # Reuses the Linux port's Win32/D3D8/DSound/DInput header shims
        # as-is; android_compat.hpp only overrides font lookup and input.
        "${CMAKE_SOURCE_DIR}/src/modern/linux/include"
    )
    target_link_libraries(th08-modern PRIVATE
        SDL2::SDL2 SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        GLESv1_CM
        android
        log
    )
    # NDK CMake builds produce a shared library, loaded by SDL2's Java
    # NativeActivity glue (see android-project-skeleton/ alongside this file).
    set_target_properties(th08-modern PROPERTIES
        LIBRARY_OUTPUT_NAME "main"
    )
elseif(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
    find_package(Fontconfig REQUIRED)
    # Prefer the broadly deployed libGL.so.1 runtime ABI over GLVND's separate
    # libOpenGL.so.0 dispatch library for the 32-bit portable build.
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    find_package(Threads REQUIRED)

    target_compile_definitions(th08-modern PRIVATE
        TH08_MODERN_LINUX
        _FILE_OFFSET_BITS=64
        _TIME_BITS=64
    )
    target_compile_options(th08-modern PRIVATE
        # Keep the original /Od-style TU boundaries in every CMake build type.
        -O0
        -fpermissive
        -fdata-sections
        -fno-omit-frame-pointer
        -fno-pie
        -Wno-write-strings
        "-include${CMAKE_SOURCE_DIR}/src/modern/linux/linux_compat.hpp"
    )
    target_link_options(th08-modern PRIVATE
        -no-pie
        -rdynamic
        "-Wl,-T,${CMAKE_SOURCE_DIR}/src/modern/linux/th08-layout.ld"
    )
    set_property(TARGET th08-modern APPEND PROPERTY LINK_DEPENDS
        "${CMAKE_SOURCE_DIR}/src/modern/linux/th08-layout.ld"
    )
    target_include_directories(th08-modern BEFORE PRIVATE
        "${CMAKE_SOURCE_DIR}/src/modern/linux/include"
    )
    target_link_libraries(th08-modern PRIVATE
        PkgConfig::SDL2
        PkgConfig::SDL2_IMAGE
        PkgConfig::SDL2_TTF
        Fontconfig::Fontconfig
        OpenGL::GL
        Threads::Threads
        dl
    )
    add_custom_command(TARGET th08-modern POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${CMAKE_SOURCE_DIR}/resources/modern-icon.png"
                "$<TARGET_FILE_DIR:th08-modern>/th08-modern.png"
        VERBATIM
    )
else()
    message(FATAL_ERROR "The modern Windows target currently supports MSVC, MinGW, Linux, and Android.")
endif()

set_target_properties(th08-modern PROPERTIES OUTPUT_NAME "th08-modern")
