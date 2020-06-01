
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

void GUI_Init()
{
	ctx = nk_d3d9_init(pD3DDevice, pGame->GetScreenWidth(), pGame->GetScreenHeight());

	struct nk_font_atlas* atlas;
	nk_d3d9_font_stash_begin(&atlas);
    //struct nk_font* droid = nk_font_atlas_add_from_file(atlas, "extra_font/DroidSans.ttf", 14, 0);
	nk_d3d9_font_stash_end();
    //nk_style_set_font(ctx, &droid->handle);
}

void GUI_Shutdown()
{
	nk_d3d9_shutdown();
}

void GUI_ShowHelp();
void GUI_Render()
{
    nk_d3d9_resize(pGame->GetScreenWidth(), pGame->GetScreenHeight());

    if(bRenderHelp) GUI_ShowHelp();

    nk_input_begin(ctx);
	nk_d3d9_render(NK_ANTI_ALIASING_OFF);
    nk_input_end(ctx);
}

int GUI_Event(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    return nk_d3d9_handle_event(wnd, msg, wparam, lparam);
}

void GUI_Release()
{
	nk_d3d9_release();
}

void GUI_ToggleHelp()
{
    bRenderHelp = !bRenderHelp;
}

void GUI_ShowHelp()
{
    int x = (pGame->GetScreenWidth() - 430) / 2,
        y = (pGame->GetScreenHeight() - 250) / 2;

    if (nk_begin(ctx, "SA:MP Key Binds", nk_rect(x, y, 430, 250),
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