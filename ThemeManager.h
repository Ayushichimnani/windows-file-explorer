#pragma once
#include <windows.h>

/**
 * @file ThemeManager.h
 * @brief Light / Dark theme management, palettes, and typography.
 * @author Member 1 (Main Window & Theme Module)
 */
enum class AppTheme {
    LIGHT,
    DARK
};

class ThemeManager {
private:
    AppTheme m_currentTheme;

    COLORREF m_lightBg;
    COLORREF m_lightToolbar;
    COLORREF m_lightSidebar;
    COLORREF m_lightList;
    COLORREF m_lightBorder;
    COLORREF m_lightText;
    COLORREF m_lightTextSecondary;
    COLORREF m_lightAccent;
    COLORREF m_lightSelection;

    COLORREF m_darkBg;
    COLORREF m_darkToolbar;
    COLORREF m_darkSidebar;
    COLORREF m_darkList;
    COLORREF m_darkBorder;
    COLORREF m_darkText;
    COLORREF m_darkTextSecondary;
    COLORREF m_darkAccent;
    COLORREF m_darkSelection;

    HBRUSH m_hbrBg;
    HBRUSH m_hbrToolbar;
    HBRUSH m_hbrSidebar;
    HBRUSH m_hbrList;
    HPEN   m_hpenBorder;
    HFONT  m_hFontRegular;
    HFONT  m_hFontBold;
    HFONT  m_hFontDetails;

    void CreateGDIObjects();
    void DeleteGDIObjects();

public:
    ThemeManager();
    ~ThemeManager();

    void Init(HWND hWnd);
    void ToggleTheme(HWND hWnd);
    void SetTheme(AppTheme theme, HWND hWnd);
    AppTheme GetTheme() const noexcept { return m_currentTheme; }

    COLORREF GetBgColor() const noexcept;
    COLORREF GetToolbarColor() const noexcept;
    COLORREF GetSidebarColor() const noexcept;
    COLORREF GetListColor() const noexcept;
    COLORREF GetBorderColor() const noexcept;
    COLORREF GetTextColor() const noexcept;
    COLORREF GetSecondaryTextColor() const noexcept;
    COLORREF GetAccentColor() const noexcept;
    COLORREF GetSelectionColor() const noexcept;

    HBRUSH GetBgBrush() const noexcept { return m_hbrBg; }
    HBRUSH GetToolbarBrush() const noexcept { return m_hbrToolbar; }
    HBRUSH GetSidebarBrush() const noexcept { return m_hbrSidebar; }
    HBRUSH GetListBrush() const noexcept { return m_hbrList; }
    HPEN   GetBorderPen() const noexcept { return m_hpenBorder; }

    HFONT  GetRegularFont() const noexcept { return m_hFontRegular; }
    HFONT  GetBoldFont() const noexcept { return m_hFontBold; }
    HFONT  GetDetailsFont() const noexcept { return m_hFontDetails; }
};
