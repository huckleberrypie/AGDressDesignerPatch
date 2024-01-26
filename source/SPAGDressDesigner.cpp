#include "Utils/MemoryMgr.h"
#include "Utils/Patterns.h"

// TODO: Add support for saving paper dolls/patterns/whatever to the user's documents folder

void OnInitializeHook()
{
    using namespace Memory::VP;
    using namespace hook;

    auto comparison_pattern = pattern("D4 FF 75 D8 FF 75 DC").get_one(); // Look for this pattern
	Patch<uint8_t>( 0x8BAAB, 0xD0 ); // change D8 to D0; this changes the hMenu value in CreateWindowExA to NULL
    // Patch(comparison_pattern.get<void>(), { 0xD4, 0xFF, 0x75, 0xD0, 0x75, });
	// or we could alternatively use this

}