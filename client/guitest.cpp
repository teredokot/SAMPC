
#include "main.h"

#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING

#define NK_INCLUDE_DEFAULT_FONT
#define NK_INCLUDE_FIXED_TYPES
#define NK_D3D9_IMPLEMENTATION
#define NK_IMPLEMENTATION
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_d3d9.h>

static struct nk_context* ctx;

static bool bRenderHelp = false;
static int w, h;

void GUI_Init()
{
	w = pGame->GetScreenWidth();
	h = pGame->GetScreenHeight();
	ctx = nk_d3d9_init(pD3DDevice, w, h);

	struct nk_font_atlas* atlas;
	nk_d3d9_font_stash_begin(&atlas);
	//struct nk_font* droid = nk_font_atlas_add_from_file(atlas, "extra_font/DroidSans.ttf", 14, 0);
	nk_d3d9_font_stash_end();
	//nk_style_set_font(ctx, &droid->handle);
#ifdef USE_NUKLEAR_INPUT 
	nk_style_load_all_cursors(ctx, atlas->cursors);
	nk_style_hide_cursor(ctx);
#endif
}

/*void GUI_Shutdown()
{
	nk_d3d9_shutdown();
}*/

int GUI_Event(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return nk_d3d9_handle_event(wnd, msg, wparam, lparam);
}

void GUI_Release()
{
	nk_d3d9_release();
}

#ifdef USE_NUKLEAR_INPUT
void GUI_ShowCursor(bool bShow)
{
	if (bShow)
		nk_style_show_cursor(ctx);
	else
		nk_style_hide_cursor(ctx);
}
#endif

void GUI_ToggleHelp()
{
	bRenderHelp = !bRenderHelp;
}

#ifdef USE_NUKLEAR_INPUT
void GUI_RenderInput(char* szBuffer, int iMax, float fY)
{
	if (nk_begin(ctx, "Input", nk_rect(20, fY, 420, 40), NK_WINDOW_NO_SCROLLBAR)) { 
		nk_flags flags = 0;
		nk_layout_row_dynamic(ctx, 30, 1); // BUG? nk_edit_string() will not add it to window until something else put in front
		nk_edit_focus(ctx, 0);
		flags = nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD | NK_EDIT_SIG_ENTER | NK_EDIT_GOTO_END_ON_ACTIVATE | NK_EDIT_NO_HORIZONTAL_SCROLL, szBuffer, iMax, 0);
		if ((flags & NK_EDIT_COMMITED)) {
			if(pCmdWindow) pCmdWindow->ProcessInput(szBuffer);
			szBuffer[0] = '\0';
		}
	}
	nk_end(ctx);
}
#endif

void GUI_ShowHelp()
{
	float x = (float)(w - 430) / 2,
		y = (float)(h - 250) / 2;

	if (nk_begin(ctx, "SA:MP Key Binds", nk_rect(x, y, 430.0f, 250.0f),
		NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
	{
		const float ratio[] = { 50, 350 };
		const nk_color keycolor = nk_rgb(255, 255, 255);

		nk_layout_row(ctx, NK_STATIC, 15, 2, ratio);
		nk_label_colored(ctx, "F1", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Display this help dialog", NK_TEXT_LEFT);

		nk_label_colored(ctx, "TAB", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Display the scoreboard", NK_TEXT_LEFT);

		nk_label_colored(ctx, "F4", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Allows you to change class next time you respawn", NK_TEXT_LEFT);

		nk_label_colored(ctx, "F5", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Show bandwidth statistics", NK_TEXT_LEFT);

		nk_label_colored(ctx, "F7", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Toggle the chat box", NK_TEXT_LEFT);

		nk_label_colored(ctx, "F8", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Take a screenshot", NK_TEXT_LEFT);

		nk_label_colored(ctx, "F9", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Toggle the deathwindow", NK_TEXT_LEFT);

		nk_label_colored(ctx, "T/F6", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Allows you to enter a chat message", NK_TEXT_LEFT);

		nk_label_colored(ctx, "G", NK_TEXT_LEFT, keycolor);
		nk_label(ctx, "Enter vehicle as passenger", NK_TEXT_LEFT);
	}
	nk_end(ctx);
}

void GUI_Render()
{
	w = pGame->GetScreenWidth();
	h = pGame->GetScreenHeight();
	nk_d3d9_resize(w, h);

	if (bRenderHelp) GUI_ShowHelp();

	nk_input_begin(ctx);
	nk_d3d9_render(NK_ANTI_ALIASING_OFF);
	nk_input_end(ctx);
}
