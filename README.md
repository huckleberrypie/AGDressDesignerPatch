# Patch for American Girls Dress Designer

Patch for [American Girls Dress Designer](https://americangirl.fandom.com/wiki/The_American_Girls_Dress_Designer),
fixing a long-standing issue with using the options menu or quitting due to a bug within the game's code which
slipped by unnoticed on Windows 9x but crashes on any Windows NT release.

## Featured fixes

### Bug fixes

* Fixed issues with CreateWindowExA API call where invoking the Quit or Options menu crashes due to a wrong value used.

## Quality-of-life fixes

* Add the ability to load and save user-created dresses from the user's Documents folder.

## Usage (for gamers and parents)

If you just want to fix your game, download the latest release.

### Setup

1.  Download the latest `.zip` from the **[Releases](../../releases)** page.

2.  Extract the contents of the folder to where you installed the game (e.g. ** C:\Program Files (x86)\American Girl\The American Girls Dress Designer **)

## Building from source (for developers)

### Prerequisites

* Visual Studio 2026
* Required libraries: `minhook`

### Installation
1.  Clone the repository:
    ```bash
    git clone https://github.com/huckleberrypie/AGDressDesignerPatch.git
    cd AGDressDesignerPatch
    ```
2.   Download the latest **lib** version of **[Minhook](https://github.com/TsudaKageyu/minhook/releases/)**, place the `include` and `lib` folders inside the project source folder and add those libraries in your configuration. Alternatively you can use vcpkg to incorporate them.

3.   Build the solution using C++ 17 then copy the resulting .asi library to your game folder and start the game as usual. Use ThirteenAG's ASI Loader to run the .ASI file. Let me know if you have issues building this and I will try to assist in any way I can.

## Disclaimer
The following software is built with partial AI assistance using Google Gemini. I am personally not in favour of how corpos pervasively shove AI up everyone's throats (especially given the implications such as hardware shortages, climate concerns, resource usage as well as potential labour displacement of artists and other creatives) but I am not completely against the technology if it's used judiciously.

