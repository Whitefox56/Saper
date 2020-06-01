#pragma once
#include "framework.h"
#include "GameContext.h"

// Game GUI
// Provides display of the playing field and interaction with it
class GameUI
{
	// cell size in pixels
	const static int CELL_SIZE;
	// Game context
	GameContext* gameContext;
public:
	// Creates and initializes a new interface exam
	GameUI(GameContext* gameContext);

	// Displays the playing field in the specified graphics context.
	void Draw(HDC hdc);

	// Handles clicking on the left mouse button
	void ClickLeft(int x, int y);
	// Handles right mouse click
	void ClickRight(int x, int y);

	// Returns the required width of the graphics context
	int GetWidth();
	// Returns the required height of the graphics context
	int GetHeight();
};

