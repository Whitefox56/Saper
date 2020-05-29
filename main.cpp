// main.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "resource.h"
#include "GameContext.h"
#include "GameUI.h"


#define IDT_STEP_TIMER 1

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

GameUI* gameUI;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Settings(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MINES, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINES));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MINES);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void SetClientSize(HWND hwnd, int clientWidth, int clientHeight)
{
	if (IsWindow(hwnd))
	{
		RECT newClientRC;
		GetClientRect(hwnd, &newClientRC);
		if ((newClientRC.right - newClientRC.left) == clientWidth &&
			(newClientRC.bottom - newClientRC.top) == clientHeight)
			return;

		DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
		DWORD dwExStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
		HMENU menu = GetMenu(hwnd);

		RECT rc = { 0, 0, clientWidth, clientHeight };

		AdjustWindowRectEx(&rc, dwStyle, menu ? TRUE : FALSE, dwExStyle);

		SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
			SWP_NOZORDER | SWP_NOMOVE);
	}
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		gameUI = new GameUI(GameContext::GetInstance());
		GameContext::GetInstance()->Reset();
		GameContext::GetInstance()->SeedRandom(time(0));
		SetTimer(hWnd, IDT_STEP_TIMER, 200, NULL);
		break;
	case WM_TIMER:
	{
		switch (wParam)
		{
		case IDT_STEP_TIMER:
			// таймер сообщает игровому контексту о прошедшем времени
			GameContext::GetInstance()->TimeStep(200);
			InvalidateRect(hWnd, NULL, false);
			return 0;
		}
	}
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
			case ID_NEWGAME:
				// нова€ игра
				GameContext::GetInstance()->Reset();
				InvalidateRect(hWnd, NULL, false);
				break;
			case IDM_SETTINGS:
				// настройки
				DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, Settings);
				InvalidateRect(hWnd, NULL, false);
				break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			// настраиваетс€ размер окна
			SetClientSize(hWnd, gameUI->GetWidth(), gameUI->GetHeight());

            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

			// double-buffered
			HDC memDC = CreateCompatibleDC(hdc);

			// get window's client rectangle. We need this for bitmap creation.
			RECT rcClientRect;
			GetClientRect(hWnd, &rcClientRect);

			// now we can create bitmap where we shall do our drawing
			HBITMAP bmp = CreateCompatibleBitmap(hdc,
				rcClientRect.right - rcClientRect.left,
				rcClientRect.bottom - rcClientRect.top);

			// we need to save original bitmap, and select it back when we are done,
			// in order to avoid GDI leaks!
			HBITMAP oldBmp = (HBITMAP) SelectObject(memDC, bmp);

			gameUI->Draw(memDC);

			// OK, everything is drawn into memory DC, 
			// now is the time to draw that final result into our target DC

			BitBlt(hdc, 0, 0, rcClientRect.right - rcClientRect.left,
				rcClientRect.bottom - rcClientRect.top, memDC, 0, 0, SRCCOPY);

			// all done, now we need to cleanup
			SelectObject(memDC, oldBmp); // select back original bitmap
			DeleteObject(bmp); // delete bitmap since it is no longer required
			DeleteDC(memDC);   // delete memory DC since it is no longer required
			EndPaint(hWnd, &ps); //завершение рисовани€
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
	case WM_LBUTTONDOWN:
	{
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);
		gameUI->ClickLeft(mouseX, mouseY);
		InvalidateRect(hWnd, NULL, false);
	}
	break;
	case WM_RBUTTONDOWN:
	{
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);
		gameUI->ClickRight(mouseX, mouseY);
		InvalidateRect(hWnd, NULL, false);
	}
	break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// считывает с полей параметры игры
void ReadSettings(HWND hDlg, int& width, int& height, int& minesCount, int& timeLimit) {
	wchar_t str[20];
	Edit_GetText(GetDlgItem(hDlg, IDC_HEIGHT_EDIT), str, 20);
	height = _wtoi(str);
	Edit_GetText(GetDlgItem(hDlg, IDC_WIDTH_EDIT), str, 20);
	width = _wtoi(str);
	Edit_GetText(GetDlgItem(hDlg, IDC_MINES_EDIT), str, 20);
	minesCount = _wtoi(str);
	auto checked = Button_GetCheck(GetDlgItem(hDlg, IDC_TIME_CHECK)) == BST_CHECKED;
	if (checked) {
		Edit_GetText(GetDlgItem(hDlg, IDC_TIME_EDIT), str, 20);
		timeLimit = _wtoi(str);
	}
	else {
		timeLimit = 0;
	}
}

