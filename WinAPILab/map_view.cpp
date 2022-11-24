#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <map>
#include <regex>
#include <iterator>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <cstdio>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <tchar.h>
#include "map_view.h"
#include "tictactoe.h"
#include "main_logic.h"

namespace mapView {
	MapView::MapView(std::string mapName, int fileBufSize) {
		this->szMapName = (TCHAR*)TEXT(mapName.c_str());
		this->fileBufSize = fileBufSize;

		hMapField = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, (LPCSTR)(this->szMapName));

		if (hMapField == NULL) {
			hMapField = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, fileBufSize, (LPCSTR)(this->szMapName));
			if (hMapField == NULL) {
				return;
			}

			lpMap = (LPCTSTR)MapViewOfFile(hMapField, FILE_MAP_ALL_ACCESS, 0, 0, fileBufSize);
			if (lpMap == NULL) {
				CloseHandle(hMapField);
				return;
			}

			initSync = true;
		} else {
			lpMap = (LPCTSTR)MapViewOfFile(hMapField, FILE_MAP_ALL_ACCESS, 0, 0, fileBufSize);
			if (lpMap == NULL) {
				CloseHandle(hMapField);
				return;
			}
		}
	}

	MapView::~MapView() {
		UnmapViewOfFile(lpMap);
		CloseHandle(hMapField);
	}

	FieldMapView::FieldMapView(std::string mapName, int fileBufSize) : MapView(mapName, fileBufSize) {
		if (this->initSync) {
			this->SyncToMapView();
		} else {
			this->SyncFromMapView();
		}
	}

	void FieldMapView::SyncFromMapView() {
		int xLength = Game->N * sizeof(Game->field);
		for (int i = 0; i < Game->N; i++) {
			memcpy_s(Game->field[i], xLength, (char*)this->lpMap + xLength * i, xLength);
		}

		memcpy_s(&(Game->lastI), sizeof(int), (char*)this->lpMap + xLength * Game->N, sizeof(int));
		memcpy_s(&(Game->lastJ), sizeof(int), (char*)this->lpMap + xLength * Game->N + sizeof(int), sizeof(int));
	}

	void FieldMapView::SyncToMapView() {
		int xLength = Game->N * sizeof(Game->field);
		for (int i = 0; i < Game->N; i++) {
			memcpy_s((char*)this->lpMap + xLength * i, xLength, Game->field[i], xLength);
		}

		memcpy_s((char*)this->lpMap + xLength * Game->N, sizeof(int), &(Game->lastI), sizeof(int));
		memcpy_s((char*)this->lpMap + xLength * Game->N + sizeof(int), sizeof(int), &(Game->lastJ), sizeof(int));
	}

	GamestateMapView::GamestateMapView(std::string mapName, int fileBufSize) : MapView(mapName, fileBufSize) {
		if (this->initSync) {
			this->SyncToMapView();
		} else {
			this->SyncFromMapView();
		}
	}

	GamestateMapView::~GamestateMapView() {
		if (Game->clientPlayer == gameState::Player1) {
			bool crossExist = false;
			memcpy_s((bool*)this->lpMap, sizeof(bool), &crossExist, sizeof(bool));
		}
	}

	void GamestateMapView::SyncFromMapView() {
		bool crossExist;
		memcpy_s(&crossExist, sizeof(bool), (bool*)this->lpMap, sizeof(bool));
		if (crossExist) {
			Game->clientPlayer = gameState::Player2;
		} else {
			Game->clientPlayer = gameState::Player1;
		}
	}

	void GamestateMapView::SyncToMapView() {
		bool cross = false;
		if (Game->clientPlayer == gameState::Player1) {
			cross = true;
		}
		memcpy_s((bool*)this->lpMap, sizeof(bool), &cross, sizeof(bool));
	}

	FieldMapView::~FieldMapView() {
	
	}
}

namespace saveFileHandler {
	TCHAR szSave[] = TEXT("game.sav");

	void LoadSave() {
		FILE* fSave = fopen(szSave, "ab+");
		if (fSave == NULL) {
			Game->InitializeGameField();
			Game->newGame = true;
			return;
		}

		fseek(fSave, 0, SEEK_END);
		long lSize = ftell(fSave);
		rewind(fSave);

		if (lSize > 0) {
			gameState::Player** field = new gameState::Player * [Game->N];
			for (int i = 0; i < Game->N; i++) {
				field[i] = new gameState::Player[Game->N];
			}

			for (int i = 0; i < Game->N; i++) {
				fread(field[i], sizeof(field[i][0]), Game->N, fSave);
			}

			Game->InitializeGameField(field);

			for (int i = 0; i < Game->N; i++) {
				delete[] field[i];
			}
			delete field;
		} else {
			Game->InitializeGameField();
			Game->newGame = true;
		}
		fread(&Game->currPlayer, sizeof(gameState::Player), 1, fSave);

		fclose(fSave);
	}

	void SaveGame() {
		if (Game->winCombination) {
			std::remove(szSave);
			return;
		}

		FILE* file = fopen(szSave, "wb+");
		if (file == NULL)
			return;

		fseek(file, 0, SEEK_SET);

		for (int i = 0; i < Game->N; i++) {
			fwrite(Game->field[i], sizeof(Game->field[i][0]), Game->N, file);
		}
		fwrite(&Game->currPlayer, sizeof(gameState::Player), 1, file);

		fclose(file);
	}
}