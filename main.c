#include <windows.h>

#define CM_SAVE 1

LRESULT CALLBACK WindowProcW(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hEdit;
    switch (uMsg)
    {
        case WM_CREATE:
        {
            //add menu
            HMENU hMenu = CreateMenu();
            HMENU hSubMenu = CreatePopupMenu();
            AppendMenuW(hSubMenu, MF_STRING, CM_SAVE, L"&Save (ctr+s)");
            AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"&File");

            SetMenu(hwnd, hMenu);
            //add multiline edit control
            hEdit = CreateWindowExW(
                0, 
                L"EDIT", 
                L"", 
                WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | 
                ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL, 
                0, 0, 0, 0, 
                hwnd, 
                hMenu,
                NULL, 
                NULL);
            
            return 0;
        }

        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
      

        case WM_CTLCOLOREDIT:
        {
            SetBkColor((HDC)wParam, RGB(0, 0, 0));
            SetTextColor((HDC)wParam, RGB(255, 255, 255));
            
            return (INT_PTR)CreateSolidBrush(RGB(0, 0, 0));
        }

        case WM_SIZE:
        {
            MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            return 0;
        }

        case WM_CLOSE:
        {
            return TryQuit(hwnd);
        }

        case WM_COMMAND:
        {
            if (wParam == CM_SAVE)
            {
                MessageBoxW(hwnd, L"Save", L"Save", MB_OK);
            }

            return 0;
        }
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

int TryQuit(HWND hwnd)
{
    if (MessageBoxW(hwnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
    {
        DestroyWindow(hwnd);
        return 0;
    }

    return 1;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    LPCWSTR CLASS_NAME = L"CJ_WIN_CLASS_W";
    LPCWSTR WINDOW_NAME = L"CJ's Win32 App";
    // Register the window class
    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProcW;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClassW(&wc);


    // Create the window
    HWND hwnd = CreateWindowExW(
        0, // Optional window styles
        CLASS_NAME, // Window class
        WINDOW_NAME, // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, //X Y Width Height
        NULL, // Parent window
        NULL, // Menu
        hInstance, // Instance handle
        NULL // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    //add button    

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);

        if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState('S') & 0x8000)
        {
            SendMessage(hwnd, WM_COMMAND, CM_SAVE, 0);
        }

        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
        {
            if (TryQuit(hwnd) == 0)
            {
                break;
            }
        }


        DispatchMessage(&msg);
    }

    return 0;
}