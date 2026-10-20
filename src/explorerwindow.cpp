#include "ExplorerWindow.h"
#include "Resource.h"
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <windowsx.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

#define WINDOW_CLASS_NAME L"CppFileExplorerMainWindow"

ExplorerWindow::ExplorerWindow()
    : m_hWnd(NULL)
    , m_hInstance(NULL)
    , m_hBtnBack(NULL)
    , m_hBtnForward(NULL)
    , m_hBtnUp(NULL)
    , m_hBtnRefresh(NULL)
    , m_hAddressEdit(NULL)
    , m_hBtnGo(NULL)
    , m_hSearchEdit(NULL)
    , m_hBtnNewFolder(NULL)
    , m_hBtnViewMode(NULL)
    , m_hBtnDetails(NULL)
    , m_hBtnHelp(NULL)
    , m_hSidebarTree(NULL)
    , m_hMainListView(NULL)
    , m_hDetailsPanel(NULL)
    , m_hStatusBar(NULL)
    , m_hToolTip(NULL)
    , m_hNavFont(NULL)
    , m_sidebarWidth(220)
    , m_detailsWidth(240)
    , m_showDetailsPanel(true)
    , m_currentViewStyle(LVS_REPORT)
    , m_sortColumn(0)
    , m_sortAscending(true)
    , m_isCutOperation(false)
    , m_hSysImageListSmall(NULL)
    , m_hSysImageListLarge(NULL) {
}

ExplorerWindow::~ExplorerWindow() {
    if (m_hNavFont) {
        DeleteObject(m_hNavFont);
        m_hNavFont = NULL;
    }
}

bool ExplorerWindow::Register(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = ExplorerWindow::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDR_MAINMENU);
    wcex.lpszClassName = WINDOW_CLASS_NAME;
    wcex.hIconSm = LoadIconW(NULL, IDI_APPLICATION);

    return RegisterClassExW(&wcex) != 0;
}

