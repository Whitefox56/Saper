#include "GameUI.h"

const int GameUI::CELL_SIZE = 25;

GameUI::GameUI(GameContext* gameContext)
{
	this->gameContext = gameContext;
}

void GameUI::Draw(HDC hdc)
{
	int height = gameContext->GetActiveHeight();
	int width = gameContext->GetActiveWidth();

	HBRUSH emptyBrush = CreateSolidBrush(RGB(128, 128, 255));
	HBRUSH revealedBrush = CreateSolidBrush(RGB(255, 255, 255));
	HBRUSH explodeBrush = CreateSolidBrush(RGB(255, 0, 0));
	HBRUSH flagBrush = CreateSolidBrush(RGB(255, 128, 0));
	HBRUSH mineBrush = CreateSolidBrush(RGB(64, 64, 64));
	HBRUSH completeBrush = CreateSolidBrush(RGB(0, 192, 64));

	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			int left = col * CELL_SIZE;
			int top = row * CELL_SIZE;
			int right = col * CELL_SIZE + CELL_SIZE;
			int bottom = row * CELL_SIZE + CELL_SIZE;

			// draws cells in different colors
			if (gameContext->IsRevealed(row, col))
				if (gameContext->HasMine(row, col))
					SelectBrush(hdc, explodeBrush);
				else
					SelectBrush(hdc, revealedBrush);
			else
				if (gameContext->GetState() == Complete &&
					gameContext->HasMine(row, col))
					SelectBrush(hdc, completeBrush);
				else
					SelectBrush(hdc, emptyBrush);

			Rectangle(hdc, left, top, right, bottom);

			// draws numbers with the number of mines
			if (gameContext->IsRevealed(row, col) &&
				!gameContext->HasMine(row, col) &&
				gameContext->MinesAround(row, col) > 0) {
				wchar_t str[5];
				_itow_s(gameContext->MinesAround(row, col), str, 10);
				SelectBrush(hdc, explodeBrush);
				TextOut(hdc, left + 7, top + 5, str, 1);
			}

			// draws flags
			if (gameContext->IsFlagged(row, col)) {
				SelectBrush(hdc, flagBrush);
				POINT points[6];
				points[0].x = left + 8;
				points[0].y = top + 20;
				points[1].x = left + 8;
				points[1].y = top + 5;
				points[2].x = left + 10;
				points[2].y = top + 5;
				points[3].x = left + 18;
				points[3].y = top + 10;
				points[4].x = left + 10;
				points[4].y = top + 15;
				points[5].x = left + 10;
				points[5].y = top + 20;
				Polygon(hdc, points, 6);
			}

			// draws mines
			if (gameContext->GetState() == Failure &&
				gameContext->HasMine(row, col) &&
				!gameContext->IsFlagged(row, col)) {
				SelectBrush(hdc, mineBrush);
				Ellipse(hdc, left + 5, top + 5, right - 5, bottom - 5);
			}
		}
	}


	DeleteObject(emptyBrush);
	DeleteObject(revealedBrush);
	DeleteObject(explodeBrush);
	DeleteObject(flagBrush);
	DeleteObject(mineBrush);
	DeleteObject(completeBrush);
}

void GameUI::ClickLeft(int x, int y)
{
	int row = y / CELL_SIZE;
	int col = x / CELL_SIZE;
	if (row < 0 || row >= gameContext->GetHeight() ||
		col < 0 || col >= gameContext->GetWidth())
		return;
	// left click - open cell
	gameContext->Reveal(row, col);
}

void GameUI::ClickRight(int x, int y)
{
	int row = y / CELL_SIZE;
	int col = x / CELL_SIZE;
	if (row < 0 || row >= gameContext->GetHeight() ||
		col < 0 || col >= gameContext->GetWidth())
		return;
	// right click - check the box
	gameContext->ToggleFlagged(row, col);
}

int GameUI::GetWidth()
{
	return gameContext->GetWidth() * CELL_SIZE;
}

int GameUI::GetHeight()
{
	return gameContext->GetHeight() * CELL_SIZE;
}
