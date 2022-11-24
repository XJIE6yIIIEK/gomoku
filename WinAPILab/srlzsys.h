#pragma once
#include <map>
#include <string>

namespace serialize {

	enum FileWriters {
		MapView,
		StandartIO,
		WinAPIFunc,
		FileVars,
	};

	enum Commandlets {
		File,
		Size
	};

	enum Option {
		Width,
		Height,
		X,
		Y,
		BackgroundHue,
		GridHue,
		Cells,
		Circle,
		Cross
	};

	struct FileMap {
		HANDLE hFile;
		HANDLE hMap;
		size_t fSize;
		unsigned char* dataPtr;
	};

	struct RegResult {
		std::string option;
		std::string value;
	};

	extern std::map<std::string, Commandlets> comamandletsMap;

	extern std::map<std::string, Option> optionMap;

	void SetCommandLineArguments();
	void LoadSettings();
	void MapViewFileWriter();
	void MapViewLoader();
	void StandartIOFileWriter();
	void StandartIOLoader();
	void WinAPIFuncFileWriter();
	void WinAPIFuncLoader();
	void FileVarsFileWriter();
	void FileVarsLoader();
	extern void(*SetConfigFile)();
	extern void(*LoadConfigFile)();
}