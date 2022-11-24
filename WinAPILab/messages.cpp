#include <windows.h>
#include <map>
#include <string>
#include "messages.h"

namespace texts {
	std::map<MessageType, std::string> messageTexts = {
		{CrossTurn, "Ход крестиков!"},
		{CircleTurn, "Ход ноликов!"},
		{Win, "Вы победили!"},
		{Loose, "Вы проиграли!"},
		{StartGame, "Игра начинается"}
	};

	std::map<MessageType, std::string> messageCaps = {
		{CrossTurn, "Следущий ход"},
		{CircleTurn, "Следущий ход"},
		{Win, "Победа"},
		{Loose, "Поражение"},
		{StartGame, "Старт"}
	};
}

int MessageController::StartGame(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::StartGame].c_str(),
		texts::messageCaps[texts::StartGame].c_str(),
		MB_OK | MB_ICONEXCLAMATION | MB_DEFAULT_DESKTOP_ONLY
	);

	return 0;
}

int MessageController::CircleTurn(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::CircleTurn].c_str(),
		texts::messageCaps[texts::CircleTurn].c_str(),
		MB_OK | MB_ICONEXCLAMATION | MB_DEFAULT_DESKTOP_ONLY
	);

	return 0;
}

int MessageController::CrossTurn(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::CrossTurn].c_str(),
		texts::messageCaps[texts::CrossTurn].c_str(),
		MB_OK | MB_ICONEXCLAMATION | MB_DEFAULT_DESKTOP_ONLY
	);

	return 0;
}

int MessageController::Loose(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::Loose].c_str(),
		texts::messageCaps[texts::Loose].c_str(),
		MB_OK | MB_ICONEXCLAMATION | MB_DEFAULT_DESKTOP_ONLY
	);

	return 0;
}

int MessageController::Win(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::Win].c_str(),
		texts::messageCaps[texts::Win].c_str(),
		MB_OK | MB_ICONINFORMATION | MB_DEFAULT_DESKTOP_ONLY
	);

	return 0;
}