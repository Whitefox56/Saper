#pragma once
#include <stdlib.h>

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
	bool minesPlaced;

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

	void Reset();
};

