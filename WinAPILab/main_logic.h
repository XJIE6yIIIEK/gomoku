#pragma once
#include <windows.h>
#include <random>
#include "guilib.h"
#include "tictactoe.h"
#include <tchar.h>

extern const char* crossName;
extern const char* circleName;

extern const TCHAR szWinClass[];
extern const TCHAR szWinName[];
extern WNDCLASS wincl;

extern std::random_device rd;
extern std::mt19937 mersenne;

extern GraphicEngine* graphicEngine;

extern gameState::GameState* Game;

extern CRITICAL_SECTION figureCriticalSection;
extern CRITICAL_SECTION resizeCriticalSection;
extern CRITICAL_SECTION backgroundCriticalSection;