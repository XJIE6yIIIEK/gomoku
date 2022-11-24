#pragma comment(lib, "Msimg32.lib")
#include <windows.h>
#include <windowsx.h>
#include <time.h>
#include <string>
#include <vector>
#include <random>
#include "guilib.h"
#include "tictactoe.h"
#include "main_logic.h"

std::vector<std::string> imageNames(0);

int components = 4;

unsigned char* crossImage;
int* crossImageWidth;
int* crossImageHeight;

unsigned char* circleImage;
int* circleImageWidth;
int* circleImageHeight;

void (*DrawCirclePtr)();
void (*DrawCrossPtr)();

HANDLE resizeThread;

FigureLParams::FigureLParams(GraphicEngine* graphicEngine, int i, int j) {
	this->graphicEngine = graphicEngine;
	this->i = i;
	this->j = j;
}

void GraphicEngine::DrawBackground() {
	RECT rect;
	rect.top = rect.left = 0;
	rect.right = this->clientSettings->width;
	rect.bottom = this->clientSettings->height;
	HBRUSH hBrush = CreateSolidBrush(this->backgroundRGB);
	FillRect(this->backgroundHDC, &rect, hBrush);
	DeleteObject(hBrush);
}

void GraphicEngine::UpdateBackground() {
	if (this->backgroundHDC != NULL) {
		DeleteObject(this->backgroundHDC);
	}

	if (this->hBmpBackground != NULL) {
		DeleteObject(this->hBmpBackground);
	}

	HDC hdc = GetDC(graphicEngine->hWindow);
	this->backgroundHDC = CreateCompatibleDC(hdc);
	this->hBmpBackground = CreateCompatibleBitmap(hdc, clientSettings->width, clientSettings->height);
	SelectObject(backgroundHDC, hBmpBackground);
	DeleteObject(hdc);

	this->DrawBackground();
}

WindowSettings::WindowSettings() {

}

_GridOptions::_GridOptions() {

}

ClientSettings::ClientSettings(RECT* clientRect) {
	this->width = clientRect->right - clientRect->left;
	this->height = clientRect->bottom - clientRect->top;
}

GraphicEngine::GraphicEngine() {
	this->windowSettings = new WindowSettings();
	this->gridOptions = new _GridOptions();
}

void GraphicEngine::SetBackgroundHue(short hue) {
	this->backgroundHue = hue;
	this->backgroundRGB = this->HSLtoRGB(hue, this->backgroundSaturation);
}

short GraphicEngine::GetBackgroundHue() {
	return this->backgroundHue;
}

void GraphicEngine::SetGridHue(short hue) {
	this->gridOptions->gridHue = hue;
	this->gridOptions->gridRGB = this->HSLtoRGB(hue, this->gridOptions->gridSaturation);
}

short GraphicEngine::GetGridHue() {
	return this->gridOptions->gridHue;
}

void GraphicEngine::InitGraphicEngine(HWND hWindow) {
	RECT* clientRect = new RECT;
	RECT* windowRect = new RECT;

	this->GetNewRects(hWindow, clientRect, windowRect);

	this->hWindow = hWindow;

	this->clientSettings = new ClientSettings(clientRect);
	this->gridOptions->InitGridOptions(clientRect, windowRect, this->clientSettings);

	UpdateBackground();

	delete clientRect;
	delete windowRect;
}

void _GridOptions::UpdateGridColor() {
	this->gridRGB = graphicEngine->HSLtoRGB(this->gridHue, this->gridSaturation);
}

int GraphicEngine::GetWindowHeight() {
	return this->windowSettings->height;
}

int GraphicEngine::GetWindowWidth() {
	return this->windowSettings->width;
}

int GraphicEngine::GetWindowX() {
	return this->windowSettings->posX;
}

int GraphicEngine::GetWindowY() {
	return this->windowSettings->posY;
}

void GraphicEngine::SetWindowHeight(int height) {
	if (this->windowSettings->height == -1) {
		this->windowSettings->height = height;
	}
}

void GraphicEngine::SetWindowWidth(int width) {
	if (this->windowSettings->width == -1) {
		this->windowSettings->width = width;
	}
}

