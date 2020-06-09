#pragma once

void GUI_Init();
//void GUI_Shutdown();
void GUI_Render();
int GUI_Event(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
void GUI_Release();
void GUI_ToggleHelp();
#ifdef USE_NUKLEAR_INPUT
void GUI_ShowCursor(bool bShow);
void GUI_RenderInput(char* szBuffer, int iMax, float fY);
#endif
