#pragma once
#include <windows.h>
#include <windowsx.h>
#include <string>
#include "tictactoe.h"
#include <vector>
extern std::vector<std::string> imageNames;

extern int components;

extern unsigned char* crossImage;
extern int* crossImageWidth;
extern int* crossImageHeight;

extern unsigned char* circleImage;
extern int* circleImageWidth;
extern int* circleImageHeight;

DWORD WINAPI GridDrawingProc(_In_ LPVOID);
DWORD WINAPI GradientDrawingProc(_In_ LPVOID);
DWORD WINAPI DrawCircleProc(_In_ LPVOID);
DWORD WINAPI DrawCrossProc(_In_ LPVOID);
DWORD WINAPI OnResizeProc(_In_ LPVOID);

void FigureDrawProcPrep(int, int, bool);
void FigureDrawProcPrep(WPARAM);

void ResizeProcPrep();

class WindowSettings {
	public:
		int posX = 0;
		int posY = 0;
		int width = -1;
		int height = -1;

		//Инициализация
		WindowSettings(RECT*);
		WindowSettings();
		~WindowSettings();

		//События отрисовки
		void OnMove(HWND);
		void UpdateValues(RECT*);
		void OnResize(RECT*);

		//Utility
		void CorrectSize();
};

class ClientSettings {
	public:
		int width;
		int height;

		bool needUpdate = false;

		//Инициализация
		ClientSettings(RECT*);
		~ClientSettings();

		//События отрисовки
		void UpdateValues(RECT*);
		void OnResize(RECT*);
};

class _GridOptions {
	public:
		//Параметры маски
		HDC gridMaskHDC;
		HBITMAP hBmpGridMask;

		//Параметры фигур
		int figuresWidth = 3;
		COLORREF figuresRGB = RGB(255, 150, 0);

		//Параметры сетки
		HDC gridHDC;
		HBITMAP hBmpGrid;
		COLORREF gridRGB;
		short gridHue = 0;
		float gridSaturation = 0.5f;
		int cellSize;
		int marginX;
		int marginY;
		int MINMARGIN = 15;

		//Инициализация
		//_GridOptions(RECT*, RECT*, ClientSettings*);
		_GridOptions();
		~_GridOptions();
		void InitGridOptions(RECT*, RECT*, ClientSettings*);

		//События отрисовки
		void UpdateGridParameters(RECT*, RECT*);
		void UpdateBuffers(ClientSettings*);
		void OnResize(RECT*, RECT*);
		void MaskUpdate(int, int, int, int);

		//Функции отрисовки
		void DrawGrid();
		void DrawFigures();
		void DrawCircle(int, int);
		void DrawCross(int, int);
		void PlaceFigure(gameState::Player);

		//Utility
		void ChangeGridColor(short);
		void UpdateGridColor();
		bool CheckWithinBorder(LPARAM lParam, HWND hWindow, int* cellY, int* cellX, int xPos, int yPos);
};

class GraphicEngine {
	public:
		HDC backgroundHDC;
		HBITMAP hBmpBackground;
		HWND hWindow;

		COLORREF backgroundRGB;
		short backgroundHue;
		float backgroundSaturation = 0.3f;

		_GridOptions* gridOptions;
		WindowSettings* windowSettings;
		ClientSettings* clientSettings;

		HDC bufHDC;
		HBITMAP hBmpBuf;

		//Инициализация
		GraphicEngine();
		~GraphicEngine();
		void InitGraphicEngine(HWND);

		//События отрисовки
		void DrawFrame();
		void GetNewRects(HWND, RECT*, RECT*);
		void OnMove(HWND);
		void OnResize();

		//Работа с задним фоном
		void RandomizeBackgroundColor();
		void UpdateBackground();
		void DrawBackground();
		void GradientLoop();

		//Pass functions
		void ChangeGridColor(short);

		//Utility
		COLORREF HSLtoRGB(short, float);
		float HSLFunc(short, short, float);

		int GetWindowHeight();
		int GetWindowWidth();
		int GetWindowX();
		int GetWindowY();
		void SetWindowHeight(int);
		void SetWindowWidth(int);
		void SetWindowX(int);
		void SetWindowY(int);

		void SetBackgroundHue(short);
		short GetBackgroundHue();

		void SetGridHue(short);
		short GetGridHue();
};

class FigureLParams {
public:
	GraphicEngine* graphicEngine;
	int i;
	int j;

	FigureLParams(GraphicEngine*, int, int);
};