bool ExplorerWindow::Create(HINSTANCE hInstance, int nCmdShow) {
    m_hInstance = hInstance;

    m_hWnd = CreateWindowExW(
        WS_EX_ACCEPTFILES | WS_EX_CONTROLPARENT,
        WINDOW_CLASS_NAME,
        L"C++ Based File Explorer",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, 1140, 720,
        NULL, NULL, hInstance, this
    );

    if (!m_hWnd) return false;

    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

    // Initial folder navigation: Desktop or C:\ root
    std::wstring initialPath = GetKnownFolder(FOLDERID_Desktop);
    if (initialPath.empty() || GetFileAttributesW(initialPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        initialPath = L"C:\\";
    }
    OnNavigate(initialPath, false);

    return true;
}

void ExplorerWindow::SetupSystemImageLists() {
    SHFILEINFOW sfiSmall = { 0 };
    m_hSysImageListSmall = reinterpret_cast<HIMAGELIST>(
        SHGetFileInfoW(L"C:\\", 0, &sfiSmall, sizeof(sfiSmall), SHGFI_SYSICONINDEX | SHGFI_SMALLICON)
    );

    SHFILEINFOW sfiLarge = { 0 };
    m_hSysImageListLarge = reinterpret_cast<HIMAGELIST>(
        SHGetFileInfoW(L"C:\\", 0, &sfiLarge, sizeof(sfiLarge), SHGFI_SYSICONINDEX | SHGFI_LARGEICON)
    );

    if (m_hSidebarTree && m_hSysImageListSmall) {
        TreeView_SetImageList(m_hSidebarTree, m_hSysImageListSmall, TVSIL_NORMAL);
    }

    if (m_hMainListView) {
        if (m_hSysImageListSmall) {
            ListView_SetImageList(m_hMainListView, m_hSysImageListSmall, LVSIL_SMALL);
        }
        if (m_hSysImageListLarge) {
            ListView_SetImageList(m_hMainListView, m_hSysImageListLarge, LVSIL_NORMAL);
        }
    }
}

void ExplorerWindow::SetupTooltips() {
    m_hToolTip = CreateWindowExW(
        WS_EX_TOPMOST, TOOLTIPS_CLASSW, NULL,
        WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        m_hWnd, NULL, m_hInstance, NULL
    );

    SetWindowPos(m_hToolTip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    AddTooltipForControl(m_hBtnBack, L"Back (Alt + Left Arrow)");
    AddTooltipForControl(m_hBtnForward, L"Forward (Alt + Right Arrow)");
    AddTooltipForControl(m_hBtnUp, L"Up to parent directory (Backspace)");
    AddTooltipForControl(m_hBtnRefresh, L"Refresh folder (F5)");
    AddTooltipForControl(m_hBtnGo, L"Go to address (Enter)");
    AddTooltipForControl(m_hBtnNewFolder, L"Create New Folder (Ctrl + N)");
    AddTooltipForControl(m_hBtnViewMode, L"Switch View (Details / Large Icons / List)");
    AddTooltipForControl(m_hBtnDetails, L"Toggle Details Panel");
    AddTooltipForControl(m_hBtnHelp, L"Help & Keyboard Shortcuts (F1)");
}

void ExplorerWindow::AddTooltipForControl(HWND hControl, const wchar_t* tipText) {
    if (!m_hToolTip || !hControl || !tipText) return;

    TOOLINFOW ti = { 0 };
    ti.cbSize = sizeof(TOOLINFOW);
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.hwnd = m_hWnd;
    ti.uId = reinterpret_cast<UINT_PTR>(hControl);
    ti.lpszText = const_cast<LPWSTR>(tipText);

    SendMessageW(m_hToolTip, TTM_ADDTOOLW, 0, reinterpret_cast<LPARAM>(&ti));
}

void ExplorerWindow::CreateChildControls() {
    HFONT hFont = m_themeManager.GetRegularFont();

    // Create navigation arrow font (Segoe UI 16px Semibold)
    m_hNavFont = CreateFontW(
        -16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI"
    );

    m_hBtnBack = CreateWindowExW(0, L"BUTTON", L"\u2190", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                0, 0, 0, 0, m_hWnd, (HMENU)IDC_NAV_BACK, m_hInstance, NULL);
    m_hBtnForward = CreateWindowExW(0, L"BUTTON", L"\u2192", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   0, 0, 0, 0, m_hWnd, (HMENU)IDC_NAV_FORWARD, m_hInstance, NULL);
    m_hBtnUp = CreateWindowExW(0, L"BUTTON", L"\u2191", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                              0, 0, 0, 0, m_hWnd, (HMENU)IDC_NAV_UP, m_hInstance, NULL);
    m_hBtnRefresh = CreateWindowExW(0, L"BUTTON", L"\u21BB", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   0, 0, 0, 0, m_hWnd, (HMENU)IDC_NAV_REFRESH, m_hInstance, NULL);

    SendMessageW(m_hBtnBack, WM_SETFONT, (WPARAM)m_hNavFont, TRUE);
    SendMessageW(m_hBtnForward, WM_SETFONT, (WPARAM)m_hNavFont, TRUE);
    SendMessageW(m_hBtnUp, WM_SETFONT, (WPARAM)m_hNavFont, TRUE);
    SendMessageW(m_hBtnRefresh, WM_SETFONT, (WPARAM)m_hNavFont, TRUE);

    m_hAddressEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                    WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                    0, 0, 0, 0, m_hWnd, (HMENU)IDC_ADDRESS_BAR, m_hInstance, NULL);

    m_hBtnGo = CreateWindowExW(0, L"BUTTON", L"Go", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                              0, 0, 0, 0, m_hWnd, (HMENU)IDC_GO_BUTTON, m_hInstance, NULL);

    m_hSearchEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
                                   WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                   0, 0, 0, 0, m_hWnd, (HMENU)IDC_SEARCH_BOX, m_hInstance, NULL);

    Edit_SetCueBannerTextFocused(m_hSearchEdit, L"Search files...", FALSE);

    m_hBtnNewFolder = CreateWindowExW(0, L"BUTTON", L"+ Folder", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                     0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_NEWFOLDER, m_hInstance, NULL);
    m_hBtnViewMode = CreateWindowExW(0, L"BUTTON", L"View", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                    0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_VIEW_MODE, m_hInstance, NULL);
    m_hBtnDetails = CreateWindowExW(0, L"BUTTON", L"Details", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                   0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_TOGGLE_DETAILS, m_hInstance, NULL);
    m_hBtnHelp = CreateWindowExW(0, L"BUTTON", L"Help", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                0, 0, 0, 0, m_hWnd, (HMENU)IDC_BTN_HELP, m_hInstance, NULL);

    HWND standardControls[] = { m_hAddressEdit, m_hBtnGo, m_hSearchEdit,
                                m_hBtnNewFolder, m_hBtnViewMode, m_hBtnDetails, m_hBtnHelp };
    for (HWND hCtrl : standardControls) {
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)hFont, TRUE);
    }

    m_hSidebarTree = CreateWindowExW(
        0, WC_TREEVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
        0, 0, 0, 0, m_hWnd, (HMENU)IDC_SIDEBAR_TREE, m_hInstance, NULL
    );
    SendMessageW(m_hSidebarTree, WM_SETFONT, (WPARAM)hFont, TRUE);

    m_hMainListView = CreateWindowExW(
        0, WC_LISTVIEWW, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_TABSTOP,
        0, 0, 0, 0, m_hWnd, (HMENU)IDC_MAIN_LISTVIEW, m_hInstance, NULL
    );
    SendMessageW(m_hMainListView, WM_SETFONT, (WPARAM)hFont, TRUE);

    ListView_SetExtendedListViewStyle(
        m_hMainListView,
        LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_SUBITEMIMAGES
    );

    LVCOLUMNW lvc = { 0 };
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvc.cx = 280;
    lvc.pszText = const_cast<LPWSTR>(L"Name");
    lvc.iSubItem = 0;
    ListView_InsertColumn(m_hMainListView, 0, &lvc);

    lvc.cx = 160;
    lvc.pszText = const_cast<LPWSTR>(L"Date Modified");
    lvc.iSubItem = 1;
    ListView_InsertColumn(m_hMainListView, 1, &lvc);

    lvc.cx = 140;
    lvc.pszText = const_cast<LPWSTR>(L"Type");
    lvc.iSubItem = 2;
    ListView_InsertColumn(m_hMainListView, 2, &lvc);

    lvc.cx = 100;
    lvc.pszText = const_cast<LPWSTR>(L"Size");
    lvc.iSubItem = 3;
    ListView_InsertColumn(m_hMainListView, 3, &lvc);

    m_hDetailsPanel = CreateWindowExW(
        0, L"STATIC", L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER | SS_NOTIFY,
        0, 0, 0, 0, m_hWnd, (HMENU)IDC_DETAILS_PANEL, m_hInstance, NULL
    );
    SendMessageW(m_hDetailsPanel, WM_SETFONT, (WPARAM)hFont, TRUE);

    m_hStatusBar = CreateWindowExW(
        0, STATUSCLASSNAMEW, L"",
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0, m_hWnd, (HMENU)IDC_STATUS_BAR, m_hInstance, NULL
    );
    SendMessageW(m_hStatusBar, WM_SETFONT, (WPARAM)hFont, TRUE);

    int statusParts[] = { 280, -1 };
    SendMessageW(m_hStatusBar, SB_SETPARTS, 2, (LPARAM)statusParts);

    SetupSystemImageLists();
    PopulateSidebar();
    SetupTooltips();
}

