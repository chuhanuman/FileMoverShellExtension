# FileMoverShellExtension

File Explorer shell extension which allows for easy moving of files via the right click menu.

To install, use CMake to compile the files then run setup.exe. It will create a settings file at %AppData%\FileMoverShellExtension\settings.txt which by default should contain "\..\*". This setting is the search path for folders to move to. For example the default setting "\..\*" will find all folders one level up and if a right click is used on special.txt in the file directory below, will give options for moving to b, c, or d. Meanwhile the setting "\..\b\1*" will find folders that start with a 1 in the sibling folder b (if it exists) and if a right click is used on special.txt in the file directory below, will give options for moving to 1a or 1b. Also by default, the right click option will only appear on files although folders can be moved if they are selected along with the file being right clicked on. If you want to enable the right click option for folders as well, you can create a registry key at "HKEY_CLASSES_ROOT\Folder\shellex\ContextMenuHandlers" with the name "FileMoverShellExtension" (or any name really) and set the default value of the key to "{2DF05694-A27D-4F6A-911F-887B6D19C7BB}".

a
- b
  - 1a
  - 1b
  - 2a
  - 2b
  - a.txt
  - b.txt
- c
  - a.txt
  - b.txt
- d
  - special.txt
