#include "../include/gui.h"
#include "../include/file_manager.h"
#include "../include/folder_tree.h"
#include "../include/file_details.h"

extern FileManager manager;
extern FolderTree folderTree;

HWND detailsText;

HBRUSH windowBrush;
HBRUSH panelBrush;
HBRUSH listBrush;

COLORREF windowColor = RGB(240, 244, 250);
COLORREF panelColor = RGB(255, 255, 255);
COLORREF textColor = RGB(35, 50, 70);
COLORREF blueColor = RGB(55, 115, 210);

// Create buttons
void addButton(HWND window, const char* name,
               int x, int y, int id)
{
    CreateWindowA(
        "BUTTON", name,
        WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
        x, y, 110, 32,
        window, (HMENU)id, NULL, NULL
    );
}

// Create GUI controls
void createControls(HWND window)
{
    CreateWindowA(
        "STATIC", "SECURE EXPLORER",
        WS_VISIBLE | WS_CHILD,
        20, 10, 250, 25,
        window, NULL, NULL, NULL
    );

    nameBox = CreateWindowA(
        "EDIT", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        20, 45, 200, 28,
        window, (HMENU)ID_NAME, NULL, NULL
    );

    addButton(window, "New Folder", 240, 42, ID_NEW_FOLDER);
    addButton(window, "New File", 360, 42, ID_NEW_FILE);
    addButton(window, "Open", 480, 42, ID_OPEN);
    addButton(window, "Delete", 600, 42, ID_DELETE);
    addButton(window, "Refresh", 720, 42, ID_REFRESH);
    addButton(window, "Back", 720, 82, ID_BACK);

    CreateWindowA(
        "STATIC", "FOLDER TREE",
        WS_VISIBLE | WS_CHILD,
        20, 120, 200, 25,
        window, NULL, NULL, NULL
    );

    treeList = CreateWindowA(
        "LISTBOX", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER |
        WS_VSCROLL | LBS_NOTIFY |
        LBS_OWNERDRAWFIXED | LBS_HASSTRINGS,
        20, 150, 220, 350,
        window, (HMENU)ID_TREE, NULL, NULL
    );

    SendMessageA(treeList, LB_SETITEMHEIGHT, 0, 28);

    CreateWindowA(
        "STATIC", "FILES AND FOLDERS",
        WS_VISIBLE | WS_CHILD,
        270, 120, 250, 25,
        window, NULL, NULL, NULL
    );

    itemList = CreateWindowA(
        "LISTBOX", "",
        WS_VISIBLE | WS_CHILD | WS_BORDER |
        WS_VSCROLL | LBS_NOTIFY |
        LBS_OWNERDRAWFIXED | LBS_HASSTRINGS,
        270, 150, 580, 210,
        window, (HMENU)ID_LIST, NULL, NULL
    );

    SendMessageA(itemList, LB_SETITEMHEIGHT, 0, 30);

    CreateWindowA(
        "STATIC", "FILE DETAILS",
        WS_VISIBLE | WS_CHILD,
        270, 370, 250, 20,
        window, NULL, NULL, NULL
    );

    detailsText = CreateWindowA(
        "STATIC", "Select a file to view details.",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        270, 395, 580, 105,
        window, NULL, NULL, NULL
    );

    pathText = CreateWindowA(
        "STATIC", "",
        WS_VISIBLE | WS_CHILD,
        20, 510, 800, 25,
        window, NULL, NULL, NULL
    );

    statusText = CreateWindowA(
        "STATIC", "Ready",
        WS_VISIBLE | WS_CHILD,
        20, 540, 800, 25,
        window, NULL, NULL, NULL
    );

    manager.refresh();
}

// Draw rounded buttons
void drawButton(DRAWITEMSTRUCT* dis)
{
    HDC dc = dis->hDC;
    RECT rect = dis->rcItem;

    bool pressed = (dis->itemState & ODS_SELECTED);

    HBRUSH brush;

    if (pressed)
        brush = CreateSolidBrush(RGB(35, 85, 165));
    else
        brush = CreateSolidBrush(blueColor);

    HPEN pen = CreatePen(PS_SOLID, 1, blueColor);

    HGDIOBJ oldBrush = SelectObject(dc, brush);
    HGDIOBJ oldPen = SelectObject(dc, pen);

    RoundRect(
        dc,
        rect.left + 2,
        rect.top + 2,
        rect.right - 2,
        rect.bottom - 2,
        10, 10
    );

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);

    DeleteObject(brush);
    DeleteObject(pen);

    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));

    char name[100];

    GetWindowTextA(dis->hwndItem, name, 100);

    DrawTextA(
        dc, name, -1, &rect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );
}