void ExplorerWindow::PopulateSidebar() {
    TreeView_DeleteAllItems(m_hSidebarTree);
    m_sidebarPaths.clear();

    TVINSERTSTRUCTW tvis = { 0 };
    tvis.hParent = TVI_ROOT;
    tvis.hInsertAfter = TVI_LAST;
    tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

    tvis.item.pszText = const_cast<LPWSTR>(L"Quick Access");
    tvis.item.iImage = 0;
    tvis.item.iSelectedImage = 0;
    tvis.item.lParam = (LPARAM)0;
    HTREEITEM hQuickAccess = TreeView_InsertItem(m_hSidebarTree, &tvis);

    struct KnownFolderEntry {
        const wchar_t* name;
        KNOWNFOLDERID id;
    };

    KnownFolderEntry knownFolders[] = {
        { L"Desktop",   FOLDERID_Desktop },
        { L"Downloads", FOLDERID_Downloads },
        { L"Documents", FOLDERID_Documents },
        { L"Pictures",  FOLDERID_Pictures }
    };

    for (const auto& kf : knownFolders) {
        std::wstring path = GetKnownFolder(kf.id);
        if (!path.empty()) {
            SHFILEINFOW sfi = { 0 };
            SHGetFileInfoW(path.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

            auto pStored = std::make_unique<std::wstring>(path);
            LPARAM ptrVal = reinterpret_cast<LPARAM>(pStored.get());
            m_sidebarPaths.push_back(std::move(pStored));

            TVINSERTSTRUCTW child = { 0 };
            child.hParent = hQuickAccess;
            child.hInsertAfter = TVI_LAST;
            child.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
            child.item.pszText = const_cast<LPWSTR>(kf.name);
            child.item.iImage = sfi.iIcon;
            child.item.iSelectedImage = sfi.iIcon;
            child.item.lParam = ptrVal;

            TreeView_InsertItem(m_hSidebarTree, &child);
        }
    }
    TreeView_Expand(m_hSidebarTree, hQuickAccess, TVE_EXPAND);

    tvis.item.pszText = const_cast<LPWSTR>(L"This PC");
    tvis.item.iImage = 0;
    tvis.item.iSelectedImage = 0;
    tvis.item.lParam = (LPARAM)0;
    HTREEITEM hThisPC = TreeView_InsertItem(m_hSidebarTree, &tvis);

    // Enumerate logical drives
    wchar_t driveStrings[512] = { 0 };
    DWORD len = GetLogicalDriveStringsW(512, driveStrings);
    if (len > 0) {
        const wchar_t* pDrive = driveStrings;
        while (*pDrive) {
            std::wstring drivePath = pDrive;
            SHFILEINFOW sfi = { 0 };
            SHGetFileInfoW(drivePath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_DISPLAYNAME);

            std::wstring displayName = sfi.szDisplayName[0] ? sfi.szDisplayName : drivePath;

            auto pStored = std::make_unique<std::wstring>(drivePath);
            LPARAM ptrVal = reinterpret_cast<LPARAM>(pStored.get());
            m_sidebarPaths.push_back(std::move(pStored));

            TVINSERTSTRUCTW child = { 0 };
            child.hParent = hThisPC;
            child.hInsertAfter = TVI_LAST;
            child.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
            child.item.pszText = const_cast<LPWSTR>(displayName.c_str());
            child.item.iImage = sfi.iIcon;
            child.item.iSelectedImage = sfi.iIcon;
            child.item.lParam = ptrVal;

            TreeView_InsertItem(m_hSidebarTree, &child);

            pDrive += wcslen(pDrive) + 1;
        }
    }
    TreeView_Expand(m_hSidebarTree, hThisPC, TVE_EXPAND);
}

void ExplorerWindow::LayoutControls() {
    if (!m_hWnd) return;

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    int totalWidth = rcClient.right - rcClient.left;
    int totalHeight = rcClient.bottom - rcClient.top;

    if (totalWidth <= 0 || totalHeight <= 0) return;

    SendMessageW(m_hStatusBar, WM_SIZE, 0, 0);
    RECT rcStatus;
    GetWindowRect(m_hStatusBar, &rcStatus);
    int statusHeight = rcStatus.bottom - rcStatus.top;
    if (statusHeight <= 0) statusHeight = 24;

    int tbTop = 6;
    int tbHeight = 30;
    int curX = 8;

    // Navigation buttons
    MoveWindow(m_hBtnBack, curX, tbTop, 32, tbHeight, TRUE); curX += 34;
    MoveWindow(m_hBtnForward, curX, tbTop, 32, tbHeight, TRUE); curX += 34;
    MoveWindow(m_hBtnUp, curX, tbTop, 32, tbHeight, TRUE); curX += 34;
    MoveWindow(m_hBtnRefresh, curX, tbTop, 32, tbHeight, TRUE); curX += 38;

    // Right-side action buttons: Help, Details, View, +Folder
    int rightX = totalWidth - 8;
    rightX -= 52; MoveWindow(m_hBtnHelp, rightX, tbTop, 48, tbHeight, TRUE);
    rightX -= 64; MoveWindow(m_hBtnDetails, rightX, tbTop, 60, tbHeight, TRUE);
    rightX -= 54; MoveWindow(m_hBtnViewMode, rightX, tbTop, 50, tbHeight, TRUE);
    rightX -= 72; MoveWindow(m_hBtnNewFolder, rightX, tbTop, 68, tbHeight, TRUE);

    // Search Box
    int searchWidth = 180;
    rightX -= (searchWidth + 8);
    MoveWindow(m_hSearchEdit, rightX, tbTop + 1, searchWidth, tbHeight - 2, TRUE);

    // Address Bar & Go Button
    int goBtnWidth = 36;
    int addressWidth = rightX - curX - goBtnWidth - 12;
    if (addressWidth < 120) addressWidth = 120;

    MoveWindow(m_hAddressEdit, curX, tbTop + 1, addressWidth, tbHeight - 2, TRUE);
    curX += addressWidth + 4;
    MoveWindow(m_hBtnGo, curX, tbTop, goBtnWidth, tbHeight, TRUE);

    // Content area (Split View: TreeView on left, ListView center, Details on right)
    int contentTop = tbTop + tbHeight + 8;
    int contentHeight = totalHeight - contentTop - statusHeight;

    if (m_sidebarWidth < 140) m_sidebarWidth = 140;
    if (m_sidebarWidth > totalWidth / 2) m_sidebarWidth = totalWidth / 2;

    MoveWindow(m_hSidebarTree, 0, contentTop, m_sidebarWidth, contentHeight, TRUE);

    int splitterWidth = 4;
    int mainLeft = m_sidebarWidth + splitterWidth;
    int actualDetailsWidth = m_showDetailsPanel ? m_detailsWidth : 0;
    int mainWidth = totalWidth - mainLeft - actualDetailsWidth;

    if (mainWidth < 150) mainWidth = 150;

    MoveWindow(m_hMainListView, mainLeft, contentTop, mainWidth, contentHeight, TRUE);

    if (m_showDetailsPanel) {
        ShowWindow(m_hDetailsPanel, SW_SHOW);
        MoveWindow(m_hDetailsPanel, mainLeft + mainWidth, contentTop, actualDetailsWidth, contentHeight, TRUE);
    } else {
        ShowWindow(m_hDetailsPanel, SW_HIDE);
    }
}

std::wstring ExplorerWindow::NormalizePath(const std::wstring& path) {
    if (path.empty()) return L"";
    wchar_t buf[MAX_PATH] = { 0 };
    if (GetFullPathNameW(path.c_str(), MAX_PATH, buf, NULL) == 0) {
        return path;
    }
    if (wcslen(buf) == 2 && buf[1] == L':') {
        wcscat_s(buf, L"\\");
    }
    return std::wstring(buf);
}

std::wstring ExplorerWindow::FormatFileSize(uint64_t bytes) {
    if (bytes == 0) return L"0 KB";
    const wchar_t* units[] = { L"B", L"KB", L"MB", L"GB", L"TB" };
    int unitIndex = 0;
    double size = static_cast<double>(bytes);
    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }
    std::wstringstream ss;
    if (unitIndex == 0) {
        ss << bytes << L" B";
    } else {
        ss << std::fixed << std::setprecision(1) << size << L" " << units[unitIndex];
    }
    return ss.str();
}

