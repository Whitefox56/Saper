#include "GameContext.h"

GameContext* GameContext::instance = nullptr;

GameContext::~GameContext()
{
	settingsWidth = 10;
	settingsHeight = 10;
	settingsMinesCount = 30;
	settingsTimeLimit = 0;

	mines = nullptr;
	flagged = nullptr;
	revealed = nullptr;

	Reset();
}

void GameContext::DeleteField()
{
	if (mines != nullptr) {
		for (int row = 0; row < height; row++) {
			delete[] mines[row];
		}

		delete[] mines;
		mines = nullptr;
	}

	if (flagged != nullptr) {
		for (int row = 0; row < height; row++) {
			delete[] flagged[row];
		}
		delete[] flagged;
		flagged = nullptr;
	}

	if (revealed != nullptr) {
		for (int row = 0; row < height; row++) {
			delete[] revealed[row];
		}
		delete[] revealed;
		revealed = nullptr;
	}
}

void GameContext::CreateField()
{
	mines = new bool*[height];
	flagged = new bool*[height];
	revealed = new bool*[height];
	for (int row = 0; row < height; row++) {
		mines[row] = new bool[width];
		flagged[row] = new bool[width];
		revealed[row] = new bool[width];
		for (int col = 0; col < width; col++) {
			mines[row][col] = false;
			flagged[row][col] = false;
			revealed[row][col] = false;
		}
	}
}

void GameContext::PlaceMines()
{
	int placedCount = 0;
	while (placedCount < minesCount) {
		int row = rand() % height;
		int col = rand() % width;
	}
}

GameContext::~GameContext()
{
	DeleteField();
}

GameContext * GameContext::GetInstance()
{
	if (instance == nullptr) {
		instance = new GameContext();
	}
	return instance;
}

void GameContext::Reset()
{
	DeleteField();

	width = settingsWidth;
	height = settingsHeight;
	minesCount = settingsMinesCount;
	timeLimit = settingsTimeLimit;

	timeLeft = timeLimit;

	minesPlaced = false;

	CreateField();
}
