#include <windows.h>

#define ID_BACK       10
#define ID_FORWARD    11
#define ID_UP         12
#define ID_REFRESH    13

#define ID_ADDRESS    20
#define ID_SEARCH     21

#define ID_NEW        1
#define ID_OPEN       2
#define ID_EXIT       3

#define ID_VIEW_REFRESH 4
#define ID_LIST_VIEW    5
#define ID_DETAILS_VIEW 6

#define ID_FILE_AREA  30
#define ID_STATUS     40


// ================= WINDOW PROCEDURE =================

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg,
                            WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            int id = LOWORD(wParam);

            if (id == ID_BACK)
            {
                MessageBoxA(
                    hwnd,
                    "Back button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            else if (id == ID_FORWARD)
            {
                MessageBoxA(
                    hwnd,
                    "Forward button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            else if (id == ID_UP)
            {
                MessageBoxA(
                    hwnd,
                    "Up button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            else if (id == ID_REFRESH)
            {
                MessageBoxA(
                    hwnd,
                    "Refresh button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            else if (id == ID_SEARCH)
            {
                MessageBoxA(
                    hwnd,
                    "Search button clicked",
                    "RS File Explorer",
                    MB_OK
                );
            }

            // ---------- FILE MENU ----------

            else if (id == ID_NEW)
            {
                MessageBoxA(
                    hwnd,
                    "New option selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            else if (id == ID_OPEN)
            {
                MessageBoxA(
                    hwnd,
                    "Open option selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            else if (id == ID_EXIT)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }

            // ---------- VIEW MENU ----------

            else if (id == ID_VIEW_REFRESH)
            {
                MessageBoxA(
                    hwnd,
                    "Refresh option selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            else if (id == ID_LIST_VIEW)
            {
                MessageBoxA(
                    hwnd,
                    "List View selected",
                    "RS File Explorer",
                    MB_OK
                );
            }

            else if (id == ID_DETAILS_VIEW)
            {
                MessageBoxA(
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


// ================= MAIN FUNCTION =================

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    const char CLASS_NAME[] = "RSFileExplorer";

    // ---------- WINDOW CLASS ----------

    WNDCLASSA wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);


    // ---------- MAIN WINDOW ----------

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "RS File Explorer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        1000,
        650,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL)
        return 0;


    // =================================================
    // MENU BAR
    // =================================================

    HMENU hMenuBar = CreateMenu();

    HMENU hFileMenu = CreatePopupMenu();

    AppendMenuA(
        hFileMenu,
        MF_STRING,
        ID_NEW,
        "New"
    );

    AppendMenuA(
        hFileMenu,
        MF_STRING,
        ID_OPEN,
        "Open"
    );

    AppendMenuA(
        hFileMenu,
        MF_SEPARATOR,
        0,
        NULL
    );

    AppendMenuA(
        hFileMenu,
        MF_STRING,
        ID_EXIT,
        "Exit"
    );


    HMENU hViewMenu = CreatePopupMenu();

    AppendMenuA(
        hViewMenu,
        MF_STRING,
        ID_VIEW_REFRESH,
        "Refresh"
    );

    AppendMenuA(
        hViewMenu,
        MF_STRING,
        ID_LIST_VIEW,
        "List View"
    );

    AppendMenuA(
        hViewMenu,
        MF_STRING,
        ID_DETAILS_VIEW,
        "Details View"
    );


    AppendMenuA(
        hMenuBar,
        MF_POPUP,
        (UINT_PTR)hFileMenu,
        "File"
    );

    AppendMenuA(
        hMenuBar,
        MF_POPUP,
        (UINT_PTR)hViewMenu,
        "View"
    );

    SetMenu(hwnd, hMenuBar);


    // =================================================
    // TOOLBAR
    // =================================================

    CreateWindowA(
        "BUTTON",
        "< Back",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        15, 35, 80, 32,
        hwnd,
        (HMENU)ID_BACK,
        hInstance,
        NULL
    );

    CreateWindowA(
        "BUTTON",
        "Forward >",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        100, 35, 90, 32,
        hwnd,
        (HMENU)ID_FORWARD,
        hInstance,
        NULL
    );

    CreateWindowA(
        "BUTTON",
        "Up",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        195, 35, 60, 32,
        hwnd,
        (HMENU)ID_UP,
        hInstance,
        NULL
    );

    CreateWindowA(
        "BUTTON",
        "Refresh",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        260, 35, 80, 32,
        hwnd,
        (HMENU)ID_REFRESH,
        hInstance,
        NULL
    );


    // =================================================
    // ADDRESS BAR
    // =================================================

    CreateWindowA(
        "STATIC",
        "Address:",
        WS_VISIBLE | WS_CHILD,
        355, 43, 55, 20,
        hwnd,
        NULL,
        hInstance,
        NULL
    );

    CreateWindowA(
        "EDIT",
        "C:\\Users",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT,
        415, 38, 440, 27,
        hwnd,
        (HMENU)ID_ADDRESS,
        hInstance,
        NULL
    );


    // =================================================
    // SEARCH BUTTON
    // =================================================

    CreateWindowA(
        "BUTTON",
        "Search",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        865, 35, 80, 32,
        hwnd,
        (HMENU)ID_SEARCH,
        hInstance,
        NULL
    );


    // =================================================
    // FILE DISPLAY AREA
    // =================================================

    CreateWindowA(
        "STATIC",
        "Files and Folders",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        20, 85, 300, 25,
        hwnd,
        NULL,
        hInstance,
        NULL
    );


    CreateWindowA(
        "LISTBOX",
        NULL,
        WS_VISIBLE |
        WS_CHILD |
        WS_BORDER |
        LBS_NOTIFY |
        WS_VSCROLL,
        20, 115, 925, 430,
        hwnd,
        (HMENU)ID_FILE_AREA,
        hInstance,
        NULL
    );


    // =================================================
    // STATUS BAR
    // =================================================

    CreateWindowA(
        "STATIC",
        "Ready    |    0 items",
        WS_VISIBLE |
        WS_CHILD |
        WS_BORDER |
        SS_LEFT,
        20, 560, 925, 30,
        hwnd,
        (HMENU)ID_STATUS,
        hInstance,
        NULL
    );


    // ---------- SHOW WINDOW ----------

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);


    // =================================================
    // MESSAGE LOOP
    // =================================================

    MSG msg = {};

    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}