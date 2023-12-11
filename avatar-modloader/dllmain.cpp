// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

// Redirect exports from dbgcore.
#pragma comment(linker, "/export:MiniDumpReadDumpStream=dbgcore_old.MiniDumpReadDumpStream")
#pragma comment(linker, "/export:MiniDumpWriteDump=dbgcore_old.MiniDumpWriteDump")

typedef bool (__fastcall *open_file_stream_proc)(__int64 stream, LPCSTR file_path, unsigned int flags);

uintptr_t old_getsystemtimeasfiletime = 0;
open_file_stream_proc old_open_file_stream = nullptr;

const LPCSTR patterns[] = { "48 89 5C 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 81 EC ? ? ? ? 41 8B F8" };

/// <summary>
/// Checks if a file exists, also supports relative file paths.
/// </summary>
/// <param name="file_path">The relative or absolute path to check.</param>
/// <returns>True if the file exists.</returns>
bool file_exists(LPCSTR file_path) {
    DWORD dwAttrib = GetFileAttributesA(file_path);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

/// <summary>
/// Called whenever the game wants to load an asset.
/// </summary>
/// <param name="stream">The internal game stream struct.</param>
/// <param name="file_path">The relative or absolute path of the file to load.</param>
/// <param name="flags">Flags that tell whether or not to load the asset all at once, stream it, or override sdfdata files on disk.</param>
/// <returns>Whether or not load was successful.</returns>
bool __fastcall hk_open_file_stream(uintptr_t stream, LPCSTR file_path, unsigned int flags) {
    if (file_exists(file_path)) {
#if _DEBUG
        printf("Overriding asset on disk: %s\n", file_path);
#endif

        return (*old_open_file_stream)(stream, file_path, flags | (1 << 0xA));
    }
    else {
        return old_open_file_stream(stream, file_path, flags);
    }
}

/// <summary>
/// Called right after unpacking of the executable.
/// </summary>
/// <param name="lpSystemTimeAsFileTime"></param>
/// <returns></returns>
DWORD WINAPI hk_getsystemtimeasfiletime(LPCWSTR lpSystemTimeAsFileTime) {
    Detours::X64::DetourRemove(old_getsystemtimeasfiletime);

    MainModule Module;

    uintptr_t Result = (uintptr_t)-1;

    for (auto i = 0; i < ARRAYSIZE(patterns); i++) {
        Result = FindPattern(patterns[i], Module.GetBaseAddress(), Module.GetCodeSize());

        if (Result != (uintptr_t)-1) {
            break;
        }
    }

    if (Result == (uintptr_t)-1)
    {
        MessageBoxA(GetActiveWindow(), "Failed to find required modloader functions! Mod support will not be available.", "Avatar ModLoader", MB_OK | MB_ICONERROR);
    }
    else
    {
        old_open_file_stream = (open_file_stream_proc)Detours::X64::DetourFunction(Result + Module.GetBaseAddress(), (uintptr_t)hk_open_file_stream);
    }    

    return GetFileAttributesW(lpSystemTimeAsFileTime);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
#if _DEBUG
        AllocConsole();

        FILE* file = nullptr;
        freopen_s(&file, "CONOUT$", "w", stdout);

        MessageBoxA(NULL, "Attach", "Dbg", MB_OK);

        printf("Avatar ModLoader v1.0\n");

        Detours::SetGlobalOptions(Detours::OPT_BREAK_ON_FAIL);
#endif

        old_getsystemtimeasfiletime = Detours::X64::DetourFunction((uintptr_t)GetFileAttributesW, (uintptr_t)hk_getsystemtimeasfiletime);
    }

    return TRUE;
}

