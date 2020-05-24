#include "GameContext.h"

GameContext* GameContext::instance = nullptr;

GameContext::GameContext()
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

GameContext::~GameContext() {
	DeleteField();
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

		if (!revealed[row][col] && !mines[row][col]) {
			mines[row][col] = true;
			placedCount++;
		}
	}

	state = Active;
}

GameContext* GameContext::GetInstance()
{
	if (instance == nullptr) {
		instance = new GameContext();
	}
	return instance;
}

bool GameContext::OutOfBounds(int row, int col)
{
	return row < 0 || col < 0 || row >= height || col >= width;
}

bool GameContext::IsRevealed(int row, int col)
{
	if (OutOfBounds(row, col))
		throw std::out_of_range("The specified cell is out of bounds.");

	return revealed[row][col];
}

bool GameContext::IsFlagged(int row, int col)
{
	if (OutOfBounds(row, col))
		throw std::out_of_range("The specified cell is out of bounds.");

	return flagged[row][col];
}

bool GameContext::HasMine(int row, int col)
{
	if (OutOfBounds(row, col))
		throw std::out_of_range("The specified cell is out of bounds.");

	return mines[row][col];
}

void GameContext::SetWidth(int value)
{
	if (value < 10) value = 10;
	if (value > 30) value = 30;
	settingsWidth = value;
}

void GameContext::SetHeight(int value)
{
	if (value < 10) value = 10;
	if (value > 24) value = 24;
	settingsHeight = value;
}

void GameContext::SetMinesCount(int value)
{
	int min = 10;
	int max = width * height - width - height;
	if (value < min) value = min;
	if (value > max) value = max;
	settingsMinesCount = value;
}

void GameContext::SetTimeLimit(int value)
{
	if (value > 900) value = 900;
	if (value != 0 && value < 5) value = 5;
	settingsTimeLimit = value;
}

int GameContext::GetWidth()
{
	return settingsWidth;
}

int GameContext::GetHeight()
{
	return settingsHeight;
}

int GameContext::GetMinesCount()
{
	return settingsMinesCount;
}

int GameContext::GetTimeLimit()
{
	return settingsTimeLimit;
}

int GameContext::GetActiveHeight()
{
	return height;
}

int GameContext::GetActiveWidth()
{
	return width;
}

int GameContext::GetActiveTimeLimit()
{
	return timeLimit;
}

int GameContext::GetActiveMinesCount()
{
	return minesCount;
}

int GameContext::GetTimeLeft()
{
	return timeLeft;
}

GameState GameContext::GetState()
{
	return state;
}

int GameContext::MinesAround(int row, int col)
{
	if (OutOfBounds(row, col))
		throw std::out_of_range("The specified cell is out of bounds.");

	int count = 0;
	for (int offsetRow = -1; offsetRow <= 1; offsetRow++)
		for (int offsetCol = -1; offsetCol <= 1; offsetCol++)
			if (!OutOfBounds(row + offsetRow, col + offsetCol))
				if (mines[row + offsetRow][col + offsetCol])
					count++;

	return count;
}

void GameContext::TimeStep()
{
	if (timeLimit > 0 && timeLeft > 0)
		timeLeft--;
}

void GameContext::Reveal(int row, int col)
{
	if (OutOfBounds(row, col))
		throw std::out_of_range("The specified cell is out of bounds.");

	if (state != Ready || state != Active)
		return;

	if (revealed[row][col]) return;
	if (flagged[row][col]) return;

	revealed[row][col] = true;

	if (mines[row][col]) {
		state = Failure;
		return;
	}

	if (state == Ready)
		PlaceMines();

	if (MinesAround(row, col) == 0) {
		for (int offsetRow = -1; offsetRow <= 1; offsetRow++)
			for (int offsetCol = -1; offsetCol <= 1; offsetCol++)
				if (!OutOfBounds(row + offsetRow, col + offsetCol))
					Reveal(row + offsetRow, col + offsetCol);
	}
}

void GameContext::Reset()
{
	DeleteField();

	width = settingsWidth;
	height = settingsHeight;
	minesCount = settingsMinesCount;
	timeLimit = settingsTimeLimit;

	timeLeft = timeLimit;

	CreateField();

	state = Ready;
}