void GraphicEngine::SetWindowX(int posX) {
	if (this->windowSettings->posX == 0) {
		this->windowSettings->posX = posX;
	}
}

void GraphicEngine::SetWindowY(int posY) {
	if (this->windowSettings->posY == 0) {
		this->windowSettings->posY = posY;
	}
}

GraphicEngine::~GraphicEngine() {
	delete this->clientSettings;
	delete this->windowSettings;
	this->gridOptions->~_GridOptions();

	DeleteObject(this->hBmpBackground);
	DeleteObject(this->hBmpBuf);
	DeleteObject(this->bufHDC);
	DeleteObject(this->backgroundHDC);
}

ClientSettings::~ClientSettings() {

}

WindowSettings::~WindowSettings() {

}

void _GridOptions::InitGridOptions(RECT* clientRect, RECT* windowRect, ClientSettings* clientSettings) {
	this->UpdateGridParameters(clientRect, windowRect);
	this->UpdateGridColor();
	this->UpdateBuffers(clientSettings);
}

void GraphicEngine::DrawFrame() {
	HDC mainHDC;
	PAINTSTRUCT ps;

	RECT* dcRect = new RECT;
	GetClientRect(graphicEngine->hWindow, dcRect);

	InvalidateRect(graphicEngine->hWindow, dcRect, TRUE);

	mainHDC = BeginPaint(graphicEngine->hWindow, &ps);

	if (this->clientSettings->needUpdate) {
		if (this->bufHDC != NULL) {
			DeleteObject(this->bufHDC);
		}

		if (this->hBmpBuf != NULL) {
			DeleteObject(this->hBmpBuf);
		}

		this->bufHDC = CreateCompatibleDC(mainHDC);
		this->hBmpBuf = CreateCompatibleBitmap(mainHDC, this->clientSettings->width, this->clientSettings->height);
		SelectObject(bufHDC, hBmpBuf);
		this->clientSettings->needUpdate = false;
	}

	BitBlt(this->bufHDC, 0, 0, this->clientSettings->width, this->clientSettings->height, this->backgroundHDC, 0, 0, SRCCOPY);

	BitBlt(this->bufHDC, 0, 0, this->clientSettings->width, this->clientSettings->height, this->gridOptions->gridMaskHDC, 0, 0, SRCAND);
	BitBlt(this->bufHDC, 0, 0, this->clientSettings->width, this->clientSettings->height, this->gridOptions->gridHDC, 0, 0, SRCPAINT);


	BitBlt(mainHDC, 0, 0, this->clientSettings->width, this->clientSettings->height, this->bufHDC, 0, 0, SRCCOPY);

	DeleteObject(mainHDC);
	delete dcRect;
	EndPaint(graphicEngine->hWindow, &ps);
	UpdateWindow(graphicEngine->hWindow);
}

COLORREF GraphicEngine::HSLtoRGB(short hue, float saturation) {
	return RGB(255 * this->HSLFunc(0, hue, saturation), 255 * this->HSLFunc(8, hue, saturation), 255 * this->HSLFunc(4, hue, saturation));
}

float GraphicEngine::HSLFunc(short n, short hue, float saturation) {
	float degree = (float)hue / 30;
	float a = saturation * 0.5f;
	float k = ((n + (int)degree) % 12) + degree - (int)degree;
	float result = 0.5 - a * max(-1, min(min(k - 3, 9 - k), 1));
	return result;
}

void WindowSettings::OnMove(HWND hWindow) {
	RECT* wRect = new RECT;
	GetWindowRect(hWindow, wRect);
	this->posX = wRect->left;
	this->posY = wRect->top;
	delete wRect;
}

void GraphicEngine::OnMove(HWND hWindow) {
	this->windowSettings->OnMove(hWindow);
}

void _GridOptions::ChangeGridColor(short wheelDir) {
	short normalizedDir = wheelDir / abs(wheelDir);
	this->gridHue += normalizedDir;
	if (this->gridHue + normalizedDir > 359) {
		this->gridHue = 0;
	} else if (this->gridHue + normalizedDir < 0) {
		this->gridHue = 359;
	}

	this->gridRGB = graphicEngine->HSLtoRGB(this->gridHue, this->gridSaturation);

	this->DrawGrid();
}

