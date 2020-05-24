// main.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "resource.h"
#include "GameContext.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

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

    // TODO: Place code here.

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINES));
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

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDM_SETTINGS:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, Settings);
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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

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

			GameContext* gameContext = GameContext::GetInstance();
			int height = gameContext->GetActiveHeight();
			int width = gameContext->GetActiveWidth();

			HBRUSH emptyBrush = CreateSolidBrush(RGB(128, 128, 255));
			HBRUSH revealedBrush = CreateSolidBrush(RGB(255, 255, 255));
			for (int row = 0; row < height; row++) {
				for (int col = 0; col < width; col++) {
					int left = col * 25;
					int top = row * 25;
					int right = col * 25 + 25;
					int bottom = row * 25 + 25;
					if (gameContext->IsRevealed(row, col))
						SelectBrush(memDC, revealedBrush);
					else
						SelectBrush(memDC, emptyBrush);

					Rectangle(memDC, left, top, right, bottom);
				}
			}
			DeleteObject(emptyBrush);
			DeleteObject(revealedBrush);

			// OK, everything is drawn into memory DC, 
			// now is the time to draw that final result into our target DC

			BitBlt(hdc, 0, 0, rcClientRect.right - rcClientRect.left,
				rcClientRect.bottom - rcClientRect.top, memDC, 0, 0, SRCCOPY);

			// all done, now we need to cleanup
			SelectObject(memDC, oldBmp); // select back original bitmap
			DeleteObject(bmp); // delete bitmap since it is no longer required
			DeleteDC(memDC);   // delete memory DC since it is no longer required
			EndPaint(hWnd, &ps); //завершение рисования
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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

INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		wchar_t str[4];

		Edit_SetText(GetDlgItem(hDlg, IDC_HEIGHT_EDIT), L"10");
		Edit_LimitText(GetDlgItem(hDlg, IDC_HEIGHT_EDIT), 2);

		Edit_SetText(GetDlgItem(hDlg, IDC_WIDTH_EDIT), L"10");
		Edit_LimitText(GetDlgItem(hDlg, IDC_WIDTH_EDIT), 2);

		Edit_SetText(GetDlgItem(hDlg, IDC_MINES_EDIT), L"50");
		Edit_LimitText(GetDlgItem(hDlg, IDC_MINES_EDIT), 3);

		_itow_s(120, str, 10);
		Edit_SetText(GetDlgItem(hDlg, IDC_TIME_EDIT), str);
		Edit_LimitText(GetDlgItem(hDlg, IDC_TIME_EDIT), 3);

		Button_SetCheck(GetDlgItem(hDlg, IDC_TIME_CHECK), BST_CHECKED);

		return (INT_PTR)TRUE;
	}
	break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDC_TIME_CHECK) {
			auto checked = Button_GetCheck(GetDlgItem(hDlg, IDC_TIME_CHECK)) == BST_CHECKED;
			Edit_Enable(GetDlgItem(hDlg, IDC_TIME_EDIT), checked);
		}
		break;
	}
	return (INT_PTR)FALSE;
}