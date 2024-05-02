#include <windows.h>

#define CMD_SAVE 1
#define CMD_OPEN 2

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
            AppendMenuW(hSubMenu, MF_STRING, CMD_SAVE, L"&Save (ctr+s)");
            AppendMenuW(hSubMenu, MF_STRING, CMD_OPEN, L"&Open (ctr+o)");
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
            if (wParam == CMD_SAVE)
            {
                if (MessageBoxW(hwnd, L"Save", L"Save", MB_OKCANCEL) == IDOK)
                {
                    //save file
                    OPENFILENAMEW ofn = { 0 };
                    WCHAR szFile[MAX_PATH] = L"";
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFile = szFile;
                    ofn.lpstrFile[0] = L'\0';
                    ofn.nMaxFile = sizeof(szFile);
                    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
                    ofn.nFilterIndex = 1;
                    ofn.lpstrFileTitle = NULL;
                    ofn.nMaxFileTitle = 0;
                    ofn.lpstrInitialDir = NULL;
                    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;


                    if (GetSaveFileNameW(&ofn))
                    {
                        //save file
                        HANDLE hFile = CreateFileW(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                        if (hFile == INVALID_HANDLE_VALUE)
                        {
                            MessageBoxW(hwnd, L"Error saving file", L"Error", MB_OK);
                        }
                        else
                        {
                            DWORD dwBytesWritten = 0;
                            WCHAR szText[1024] = L"";
                            GetWindowTextW(hEdit, szText, 1024);
                            WriteFile(hFile, szText, wcslen(szText) * sizeof(WCHAR), &dwBytesWritten, NULL);
                            CloseHandle(hFile);
                        }
                    }
                }
            }
            else if (wParam == CMD_OPEN)
            {
                //open file
                OPENFILENAMEW ofn = { 0 };
                WCHAR szFile[MAX_PATH] = L"";
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hwnd;
                ofn.lpstrFile = szFile;
                ofn.lpstrFile[0] = L'\0';
                ofn.nMaxFile = sizeof(szFile);
                ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
                ofn.nFilterIndex = 1;
                ofn.lpstrFileTitle = NULL;
                ofn.nMaxFileTitle = 0;
                ofn.lpstrInitialDir = NULL;
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_OVERWRITEPROMPT;

                if (GetOpenFileNameW(&ofn))
                {
                    //open file
                    HANDLE hFile = CreateFileW(ofn.lpstrFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                    if (hFile == INVALID_HANDLE_VALUE)
                    {
                        MessageBoxW(hwnd, L"Error opening file", L"Error", MB_OK);
                    }
                    else
                    {
                        DWORD dwBytesRead = 0;
                        WCHAR szText[1024] = L"";
                        ReadFile(hFile, szText, 1024, &dwBytesRead, NULL);
                        SetWindowTextW(hEdit, szText);
                        CloseHandle(hFile);
                    }
                }
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
        return 1;
    }

    return 0;
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
            SendMessage(hwnd, WM_COMMAND, CMD_SAVE, 0);
        }
        else if (GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState('O') & 0x8000)
        {
            SendMessage(hwnd, WM_COMMAND, CMD_OPEN, 0);
        }
        
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE)
        {
            if (TryQuit(hwnd))
            {
                break;
            }
        }


        DispatchMessage(&msg);
    }

    return 0;
}
