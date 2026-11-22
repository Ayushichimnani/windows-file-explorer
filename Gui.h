#ifndef GUI_H
#define GUI_H

#include "common.h"

LRESULT CALLBACK WindowProcedure(
    HWND window, UINT message,
    WPARAM wParam, LPARAM lParam
);

void createControls(HWND window);

#endif