bool _GridOptions::CheckWithinBorder(LPARAM lParam, HWND hWindow, int* cellY, int* cellX, int xPos, int yPos) {
	for (int i = 0; i < Game->N; i++) {
		for (int j = 0; j < Game->N; j++) {
			if (yPos > this->marginY && xPos > this->marginX && xPos < this->marginX + this->cellSize * (i + 1) && yPos < this->marginY + this->cellSize * (j + 1)) {
				*cellX = i;
				*cellY = j;
				return true;
			}
		}
	}

	return false;
}

void GraphicEngine::ChangeGridColor(short wheelDir) {
	this->gridOptions->ChangeGridColor(wheelDir);
}

void _GridOptions::DrawGrid() {
	HPEN pen = CreatePen(PS_SOLID, 3, this->gridRGB);
	SelectPen(this->gridHDC, pen);

	for (int i = 0; i <= Game->N; i++) {
		MoveToEx(this->gridHDC, this->marginX, this->marginY + this->cellSize * i, NULL);
		LineTo(this->gridHDC, this->marginX + this->cellSize * Game->N, this->marginY + this->cellSize * i);
		MoveToEx(this->gridHDC, this->marginX + this->cellSize * i, this->marginY, NULL);
		LineTo(this->gridHDC, this->marginX + this->cellSize * i, this->marginY + this->cellSize * Game->N);
	}

	DeleteObject(pen);
	this->MaskUpdate(0, 0, graphicEngine->clientSettings->width, graphicEngine->clientSettings->height);
}

void WindowSettings::CorrectSize() {
	if (this->width == -1) {
		this->width = 640;
	}

	if (this->height == -1) {
		this->height = 480;
	}
}

void GraphicEngine::RandomizeBackgroundColor() {
	this->backgroundHue = mersenne() % 360;
	this->backgroundRGB = this->HSLtoRGB(this->backgroundHue, this->backgroundSaturation);
}

void GraphicEngine::GetNewRects(HWND hWindow, RECT* clientRect, RECT* windowRect) {
	GetClientRect(hWindow, clientRect);
	GetWindowRect(hWindow, windowRect);
}

void GraphicEngine::OnResize() {
	RECT* clientRect = new RECT;
	RECT* windowRect = new RECT;

	this->GetNewRects(this->hWindow, clientRect, windowRect);

	this->clientSettings->needUpdate = true;

	this->clientSettings->OnResize(clientRect);
	this->windowSettings->OnResize(windowRect);
	this->gridOptions->OnResize(clientRect, windowRect);

	this->UpdateBackground();

	delete clientRect;
	delete windowRect;
}

void ClientSettings::UpdateValues(RECT* clientRect) {
	this->width = clientRect->right - clientRect->left;
	this->height = clientRect->bottom - clientRect->top;
}

void ClientSettings::OnResize(RECT* clientRect) {
	this->UpdateValues(clientRect);
}

void WindowSettings::UpdateValues(RECT* windowRect) {
	this->width = windowRect->right - windowRect->left;
	this->height = windowRect->bottom - windowRect->top;
	this->posX = windowRect->left;
	this->posY = windowRect->top;
}

void WindowSettings::OnResize(RECT* windowRect) {
	this->UpdateValues(windowRect);
}

void _GridOptions::DrawFigures() {
	for (int i = 0; i < Game->N; i++) {
		for (int j = 0; j < Game->N; j++) {
			if (Game->field[i][j] == gameState::Player1) {
				this->DrawCross(j, i);
			} else if (Game->field[i][j] == gameState::Player2) {
				this->DrawCircle(j, i);
			}
		}
	}
}

void _GridOptions::PlaceFigure(gameState::Player player) {
	
}