std::wstring ExplorerWindow::FormatFileTime(const FILETIME& ft) {
    FILETIME localFt;
    FileTimeToLocalFileTime(&ft, &localFt);
    SYSTEMTIME st;
    FileTimeToSystemTime(&localFt, &st);
    wchar_t buf[64];
    swprintf_s(buf, L"%04d-%02d-%02d %02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
    return std::wstring(buf);
}

std::wstring ExplorerWindow::GetKnownFolder(REFKNOWNFOLDERID rfid) {
    PWSTR pPath = NULL;
    if (SUCCEEDED(SHGetKnownFolderPath(rfid, 0, NULL, &pPath))) {
        std::wstring res(pPath);
        CoTaskMemFree(pPath);
        return res;
    }
    return L"";
}

bool ExplorerWindow::CanGoUp() const noexcept {
    if (m_currentPath.empty()) return false;
    if (m_currentPath.length() <= 3 && m_currentPath[1] == L':') return false;
    return true;
}

std::wstring ExplorerWindow::GoUp() {
    if (!CanGoUp()) return m_currentPath;
    wchar_t buf[MAX_PATH] = { 0 };
    wcsncpy_s(buf, m_currentPath.c_str(), _TRUNCATE);
    PathRemoveBackslashW(buf);
    PathRemoveFileSpecW(buf);
    if (wcslen(buf) == 2 && buf[1] == L':') {
        wcscat_s(buf, L"\\");
    }
    return std::wstring(buf);
}

std::wstring ExplorerWindow::GoBack() {
    if (m_backStack.empty()) return m_currentPath;
    m_forwardStack.push_back(m_currentPath);
    std::wstring prev = m_backStack.back();
    m_backStack.pop_back();
    return prev;
}

std::wstring ExplorerWindow::GoForward() {
    if (m_forwardStack.empty()) return m_currentPath;
    m_backStack.push_back(m_currentPath);
    std::wstring nxt = m_forwardStack.back();
    m_forwardStack.pop_back();
    return nxt;
}

bool ExplorerWindow::LoadDirectory(const std::wstring& folderPath, std::wstring& errorMsg) {
    m_currentItems.clear();

    std::wstring searchPattern = folderPath;
    if (!searchPattern.empty() && searchPattern.back() != L'\\') {
        searchPattern += L"\\";
    }
    searchPattern += L"*";

    WIN32_FIND_DATAW fd = { 0 };
    HANDLE hFind = FindFirstFileW(searchPattern.c_str(), &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD dwErr = GetLastError();
        errorMsg = L"Cannot access directory: " + folderPath + L" (Error code " + std::to_wstring(dwErr) + L")";
        return false;
    }

    std::vector<ExplorerItem> dirs;
    std::vector<ExplorerItem> files;

    do {
        if (wcscmp(fd.cFileName, L".") == 0 || wcscmp(fd.cFileName, L"..") == 0) {
            continue;
        }

        ExplorerItem item;
        item.name = fd.cFileName;
        item.fullPath = folderPath;
        if (item.fullPath.back() != L'\\') item.fullPath += L"\\";
        item.fullPath += fd.cFileName;

        item.isDirectory = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        item.lastWriteTime = fd.ftLastWriteTime;
        item.formattedDate = FormatFileTime(fd.ftLastWriteTime);

        SHFILEINFOW sfi = { 0 };
        SHGetFileInfoW(item.fullPath.c_str(), fd.dwFileAttributes, &sfi, sizeof(sfi),
                       SHGFI_TYPENAME | SHGFI_SYSICONINDEX | SHGFI_USEFILEATTRIBUTES);
        item.typeDescription = sfi.szTypeName[0] ? sfi.szTypeName : (item.isDirectory ? L"File folder" : L"File");
        item.iconIndex = sfi.iIcon;

        if (item.isDirectory) {
            item.formattedSize = L"";
            item.rawSize = 0;
            dirs.push_back(item);
        } else {
            ULARGE_INTEGER uli;
            uli.LowPart = fd.nFileSizeLow;
            uli.HighPart = fd.nFileSizeHigh;
            item.rawSize = uli.QuadPart;
            item.formattedSize = FormatFileSize(uli.QuadPart);
            files.push_back(item);
        }
    } while (FindNextFileW(hFind, &fd));

    FindClose(hFind);

    m_currentItems.reserve(dirs.size() + files.size());
    for (auto& d : dirs) m_currentItems.push_back(std::move(d));
    for (auto& f : files) m_currentItems.push_back(std::move(f));

    return true;
}

void ExplorerWindow::SortDisplayedItems() {
    std::sort(m_displayedItems.begin(), m_displayedItems.end(),
        [this](const ExplorerItem& a, const ExplorerItem& b) {
            // Folders always precede files
            if (a.isDirectory != b.isDirectory) {
                return a.isDirectory > b.isDirectory;
            }

            int cmp = 0;
            switch (m_sortColumn) {
            case 0: // Name
                cmp = _wcsicmp(a.name.c_str(), b.name.c_str());
                break;
            case 1: // Date Modified
                cmp = CompareFileTime(&a.lastWriteTime, &b.lastWriteTime);
                break;
            case 2: // Type
                cmp = _wcsicmp(a.typeDescription.c_str(), b.typeDescription.c_str());
                break;
            case 3: // Size
                if (a.rawSize < b.rawSize) cmp = -1;
                else if (a.rawSize > b.rawSize) cmp = 1;
                else cmp = 0;
                break;
            default:
                cmp = _wcsicmp(a.name.c_str(), b.name.c_str());
                break;
            }

            return m_sortAscending ? (cmp < 0) : (cmp > 0);
        });
}

void ExplorerWindow::OnNavigate(const std::wstring& path, bool recordHistory) {
    std::wstring clean = NormalizePath(path);
    if (clean.empty()) return;

    std::wstring errorMsg;
    if (!LoadDirectory(clean, errorMsg)) {
        SendMessageW(m_hStatusBar, SB_SETTEXTW, 1, (LPARAM)errorMsg.c_str());
        MessageBoxW(m_hWnd, errorMsg.c_str(), L"Navigation Error", MB_ICONWARNING | MB_OK);
        return;
    }

    if (recordHistory) {
        if (!m_currentPath.empty()) {
            m_backStack.push_back(m_currentPath);
        }
        m_forwardStack.clear();
    }
    m_currentPath = clean;

    SetWindowTextW(m_hAddressEdit, m_currentPath.c_str());
    SetWindowTextW(m_hSearchEdit, L"");

    PopulateListView();
    UpdateNavigationControls();
    UpdateStatusBar();
    UpdateDetailsPanel();
}

void ExplorerWindow::PopulateListView() {
    ListView_DeleteAllItems(m_hMainListView);

    wchar_t queryBuf[256] = { 0 };
    GetWindowTextW(m_hSearchEdit, queryBuf, 256);
    std::wstring query(queryBuf);

    if (query.empty()) {
        m_displayedItems = m_currentItems;
    } else {
        m_displayedItems.clear();
        std::wstring lowerQuery = query;
        for (auto& c : lowerQuery) c = towlower(c);

        for (const auto& item : m_currentItems) {
            std::wstring lowerName = item.name;
            for (auto& c : lowerName) c = towlower(c);
            if (lowerName.find(lowerQuery) != std::wstring::npos) {
                m_displayedItems.push_back(item);
            }
        }
    }

    SortDisplayedItems();

    for (size_t i = 0; i < m_displayedItems.size(); ++i) {
        const auto& item = m_displayedItems[i];

        LVITEMW lvi = { 0 };
        lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
        lvi.iItem = static_cast<int>(i);
        lvi.iSubItem = 0;
        lvi.pszText = const_cast<LPWSTR>(item.name.c_str());
        lvi.iImage = item.iconIndex;
        lvi.lParam = static_cast<LPARAM>(i);

        ListView_InsertItem(m_hMainListView, &lvi);

        ListView_SetItemText(m_hMainListView, static_cast<int>(i), 1, const_cast<LPWSTR>(item.formattedDate.c_str()));
        ListView_SetItemText(m_hMainListView, static_cast<int>(i), 2, const_cast<LPWSTR>(item.typeDescription.c_str()));
        ListView_SetItemText(m_hMainListView, static_cast<int>(i), 3, const_cast<LPWSTR>(item.formattedSize.c_str()));
    }
}

void ExplorerWindow::UpdateNavigationControls() {
    EnableWindow(m_hBtnBack, CanGoBack());
    EnableWindow(m_hBtnForward, CanGoForward());
    EnableWindow(m_hBtnUp, CanGoUp());
}

void ExplorerWindow::UpdateStatusBar() {
    size_t total = m_displayedItems.size();
    int selected = ListView_GetSelectedCount(m_hMainListView);

    std::wstring info = L"Total: " + std::to_wstring(total) + L" items | " +
                        std::to_wstring(selected) + L" selected";
    SendMessageW(m_hStatusBar, SB_SETTEXTW, 0, (LPARAM)info.c_str());

    std::wstring status = L"Current: " + m_currentPath;
    SendMessageW(m_hStatusBar, SB_SETTEXTW, 1, (LPARAM)status.c_str());
}

void ExplorerWindow::UpdateDetailsPanel() {
    if (!m_showDetailsPanel) return;

    int selIndex = ListView_GetNextItem(m_hMainListView, -1, LVNI_SELECTED);
    if (selIndex >= 0 && selIndex < static_cast<int>(m_displayedItems.size())) {
        const auto& item = m_displayedItems[selIndex];
        std::wstringstream ss;
        ss << L"Name: " << item.name << L"\n\n";
        ss << L"Type: " << item.typeDescription << L"\n\n";
        if (!item.isDirectory) {
            ss << L"Size: " << item.formattedSize << L"\n\n";
        }
        ss << L"Date Modified: " << item.formattedDate << L"\n\n";
        ss << L"Location:\n" << item.fullPath;

        SetWindowTextW(m_hDetailsPanel, ss.str().c_str());
    } else {
        std::wstringstream ss;
        ss << L"Folder: " << m_currentPath << L"\n\n";
        ss << L"Items: " << m_displayedItems.size() << L" items\n\n";
        ss << L"Tip: Select an item to view its details. Use the View menu to toggle details panel or dark/light theme.";
        SetWindowTextW(m_hDetailsPanel, ss.str().c_str());
    }
}

void ExplorerWindow::OnListViewDoubleClick() {
    int selIndex = ListView_GetNextItem(m_hMainListView, -1, LVNI_SELECTED);
    if (selIndex < 0 || selIndex >= static_cast<int>(m_displayedItems.size())) {
        return;
    }

    const auto& item = m_displayedItems[selIndex];
    if (item.isDirectory) {
        OnNavigate(item.fullPath, true);
    } else {
        ShellExecuteW(m_hWnd, L"open", item.fullPath.c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}

void ExplorerWindow::OnListViewSelectionChanged() {
    UpdateStatusBar();
    UpdateDetailsPanel();
}

void ExplorerWindow::OnListViewColumnClick(int columnIndex) {
    if (m_sortColumn == columnIndex) {
        m_sortAscending = !m_sortAscending;
    } else {
        m_sortColumn = columnIndex;
        m_sortAscending = true;
    }
    PopulateListView();
}

void ExplorerWindow::OnTreeSelectionChanged(LPNMTREEVIEWW pNmtv) {
    if (!pNmtv || !pNmtv->itemNew.hItem) return;

    TVITEMW tvi = { 0 };
    tvi.mask = TVIF_PARAM;
    tvi.hItem = pNmtv->itemNew.hItem;
    if (TreeView_GetItem(m_hSidebarTree, &tvi) && tvi.lParam != 0) {
        std::wstring* pPath = reinterpret_cast<std::wstring*>(tvi.lParam);
        if (pPath && !pPath->empty()) {
            OnNavigate(*pPath, true);
        }
    }
}

void ExplorerWindow::OnSearchChanged() {
    PopulateListView();
    UpdateStatusBar();
    UpdateDetailsPanel();
}

void ExplorerWindow::OnAddressEnter() {
    wchar_t pathBuf[MAX_PATH] = { 0 };
    GetWindowTextW(m_hAddressEdit, pathBuf, MAX_PATH);
    OnNavigate(pathBuf, true);
}

void ExplorerWindow::OnNewFolder() {
    if (m_currentPath.empty()) return;

    std::wstring baseName = L"New Folder";
    std::wstring folderName = baseName;
    int counter = 2;

    std::wstring targetPath = m_currentPath;
    if (targetPath.back() != L'\\') targetPath += L"\\";
    targetPath += folderName;

    while (!CreateDirectoryW(targetPath.c_str(), NULL)) {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_EXISTS) {
            folderName = baseName + L" (" + std::to_wstring(counter++) + L")";
            targetPath = m_currentPath;
            if (targetPath.back() != L'\\') targetPath += L"\\";
            targetPath += folderName;
            if (counter > 50) {
                MessageBoxW(m_hWnd, L"Unable to create new folder (limit reached).", L"Error", MB_ICONERROR | MB_OK);
                return;
            }
        } else {
            MessageBoxW(m_hWnd, L"Failed to create new folder. Check folder permissions.", L"Create Folder Error", MB_ICONERROR | MB_OK);
            return;
        }
    }

    OnNavigate(m_currentPath, false);
}

void ExplorerWindow::OnRename() {
    int selIndex = ListView_GetNextItem(m_hMainListView, -1, LVNI_SELECTED);
    if (selIndex < 0 || selIndex >= static_cast<int>(m_displayedItems.size())) {
        MessageBoxW(m_hWnd, L"Please select an item to rename.", L"Rename", MB_ICONINFORMATION | MB_OK);
        return;
    }

    ListView_EditLabel(m_hMainListView, selIndex);
}

void ExplorerWindow::OnDelete() {
    int selIndex = ListView_GetNextItem(m_hMainListView, -1, LVNI_SELECTED);
    if (selIndex < 0 || selIndex >= static_cast<int>(m_displayedItems.size())) {
        MessageBoxW(m_hWnd, L"Please select an item to delete.", L"Delete", MB_ICONINFORMATION | MB_OK);
        return;
    }

    const auto& item = m_displayedItems[selIndex];
    std::wstring prompt = L"Are you sure you want to move '" + item.name + L"' to the Recycle Bin?";
    if (MessageBoxW(m_hWnd, prompt.c_str(), L"Confirm Delete", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        wchar_t fromPath[MAX_PATH + 2] = { 0 };
        wcsncpy_s(fromPath, item.fullPath.c_str(), _TRUNCATE);
        fromPath[wcslen(fromPath) + 1] = L'\0'; // Double null termination for SHFileOperation

        SHFILEOPSTRUCTW fileOp = { 0 };
        fileOp.hwnd = m_hWnd;
        fileOp.wFunc = FO_DELETE;
        fileOp.pFrom = fromPath;
        fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

        if (SHFileOperationW(&fileOp) == 0 && !fileOp.fAnyOperationsAborted) {
            OnNavigate(m_currentPath, false);
        } else {
            MessageBoxW(m_hWnd, L"Failed to delete the selected item.", L"Error", MB_ICONERROR | MB_OK);
        }
    }
}

void ExplorerWindow::OnCopy() {
    int selIndex = ListView_GetNextItem(m_hMainListView, -1, LVNI_SELECTED);
    if (selIndex >= 0 && selIndex < static_cast<int>(m_displayedItems.size())) {
        m_clipboardPath = m_displayedItems[selIndex].fullPath;
        m_isCutOperation = false;
        SendMessageW(m_hStatusBar, SB_SETTEXTW, 1, (LPARAM)L"Item copied to clipboard.");
    }
}

void ExplorerWindow::OnCut() {
    int selIndex = ListView_GetNextItem(m_hMainListView, -1, LVNI_SELECTED);
    if (selIndex >= 0 && selIndex < static_cast<int>(m_displayedItems.size())) {
        m_clipboardPath = m_displayedItems[selIndex].fullPath;
        m_isCutOperation = true;
        SendMessageW(m_hStatusBar, SB_SETTEXTW, 1, (LPARAM)L"Item cut to clipboard.");
    }
}

void ExplorerWindow::OnPaste() {
    if (!CanPaste()) {
        MessageBoxW(m_hWnd, L"Clipboard is empty or file no longer exists.", L"Paste", MB_ICONINFORMATION | MB_OK);
        return;
    }

    wchar_t fileName[MAX_PATH] = { 0 };
    wcsncpy_s(fileName, m_clipboardPath.c_str(), _TRUNCATE);
    PathStripPathW(fileName);

    std::wstring destPath = m_currentPath;
    if (destPath.back() != L'\\') destPath += L"\\";
    destPath += fileName;

    bool success = false;
    if (m_isCutOperation) {
        success = (MoveFileW(m_clipboardPath.c_str(), destPath.c_str()) != 0);
        if (success) {
            m_clipboardPath.clear();
            m_isCutOperation = false;
        }
    } else {
        DWORD attrs = GetFileAttributesW(m_clipboardPath.c_str());
        if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY)) {
            wchar_t srcBuf[MAX_PATH + 2] = { 0 };
            wchar_t dstBuf[MAX_PATH + 2] = { 0 };
            wcsncpy_s(srcBuf, m_clipboardPath.c_str(), _TRUNCATE);
            wcsncpy_s(dstBuf, destPath.c_str(), _TRUNCATE);

            SHFILEOPSTRUCTW sfo = { 0 };
            sfo.hwnd = m_hWnd;
            sfo.wFunc = FO_COPY;
            sfo.pFrom = srcBuf;
            sfo.pTo = dstBuf;
            sfo.fFlags = FOF_NOCONFIRMMKDIR | FOF_SILENT;
            success = (SHFileOperationW(&sfo) == 0 && !sfo.fAnyOperationsAborted);
        } else {
            success = (CopyFileW(m_clipboardPath.c_str(), destPath.c_str(), FALSE) != 0);
        }
    }

    if (success) {
        OnNavigate(m_currentPath, false);
        SendMessageW(m_hStatusBar, SB_SETTEXTW, 1, (LPARAM)L"Item pasted successfully.");
    } else {
        MessageBoxW(m_hWnd, L"Failed to paste item. Check destination and permissions.", L"Paste Error", MB_ICONERROR | MB_OK);
    }
}

void ExplorerWindow::OnProperties() {
    int selIndex = ListView_GetNextItem(m_hMainListView, -1, LVNI_SELECTED);
    std::wstring targetPath = (selIndex >= 0 && selIndex < static_cast<int>(m_displayedItems.size()))
        ? m_displayedItems[selIndex].fullPath
        : m_currentPath;

    if (!targetPath.empty()) {
        SHELLEXECUTEINFOW sei = { 0 };
        sei.cbSize = sizeof(sei);
        sei.fMask = SEE_MASK_INVOKEIDLIST;
        sei.hwnd = m_hWnd;
        sei.lpVerb = L"properties";
        sei.lpFile = targetPath.c_str();
        sei.nShow = SW_SHOW;
        ShellExecuteExW(&sei);
    }
}

void ExplorerWindow::OnFolderStats() {
    int selIndex = ListView_GetNextItem(m_hMainListView, -1, LVNI_SELECTED);
    std::wstring targetPath = (selIndex >= 0 && selIndex < static_cast<int>(m_displayedItems.size()) && m_displayedItems[selIndex].isDirectory)
        ? m_displayedItems[selIndex].fullPath
        : m_currentPath;

    if (targetPath.empty()) return;

    size_t fileCount = 0;
    size_t dirCount = 0;
    uint64_t totalBytes = 0;

    for (const auto& item : m_currentItems) {
        if (item.isDirectory) {
            dirCount++;
        } else {
            fileCount++;
            totalBytes += item.rawSize;
        }
    }

    std::wstringstream ss;
    ss << L"=== Folder Statistics (Phase 2 UI Overview) ===\n\n";
    ss << L"Target Folder: " << targetPath << L"\n\n";
    ss << L"• Immediate Files: " << fileCount << L"\n";
    ss << L"• Immediate Directories: " << dirCount << L"\n";
    ss << L"• Immediate Total Size: " << FormatFileSize(totalBytes) << L" (" << totalBytes << L" bytes)\n\n";
    ss << L"• Current View Items: " << m_displayedItems.size() << L" item(s)\n\n";
    ss << L"[Phase 3 Backend Preview]:\n";
    ss << L"Full recursive DFS & BFS tree traversal engine (with custom generic Stack ADT, "
          L"circular Queue ADT, and depth analysis) will be connected during Phase 3.";

    MessageBoxW(m_hWnd, ss.str().c_str(), L"Folder Statistics", MB_ICONINFORMATION | MB_OK);
}

void ExplorerWindow::OnToggleViewMode() {
    DWORD currentStyle = GetWindowLongW(m_hMainListView, GWL_STYLE) & LVS_TYPEMASK;
    DWORD newStyle = LVS_REPORT;

    if (currentStyle == LVS_REPORT) {
        newStyle = LVS_ICON;
    } else if (currentStyle == LVS_ICON) {
        newStyle = LVS_LIST;
    } else {
        newStyle = LVS_REPORT;
    }

    SetWindowLongW(m_hMainListView, GWL_STYLE, (GetWindowLongW(m_hMainListView, GWL_STYLE) & ~LVS_TYPEMASK) | newStyle);
    InvalidateRect(m_hMainListView, NULL, TRUE);
}

void ExplorerWindow::OnToggleTheme() {
    m_themeManager.ToggleTheme(m_hWnd);
    InvalidateRect(m_hWnd, NULL, TRUE);
    InvalidateRect(m_hMainListView, NULL, TRUE);
    InvalidateRect(m_hSidebarTree, NULL, TRUE);
    InvalidateRect(m_hDetailsPanel, NULL, TRUE);
}

void ExplorerWindow::OnToggleDetails() {
    m_showDetailsPanel = !m_showDetailsPanel;
    LayoutControls();
}

void ExplorerWindow::OnHelpShortcuts() {
    std::wstring helpText =
        L"C++ Based File Explorer — Quick Guide & Keyboard Shortcuts\n"
        L"==========================================================\n\n"
        L"Navigation Shortcuts:\n"
        L"  • Alt + Left Arrow    : Go Back (History Stack)\n"
        L"  • Alt + Right Arrow   : Go Forward (History Stack)\n"
        L"  • Backspace / Alt+Up  : Go Up to Parent Folder\n"
        L"  • F5                  : Refresh Current Directory\n"
        L"  • Ctrl + L            : Jump to Address Bar\n"
        L"  • Ctrl + F            : Jump to Search Box\n\n"
        L"File & Folder Actions:\n"
        L"  • Ctrl + N            : Create New Folder\n"
        L"  • F2                  : Rename Selected File/Folder\n"
        L"  • Delete              : Move Selected Item to Recycle Bin\n"
        L"  • Ctrl + C            : Copy Selected Item\n"
        L"  • Ctrl + X            : Cut Selected Item\n"
        L"  • Ctrl + V            : Paste from Clipboard\n"
        L"  • Ctrl + A            : Select All Items\n"
        L"  • Alt + Enter         : View Windows File Properties\n\n"
        L"View & Tools:\n"
        L"  • View Mode Button    : Switch Details / Icons / List\n"
        L"  • Details Button      : Toggle Right Details Panel\n"
        L"  • View -> Theme       : Toggle Dark / Light Theme\n"
        L"  • Tools Menu          : Folder Statistics (UI Overview)\n"
        L"  • F1                  : Open this Help dialog";

    MessageBoxW(m_hWnd, helpText.c_str(), L"Keyboard Shortcuts & Guide (F1)", MB_ICONINFORMATION | MB_OK);
}

void ExplorerWindow::OnAbout() {
    std::wstring aboutMsg =
        L"C++ Based File Explorer (Phase 2 Frontend Release)\n"
        L"Native Windows Desktop Application (Win32 API & Common Controls v6)\n"
        L"B.Tech 2nd-Year Project (DSA & OOP)\n\n"
        L"Phase 2 Frontend Highlights:\n"
        L"• Win32 Responsive Layout with Split-Pane Architecture\n"
        L"• Navigation Toolbar (Back, Forward, Up, Refresh, Address, Search)\n"
        L"• TreeView Quick Access & Logical Drive Explorer\n"
        L"• Multi-Column ListView with System Icons (Details, Icons, List)\n"
        L"• Interactive Details Inspector & Dual-part Status Bar\n"
        L"• Immersive Windows 11 Dark / Light Theme Switching\n"
        L"• Keyboard Accelerators & Context Menus\n\n"
        L"Phase 3 Integration:\n"
        L"• Custom Generic Stack & Circular Queue ADTs, Deep DFS/BFS Directory Traversal,\n"
        L"  and Custom Sorting Algorithms will be integrated in Phase 3.\n\n"
        L"Team Size: 4 Members (Division: Exactly 2 files per member)\n"
        L"Status: Phase 2 Frontend 100% Operational.";

    MessageBoxW(m_hWnd, aboutMsg.c_str(), L"About C++ Based File Explorer", MB_ICONINFORMATION | MB_OK);
}

void ExplorerWindow::OnContextMenu(int screenX, int screenY) {
    POINT pt = { screenX, screenY };
    if (screenX == -1 && screenY == -1) {
        GetCursorPos(&pt);
    }

    HMENU hMenu = CreatePopupMenu();
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CTX_OPEN, L"&Open");
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CTX_CUT, L"Cu&t\tCtrl+X");
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CTX_COPY, L"&Copy\tCtrl+C");
    InsertMenuW(hMenu, -1, MF_BYPOSITION | (CanPaste() ? MF_STRING : (MF_STRING | MF_GRAYED)),
                IDM_CTX_PASTE, L"&Paste\tCtrl+V");
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CTX_RENAME, L"&Rename\tF2");
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CTX_DELETE, L"&Delete\tDel");
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CTX_FOLDER_STATS, L"&Folder Statistics...");
    InsertMenuW(hMenu, -1, MF_BYPOSITION | MF_STRING, IDM_CTX_PROPERTIES, L"&Properties\tAlt+Enter");

    TrackPopupMenuEx(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd, NULL);
    DestroyMenu(hMenu);
}