// выписывает в пол€ параметры игры
void WriteSettings(HWND hDlg, int width, int height, int minesCount, int timeLimit) {
	wchar_t str[20];
	_itow_s(height, str, 10);
	Edit_SetText(GetDlgItem(hDlg, IDC_HEIGHT_EDIT), str);

	_itow_s(width, str, 10);
	Edit_SetText(GetDlgItem(hDlg, IDC_WIDTH_EDIT), str);

	_itow_s(minesCount, str, 10);
	Edit_SetText(GetDlgItem(hDlg, IDC_MINES_EDIT), str);

	if (timeLimit > 0) {
		_itow_s(timeLimit, str, 10);
		Edit_SetText(GetDlgItem(hDlg, IDC_TIME_EDIT), str);
		Button_SetCheck(GetDlgItem(hDlg, IDC_TIME_CHECK), BST_CHECKED);
		Edit_Enable(GetDlgItem(hDlg, IDC_TIME_EDIT), TRUE);
	}
	else {
		Edit_SetText(GetDlgItem(hDlg, IDC_TIME_EDIT), L"");
		Button_SetCheck(GetDlgItem(hDlg, IDC_TIME_CHECK), BST_UNCHECKED);
		Edit_Enable(GetDlgItem(hDlg, IDC_TIME_EDIT), FALSE);
	}
}

// ‘ункци€ обработки событий в окне настроек
INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		// инициализцаци€ окна
		// в пол€ ввод€тс€ текущие параметры игры
		GameContext* gameContext = GameContext::GetInstance();

		Edit_LimitText(GetDlgItem(hDlg, IDC_HEIGHT_EDIT), 4);
		Edit_LimitText(GetDlgItem(hDlg, IDC_WIDTH_EDIT), 4);
		Edit_LimitText(GetDlgItem(hDlg, IDC_MINES_EDIT), 4);
		Edit_LimitText(GetDlgItem(hDlg, IDC_TIME_EDIT), 4);

		WriteSettings(hDlg, gameContext->GetWidth(), gameContext->GetHeight(),
			gameContext->GetMinesCount(), gameContext->GetTimeLimit());

		return (INT_PTR)TRUE;
	}
	break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			// при нажатии на ќ  введЄнные параметры передаютс€ игровому контексту
			if (LOWORD(wParam) == IDOK) {
				int height;
				int width;
				int minesCount;
				int timeLimit;
				ReadSettings(hDlg, width, height, minesCount, timeLimit);
				GameContext* gameContext = GameContext::GetInstance();
				gameContext->SetHeight(height);
				gameContext->SetWidth(width);
				gameContext->SetMinesCount(minesCount);
				gameContext->SetTimeLimit(timeLimit);
				gameContext->Reset();
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_TIME_CHECK) {
			auto checked = Button_GetCheck(GetDlgItem(hDlg, IDC_TIME_CHECK)) == BST_CHECKED;
			Edit_Enable(GetDlgItem(hDlg, IDC_TIME_EDIT), checked);
		}
		else if (HIWORD(wParam) == EN_KILLFOCUS) {
			if (LOWORD(wParam) == IDC_HEIGHT_EDIT ||
				LOWORD(wParam) == IDC_WIDTH_EDIT ||
				LOWORD(wParam) == IDC_MINES_EDIT ||
				LOWORD(wParam) == IDC_TIME_EDIT) {
				GameContext* gameContext = GameContext::GetInstance();
				// сохран€ет текущие параметры
				int currentHeight = gameContext->GetHeight();
				int currentWidth = gameContext->GetWidth();
				int currentMinesCount = gameContext->GetMinesCount();
				int currentTimeLimit = gameContext->GetTimeLimit();

				int height;
				int width;
				int minesCount;
				int timeLimit;

				// считывает введЄнные параметры
				ReadSettings(hDlg, width, height, minesCount, timeLimit);

				// примен€ет новые параметры игры
				gameContext->SetHeight(height);
				gameContext->SetWidth(width);
				gameContext->SetMinesCount(minesCount);
				gameContext->SetTimeLimit(timeLimit);

				// игровой контекст приводит параметры к допустимым
				height = gameContext->GetHeight();
				width = gameContext->GetWidth();
				minesCount = gameContext->GetMinesCount();
				timeLimit = gameContext->GetTimeLimit();

				// вписывает допустимые параметры в пол€
				WriteSettings(hDlg, width, height, minesCount, timeLimit);

				// восстанавливает текущие параметры
				gameContext->SetHeight(currentHeight);
				gameContext->SetWidth(currentWidth);
				gameContext->SetMinesCount(currentMinesCount);
				gameContext->SetTimeLimit(currentTimeLimit);
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}