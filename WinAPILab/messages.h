#pragma once
#include <map>
#include <string>

namespace texts {
	enum MessageType {
		CrossTurn,
		CircleTurn,
		Win,
		Loose,
		StartGame
	};

	extern std::map<MessageType, std::string> messageTexts;
	extern std::map<MessageType, std::string> messageCaps;
}

class MessageController {
public:
	static int StartGame(HWND);
	static int CircleTurn(HWND);
	static int CrossTurn(HWND);
	static int Win(HWND);
	static int Loose(HWND);
};
