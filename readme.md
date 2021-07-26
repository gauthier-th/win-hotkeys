# Win Hotkeys

A program to register a system-wide hotkey in background which launch another program.

## Configuration

You have to create a file name `hotkeys.conf` in the same folder as the executable.

You can place as many as hotkeys as you want, with the three following parameters for each hotkey: 
 - 1st line: modifiers (see list below)
 - 2nd line: the virtual-key code of the hotkey. See [Virtual Key Codes](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes).
 - 3rd line: the path of the program to launch.

## Examples

Launch a snipping tool when `ALT+PRINT SCREEN` is pressed:
```
MOD_ALT+MOD_NOREPEAT
44
C:\your\path\to\snipping-tool.exe
```

Launch program A when `CTRL+A` is pressed and program B when `CTRL+B` is pressed:
```
MOD_CONTROL+MOD_NOREPEAT
65
C:\your\path\to\program-a.exe

MOD_CONTROL+MOD_NOREPEAT
66
C:\your\path\to\program-b.exe
```

## Modifiers

|   Modifier   |                                                                                    Description                                                                                    |
|:------------:|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------:|
| MOD_ALT      |  Either ALT key must be held down.                                                                                                                                                |
| MOD_CONTROL  |  Either CTRL key must be held down.                                                                                                                                               |
| MOD_NOREPEAT |  Changes the hotkey behavior so that the keyboard auto-repeat does not yield multiple hotkey notifications.  Windows Vista:  This flag is not supported.                          |
| MOD_SHIFT    |  Either SHIFT key must be held down.                                                                                                                                              |
| MOD_WIN      |  Either WINDOWS key was held down. These keys are labeled with the  Windows logo. Keyboard shortcuts that involve the WINDOWS key are  reserved for use by the operating system.  |

## License

MIT License

Copyright (c) 2021 gauthier-th (mail@gauthier-th.fr)