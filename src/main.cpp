#include <windows.h>

// Window procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            int id = LOWORD(wParam);

            // Back button
            if (id == 10)
            {
                MessageBox(
                    hwnd,
                    "Back button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // Forward button
            else if (id == 11)
            {
                MessageBox(
                    hwnd,
                    "Forward button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // Up button
            else if (id == 12)
            {
                MessageBox(
                    hwnd,
                    "Up button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // Refresh button
            else if (id == 13)
            {
                MessageBox(
                    hwnd,
                    "Refresh button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // Search button
            else if (id == 21)
            {
                MessageBox(
                    hwnd,
                    "Search button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // File -> New
            else if (id == 1)
            {
                MessageBox(
                    hwnd,
                    "New option selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // File -> Open
            else if (id == 2)
            {
                MessageBox(
                    hwnd,
                    "Open option selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // File -> Exit
            else if (id == 3)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }

            // View -> Refresh
            else if (id == 4)
            {
                MessageBox(
                    hwnd,
                    "Refresh option selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // View -> List View
            else if (id == 5)
            {
                MessageBox(
                    hwnd,
                    "List View selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // View -> Details View
            else if (id == 6)
            {
                MessageBox(
                    hwnd,
                    "Details View selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    // Window class name
    const char CLASS_NAME[] = "RSFileExplorer";

    // Register window class
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Create main window
    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "RS File Explorer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        1000, 650,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
        return 0;

    // ---------------- MENU BAR ----------------

    HMENU hMenuBar = CreateMenu();

    HMENU hFileMenu = CreatePopupMenu();

    AppendMenu(hFileMenu, MF_STRING, 1, "New");
    AppendMenu(hFileMenu, MF_STRING, 2, "Open");
    AppendMenu(hFileMenu, MF_STRING, 3, "Exit");

    HMENU hViewMenu = CreatePopupMenu();

    AppendMenu(hViewMenu, MF_STRING, 4, "Refresh");
    AppendMenu(hViewMenu, MF_STRING, 5, "List View");
    AppendMenu(hViewMenu, MF_STRING, 6, "Details View");

    AppendMenu(
        hMenuBar,
        MF_POPUP,
        (UINT_PTR)hFileMenu,
        "File"
    );

    AppendMenu(
        hMenuBar,
        MF_POPUP,
        (UINT_PTR)hViewMenu,
        "View"
    );

    SetMenu(hwnd, hMenuBar);


    // ---------------- TOOLBAR BUTTONS ----------------

    // Back button
    CreateWindow(
        "BUTTON",
        "< Back",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        10, 35, 80, 32,
        hwnd,
        (HMENU)10,
        hInstance,
        NULL
    );

    // Forward button
    CreateWindow(
        "BUTTON",
        "Forward >",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        95, 35, 90, 32,
        hwnd,
        (HMENU)11,
        hInstance,
        NULL
    );

    // Up button
    CreateWindow(
        "BUTTON",
        "Up",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        190, 35, 60, 32,
        hwnd,
        (HMENU)12,
        hInstance,
        NULL
    );

    // Refresh button
    CreateWindow(
        "BUTTON",
        "Refresh",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        255, 35, 80, 32,
        hwnd,
        (HMENU)13,
        hInstance,
        NULL
    );


    // ---------------- ADDRESS BAR ----------------

    CreateWindow(
        "STATIC",
        "Address:",
        WS_VISIBLE | WS_CHILD,
        350, 43, 55, 20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    CreateWindow(
        "EDIT",
        "C:\\Users",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
        410, 38, 450, 27,
        hwnd,
        (HMENU)20,
        hInstance,
        NULL
    );


    // ---------------- SEARCH BUTTON ----------------

    CreateWindow(
        "BUTTON",
        "Search",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        870, 35, 80, 32,
        hwnd,
        (HMENU)21,
        hInstance,
        NULL
    );


    // ---------------- MAIN FILE DISPLAY AREA ----------------

    CreateWindow(
        "STATIC",
        "Files and Folders",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        20, 85, 300, 25,
        hwnd,
        NULL,
        hInstance,
        NULL
    );


    // List box for files/folders
    CreateWindow(
        "LISTBOX",
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER |
        LBS_NOTIFY | WS_VSCROLL,
        20, 115, 930, 430,
        hwnd,
        (HMENU)30,
        hInstance,
        NULL
    );


    // ---------------- STATUS BAR ----------------

    CreateWindow(
        "STATIC",
        "Ready    |    0 items",
        WS_VISIBLE | WS_CHILD | WS_BORDER | SS_LEFT,
        20, 560, 930, 30,
        hwnd,
        (HMENU)40,
        hInstance,
        NULL
    );


    // Show window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);


    // ---------------- MESSAGE LOOP ----------------

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
} 