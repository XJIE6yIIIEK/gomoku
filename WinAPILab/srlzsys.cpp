#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <tchar.h>
#include <string>
#include <map>
#include <regex>
#include <iterator>
#include <iostream>
#include <fstream>
#include "tictactoe.h"
#include "srlzsys.h"
#include "guilib.h"
#include "main_logic.h"

namespace serialize {
	std::map<std::string, Commandlets> comamandletsMap = {
		{"-m", File},
		{"-s", File},
		{"-w", File},
		{"-fv", File},
		{"-size", Size}
	};

	std::map<std::string, FileWriters> fileWritersMap = {
		{"-m", MapView},
		{"-s", StandartIO},
		{"-w", WinAPIFunc},
		{"-fv", FileVars}
	};

	std::map<std::string, Option> optionMap = {
		{"window.size.width", Width},
		{"window.size.height", Height},
		{"game.cells", Cells},
		{"window.position.x", X},
		{"window.position.y", Y},
		{"window.background.hue", BackgroundHue},
		{"window.grid.hue", GridHue},
		{"image.circle", Circle},
		{"image.cross", Cross}
	};

	void(*SetConfigFile)() = MapViewFileWriter;
	void(*LoadConfigFile)() = MapViewLoader;

	void SetIOMode(std::string commandlet) {
		switch (fileWritersMap[commandlet]) {
			case MapView:
			{
				SetConfigFile = MapViewFileWriter;
				LoadConfigFile = MapViewLoader;
			} break;

			case StandartIO:
			{
				SetConfigFile = StandartIOFileWriter;
				LoadConfigFile = StandartIOLoader;
			} break;

			case WinAPIFunc:
			{
				SetConfigFile = WinAPIFuncFileWriter;
				LoadConfigFile = WinAPIFuncLoader;
			} break;

			case FileVars:
			{
				SetConfigFile = FileVarsFileWriter;
				LoadConfigFile = FileVarsLoader;
			} break;
		}
	}

	int HandleCommandlet(char** argv, std::string commandlet, int i) {
		switch (comamandletsMap[commandlet]) {
			case File:
			{
				SetIOMode(commandlet);
				return 0;
			}

			case Size:
			{
				graphicEngine->SetWindowWidth(std::stoi(std::string(argv[i + 1])));
				graphicEngine->SetWindowHeight(std::stoi(std::string(argv[i + 2])));
				return 2;
			}
		}
	}

	void SetCommandLineArguments() {
		char** argv = nullptr;
		int argc;
		LPWSTR* lpArgv = CommandLineToArgvW(GetCommandLineW(), &argc);
		argv = new char* [argc];

		for (int i = 0; i < argc; i++) {
			int size = wcslen(lpArgv[i]) + 1;
			argv[i] = new char[size];
			wcstombs(argv[i], lpArgv[i], size);
		}

		LocalFree(lpArgv);

		if (argc > 1) {
			for (int i = 1; i < argc; i++) {
				std::string commandlet(argv[i]);
				i += HandleCommandlet(argv, commandlet, i);
			}
		}

		for (int i = 0; i < argc; i++)
			delete argv[i];
		delete argv;
	}

	void SetSettings(RegResult* regRes) {
		switch (optionMap[regRes->option]) {
			case Cross: {
				crossName = regRes->value.c_str();
			} break;

			case Circle: {
				circleName = regRes->value.c_str();
			} break;

			case Cells: {
				try {
					Game->AllocateField(std::stoi(regRes->value));
				} catch (std::invalid_argument) {
					Game->AllocateField(Game->N);
				}
			} break;

			case X: {
				try {
					graphicEngine->SetWindowX(std::stoi(regRes->value));
				} catch (std::invalid_argument) {
					graphicEngine->SetWindowX(0);
				}
				delete regRes;
			} break;

			case Y: {
				try {
					graphicEngine->SetWindowY(std::stoi(regRes->value));
				} catch (std::invalid_argument) {
					graphicEngine->SetWindowY(0);
				}
				delete regRes;
			} break;

			case BackgroundHue: {
				graphicEngine->SetBackgroundHue((short)std::stoi(regRes->value));
			} break;

			case GridHue: {
				graphicEngine->SetGridHue((short)std::stoi(regRes->value));
			} break;

			case Width: {
				try {
					if (graphicEngine->GetWindowWidth() == -1)
						graphicEngine->SetWindowWidth(std::stoi(regRes->value));
				} catch (std::invalid_argument) {
					graphicEngine->SetWindowWidth(640);
				}
				delete regRes;
			} break;

			case Height: {
				try {
					if (graphicEngine->GetWindowHeight() == -1)
						graphicEngine->SetWindowHeight(std::stoi(regRes->value));
				} catch (std::invalid_argument) {
					graphicEngine->SetWindowHeight(640);
				}
				delete regRes;
			} break;
		}
	}

	void SetSettings(std::vector<std::string>& arg) {
		std::regex reg("(.+)?=(.+)?");
		for (size_t i = 0; i < arg.size(); i++) {
			RegResult* regRes = new RegResult;
			std::smatch match;
			std::regex_match(arg[i], match, reg);

			regRes->option = match.str(1);
			regRes->value = match.str(2);

			SetSettings(regRes);
		}
	}

	void SetSettings(std::string str) {
		std::regex reg("(.+)?=(.+)?");
		std::vector<std::string> strVec(0);
		std::sregex_iterator currentMatch(str.begin(), str.end(), reg);
		std::sregex_iterator lastMatch;
		while (currentMatch != lastMatch) {
			std::smatch match = *currentMatch;
			strVec.push_back(match.str());
			currentMatch++;
		}
		SetSettings(strVec);
	}

