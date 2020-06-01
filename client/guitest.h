#pragma once

void GUI_Init();
void GUI_Shutdown();
void GUI_Render();
int GUI_Event(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
void GUI_Release();
void GUI_ToggleHelp();
