#pragma once
#include <stdlib.h>
#include <stdexcept>

enum GameState {
	Ready,
	Active,
	Complete,
	Failure
};

class GameContext
{
	int settingsWidth;
	int settingsHeight;
	int settingsMinesCount;
	int settingsTimeLimit;

	int width;
	int height;
	int minesCount;
	int timeLimit;
	int timeLeft;

	GameState state;

	bool** mines;
	bool** flagged;
	bool** revealed;

	GameContext();
	~GameContext();
	static GameContext* instance;
	
	void DeleteField();
	void CreateField();
	void PlaceMines();

public:
	static GameContext* GetInstance();

	bool OutOfBounds(int row, int col);

	bool IsRevealed(int row, int col);
	bool IsFlagged(int row, int col);
	bool HasMine(int row, int col);

	void SetWidth(int value);
	void SetHeight(int value);
	void SetMinesCount(int value);
	void SetTimeLimit(int value);

	int GetWidth();
	int GetHeight();
	int GetMinesCount();
	int GetTimeLimit();

	int GetActiveWidth();
	int GetActiveHeight();
	int GetActiveMinesCount();
	int GetActiveTimeLimit();
	int GetTimeLeft();
	GameState GetState();

	int MinesAround(int row, int col);

	void TimeStep();

	void Reveal(int row, int col);

	void Reset();
};