	std::string PrepareConfigText() {
		std::string str;
		str = "window.size.width=" + std::to_string(graphicEngine->GetWindowWidth()) + '\n';
		str += "window.size.height=" + std::to_string(graphicEngine->GetWindowHeight()) + '\n';
		str += "window.position.x=" + std::to_string(graphicEngine->GetWindowX()) + '\n';
		str += "window.position.y=" + std::to_string(graphicEngine->GetWindowY()) + '\n';
		str += "window.background.hue=" + std::to_string(graphicEngine->GetBackgroundHue()) + '\n';
		str += "window.grid.hue=" + std::to_string(graphicEngine->GetGridHue()) + '\n';
		str += "game.cells=" + std::to_string(Game->N) + '\n';

		if (circleName != NULL) {
			str += "image.circle=" + (std::string)circleName + '\n';
		}

		if (crossName != NULL) {
			str += "image.cross=" + (std::string)crossName + '\n';
		}

		str.erase(std::prev(str.end()));

		return str;
	}

	void MapViewLoader() {
		HANDLE hFile = CreateFile("config.ini", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			return;
		}

		DWORD dwFileSize = GetFileSize(hFile, nullptr);
		if (dwFileSize == INVALID_FILE_SIZE) {
			CloseHandle(hFile);
			return;
		}

		HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (hMapping == nullptr) {
			CloseHandle(hFile);
			return;
		}

		LPVOID lpvFile = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, dwFileSize);
		char* dataPtr = (char*)lpvFile;
		if (dataPtr == nullptr) {
			CloseHandle(hMapping);
			CloseHandle(hFile);
			return;
		}

		std::string str(dataPtr);

		SetSettings(str);

		UnmapViewOfFile(dataPtr);
		CloseHandle(hMapping);
		CloseHandle(hFile);
	}

	void StandartIOLoader() {
		std::ifstream readFile;
		readFile.open("config.ini");
		if (!readFile.is_open())
			return;
		std::vector<std::string> strVec(0);

		while (!readFile.eof()) {
			std::string str;
			std::getline(readFile, str);
			if (str != "")
				strVec.push_back(str);
		}
		readFile.close();

		SetSettings(strVec);
	}

	void WinAPIFuncLoader() {
		HANDLE hFile = CreateFile("config.ini", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			return;
		}

		DWORD dwFileSize = GetFileSize(hFile, nullptr);
		if (dwFileSize == INVALID_FILE_SIZE) {
			CloseHandle(hFile);
			return;
		}

		int dwBufSize = dwFileSize / sizeof(char) + 1;
		char* dataPtr = new char[dwBufSize];
		LPDWORD lpdwBytesReaded = nullptr;
		ReadFile(hFile, dataPtr, dwFileSize, lpdwBytesReaded, nullptr);
		dataPtr[dwBufSize - 1] = '\0';
		std::string str(dataPtr);

		SetSettings(str);

		CloseHandle(hFile);
		delete[] dataPtr;
		delete lpdwBytesReaded;
	}

	void FileVarsLoader() {
		FILE* file = fopen("config.ini", "r+");
		if (file == NULL)
			return;
		fseek(file, 0, SEEK_END);
		long lSize = ftell(file);
		rewind(file);

		char* buffer = new char[lSize];
		fread(buffer, sizeof(char), lSize, file);

		std::string str(buffer);
		SetSettings(str);

		fclose(file);
		delete[] buffer;
	}

	void MapViewFileWriter() {
		std::string str = PrepareConfigText();

		DWORD dwSize = (sizeof(char) * (int)str.size());
		HANDLE hFile = CreateFile("config.ini", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			hFile = CreateFile("config.ini", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile == INVALID_HANDLE_VALUE) {
				return;
			}
		}

		HANDLE hMapping = CreateFileMapping(hFile, nullptr, PAGE_READWRITE, 0, dwSize, nullptr);
		if (hMapping == nullptr) {
			DWORD s = GetLastError();
			CloseHandle(hFile);
			return;
		}

		LPVOID lpvFile = MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, dwSize);
		char* buff = (char*)lpvFile;
		const char* temp = str.c_str();
		strcpy(buff, str.c_str());

		UnmapViewOfFile(lpvFile);
		CloseHandle(hMapping);
		SetFilePointer(hFile, dwSize, nullptr, FILE_BEGIN);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}

	void StandartIOFileWriter() {
		std::ofstream writeFile;
		writeFile.open("config.ini");

		std::string str = PrepareConfigText();

		writeFile << str;
		writeFile.close();
	}

	void WinAPIFuncFileWriter() {
		std::string str = PrepareConfigText();

		DWORD dwSize = sizeof(char) * (int)str.size();
		HANDLE hFile = CreateFile("config.ini", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile == INVALID_HANDLE_VALUE) {
			hFile = CreateFile("config.ini", GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (hFile == INVALID_HANDLE_VALUE) {
				return;
			}
		}
		LPDWORD lpdwBytesReaded = nullptr;
		WriteFile(hFile, str.c_str(), dwSize, lpdwBytesReaded, nullptr);

		SetFilePointer(hFile, dwSize, nullptr, FILE_BEGIN);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
		delete lpdwBytesReaded;
	}

	void FileVarsFileWriter() {
		std::string str = PrepareConfigText();

		FILE* file = fopen("config.ini", "w+");
		if (file == NULL)
			return;

		fwrite(str.c_str(), sizeof(char), (int)str.size(), file);
		fclose(file);
	}
}