#pragma once
#include <stdexcept>

// Game state
enum GameState {
	// Beginning of the game
	Ready,
	// The game is on
	Active,
	// Game completed successfully
	Complete,
	// Game failed
	Failure
};

// Game context.
// Controls gameplay.
class GameContext
{
	// Game settings
	int settingsWidth;
	int settingsHeight;
	int settingsMinesCount;
	int settingsTimeLimit;

	// field width in cells
	int width;
	// field height in cells
	int height;
	// number of mines
	int minesCount;
	// time limit in seconds
	int timeLimit;
	// remaining time in milliseconds
	int timeLeft;
	// number of open cells
	int revealedCount;

	// game state
	GameState state;

	// mines
	bool** mines;
	// flags
	bool** flagged;
	// open cells
	bool** revealed;

	// creates and initializes a new game context
	GameContext();
	// frees up occupied resources
	~GameContext();
	// single instance of a class
	static GameContext* instance;
	
	// removes arrays with cell properties
	void DeleteField();
	// creates arrays with cell properties
	void CreateField();
	// places mines on the field
	void PlaceMines();

public:
	// returns a single instance of the game context
	static GameContext* GetInstance();

	// returns true if the specified cell is outside the field
	bool OutOfBounds(int row, int col);

	// returns true if the cell is open
	bool IsRevealed(int row, int col);
	// returns true if the box is checked
	bool IsFlagged(int row, int col);
	// returns true if there is a mine on the cell
	bool HasMine(int row, int col);

	// adjusts the field width
	void SetWidth(int value);
	// adjusts field height
	void SetHeight(int value);
	// sets the number of minutes
	void SetMinesCount(int value);
	// sets the time limit in seconds	
	void SetTimeLimit(int value);

	// returns custom field width
	int GetWidth();
	// returns the adjusted field height
	int GetHeight();
	// returns the configured number of min
	int GetMinesCount();
	// returns the configured time limit in seconds
	int GetTimeLimit();

	// returns the actual field width
	int GetActiveWidth();
	// returns the current field height
	int GetActiveHeight();
	// returns the actual number of min
	int GetActiveMinesCount();
	// returns the current time limit in seconds
	int GetActiveTimeLimit();
	// returns the remaining time in milliseconds
	int GetTimeLeft();
	// returns the state of the game
	GameState GetState();

	// returns the number of mines surrounded by cells
	int MinesAround(int row, int col);

	// counts elapsed time in milliseconds
	void TimeStep(unsigned int ms);

	// opens the cage
	void Reveal(int row, int col);
	// check or uncheck
	void ToggleFlagged(int row, int col);

	// initializes random number generator
	void SeedRandom(unsigned int seed);

	// starts a new game
	void Reset();
};