void _GridOptions::UpdateBuffers(ClientSettings* clientSettings) {
	if (this->hBmpGrid != NULL) {
		DeleteObject(this->hBmpGrid);
	}

	if (this->hBmpGridMask != NULL) {
		DeleteObject(this->hBmpGridMask);
	}

	if (this->gridHDC != NULL) {
		DeleteObject(this->gridHDC);
	}

	if (this->gridMaskHDC != NULL) {
		DeleteObject(this->gridMaskHDC);
	}

	HDC hdc = GetDC(graphicEngine->hWindow);

	this->gridHDC = CreateCompatibleDC(hdc);
	this->gridMaskHDC = CreateCompatibleDC(hdc);

	this->hBmpGrid = CreateCompatibleBitmap(hdc, clientSettings->width, clientSettings->height);
	this->hBmpGridMask = CreateBitmap(clientSettings->width, clientSettings->height, 1, 1, NULL);

	SelectObject(gridHDC, hBmpGrid);
	SelectObject(gridMaskHDC, hBmpGridMask);

	DeleteObject(hdc);

	this->DrawGrid();
	this->DrawFigures();
}

void _GridOptions::UpdateGridParameters(RECT* clientRect, RECT* windowRect) {
	int temp;
	int cellsSizeOnX = ((temp = ((windowRect->right - windowRect->left) - 2 * this->MINMARGIN) / Game->N) < 0) ? 0 : temp;
	int cellsSizeOnY = ((temp = (clientRect->bottom - 2 * this->MINMARGIN) / Game->N) < 0) ? 0 : temp;
	this->cellSize = (cellsSizeOnX < cellsSizeOnY) ? cellsSizeOnX : cellsSizeOnY;

	this->marginX = (((windowRect->right - windowRect->left) - (this->cellSize * Game->N)) / 2) - ((windowRect->right - windowRect->left) - clientRect->right) / 2;
	this->marginY = (clientRect->bottom - (this->cellSize * Game->N)) / 2;
}

void _GridOptions::OnResize(RECT* clientRect, RECT* windowRect) {
	this->UpdateGridParameters(clientRect, windowRect);
	this->UpdateBuffers(graphicEngine->clientSettings);
}

void _GridOptions::DrawCircle(int xIndex, int yIndex) {
	HPEN hPen;
	hPen = (HPEN)SelectObject(this->gridHDC, GetStockObject(HOLLOW_BRUSH));
	int outerLeft = this->marginX + this->cellSize * xIndex + 5;
	int outerTop = this->marginY + this->cellSize * yIndex + 5;
	int outerRight = this->marginX + this->cellSize * (xIndex + 1) - 5;
	int outerBottom = this->marginY + this->cellSize * (yIndex + 1) - 5;
	if (outerRight - outerLeft < 1) {
		DeleteObject(hPen);
		return;
	}
	Ellipse(this->gridHDC, outerLeft, outerTop, outerRight, outerBottom);
	DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, this->figuresWidth, this->figuresRGB);
	SelectPen(this->gridHDC, hPen);
	Ellipse(this->gridHDC, outerLeft, outerTop, outerRight, outerBottom);
	DeleteObject(hPen);
	this->MaskUpdate(outerLeft, outerTop, outerRight - outerLeft, outerRight - outerLeft);
}

void _GridOptions::DrawCross(int xIndex, int yIndex) {
	HPEN pen;
	pen = CreatePen(PS_SOLID, this->figuresWidth, this->figuresRGB);
	SelectPen(this->gridHDC, pen);
	int leftUpX = this->marginX + this->cellSize * xIndex + 5;
	int leftUpY = this->marginY + this->cellSize * yIndex + 5;
	int rightBottomX = this->marginX + this->cellSize * (xIndex + 1) - 5;
	int rightBottomY = this->marginY + this->cellSize * (yIndex + 1) - 5;

	int rightUpX = this->marginX + this->cellSize * (xIndex + 1) - 5;
	int rightUpY = this->marginY + this->cellSize * (yIndex)+5;
	int leftBottomX = this->marginX + this->cellSize * (xIndex)+5;
	int leftBottomY = this->marginY + this->cellSize * (yIndex + 1) - 5;
	MoveToEx(this->gridHDC, leftUpX, leftUpY, NULL);
	LineTo(this->gridHDC, rightBottomX, rightBottomY);
	MoveToEx(this->gridHDC, rightUpX, rightUpY, NULL);
	LineTo(this->gridHDC, leftBottomX, leftBottomY);
	DeleteObject(pen);
	this->MaskUpdate(leftUpX, leftUpY, rightUpX - leftUpX, leftBottomY - leftUpY);
}

