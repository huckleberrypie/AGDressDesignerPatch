#include "Utils/MemoryMgr.h"
#include "MinHook.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <io.h>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "libMinHook.x86.lib")

// --- TYPEDEFS ---
typedef HANDLE(WINAPI* CREATEFILEA)(LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef int(__cdecl* ACCESS)(const char*, int);
typedef DWORD(WINAPI* GETFILEATTRIBUTESA)(LPCSTR);
typedef BOOL(WINAPI* GETFILEATTRIBUTESEXA)(LPCSTR, GET_FILEEX_INFO_LEVELS, LPVOID);
typedef BOOL(WINAPI* PATHFILEEXISTSA)(LPCSTR);
typedef HANDLE(WINAPI* FINDFIRSTFILEA)(LPCSTR, LPWIN32_FIND_DATAA);

CREATEFILEA fpCreateFileA = NULL;
ACCESS fp_access = NULL;
GETFILEATTRIBUTESA fpGetFileAttributesA = NULL;
GETFILEATTRIBUTESEXA fpGetFileAttributesExA = NULL;
PATHFILEEXISTSA fpPathFileExistsA = NULL;
FINDFIRSTFILEA fpFindFirstFileA = NULL;

// --- GLOBALS ---
std::string g_BaseFolder = "";
thread_local bool bInsideHook = false;

// --- HELPERS ---

bool GetRedirectedPath(LPCSTR lpFileName, std::string& outPath) {
    if (!lpFileName) return false;
    if (strstr(lpFileName, "C:\\") && strstr(lpFileName, "Data")) {
        const char* dataPtr = strstr(lpFileName, "Data");
        if (dataPtr) {
            const char* relative = strchr(dataPtr, '\\');
            if (relative && !g_BaseFolder.empty()) {
                outPath = g_BaseFolder + (relative + 1);
                return true;
            }
        }
    }
    return false;
}

// Minimalist creation check for ad-hoc subfolders (like fabrics)
void EnsureDirectoryTree(const std::string& fullPath) {
    size_t lastSlash = fullPath.find_last_of("\\");
    if (lastSlash != std::string::npos) {
        std::string folderOnly = fullPath.substr(0, lastSlash);
        if (!PathFileExistsA(folderOnly.c_str())) {
            SHCreateDirectoryExA(NULL, folderOnly.c_str(), NULL);
            Sleep(10);
        }
    }
}

// --- DETOURS ---

HANDLE WINAPI DetourCreateFileA(LPCSTR lpFileName, DWORD dwAccess, DWORD dwShare, LPSECURITY_ATTRIBUTES lpSec, DWORD dwCreation, DWORD dwFlags, HANDLE hTemplate) {
    std::string redirected;
    if (!bInsideHook && GetRedirectedPath(lpFileName, redirected)) {
        bInsideHook = true;
        EnsureDirectoryTree(redirected);
        HANDLE hResult = fpCreateFileA(redirected.c_str(), dwAccess, dwShare, lpSec, dwCreation, dwFlags, hTemplate);
        bInsideHook = false;
        return hResult;
    }
    return fpCreateFileA(lpFileName, dwAccess, dwShare, lpSec, dwCreation, dwFlags, hTemplate);
}

int __cdecl Detour_access(const char* path, int mode) {
    std::string redirected;
    if (!bInsideHook && GetRedirectedPath(path, redirected)) return fp_access(redirected.c_str(), mode);
    return fp_access(path, mode);
}

DWORD WINAPI DetourGetFileAttributesA(LPCSTR lpFileName) {
    std::string redirected;
    if (!bInsideHook && GetRedirectedPath(lpFileName, redirected)) return fpGetFileAttributesA(redirected.c_str());
    return fpGetFileAttributesA(lpFileName);
}

BOOL WINAPI DetourGetFileAttributesExA(LPCSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId, LPVOID lpFileInformation) {
    std::string redirected;
    if (!bInsideHook && GetRedirectedPath(lpFileName, redirected)) return fpGetFileAttributesExA(redirected.c_str(), fInfoLevelId, lpFileInformation);
    return fpGetFileAttributesExA(lpFileName, fInfoLevelId, lpFileInformation);
}

BOOL WINAPI DetourPathFileExistsA(LPCSTR pszPath) {
    std::string redirected;
    if (!bInsideHook && GetRedirectedPath(pszPath, redirected)) return fpPathFileExistsA(redirected.c_str());
    return fpPathFileExistsA(pszPath);
}

HANDLE WINAPI DetourFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
    std::string redirected;
    if (!bInsideHook && GetRedirectedPath(lpFileName, redirected)) {
        bInsideHook = true;
        HANDLE hResult = fpFindFirstFileA(redirected.c_str(), lpFindFileData);
        bInsideHook = false;
        return hResult;
    }
    return fpFindFirstFileA(lpFileName, lpFindFileData);
}

// --- MAIN ---

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        char docs[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, docs))) {
            g_BaseFolder = std::string(docs) + "\\The American Girls Dress Designer\\Data\\";

            // 1. Create the base "Data" folder first
            SHCreateDirectoryExA(NULL, g_BaseFolder.c_str(), NULL);

            // 2. Pre-create only the core character folders.
            // This is the ONLY way to prevent the Abnormal Termination crash 
            // because it ensures the engine never hits a "folder creation" lag spike.
            const char* characters[] = { "Samantha", "Molly", "Kirsten", "Felicity", "Addy", "Josefina" };
            for (const char* name : characters) {
                std::string full = g_BaseFolder + name;
                CreateDirectoryA(full.c_str(), NULL); // Silently fails if exists
            }
        }
        // Patch that address in the exe in-memory as a fix for this hMenu issue:
        // https://tcrf.net/Notes:The_American_Girls_Dress_Designer
        using namespace Memory::VP;
        Patch<uint8_t>(0x48BAAB, 0xD0);

        if (MH_Initialize() == MH_OK) {
            MH_CreateHookApi(L"kernel32", "CreateFileA", &DetourCreateFileA, (LPVOID*)&fpCreateFileA);
            MH_CreateHookApi(L"kernel32", "GetFileAttributesA", &DetourGetFileAttributesA, (LPVOID*)&fpGetFileAttributesA);
            MH_CreateHookApi(L"kernel32", "GetFileAttributesExA", &DetourGetFileAttributesExA, (LPVOID*)&fpGetFileAttributesExA);
            MH_CreateHookApi(L"kernel32", "FindFirstFileA", &DetourFindFirstFileA, (LPVOID*)&fpFindFirstFileA);
            MH_CreateHookApi(L"shlwapi", "PathFileExistsA", &DetourPathFileExistsA, (LPVOID*)&fpPathFileExistsA);
            MH_CreateHookApi(L"msvcrt", "_access", &Detour_access, (LPVOID*)&fp_access);
            MH_EnableHook(MH_ALL_HOOKS);
        }
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        MH_Uninitialize();
    }
    return TRUE;
}