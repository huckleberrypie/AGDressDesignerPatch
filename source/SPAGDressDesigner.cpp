#include "Utils/MemoryMgr.h"

// TODO: Add support for saving paper dolls/patterns/whatever to the user's documents folder

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        using namespace Memory::VP;

        Patch<uint8_t>(0x48BAAB, 0xD0); // change D8 to D0; this changes the hMenu value in CreateWindowExA to NULL
    }
    return TRUE;
}