#include <windows.h>
#include <map>
#include <string>
#include "messages.h"

namespace texts {
	std::map<MessageType, std::string> messageTexts = {
		{NextTurn, "Ход "},
		{YourTurn, "Ваш ход!"},
		{Win, "Вы победили!"},
		{Loose, "Вы проиграли!"},
		{StartGame, "Игра начинается"}
	};

	std::map<MessageType, std::string> messageCaps = {
		{NextTurn, "Следущий ход"},
		{YourTurn, "Следущий ход"},
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
		MB_OK | MB_ICONEXCLAMATION
	);
}

int MessageController::NextTurn(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::NextTurn].c_str(),
		texts::messageCaps[texts::NextTurn].c_str(),
		MB_OK | MB_ICONEXCLAMATION
	);
}

int MessageController::YourTurn(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::YourTurn].c_str(),
		texts::messageCaps[texts::YourTurn].c_str(),
		MB_OK | MB_ICONEXCLAMATION
	);
}

int MessageController::Loose(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::Loose].c_str(),
		texts::messageCaps[texts::Loose].c_str(),
		MB_OK | MB_ICONEXCLAMATION
	);
}

int MessageController::Win(HWND hWindow) {
	int msgId = MessageBox(
		hWindow,
		texts::messageTexts[texts::Win].c_str(),
		texts::messageCaps[texts::Win].c_str(),
		MB_OK | MB_ICONINFORMATION
	);
}