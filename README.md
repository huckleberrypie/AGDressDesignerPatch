# Patch for American Girls Dress Designer

Patch for [American Girls Dress Designer](https://americangirl.fandom.com/wiki/The_American_Girls_Dress_Designer),
fixing a long-standing issue with using the options menu or quitting due to a bug within the game's code which
slipped by unnoticed on Windows 9x but crashes on any Windows NT release.

## Featured fixes

### Bug fixes

* Fixed issues with CreateWindowExA API call where invoking the Quit or Options menu crashes due to a wrong value used.

## Planned fixes

* Add the ability to load and save user-created dresses from the user's Documents folder.
