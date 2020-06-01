#include "GameContext.h"

GameContext* GameContext::instance = nullptr;

GameContext::GameContext()
{
	settingsWidth = 10;
	settingsHeight = 10;
	settingsMinesCount = 12;
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
	// loop repeats until all mines are placed
	int placedCount = 0;
	while (placedCount < minesCount) {
		int row = rand() % height;
		int col = rand() % width;

		if (!revealed[row][col] && !mines[row][col]) {
			mines[row][col] = true;
			placedCount++;
		}
	}

	// after placement of mines the countdown begins
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
	int max = settingsWidth * settingsHeight - settingsWidth - settingsHeight;
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

	// counts a mine in neighboring cells
	int count = 0;
	for (int offsetRow = -1; offsetRow <= 1; offsetRow++)
		for (int offsetCol = -1; offsetCol <= 1; offsetCol++)
			if (!OutOfBounds(row + offsetRow, col + offsetCol))
				if (mines[row + offsetRow][col + offsetCol])
					count++;

	return count;
}

void GameContext::TimeStep(unsigned int ms)
{
	// time is reported, if a time limit is set, after placing min
	if (state != Active) return;
	if (timeLimit == 0) return;
	timeLeft -= ms;
	if (timeLeft <= 0) {
		timeLeft = 0;
		state = Failure;
	}
}

void GameContext::Reveal(int row, int col)
{
	if (OutOfBounds(row, col))
		throw std::out_of_range("The specified cell is out of bounds.");

	if (state != Ready && state != Active)
		return;

	if (revealed[row][col]) return;
	if (flagged[row][col]) return;

	revealed[row][col] = true;
	revealedCount++;

	// if there is a mine in an open cage, the game is over
	if (mines[row][col]) {
		state = Failure;
		return;
	}

	// if all cells except mines are open, the game is completed
	if (revealedCount == width * height - minesCount) {
		state = Complete;
		return;
	}

	// after the first move, mines are placed,
	// so the first move is always successful
	if (state == Ready)
		PlaceMines();

	// if there are no mines around the cell, the neighboring cells open automatically
	if (MinesAround(row, col) == 0) {
		for (int offsetRow = -1; offsetRow <= 1; offsetRow++)
			for (int offsetCol = -1; offsetCol <= 1; offsetCol++)
				if (!OutOfBounds(row + offsetRow, col + offsetCol))
					Reveal(row + offsetRow, col + offsetCol);
	}
}

void GameContext::ToggleFlagged(int row, int col)
{
	if (OutOfBounds(row, col))
		throw std::out_of_range("The specified cell is out of bounds.");

	if (state != Ready && state != Active)
		return;

	if (revealed[row][col]) return;

	flagged[row][col] = !flagged[row][col];
}

void GameContext::SeedRandom(unsigned int seed)
{
	srand(seed);
}

void GameContext::Reset()
{
	DeleteField();

	// settings apply
	width = settingsWidth;
	height = settingsHeight;
	minesCount = settingsMinesCount;
	timeLimit = settingsTimeLimit;

	// reset countdown
	timeLeft = timeLimit * 1000;

	CreateField();

	revealedCount = 0;

	state = Ready;
}
