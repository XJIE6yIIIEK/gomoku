#include "guilib.h"
#include "main_logic.h"
#include "tictactoe.h"
#include "messages.h"

namespace gameState {
	GameState::GameState() {}

	GameState::~GameState() {
		for (int i = 0; i < this->N; i++)
			delete[] this->field[i];
		delete this->field;
	}

	void GameState::AllocateField(int N) {
		this->N = N;

		this->field = new Player * [N];
		for (int i = 0; i < N; i++) {
			field[i] = new Player[N];
		}
	}

	void GameState::InitializeGameField(Player** field) {
		for (int i = 0; i < this->N; i++) {
			for (int j = 0; j < this->N; j++) {
				this->field[i][j] = field[i][j];
			}
		}
	}

	void GameState::InitializeGameField() {
		for (int i = 0; i < this->N; i++) {
			for (int j = 0; j < this->N; j++)
				this->field[i][j] = Neutral;
		}
	}

	bool GameState::CheckInDirection(int i, int j, int di, int dj) {
		int count = 1;
		for (int n = 1; n <= 4; n++) {
			if (i + di * n >= 0 &&
				i + di * n < this->N &&
				j + dj * n >= 0 &&
				j + dj * n < this->N
				) {
				if (this->field[i][j] != this->field[i + di * n][j + dj * n]) {
					break;
				}
				count++;
			}
		}

		if (count == 5) {
			return true;
		}

		for (int n = 1; n <= 4; n++) {
			if (i + (-1) * di * n >= 0 &&
				i + (-1) * di * n < this->N &&
				j + (-1) * dj * n >= 0 &&
				j + (-1) * dj * n < this->N
				) {
				if (this->field[i][j] != this->field[i + (-1) * di * n][j + (-1) * dj * n]) {
					break;
				}
				count++;
			}
		}

		if (count == 5) {
			return true;
		}

		return false;
	}

	void GameState::ShowMessage(int i, int j) {
		if (field[i][j] == clientPlayer) {
			MessageController::Win(NULL);
		} else {
			MessageController::Loose(NULL);
		}
	}

	bool GameState::CheckWinCombination(int i, int j) {
		if (this->CheckInDirection(i, j, -1, -1)) {
			this->winCombination = true;
		}

		if (!this->winCombination && this->CheckInDirection(i, j, -1, 0)) {
			this->winCombination = true;
		}

		if (!this->winCombination && this->CheckInDirection(i, j, -1, 1)) {
			this->winCombination = true;
		}

		if (!this->winCombination && this->CheckInDirection(i, j, 0, 1)) {
			this->winCombination = true;
		}

		if (this->winCombination) {
			ShowMessage(i, j);
			return true;
		}

		return false;
	}

	void GameState::PlaceFigure(LPARAM lParam, HWND hWindow, bool* placed) {
		int xPos = GET_X_LPARAM(lParam);
		int yPos = GET_Y_LPARAM(lParam);

		int i = 0;
		int j = 0;
		bool found = graphicEngine->gridOptions->CheckWithinBorder(lParam, hWindow, &i, &j, xPos, yPos);
		if (found) {
			if (field[i][j] == Neutral && clientPlayer == currPlayer) {
				field[i][j] = currPlayer;

				if (currPlayer == Player1) {
					FigureDrawProcPrep(i, j, false);
				} else {
					FigureDrawProcPrep(i, j, true);
				}

				lastI = i;
				lastJ = j;
				currPlayer = (Player)(!currPlayer);
				
				*placed = true;
			}
		}
	}

	void GameState::ChangeCurrPlayer(WPARAM wParam) {
		currPlayer = (Player)wParam;
	}
}