LRESULT CALLBACK ExplorerWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ExplorerWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        pThis = reinterpret_cast<ExplorerWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->m_hWnd = hWnd;
    } else {
        pThis = reinterpret_cast<ExplorerWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT ExplorerWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        m_themeManager.Init(m_hWnd);
        CreateChildControls();
        LayoutControls();
        return 0;
    }

    case WM_SIZE: {
        LayoutControls();
        return 0;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        if (wmId == IDC_SEARCH_BOX && wmEvent == EN_CHANGE) {
            OnSearchChanged();
            return 0;
        }

        switch (wmId) {
        case IDC_NAV_BACK:
            if (CanGoBack()) {
                OnNavigate(GoBack(), false);
            }
            break;
        case IDC_NAV_FORWARD:
            if (CanGoForward()) {
                OnNavigate(GoForward(), false);
            }
            break;
        case IDC_NAV_UP:
            if (CanGoUp()) {
                OnNavigate(GoUp(), false);
            }
            break;
        case IDC_NAV_REFRESH:
        case IDM_VIEW_REFRESH:
            OnNavigate(m_currentPath, false);
            break;
        case IDC_GO_BUTTON:
            OnAddressEnter();
            break;
        case IDC_BTN_NEWFOLDER:
        case IDM_FILE_NEWFOLDER:
            OnNewFolder();
            break;
        case IDC_BTN_VIEW_MODE:
            OnToggleViewMode();
            break;
        case IDM_VIEW_DETAILS:
            SetWindowLongW(m_hMainListView, GWL_STYLE, (GetWindowLongW(m_hMainListView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_REPORT);
            InvalidateRect(m_hMainListView, NULL, TRUE);
            break;
        case IDM_VIEW_LARGEICONS:
            SetWindowLongW(m_hMainListView, GWL_STYLE, (GetWindowLongW(m_hMainListView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_ICON);
            InvalidateRect(m_hMainListView, NULL, TRUE);
            break;
        case IDM_VIEW_LIST:
            SetWindowLongW(m_hMainListView, GWL_STYLE, (GetWindowLongW(m_hMainListView, GWL_STYLE) & ~LVS_TYPEMASK) | LVS_LIST);
            InvalidateRect(m_hMainListView, NULL, TRUE);
            break;
        case IDC_BTN_TOGGLE_DETAILS:
        case IDM_VIEW_TOGGLE_DETAILS:
            OnToggleDetails();
            break;
        case IDM_VIEW_TOGGLE_THEME:
            OnToggleTheme();
            break;
        case IDM_TOOLS_FOLDER_STATS:
        case IDM_CTX_FOLDER_STATS:
            OnFolderStats();
            break;
        case IDM_TOOLS_OPEN_CMD: {
            std::wstring currentPath = m_currentPath;
            if (currentPath.empty()) currentPath = L"C:\\";
            ShellExecuteW(m_hWnd, L"open", L"cmd.exe", (L"/k cd /d \"" + currentPath + L"\"").c_str(), currentPath.c_str(), SW_SHOW);
            break;
        }
        case IDC_BTN_HELP:
        case IDM_HELP_SHORTCUTS:
            OnHelpShortcuts();
            break;
        case IDM_HELP_ABOUT:
            OnAbout();
            break;
        case IDM_FILE_RENAME:
        case IDM_CTX_RENAME:
            OnRename();
            break;
        case IDM_FILE_DELETE:
        case IDM_CTX_DELETE:
            OnDelete();
            break;
        case IDM_EDIT_COPY:
        case IDM_CTX_COPY:
            OnCopy();
            break;
        case IDM_EDIT_CUT:
        case IDM_CTX_CUT:
            OnCut();
            break;
        case IDM_EDIT_PASTE:
        case IDM_CTX_PASTE:
            OnPaste();
            break;
        case IDM_FILE_PROPERTIES:
        case IDM_CTX_PROPERTIES:
            OnProperties();
            break;
        case IDM_CTX_OPEN:
            OnListViewDoubleClick();
            break;
        case IDM_FILE_EXIT:
            DestroyWindow(m_hWnd);
            break;
        }
        return 0;
    }

    case WM_NOTIFY: {
        NMHDR* pnmh = reinterpret_cast<NMHDR*>(lParam);
        if (!pnmh) break;

        if (pnmh->idFrom == IDC_MAIN_LISTVIEW) {
            switch (pnmh->code) {
            case NM_DBLCLK:
                OnListViewDoubleClick();
                return 0;
            case LVN_ITEMCHANGED:
                OnListViewSelectionChanged();
                return 0;
            case LVN_COLUMNCLICK: {
                NMLISTVIEW* pnmv = reinterpret_cast<NMLISTVIEW*>(lParam);
                OnListViewColumnClick(pnmv->iSubItem);
                return 0;
            }
            case LVN_ENDLABELEDITW: {
                NMLVDISPINFOW* pDisp = reinterpret_cast<NMLVDISPINFOW*>(lParam);
                if (pDisp->item.pszText != NULL && wcslen(pDisp->item.pszText) > 0) {
                    int itemIdx = pDisp->item.iItem;
                    if (itemIdx >= 0 && itemIdx < static_cast<int>(m_displayedItems.size())) {
                        std::wstring oldPath = m_displayedItems[itemIdx].fullPath;
                        std::wstring parentDir = m_currentPath;
                        if (parentDir.back() != L'\\') parentDir += L"\\";
                        std::wstring newPath = parentDir + pDisp->item.pszText;

                        if (MoveFileW(oldPath.c_str(), newPath.c_str())) {
                            OnNavigate(m_currentPath, false);
                            return TRUE;
                        } else {
                            MessageBoxW(m_hWnd, L"Rename failed. A file with this name may already exist.", L"Rename Failed", MB_ICONERROR | MB_OK);
                        }
                    }
                }
                return FALSE;
            }
            }
        } else if (pnmh->idFrom == IDC_SIDEBAR_TREE) {
            if (pnmh->code == TVN_SELCHANGEDW) {
                NMTREEVIEWW* pnmtv = reinterpret_cast<NMTREEVIEWW*>(lParam);
                OnTreeSelectionChanged(pnmtv);
                return 0;
            }
        }
        break;
    }

    case WM_CONTEXTMENU: {
        HWND hTarget = reinterpret_cast<HWND>(wParam);
        if (hTarget == m_hMainListView) {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            OnContextMenu(x, y);
            return 0;
        }
        break;
    }

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC: {
        HDC hdc = reinterpret_cast<HDC>(wParam);
        HWND hCtrl = reinterpret_cast<HWND>(lParam);

        if (hCtrl == m_hDetailsPanel) {
            SetTextColor(hdc, m_themeManager.GetTextColor());
            SetBkColor(hdc, m_themeManager.GetListColor());
            return reinterpret_cast<LRESULT>(m_themeManager.GetListBrush());
        }
        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProcW(m_hWnd, uMsg, wParam, lParam);
}
