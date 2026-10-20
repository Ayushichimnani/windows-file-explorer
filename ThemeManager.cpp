#include "ThemeManager.h"
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

ThemeManager::ThemeManager()
    : m_currentTheme(AppTheme::LIGHT)
    , m_lightBg(RGB(255, 255, 255))
    , m_lightToolbar(RGB(243, 243, 243))
    , m_lightSidebar(RGB(249, 249, 249))
    , m_lightList(RGB(255, 255, 255))
    , m_lightBorder(RGB(229, 229, 229))
    , m_lightText(RGB(32, 32, 32))
    , m_lightTextSecondary(RGB(96, 96, 96))
    , m_lightAccent(RGB(0, 120, 212))
    , m_lightSelection(RGB(224, 238, 249))
    , m_darkBg(RGB(25, 25, 25))
    , m_darkToolbar(RGB(32, 32, 32))
    , m_darkSidebar(RGB(28, 28, 28))
    , m_darkList(RGB(30, 30, 30))
    , m_darkBorder(RGB(50, 50, 50))
    , m_darkText(RGB(240, 240, 240))
    , m_darkTextSecondary(RGB(160, 160, 160))
    , m_darkAccent(RGB(0, 120, 212))
    , m_darkSelection(RGB(45, 60, 80))
    , m_hbrBg(NULL)
    , m_hbrToolbar(NULL)
    , m_hbrSidebar(NULL)
    , m_hbrList(NULL)
    , m_hpenBorder(NULL)
    , m_hFontRegular(NULL)
    , m_hFontBold(NULL)
    , m_hFontDetails(NULL) {
    CreateGDIObjects();
}

ThemeManager::~ThemeManager() {
    DeleteGDIObjects();
}

void ThemeManager::CreateGDIObjects() {
    DeleteGDIObjects();

    m_hbrBg = CreateSolidBrush(GetBgColor());
    m_hbrToolbar = CreateSolidBrush(GetToolbarColor());
    m_hbrSidebar = CreateSolidBrush(GetSidebarColor());
    m_hbrList = CreateSolidBrush(GetListColor());
    m_hpenBorder = CreatePen(PS_SOLID, 1, GetBorderColor());

    m_hFontRegular = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    m_hFontBold = CreateFontW(-12, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                              DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                              CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    m_hFontDetails = CreateFontW(-14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                 CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
}

void ThemeManager::DeleteGDIObjects() {
    if (m_hbrBg) { DeleteObject(m_hbrBg); m_hbrBg = NULL; }
    if (m_hbrToolbar) { DeleteObject(m_hbrToolbar); m_hbrToolbar = NULL; }
    if (m_hbrSidebar) { DeleteObject(m_hbrSidebar); m_hbrSidebar = NULL; }
    if (m_hbrList) { DeleteObject(m_hbrList); m_hbrList = NULL; }
    if (m_hpenBorder) { DeleteObject(m_hpenBorder); m_hpenBorder = NULL; }
    if (m_hFontRegular) { DeleteObject(m_hFontRegular); m_hFontRegular = NULL; }
    if (m_hFontBold) { DeleteObject(m_hFontBold); m_hFontBold = NULL; }
    if (m_hFontDetails) { DeleteObject(m_hFontDetails); m_hFontDetails = NULL; }
}

void ThemeManager::Init(HWND hWnd) {
    SetTheme(AppTheme::LIGHT, hWnd);
}

void ThemeManager::ToggleTheme(HWND hWnd) {
    SetTheme(m_currentTheme == AppTheme::LIGHT ? AppTheme::DARK : AppTheme::LIGHT, hWnd);
}

void ThemeManager::SetTheme(AppTheme theme, HWND hWnd) {
    m_currentTheme = theme;
    CreateGDIObjects();

    BOOL isDark = (theme == AppTheme::DARK);
    DwmSetWindowAttribute(hWnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &isDark, sizeof(isDark));

    InvalidateRect(hWnd, NULL, TRUE);
}

COLORREF ThemeManager::GetBgColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightBg : m_darkBg;
}

COLORREF ThemeManager::GetToolbarColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightToolbar : m_darkToolbar;
}

COLORREF ThemeManager::GetSidebarColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightSidebar : m_darkSidebar;
}

COLORREF ThemeManager::GetListColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightList : m_darkList;
}

COLORREF ThemeManager::GetBorderColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightBorder : m_darkBorder;
}

COLORREF ThemeManager::GetTextColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightText : m_darkText;
}

COLORREF ThemeManager::GetSecondaryTextColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightTextSecondary : m_darkTextSecondary;
}

COLORREF ThemeManager::GetAccentColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightAccent : m_darkAccent;
}

COLORREF ThemeManager::GetSelectionColor() const noexcept {
    return (m_currentTheme == AppTheme::LIGHT) ? m_lightSelection : m_darkSelection;
}
