#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <tchar.h>
#include <random>
#include <string>
#include "guilib.h"
#include "tictactoe.h"
#include "srlzsys.h"
#include "main_logic.h"
#include "map_view.h"
#include "messages.h"

UINT FieldUpdMsg;
const TCHAR FIELD_UPD_MSG_NAME[] = _T("FieldUpdate");

const char* crossName;
const char* circleName;

const TCHAR szWinClass[] = _T("tictactoe");
const TCHAR szWinName[] = _T("TicTacToe");
std::string winName = "TicTacToe";

WNDCLASS wincl = { 0 };

bool gradientThreadSuspended = false;
HANDLE gridThread;
HANDLE gradientThread;

gameState::GameState* Game;

mapView::GamestateMapView* gamestateMapView;
mapView::FieldMapView* fieldMapView;

std::random_device rd;
std::mt19937 mersenne;

GraphicEngine* graphicEngine;

CRITICAL_SECTION figureCriticalSection;
CRITICAL_SECTION resizeCriticalSection;
CRITICAL_SECTION backgroundCriticalSection;

void RunNotepad() {
	STARTUPINFO sInfo;
	PROCESS_INFORMATION pInfo;

	ZeroMemory(&sInfo, sizeof(STARTUPINFO));

	puts("Starting Notepad...");
	CreateProcess(_T("C:\\Windows\\Notepad.exe"),
				  NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

int GetThreadPriority(int n) {
	switch (n) {
		case 1: {
			return THREAD_PRIORITY_IDLE;
		}

		case 2: {
			return THREAD_PRIORITY_LOWEST;
		}

		case 3: {
			return THREAD_PRIORITY_BELOW_NORMAL;
		}

		case 4: {
			return THREAD_PRIORITY_NORMAL;
		}

		case 5: {
			return THREAD_PRIORITY_ABOVE_NORMAL;
		}

		case 6: {
			return THREAD_PRIORITY_HIGHEST;
		}

		case 7: {
			return THREAD_PRIORITY_TIME_CRITICAL;
		}
	}
	return THREAD_PRIORITY_NORMAL;
}

LRESULT CALLBACK WindowProcedure(HWND hWindow, UINT message, WPARAM wParam, LPARAM lParam) {
	bool placed = false;
	switch (message) {
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}
		
		case WM_LBUTTONUP: {
			Game->PlaceFigure(lParam, hWindow, &placed);
			if (placed) {
				fieldMapView->SyncToMapView();
				bool posted = PostMessage(HWND_BROADCAST, FieldUpdMsg, Game->currPlayer, NULL);
			}
		}
		case WM_PAINT: {
			return 0;
		}

		case WM_SIZE: {
			graphicEngine->OnResize();
			return 0;
		}

		case WM_MOVE: {			
			return 0;
		}

		case WM_MOUSEWHEEL: {
			short wheelDir = GET_WHEEL_DELTA_WPARAM(wParam);
			graphicEngine->ChangeGridColor(wheelDir);
			return 0;
		}

		case WM_KEYUP: {
			switch (wParam) {
				case VK_RETURN: {
					/*RandomizeBackgroundColor(hBrush, hWindow);*/
					return 0;
				}

				case 0x51: {
					char highBit = ((unsigned short)GetKeyState(VK_CONTROL)) >> 15;
					if ((bool)highBit) {
						PostQuitMessage(0);
						return 0;
					}
				} break;

				case 0x43: {
					char highBit = ((unsigned short)GetKeyState(VK_SHIFT)) >> 15;
					if ((bool)highBit) {
						RunNotepad();
						return 0;
					}
				} break;

				case 0x31:
				case 0x32:
				case 0x33:
				case 0x34:
				case 0x35:
				case 0x36:
				case 0x37: {
					SetThreadPriority(gradientThread, GetThreadPriority(wParam - 0x30));
				} break;

				case VK_ESCAPE: {
					PostQuitMessage(0);
					return 0;
				}

				case VK_SPACE: {
					if (gradientThreadSuspended) {
						ResumeThread(gradientThread);
					} else {
						SuspendThread(gradientThread);
					}
					gradientThreadSuspended = !gradientThreadSuspended;
				}
			}
		}
	}

	if (message == FieldUpdMsg) {
		fieldMapView->SyncFromMapView();
		FigureDrawProcPrep(wParam);
		if (Game->CheckWinCombination(Game->lastI, Game->lastJ)) {
			PostQuitMessage(0);
			return 0;
		}
		Game->ChangeCurrPlayer(wParam);
		if(Game->currPlayer == gameState::Player1){
			MessageController::CrossTurn(NULL);
		} else {
			MessageController::CircleTurn(NULL);
		}
		return 0;
	}

	return DefWindowProc(hWindow, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	wchar_t gr[128] = L"TEST";
	SetConsoleTitle((LPCSTR)gr);

	BOOL bMessageOk;
	MSG message;
	HWND hWindow;

	std::mt19937 mersenne(rd());

	Game = new gameState::GameState();
	graphicEngine = new GraphicEngine();

	serialize::SetCommandLineArguments();
	serialize::LoadConfigFile();
	saveFileHandler::LoadSave();

	fieldMapView = new mapView::FieldMapView("FieldMap", Game->N * Game->N * sizeof(Game->field) + sizeof(int) * 2);
	gamestateMapView = new mapView::GamestateMapView("GamestateMap", sizeof(bool));

	if (fieldMapView == nullptr || gamestateMapView == nullptr) {
		return 1;
	}

	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szWinClass;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);

	graphicEngine->windowSettings->CorrectSize();

	if (!RegisterClass(&wincl))
		return 1;

	hWindow = CreateWindow(
		szWinClass,
		((Game->clientPlayer == gameState::Player1) ? winName + " Крестики" : winName + " Нолики").c_str(),
		WS_OVERLAPPEDWINDOW,
		graphicEngine->GetWindowX(),
		graphicEngine->GetWindowY(),
		graphicEngine->GetWindowWidth(),
		graphicEngine->GetWindowHeight(),
		HWND_DESKTOP,
		NULL,
		hThisInstance,
		NULL
	);

	try {
		HMODULE hModule = LoadLibrary(_T("imagelib.dll"));
		if (hModule != NULL) {
			typedef unsigned char* (_cdecl* DLLFUNC)(const char*, int*, int*);
			DLLFUNC dllFunc = (DLLFUNC)GetProcAddress(hModule, _T("LoadImageS"));
			if (dllFunc != NULL) {
				if (crossName != NULL) {
					crossImageWidth = new int(0);
					crossImageHeight = new int(0);
					crossImage = dllFunc(crossName, crossImageWidth, crossImageHeight);
				}

				if (circleName != NULL) {
					circleImageWidth = new int(0);
					circleImageHeight = new int(0);
					circleImage = dllFunc(circleName, circleImageWidth, circleImageHeight);
				}
			}
			FreeLibrary(hModule);
		}
	} catch (...) {
		DWORD er = GetLastError();
	}

	InitializeCriticalSection(&figureCriticalSection);
	InitializeCriticalSection(&resizeCriticalSection);
	InitializeCriticalSection(&backgroundCriticalSection);

	graphicEngine->InitGraphicEngine(hWindow);
	ShowWindow(hWindow, nCmdShow);

	if (Game->newGame) {
		MessageController::StartGame(NULL);
	}

	gridThread = CreateThread(NULL, 0, GridDrawingProc, (LPVOID)graphicEngine, 0, NULL);
	gradientThread = CreateThread(NULL, 0, GradientDrawingProc, (LPVOID)graphicEngine, 0, NULL);

	if (gridThread == INVALID_HANDLE_VALUE || gradientThread == INVALID_HANDLE_VALUE) {
		DestroyWindow(hWindow);
		UnregisterClass(szWinClass, hThisInstance);

		delete gamestateMapView;
		delete fieldMapView;
		delete Game;
		return 0;
	}

	FieldUpdMsg = RegisterWindowMessage(FIELD_UPD_MSG_NAME);

	while ((bMessageOk = GetMessage(&message, NULL, 0, 0)) != 0) {
		if (bMessageOk == -1) {
			puts("error");
			break;
		}

		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	serialize::SetConfigFile();
	
	TerminateThread(gridThread, 0);
	TerminateThread(gradientThread, 0);

	DeleteCriticalSection(&figureCriticalSection);
	DeleteCriticalSection(&resizeCriticalSection);
	DeleteCriticalSection(&backgroundCriticalSection);

	DestroyWindow(hWindow);
	UnregisterClass(szWinClass, hThisInstance);
	graphicEngine->~GraphicEngine();

	gamestateMapView->~GamestateMapView();
	fieldMapView->~FieldMapView();
	saveFileHandler::SaveGame();
	Game->~GameState();

	return 0;
}