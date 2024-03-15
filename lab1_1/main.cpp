#include <windows.h>
#include "framework.h"
#include "Resource.h"
#include <wingdi.h>
#include <string>
#include <chrono>
#include <iostream>
#include "board.h"

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

const LPCWSTR g_szClassName = L"myWindowClass";

std::chrono::steady_clock::time_point startTime;
std::chrono::steady_clock::duration elapsedTime;
bool timerRunning = false;

#define MAX_PATH_LENGTH 260
wchar_t iniFilePath[MAX_PATH_LENGTH];

bool gameOver = false;
int winnerBoard = 1;
board board1 = board(10);
board board2 = board(10);

/*

    PROCEED WITH BIG MATHS

*/
int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int windowWidth = 600;
int windowHeight = 250;
int startPosX = (screenWidth - windowWidth) / 2;
int startPosY = (screenHeight - windowHeight) * 3 / 4;
const int cellSize = 30;
int gridSize = 10;
int currentGridSize = gridSize;
const int cellMargin = 3;
const int windowMargin = 5;

int boardWidth = windowMargin * 2 + gridSize * cellSize + (gridSize - 1) * cellMargin;
int boardHeight = windowMargin * 2 + gridSize * cellSize + (gridSize - 1) * cellMargin;

void computeBoardSize(int gridSize, int cellSize, int cellMargin, int windowMargin, int &boardWidth, int &boardHeight)
{
    boardWidth = windowMargin * 2 + currentGridSize * cellSize + (currentGridSize - 1) * cellMargin;
    boardHeight = windowMargin * 2 + currentGridSize * cellSize + (currentGridSize - 1) * cellMargin;
}

/*

    BIG MATHS END

*/

COLORREF bgColor = RGB(164, 174, 196);
HWND hwndChild1, hwndChild2;
HWND mainHwnd;

