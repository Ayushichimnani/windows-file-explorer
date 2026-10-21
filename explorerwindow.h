#pragma once
#include <windows.h>
#include <commctrl.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <memory>
#include "Resource.h"
#include "ThemeManager.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "shlwapi.lib")

/**
 * @file ExplorerWindow.h
 * @brief Main Application Window and Layout Controller (Phase 2 Frontend Release).
 * @author Member 1 (Main Window & Layout Module)
 */

// Lightweight representation for explorer items (Self-contained Frontend)
struct ExplorerItem {
    std::wstring name;
    std::wstring fullPath;
    std::wstring typeDescription;
    std::wstring formattedSize;
    std::wstring formattedDate;
    uint64_t rawSize = 0;
    FILETIME lastWriteTime = { 0, 0 };
    bool isDirectory = false;
    int iconIndex = 0;
};

class ExplorerWindow {
private:
    HWND m_hWnd;
    HINSTANCE m_hInstance;

    // Navigation Controls
    HWND m_hBtnBack;
    HWND m_hBtnForward;
    HWND m_hBtnUp;
    HWND m_hBtnRefresh;
    HWND m_hAddressEdit;
    HWND m_hBtnGo;
    HWND m_hSearchEdit;

    // Action Controls
    HWND m_hBtnNewFolder;
    HWND m_hBtnViewMode;
    HWND m_hBtnDetails;
    HWND m_hBtnHelp;

    // Main Panels
    HWND m_hSidebarTree;
    HWND m_hMainListView;
    HWND m_hDetailsPanel;
    HWND m_hStatusBar;
    HWND m_hToolTip;

    // Fonts
    HFONT m_hNavFont;

    int m_sidebarWidth;
    int m_detailsWidth;
    bool m_showDetailsPanel;
    DWORD m_currentViewStyle;

    // Theme Module
    ThemeManager m_themeManager;

    // Self-contained Navigation History (LIFO Stacks)
    std::wstring m_currentPath;
    std::vector<std::wstring> m_backStack;
    std::vector<std::wstring> m_forwardStack;

    // Loaded Directory Items
    std::vector<ExplorerItem> m_currentItems;
    std::vector<ExplorerItem> m_displayedItems;

    // Sorting state (0=Name, 1=Date, 2=Type, 3=Size)
    int m_sortColumn;
    bool m_sortAscending;

    // Clipboard state
    std::wstring m_clipboardPath;
    bool m_isCutOperation;

    // Sidebar Path Storage (to safely manage TreeView item lifetimes)
    std::vector<std::unique_ptr<std::wstring>> m_sidebarPaths;

    // System Image Lists
    HIMAGELIST m_hSysImageListSmall;
    HIMAGELIST m_hSysImageListLarge;

    // Internal Helpers
    void CreateChildControls();
    void SetupTooltips();
    void AddTooltipForControl(HWND hControl, const wchar_t* tipText);
    void SetupSystemImageLists();
    void PopulateSidebar();
    void LayoutControls();
    void PopulateListView();
    void UpdateNavigationControls();
    void UpdateStatusBar();
    void UpdateDetailsPanel();

    // Frontend File Operations & Formatting
    bool LoadDirectory(const std::wstring& folderPath, std::wstring& errorMsg);
    void SortDisplayedItems();
    static std::wstring NormalizePath(const std::wstring& path);
    static std::wstring FormatFileSize(uint64_t bytes);
    static std::wstring FormatFileTime(const FILETIME& ft);
    static std::wstring GetKnownFolder(REFKNOWNFOLDERID rfid);

    // Navigation Stacks Helpers
    bool CanGoBack() const noexcept { return !m_backStack.empty(); }
    bool CanGoForward() const noexcept { return !m_forwardStack.empty(); }
    bool CanGoUp() const noexcept;
    std::wstring GoBack();
    std::wstring GoForward();
    std::wstring GoUp();
    bool CanPaste() const noexcept {
        return !m_clipboardPath.empty() && (GetFileAttributesW(m_clipboardPath.c_str()) != INVALID_FILE_ATTRIBUTES);
    }

    // Action Handlers
    void OnNavigate(const std::wstring& path, bool recordHistory = true);
    void OnListViewDoubleClick();
    void OnListViewSelectionChanged();
    void OnListViewColumnClick(int columnIndex);
    void OnTreeSelectionChanged(LPNMTREEVIEWW pNmtv);
    void OnSearchChanged();
    void OnAddressEnter();
    void OnContextMenu(int screenX, int screenY);
    void OnNewFolder();
    void OnRename();
    void OnDelete();
    void OnCopy();
    void OnCut();
    void OnPaste();
    void OnProperties();
    void OnFolderStats();
    void OnToggleViewMode();
    void OnToggleTheme();
    void OnToggleDetails();
    void OnHelpShortcuts();
    void OnAbout();

public:
    ExplorerWindow();
    ~ExplorerWindow();

    static bool Register(HINSTANCE hInstance);
    bool Create(HINSTANCE hInstance, int nCmdShow);
    HWND GetHwnd() const noexcept { return m_hWnd; }

    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};