void _GridOptions::MaskUpdate(int x, int y, int cx, int cy) {
	int width = cx + this->figuresWidth;
	int height = cy + this->figuresWidth;
	
	SetBkColor(this->gridHDC, RGB(0, 0, 0));

	BitBlt(this->gridMaskHDC, x, y, width, height, this->gridHDC, x, y, SRCCOPY);
	BitBlt(this->gridHDC, x, y, width, height, this->gridMaskHDC, x, y, SRCINVERT);
}

_GridOptions::~_GridOptions() {
	DeleteObject(this->hBmpGrid);
	DeleteObject(this->hBmpGridMask);
	DeleteObject(this->gridHDC);
	DeleteObject(this->gridMaskHDC);
}

void GraphicEngine::GradientLoop() {
	this->backgroundHue = this->backgroundHue == 359 ? 0 : this->backgroundHue + 1;
	this->backgroundRGB = this->HSLtoRGB(this->backgroundHue, this->backgroundSaturation);
	this->DrawBackground();
}

void GradientLoop(GraphicEngine* graphicEngine) {
	while (true) {
		EnterCriticalSection(&resizeCriticalSection);

		graphicEngine->GradientLoop();

		LeaveCriticalSection(&resizeCriticalSection);

		Sleep(16);
	}
}

void DrawingLoop(GraphicEngine* graphicEngine) {
	while (true) {
		EnterCriticalSection(&resizeCriticalSection);

		graphicEngine->DrawFrame();

		LeaveCriticalSection(&resizeCriticalSection);

		Sleep(16);
	}
}

void FigureDrawProcPrep(int i, int j, bool isCircle) {
	FigureLParams* params = new FigureLParams(graphicEngine, i, j);
	
	if (isCircle) {
		CreateThread(NULL, 0, DrawCircleProc, (LPVOID)params, 0, NULL);
	} else {
		CreateThread(NULL, 0, DrawCrossProc, (LPVOID)params, 0, NULL);
	}
}

void FigureDrawProcPrep(WPARAM wParam) {
	FigureLParams* params = new FigureLParams(graphicEngine, Game->lastI, Game->lastJ);

	if ((gameState::Player)wParam == gameState::Player1) {
		CreateThread(NULL, 0, DrawCircleProc, (LPVOID)params, 0, NULL);
	} else {
		CreateThread(NULL, 0, DrawCrossProc, (LPVOID)params, 0, NULL);
	}
}

void ResizeProcPrep() {
	if (resizeThread == NULL) {
		resizeThread = CreateThread(NULL, 0, OnResizeProc, (LPVOID)graphicEngine, 0, NULL);
	}
}

DWORD WINAPI OnResizeProc(_In_ LPVOID lpParam) {
	GraphicEngine* graphicEngine = (GraphicEngine*)lpParam;

	EnterCriticalSection(&resizeCriticalSection);

	graphicEngine->OnResize();

	LeaveCriticalSection(&resizeCriticalSection);

	ExitThread(0);
}

DWORD WINAPI DrawCrossProc(_In_ LPVOID lpParam) {
	FigureLParams* params = (FigureLParams*)lpParam;

	EnterCriticalSection(&resizeCriticalSection);

	params->graphicEngine->gridOptions->DrawCross(params->j, params->i);

	LeaveCriticalSection(&resizeCriticalSection);

	ExitThread(0);
}

DWORD WINAPI DrawCircleProc(_In_ LPVOID lpParam) {
	FigureLParams* params = (FigureLParams*)lpParam;

	EnterCriticalSection(&resizeCriticalSection);

	params->graphicEngine->gridOptions->DrawCircle(params->j, params->i);

	LeaveCriticalSection(&resizeCriticalSection);

	ExitThread(0);
}

DWORD WINAPI GradientDrawingProc(_In_ LPVOID lpParam) {
	GraphicEngine* graphicEngine = (GraphicEngine*)lpParam;
	GradientLoop(graphicEngine);
	ExitThread(0);
}

DWORD WINAPI GridDrawingProc(_In_ LPVOID lpParam) {
	GraphicEngine* graphicEngine = (GraphicEngine*)lpParam;
	DrawingLoop(graphicEngine);
	ExitThread(0);
}