// Draw folder and file list items
void drawListItem(DRAWITEMSTRUCT* dis)
{
    if (dis->itemID == (UINT)-1)
        return;

    HDC dc = dis->hDC;
    RECT rect = dis->rcItem;

    bool selected = (dis->itemState & ODS_SELECTED);

    HBRUSH background;

    if (selected)
        background = CreateSolidBrush(RGB(205, 225, 250));
    else
        background = CreateSolidBrush(RGB(255, 255, 255));

    FillRect(dc, &rect, background);
    DeleteObject(background);

    int length = (int)SendMessageA(
        dis->hwndItem, LB_GETTEXTLEN,
        dis->itemID, 0
    );

    if (length < 0)
        return;

    string name(length + 1, '\0');

    SendMessageA(
        dis->hwndItem, LB_GETTEXT,
        dis->itemID, (LPARAM)&name[0]
    );

    name.resize(length);

    size_t position = name.find("[Folder] ");

    bool isFolder = (position != string::npos);

    int indent = isFolder ? (int)position : 0;

    if (isFolder)
        name = name.substr(position + 9);

    int x = rect.left + 12 + indent * 5;
    int y = rect.top + 10;

    RECT iconRect = {
        x, y, x + 12, y + 12
    };

    HBRUSH iconBrush;

    if (isFolder)
        iconBrush = CreateSolidBrush(RGB(230, 170, 55));
    else
        iconBrush = CreateSolidBrush(RGB(65, 135, 220));

    FillRect(dc, &iconRect, iconBrush);
    DeleteObject(iconBrush);

    RECT textRect = {
        x + 20,
        rect.top,
        rect.right - 5,
        rect.bottom
    };

    SetBkMode(dc, TRANSPARENT);

    if (selected)
        SetTextColor(dc, RGB(25, 65, 120));
    else
        SetTextColor(dc, textColor);

    DrawTextA(
        dc, name.c_str(), -1, &textRect,
        DT_LEFT | DT_VCENTER |
        DT_SINGLELINE | DT_END_ELLIPSIS
    );
}

// Main window procedure
LRESULT CALLBACK WindowProcedure(
    HWND window, UINT message,
    WPARAM wParam, LPARAM lParam
)
{
    switch (message)
    {
        case WM_CREATE:

            windowBrush = CreateSolidBrush(windowColor);
            panelBrush = CreateSolidBrush(panelColor);
            listBrush = CreateSolidBrush(RGB(255, 255, 255));

            createControls(window);
            break;

        case WM_ERASEBKGND:
        {
            RECT rect;

            GetClientRect(window, &rect);

            FillRect(
                (HDC)wParam,
                &rect,
                windowBrush
            );

            return 1;
        }

        case WM_CTLCOLORLISTBOX:
        {
            HDC dc = (HDC)wParam;

            SetTextColor(dc, textColor);
            SetBkColor(dc, RGB(255, 255, 255));

            return (LRESULT)listBrush;
        }

        case WM_CTLCOLORSTATIC:
        {
            HDC dc = (HDC)wParam;

            SetTextColor(dc, textColor);

            if ((HWND)lParam == detailsText)
            {
                SetBkMode(dc, OPAQUE);
                SetBkColor(dc, panelColor);

                return (LRESULT)panelBrush;
            }

            SetBkMode(dc, TRANSPARENT);

            return (LRESULT)windowBrush;
        }

        case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* dis =
                (DRAWITEMSTRUCT*)lParam;

            if (dis->CtlType == ODT_BUTTON)
            {
                drawButton(dis);
                return TRUE;
            }

            if (dis->CtlType == ODT_LISTBOX)
            {
                drawListItem(dis);
                return TRUE;
            }

            break;
        }

        case WM_COMMAND:

            switch (LOWORD(wParam))
            {
                case ID_NEW_FOLDER:

                    manager.createFolder();

                    SetWindowTextA(
                        detailsText,
                        "Select a file to view details."
                    );
                    break;

                case ID_NEW_FILE:

                    manager.createFile();

                    SetWindowTextA(
                        detailsText,
                        "Select a file to view details."
                    );
                    break;

                case ID_OPEN:
                    manager.openItem();
                    break;

                case ID_DELETE:

                    manager.deleteItem();

                    SetWindowTextA(
                        detailsText,
                        "Select a file to view details."
                    );
                    break;

                case ID_REFRESH:

                    manager.refresh();

                    SetWindowTextA(
                        detailsText,
                        "Select a file to view details."
                    );
                    break;

                case ID_BACK:

                    manager.goBack();

                    SetWindowTextA(
                        detailsText,
                        "Select a file to view details."
                    );
                    break;

                case ID_LIST:

                    if (HIWORD(wParam) == LBN_SELCHANGE)
                    {
                        fs::path selected =
                            manager.selectedPath();

                        if (!selected.empty())
                        {
                            string info =
                                getFileDetails(selected);

                            SetWindowTextA(
                                detailsText,
                                info.c_str()
                            );
                        }
                    }

                    if (HIWORD(wParam) == LBN_DBLCLK)
                    {
                        manager.openItem();

                        SetWindowTextA(
                            detailsText,
                            "Select a file to view details."
                        );
                    }

                    break;

                case ID_TREE:

                    if (HIWORD(wParam) == LBN_SELCHANGE)
                    {
                        fs::path folder =
                            folderTree.selectedPath();

                        if (!folder.empty())
                        {
                            manager.openFolder(folder);

                            SetWindowTextA(
                                detailsText,
                                "Select a file to view details."
                            );
                        }
                    }

                    break;
            }

            break;

        case WM_DESTROY:

            DeleteObject(windowBrush);
            DeleteObject(panelBrush);
            DeleteObject(listBrush);

            PostQuitMessage(0);
            break;

        default:

            return DefWindowProcA(
                window, message, wParam, lParam
            );
    }

    return 0;
}
