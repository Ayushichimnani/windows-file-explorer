#include "framework.h"
#include "search_controls.h"
#include <commctrl.h>
#include <windows.h>
#include <string>

#pragma comment(lib, "Comctl32.lib")

HWND hFileList, hSearchBox;

void CreateSearchControls(HWND hWnd)
{
    hSearchBox = CreateWindowW(
        L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        20, 50, 250, 25,
        hWnd, (HMENU)2001, NULL, NULL
    );

    CreateWindowW(
        L"BUTTON", L"Search",
        WS_CHILD | WS_VISIBLE,
        280, 50, 80, 25,
        hWnd, (HMENU)2002, NULL, NULL
    );

    hFileList = CreateWindowW(
        WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER |
        LVS_REPORT | LVS_SINGLESEL,
        20, 90, 700, 350,
        hWnd, (HMENU)2003, NULL, NULL
    );

    ListView_SetExtendedListViewStyle(
        hFileList, LVS_EX_FULLROWSELECT
    );

    LVCOLUMNW col = {};
    col.mask = LVCF_TEXT | LVCF_WIDTH;

    col.pszText = (LPWSTR)L"File Name";
    col.cx = 300;
    ListView_InsertColumn(hFileList, 0, &col);
    col.pszText = (LPWSTR)L"Type";
    col.cx = 100;
    ListView_InsertColumn(hFileList, 1, &col);

    col.pszText = (LPWSTR)L"Size";
    col.cx = 100;
    ListView_InsertColumn(hFileList, 2, &col);
}
void LoadFileList()
{
    ListView_DeleteAllItems(hFileList);

    WIN32_FIND_DATAW data;
    HANDLE hFind = FindFirstFileW(L"C:\\*.*", &data);

    if (hFind == INVALID_HANDLE_VALUE)
        return;

    wchar_t query[260] = {};
    GetWindowTextW(hSearchBox, query, 260);

    std::wstring search = query;
    for (auto& c : search)
        c = towlower(c);

    int i = 0;

    do
    {
        if (wcscmp(data.cFileName, L".") == 0 ||
            wcscmp(data.cFileName, L"..") == 0)
            continue;

        std::wstring name = data.cFileName;
        for (auto& c : name)
            c = towlower(c);

        if (!search.empty() &&
            name.find(search) == std::wstring::npos)
            continue;

        LVITEMW item = {};
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.pszText = data.cFileName;

        ListView_InsertItem(hFileList, &item);

        std::wstring type =
            (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            ? L"Folder" : L"File";

        ListView_SetItemText(
            hFileList, i, 1, (LPWSTR)type.c_str());

        unsigned long long size =
            ((unsigned long long)data.nFileSizeHigh << 32)
            | data.nFileSizeLow;

        std::wstring fileSize =
            std::to_wstring(size) + L" bytes";

        ListView_SetItemText(
            hFileList, i, 2, (LPWSTR)fileSize.c_str());

        i++;

    } while (FindNextFileW(hFind, &data));

    FindClose(hFind);
}
void SearchFiles()
{
    LoadFileList();

}