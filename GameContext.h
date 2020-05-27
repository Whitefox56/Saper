#pragma once
#include <stdexcept>

// Состояние игры
enum GameState {
	// Начало игры
	Ready,
	// Игра идёт
	Active,
	// Игра завершена успешно
	Complete,
	// Игра завершена неудачно
	Failure
};

// Игровой контекст.
// Управляет игровыми процессами.
class GameContext
{
	// Настройки игры
	int settingsWidth;
	int settingsHeight;
	int settingsMinesCount;
	int settingsTimeLimit;

	// ширина поля в клетках
	int width;
	// высота поля в клетках
	int height;
	// количество мин
	int minesCount;
	// ограничение времени в секундах
	int timeLimit;
	// остаток времени в миллисекундах
	int timeLeft;
	// количество открытых клеток
	int revealedCount;

	// состояние игры
	GameState state;

	// мины
	bool** mines;
	// флаги
	bool** flagged;
	// открытые клетки
	bool** revealed;

	// создаёт и инициализирует новый игровой контекст
	GameContext();
	// освобождает занятые ресурсы
	~GameContext();
	// единственный экземпляр класса
	static GameContext* instance;
	
	// удаляет массивы со свойствами клеток
	void DeleteField();
	// создаёт массивы со свойствами клеток
	void CreateField();
	// размещает мины на поле
	void PlaceMines();

public:
	// возвращает единственный экземпляр игрового контекста
	static GameContext* GetInstance();

	// возвращает true, если указанная клетка за пределами поля
	bool OutOfBounds(int row, int col);

	// возвращает true, если клетка открыта
	bool IsRevealed(int row, int col);
	// возвращает true, если на клетке поставлен флажок
	bool IsFlagged(int row, int col);
	// возвращает true, если на клетке находится мина
	bool HasMine(int row, int col);

	// настраивает ширину поля
	void SetWidth(int value);
	// настраивает высоту поля
	void SetHeight(int value);
	// настраивает количество мин
	void SetMinesCount(int value);
	// настраивает ограничение времени в секундах
	void SetTimeLimit(int value);

	// возвращает настроенную ширину поля
	int GetWidth();
	// возвращает настроенную высоту поля
	int GetHeight();
	// возвращает настроенное количество мин
	int GetMinesCount();
	// возвращает настроенное ограничение времени в секундах
	int GetTimeLimit();

	// возвращает действующую ширину поля
	int GetActiveWidth();
	// возвращает действующую высоту поля
	int GetActiveHeight();
	// возвращает действующее количество мин
	int GetActiveMinesCount();
	// возвращает действующее ограничение времени в секундах
	int GetActiveTimeLimit();
	// возвращает оставшееся время в миллисекундах
	int GetTimeLeft();
	// возвращает состояние игры
	GameState GetState();

	// возвращает количество мин в окружении клетки
	int MinesAround(int row, int col);

	// учитывает прошедшее время в миллисекундах
	void TimeStep(unsigned int ms);

	// открывает клетку
	void Reveal(int row, int col);
	// ставит или убирает флажок
	void ToggleFlagged(int row, int col);

	// иницализирует генератор случайных чисел
	void SeedRandom(unsigned int seed);

	// начинает новую игру
	void Reset();
};

