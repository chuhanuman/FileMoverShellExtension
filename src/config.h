#ifndef CONFIG_H
#define CONFIG_H

namespace config {
	const auto PROGRAM_NAME = "FileMoverShellExtension";
	const auto SETTINGS_FILENAME = "settings.txt";
	const auto DEFAULT_SETTING = "\\..\\*";
	const auto GUID = "{2DF05694-A27D-4F6A-911F-887B6D19C7BB}";
	const auto GUID_W = L"{2DF05694-A27D-4F6A-911F-887B6D19C7BB}";
	const auto GUID_PREFIX_PATH = "CLSID\\";
	const auto DLL_PATH = "C:\\Users\\chuha\\source\\repos\\FileMoverShellExtension\\out\\build\\x64-release\\fileMover.dll";
	const auto THREADING_MODEL = "Both";
	const auto CONTEXT_MENU_HANDLER_PREFIX_PATH = "*\\shellex\\ContextMenuHandlers\\";
};

#endif // CONFIG_H