void ChangeGridSize(HWND hwndMain, int gridSize)
{
    currentGridSize = gridSize;
    board1.change_size(gridSize);
    board1.place_ships();
    board2.change_size(gridSize);
    board2.place_ships();

    RECT rect;
    GetClientRect(hwndMain, &rect);
    computeBoardSize(gridSize, cellSize, cellMargin, windowMargin, boardWidth, boardHeight);

    RECT dummy;
    dummy.left = 0;
    dummy.top = 0;
    dummy.right = boardWidth;
    dummy.bottom = boardHeight;

    AdjustWindowRectEx(&dummy, WS_OVERLAPPED | WS_CAPTION, FALSE, 0);

    int totalWidth = dummy.right - dummy.left;
    int totalHeight = dummy.bottom - dummy.top;

    int newWidth = totalWidth;
    int newHeight = totalHeight;
    MoveWindow(hwndChild1, screenWidth - newWidth, 100, newWidth, newHeight, TRUE);
    MoveWindow(hwndChild2, 10, 100, newWidth, newHeight, TRUE);

    InvalidateRect(hwndChild1, NULL, TRUE);
    InvalidateRect(hwndChild2, NULL, TRUE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_EASY:
            ChangeGridSize(hwnd, 10);
            OutputDebugString(L"Easy set\n");
            WritePrivateProfileString(L"Settings",   // section name
                                      L"Difficulty", // key name
                                      L"Easy",       // value to write
                                      iniFilePath);  // INI file name
            break;
        case ID_MEDIUM:
            ChangeGridSize(hwnd, 15);
            OutputDebugString(L"Medium set\n");
            WritePrivateProfileString(L"Settings",
                                      L"Difficulty",
                                      L"Medium",
                                      iniFilePath);
            break;
        case ID_HARD:
            ChangeGridSize(hwnd, 20);
            OutputDebugString(L"Hard set\n");
            WritePrivateProfileString(L"Settings",
                                      L"Difficulty",
                                      L"Hard",
                                      iniFilePath);
            break;

        case ID_GAME_OVER:
            OutputDebugString(L"Game Over Recv\n");
            gameOver = true;
            InvalidateRect(hwndChild1, NULL, TRUE);
            InvalidateRect(hwndChild2, NULL, TRUE);
            break;

        case ID_GAME_CONT:
            OutputDebugString(L"Game Continue Recv\n");
            gameOver = false;
            InvalidateRect(hwndChild1, NULL, TRUE);
            InvalidateRect(hwndChild2, NULL, TRUE);
            break;
        }
        break;
    case WM_TIMER:
        switch (wParam)
        {
        case ID_TIMER:
            if (timerRunning)
            {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsedTime = currentTime - startTime;
                auto mseconds = std::chrono::duration_cast<std::chrono::duration<float>>(elapsedTime).count();
                auto timeString = L"BATTLESHIPS - STATISTICS - Time Elapsed: " + std::to_wstring(mseconds) + L" seconds";
                SetWindowText(hwnd, timeString.c_str());
            }
            break;
        }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        ship **player_ships = board1.ships;
        ship **pc_ships = board2.ships;
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < player_ships[i]->size; j++)
            {
                RECT rect = {
                    windowMargin + j * (cellSize + cellMargin),
                    windowMargin + i * (cellSize + cellMargin) / 2,
                    windowMargin + (j + 1) * cellSize + j * cellMargin,
                    windowMargin + (i + 1) * cellSize + i * cellMargin / 2};

                COLORREF color = j >= player_ships[i]->hits ? RGB(0, 0, 255) : RGB(255, 0, 0);

                HBRUSH hBrush = CreateSolidBrush(color);
                HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
                RoundRect(hdc,
                          rect.left,
                          rect.top,
                          rect.right,
                          rect.bottom,
                          10,
                          10);
                SelectObject(hdc, oldBrush);
                DeleteObject(hBrush);
            }
        }
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < pc_ships[i]->size; j++)
            {
                RECT rect = {
                    windowMargin + j * (cellSize + cellMargin) + 300,
                    windowMargin + i * (cellSize + cellMargin) / 2,
                    windowMargin + (j + 1) * cellSize + j * cellMargin + 300,
                    windowMargin + (i + 1) * cellSize + i * cellMargin / 2};

                COLORREF color = j >= pc_ships[i]->hits ? RGB(0, 0, 255) : RGB(255, 0, 0);

                HBRUSH hBrush = CreateSolidBrush(color);
                HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
                RoundRect(hdc,
                          rect.left,
                          rect.top,
                          rect.right,
                          rect.bottom,
                          10,
                          10);
                SelectObject(hdc, oldBrush);
                DeleteObject(hBrush);
            }
        }
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK BoardWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);

        int row = (y - windowMargin) / (cellSize + cellMargin);
        int col = (x - windowMargin) / (cellSize + cellMargin);

        if (row >= 0 && row < currentGridSize && col >= 0 && col < currentGridSize)
        {
            board &currentBoard = hwnd == hwndChild1 ? board1 : board2;
            bool curren = hwnd == hwndChild1;
            if (curren)
                break;
            if (currentBoard.b_fields[row][col].type == field_state::empty)
            {
                currentBoard.b_fields[row][col].type = field_state::miss;
            }
            else if (currentBoard.b_fields[row][col].type == field_state::ship)
            {
                currentBoard.b_fields[row][col].s->hit();
                currentBoard.b_fields[row][col].type = field_state::hit;
                auto str = L"Ship hit: " + std::to_wstring(currentBoard.b_fields[row][col].s->hits) + L"/" + std::to_wstring(currentBoard.b_fields[row][col].s->size) + L"\n";
                OutputDebugString(str.c_str());
                if (currentBoard.b_fields[row][col].s->is_sunk())
                {
                    OutputDebugString(L"Ship sunk\n");
                    currentBoard.set_neighbours(currentBoard.b_fields[row][col].s);
                }
            }
            if (currentBoard.check_win())
            {
                gameOver = true;
                winnerBoard = hwnd == hwndChild1 ? 2 : 1;
            }
        }

        int pc_x = rand() % currentGridSize;
        int pc_y = rand() % currentGridSize;

        board &pcBoard = hwnd == hwndChild1 ? board2 : board1;

        if (pcBoard.b_fields[pc_x][pc_y].type == field_state::empty)
        {
            pcBoard.b_fields[pc_x][pc_y].type = field_state::miss;
        }
        else if (pcBoard.b_fields[pc_x][pc_y].type == field_state::ship)
        {
            pcBoard.b_fields[pc_x][pc_y].s->hit();
            pcBoard.b_fields[pc_x][pc_y].type = field_state::hit;
            if (pcBoard.b_fields[pc_x][pc_y].s->is_sunk())
            {
                pcBoard.set_neighbours(pcBoard.b_fields[pc_x][pc_y].s);
            }
        }
        if (pcBoard.check_win())
        {
            gameOver = true;
            winnerBoard = hwnd == hwndChild1 ? 1 : 2;
        }

        InvalidateRect(hwnd, NULL, FALSE);
        InvalidateRect(hwnd == hwndChild1 ? hwndChild2 : hwndChild1, NULL, FALSE);
        InvalidateRect(mainHwnd, NULL, FALSE);
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        board currentBoard = hwnd == hwndChild1 ? board1 : board2;
        bool isPlayerBoard = hwnd == hwndChild1;

        for (int row = 0; row < currentGridSize; ++row)
        {
            for (int col = 0; col < currentGridSize; ++col)
            {
                RECT rect = {
                    windowMargin + col * (cellSize + cellMargin),
                    windowMargin + row * (cellSize + cellMargin),
                    windowMargin + (col + 1) * cellSize + col * cellMargin,
                    windowMargin + (row + 1) * cellSize + row * cellMargin};
                HBRUSH hBrush = CreateSolidBrush(bgColor);
                HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
                RoundRect(hdc,         // Handle to the device context
                          rect.left,   // x-coordinate of the upper-left corner of the rectangle
                          rect.top,    // y-coordinate of the upper-left corner of the rectangle
                          rect.right,  // x-coordinate of the lower-right corner of the rectangle
                          rect.bottom, // y-coordinate of the lower-right corner of the rectangle
                          10,          // Width of the ellipse used to create the rounded corners
                          10);         // Height of the ellipse used to create the rounded corners
                SelectObject(hdc, oldBrush);
                DeleteObject(hBrush);

                if (currentBoard.b_fields[row][col].type == field_state::ship and isPlayerBoard)
                {
                    SetTextColor(hdc, RGB(0, 0, 0));
                    SetBkMode(hdc, TRANSPARENT);
                    HFONT hFont = CreateFont(-MulDiv(10, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_NORMAL,
                                             FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
                    HGDIOBJ oldFont = SelectObject(hdc, hFont);

                    ship_type shipType = currentBoard.b_fields[row][col].get_ship_type();

                    WCHAR cellText[10];
                    wsprintf(cellText, L"%d", shipType);

                    DrawText(hdc, cellText, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                    SelectObject(hdc, oldFont);
                    DeleteObject(hFont);
                }
                else if (currentBoard.b_fields[row][col].type == field_state::miss)
                {
                    HBRUSH hBrush = CreateSolidBrush(RGB(0, 160, 255));
                    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
                    RoundRect(hdc,
                              rect.left,
                              rect.top,
                              rect.right,
                              rect.bottom,
                              10,
                              10);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(hBrush);

                    SetTextColor(hdc, RGB(0, 0, 0));
                    SetBkMode(hdc, TRANSPARENT);
                    HFONT hFont = CreateFont(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD,
                                             FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
                    HGDIOBJ oldFont = SelectObject(hdc, hFont);

                    DrawText(hdc, L".", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                    SelectObject(hdc, oldFont);
                    DeleteObject(hFont);
                }
                else if (currentBoard.b_fields[row][col].type == field_state::hit)
                {
                    HBRUSH hBrush = CreateSolidBrush(RGB(255, 5, 0));
                    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
                    RoundRect(hdc,
                              rect.left,
                              rect.top,
                              rect.right,
                              rect.bottom,
                              10,
                              10);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(hBrush);

                    SetTextColor(hdc, RGB(0, 0, 0));
                    SetBkMode(hdc, TRANSPARENT);
                    HFONT hFont = CreateFont(-MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD,
                                             FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));
                    HGDIOBJ oldFont = SelectObject(hdc, hFont);

                    DrawText(hdc, L"X", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                    SelectObject(hdc, oldFont);
                    DeleteObject(hFont);
                }
                else if (currentBoard.b_fields[row][col].type == field_state::neutral)
                {
                    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 0));
                    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
                    RoundRect(hdc,
                              rect.left,
                              rect.top,
                              rect.right,
                              rect.bottom,
                              10,
                              10);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(hBrush);
                }
                else if (currentBoard.b_fields[row][col].type == field_state::testing)
                {
                    HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 255));
                    HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
                    RoundRect(hdc,
                        rect.left,
                        rect.top,
                        rect.right,
                        rect.bottom,
                        10,
                        10);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(hBrush);
                }
            }
        }

        if (gameOver)
        {
            bool winner = false;
            if (hwnd == hwndChild1 and winnerBoard == 1)
                winner = true;
            if (hwnd == hwndChild2 and winnerBoard == 2)
                winner = true;
            OutputDebugString(L"Game Over\n");
            RECT rect;
            GetClientRect(hwnd, &rect);

            COLORREF overlayColor = winner ? RGB(0, 255, 0) : RGB(255, 0, 0);
            LPCWSTR overlayText = winner ? L"You Win!" : L"Game Over";

            HBRUSH hBrush = CreateSolidBrush(overlayColor);
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

            BLENDFUNCTION bf;
            bf.BlendOp = AC_SRC_OVER;
            bf.BlendFlags = 0;
            bf.SourceConstantAlpha = 128;
            bf.AlphaFormat = 0;

            FillRect(hdcMem, &rect, hBrush);

            AlphaBlend(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, rect.right, rect.bottom, bf);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));

            int nHeight = -MulDiv(24, GetDeviceCaps(hdc, LOGPIXELSY), 72); // Converts point size to logical units based on DPI
            HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                     OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                                     DEFAULT_PITCH | FF_SWISS, L"Arial");

            HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

            DrawText(hdc, overlayText, -1, &rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            SelectObject(hdcMem, hbmOld);
            DeleteObject(hBitmap);
            DeleteDC(hdcMem);
            DeleteObject(hBrush);
        }

        EndPaint(hwnd, &ps);
        break;

        EndPaint(hwnd, &ps);
    }
    break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                   _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = static_cast<HICON>(LoadImageW(
        hInstance,
        MAKEINTRESOURCEW(IDI_BS),
        IMAGE_ICON,
        0, 0,
        LR_SHARED | LR_DEFAULTSIZE));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = static_cast<HICON>(LoadImageW(
        hInstance,                    // Module handle
        MAKEINTRESOURCEW(IDI_BS),     // Resource to load
        IMAGE_ICON,                   // Image type
        0, 0,                         // Size
        LR_SHARED | LR_DEFAULTSIZE)); // Load flags
    if (!RegisterClassEx(&wc))        // If the registration fails, you kinda done goofed
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    WNDCLASSEX wcBoard;

    wcBoard.cbSize = sizeof(WNDCLASSEX);
    wcBoard.style = 0;
    wcBoard.lpfnWndProc = BoardWndProc;
    wcBoard.cbClsExtra = 0;
    wcBoard.cbWndExtra = 0;
    wcBoard.hInstance = hInstance;
    wcBoard.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcBoard.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcBoard.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcBoard.lpszMenuName = NULL;
    wcBoard.lpszClassName = L"BoardWindowClass";
    wcBoard.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcBoard))
    {
        MessageBox(NULL, L"Board Window Registration Failed!", L"Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    GetCurrentDirectoryW(MAX_PATH_LENGTH, iniFilePath);
    wcscat_s(iniFilePath, MAX_PATH_LENGTH, L"\\game_settings.ini");
    OutputDebugString(iniFilePath);

    wchar_t difficulty[10];
    GetPrivateProfileString(L"Settings",
                            L"Difficulty",
                            L"Medium",
                            difficulty,
                            sizeof(difficulty) / sizeof(wchar_t),
                            iniFilePath);

    if (wcscmp(difficulty, L"Easy") == 0)
    {
        OutputDebugString(L"Read Easy\n");
        currentGridSize = 10;
    }
    else if (wcscmp(difficulty, L"Medium") == 0)
    {
        OutputDebugString(L"Read Medium\n");
        currentGridSize = 15;
    }
    else if (wcscmp(difficulty, L"Hard") == 0)
    {
        OutputDebugString(L"Read Hard\n");
        currentGridSize = 20;
    }

    board1.change_size(currentGridSize);
    board1.place_ships();
    board2.change_size(currentGridSize);
    board2.place_ships();

    computeBoardSize(currentGridSize, cellSize, cellMargin, windowMargin, boardWidth, boardHeight);
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = boardWidth;
    rect.bottom = boardHeight;

    AdjustWindowRectEx(&rect, WS_OVERLAPPED | WS_CAPTION, FALSE, 0);

    int totalWidth = rect.right - rect.left;
    int totalHeight = rect.bottom - rect.top;

    hwnd = CreateWindowEx(
        WS_EX_LAYERED,
        g_szClassName,
        L"BATTLESHIPS - STATISTICS",
        WS_OVERLAPPEDWINDOW,
        startPosX, startPosY, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL);

    mainHwnd = hwnd;

    HWND hwndPlayerBoard = CreateWindowEx(
        0,                       // No extended styles
        L"BoardWindowClass",     // Class name
        L"BATTLESHIPS - MY",     // Window title
        WS_OVERLAPPED,           // Style
        10, 100,                 // Position x, y
        totalWidth, totalHeight, // Width, height
        hwnd,                    // Parent window
        NULL,                    // No menus
        hInstance,               // Instance handle
        NULL);                   // No additional parameters

    if (!hwndPlayerBoard)
    {
        MessageBox(NULL, L"Failed to create player board window", L"Error", MB_OK);
        return 0;
    }

    HWND hwndPCBoard = CreateWindowEx(
        0,                             // No extended styles
        L"BoardWindowClass",           // Class name
        L"BATTLESHIPS - PC",           // Window title
        WS_OVERLAPPED,                 // Style
        screenWidth - totalWidth, 100, // Position x, y
        totalWidth, totalHeight,       // Width, height
        hwnd,                          // Parent window
        NULL,                          // No menus
        hInstance,                     // Instance handle
        NULL);                         // No additional parameters

    if (!hwndPCBoard)
    {
        MessageBox(NULL, L"Failed to create player board window", L"Error", MB_OK);
        return 0;
    }

    hwndChild1 = hwndPlayerBoard;
    hwndChild2 = hwndPCBoard;

    SetLayeredWindowAttributes(hwnd, bgColor, (255 * 30) / 100, LWA_COLORKEY | LWA_ALPHA);

    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);

    if (hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    HMENU hMenu = CreateMenu();
    HMENU hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, ID_EASY, L"Easy");
    AppendMenu(hSubMenu, MF_STRING, ID_MEDIUM, L"Medium");
    AppendMenu(hSubMenu, MF_STRING, ID_HARD, L"Hard");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"Grid Size");
    AppendMenu(hMenu, MF_STRING, ID_GAME_OVER, L"Game Over");
    AppendMenu(hMenu, MF_STRING, ID_GAME_CONT, L"Continue");
    SetMenu(hwnd, hMenu);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    ShowWindow(hwndPlayerBoard, SW_SHOW);
    UpdateWindow(hwndPlayerBoard);

    ShowWindow(hwndPCBoard, SW_SHOW);
    UpdateWindow(hwndPCBoard);

    SetTimer(hwnd, ID_TIMER, 1, NULL);
    startTime = std::chrono::steady_clock::now();
    timerRunning = true;

    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
