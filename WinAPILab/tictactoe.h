#pragma once

namespace gameState {
	enum Player {
		Player1,
		Player2,
		Neutral
	};

	class GameState {
		public:
			Player currPlayer = Player1;
			Player clientPlayer = Player1;
			Player** field;

			bool newGame = false;
			bool winCombination = false;

			int N = 21;

			int lastI = 0;
			int lastJ = 0;

			GameState();
			~GameState();
			void ShowMessage(int, int);
			void AllocateField(int);
			bool CheckInDirection(int, int, int, int);
			bool CheckWinCombination(int, int);
			void PlaceFigure(LPARAM, HWND, bool*);
			void ChangeCurrPlayer(WPARAM);
			void InitializeGameField(Player**);
			void InitializeGameField();
	};
}