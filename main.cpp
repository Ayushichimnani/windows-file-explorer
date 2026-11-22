#include "../include/gui.h"
#include "../include/file_manager.h"
#include "../include/folder_tree.h"

HWND mainWindow;
HWND nameBox;
HWND itemList;
HWND treeList;
HWND pathText;
HWND statusText;

fs::path dataFolder;

FileManager manager;
FolderTree folderTree;

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR args,
    int show
)
{
    dataFolder = fs::current_path() / "SecureExplorerData";

    fs::create_directories(dataFolder / "Documents");
    fs::create_directories(dataFolder / "Pictures");
    fs::create_directories(dataFolder / "Downloads");

    manager.initialize(dataFolder);

    const char CLASS_NAME[] = "SecureExplorer";

    WNDCLASSA wc = {};

    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.lpfnWndProc = WindowProcedure;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    mainWindow = CreateWindowA(
        CLASS_NAME,
        "SecureExplorer File Manager",
        WS_OVERLAPPEDWINDOW,
        100, 100, 900, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(mainWindow, show);

    MSG msg = {};

    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}
