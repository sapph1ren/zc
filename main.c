/* nuklear - 1.32.0 - public domain */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_IMPLEMENTATION
#define NK_GDI_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_gdi.h"

//#include "style.c"
//#include "ui.c"
#include "main.h"

struct AppFonts {
    struct GdiFont *f14;
    struct GdiFont *f16;
    struct GdiFont *f18;
	struct GdiFont* custom;
	struct GdiFont* zc;
};

static void zc_chat(struct nk_context*ctx, int x, int y, struct AppFonts* fonts) {
	if(nk_begin(ctx, "c", nk_rect(x*0.35, 0, x*0.65, y), NK_WINDOW_BORDER)) {		


		
		nk_end(ctx);
	}
	
}

bool in_voice = false;
static void zc_voice(struct nk_context*ctx, int x, int y, struct AppFonts* fonts) {
    if (nk_begin(ctx, "v", nk_rect(0, 0, x * 0.35f, y * 0.7f), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {
        if (in_voice) {
			nk_layout_row_dynamic(ctx, y*0.03, 1);
			nk_spacing(ctx, 1);

			nk_layout_row_dynamic(ctx, y*0.03, 1);
			nk_label(ctx, "Войс чат: [название]", NK_TEXT_CENTERED);


			// if(nk_group_begin(ctx, "vu", 0)){
			nk_layout_row_dynamic(ctx, y*0.03, 1);
			nk_label(ctx, "Пользователь", NK_TEXT_LEFT);
			nk_label(ctx, "Пользователь1", NK_TEXT_LEFT);
			nk_label(ctx, "Пользователь2", NK_TEXT_LEFT);
			nk_label(ctx, "Пользователь3", NK_TEXT_LEFT);
			// nk_group_end(ctx);
			// }

			nk_layout_space_begin(ctx, NK_STATIC, y*0.07, 3);
			nk_style_push_float(ctx, &ctx->style.button.rounding, 10.0f);
			nk_layout_space_push(ctx, nk_rect(x*0.11, y*0.33, y*0.06, y*0.06));
			nk_button_label(ctx, "микр");

			nk_layout_space_push(ctx, nk_rect(x*0.11+y*0.07, y*0.33, y*0.06, y*0.06));
			nk_button_label(ctx, "науш");
			nk_style_push_style_item(ctx, &ctx->style.button.normal, nk_style_item_color(nk_rgb(237, 85, 85)));
			nk_style_push_style_item(ctx, &ctx->style.button.hover, nk_style_item_color(nk_rgb(196, 71, 71)));
			nk_style_push_style_item(ctx, &ctx->style.button.active, nk_style_item_color(nk_rgb(232, 63, 63)));
			nk_layout_space_push(ctx, nk_rect(x*0.11+y*0.14, y*0.33, y*0.06, y*0.06));
			nk_button_label(ctx, "выйт");
			nk_style_pop_style_item(ctx);
			nk_style_pop_style_item(ctx);
			nk_style_pop_style_item(ctx);
			nk_style_pop_float(ctx);
			nk_layout_space_end(ctx);
			
		}
		else {
			
			nk_layout_space_begin(ctx, NK_STATIC, y*0.7, 1);
			
			nk_layout_space_push(ctx, nk_rect(x*0.13, y*0.35, x*0.08, y*0.03));
			if (nk_button_label(ctx, "Присоединиться")){
				in_voice = true;
			}

			nk_layout_space_end(ctx);

			
		}
        nk_end(ctx);
    }
}

static void zc_chats(struct nk_context*ctx, int x, int y, struct AppFonts* fonts) {
	if(nk_begin(ctx, "s", nk_rect(0,y*0.7, x*0.35, y*0.3), NK_WINDOW_BORDER)) {
		
		
		
		nk_end(ctx);
	}
	
}

static LRESULT CALLBACK
WindowProc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam){
    switch (msg)
    {
	// case WM_CLOSE:
	// 	// ShowWindow(wnd, SW_HIDE); 
	// 	return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    if (nk_gdi_handle_event(wnd, msg, wparam, lparam))
        return 0;

    return DefWindowProcW(wnd, msg, wparam, lparam);
}

void init_fonts(struct AppFonts *f, HWND hwnd, RECT rect){
	static int y =0;
	if(GetClientRect(hwnd, &rect)){
		y = rect.bottom - rect.top;
	}
    f->f14  = nk_gdifont_create("Arial", 14);
    f->f16 = nk_gdifont_create("Arial", 16);
    f->f18   = nk_gdifont_create("Arial", 18);
    f->custom   = nk_gdifont_create("Arial", y*0.017);
    f->zc   = nk_gdifont_create("Arial", 36);
}


int main(void){
    GdiFont* font;
    struct nk_context *ctx;

    WNDCLASSW wc;
    ATOM atom;
    RECT rect = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;
    HWND hwnd;
    HDC dc;
    int running = 1;
    int needs_refresh = 1;

    /* Win32 */
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(0);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"NuklearWindowClass";
    atom = RegisterClassW(&wc);

    AdjustWindowRectEx(&rect, style, FALSE, exstyle);
    hwnd = CreateWindowExW(exstyle, wc.lpszClassName, L"Zipcord", style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, wc.hInstance, NULL);
    dc = GetDC(hwnd);
	struct AppFonts f;
	init_fonts(&f, hwnd, rect);
    ctx = nk_gdi_init(f.custom, dc, WINDOW_WIDTH, WINDOW_HEIGHT);

    /* style.c */
    /*set_style(ctx, THEME_WHITE);*/
    /*set_style(ctx, THEME_RED);*/
    /*set_style(ctx, THEME_BLUE);*/
    //set_style(ctx, THEME_DARK);
	int x = 0;
	int y = 0;
    while (running) {
        /* Input */
        MSG msg;
        nk_input_begin(ctx);
        if (needs_refresh == 0) {
            if (GetMessageW(&msg, NULL, 0, 0) <= 0)
                running = 0;
            else {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
            needs_refresh = 1;
        } else needs_refresh = 0;

        while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                running = 0;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            needs_refresh = 1;
        }
        nk_input_end(ctx);
		if(GetClientRect(hwnd, &rect)){
			x = rect.right- rect.left;
			y = rect.bottom - rect.top;
		}
		zc_voice(ctx, x, y, &f);
		zc_chats(ctx, x, y, &f);
		zc_chat(ctx, x, y, &f);
		/* GUI */
        // if (nk_begin(ctx, "Demo", nk_rect(50, 50, 200, 300), NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE| NK_WINDOW_CLOSABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)){
        //     enum {EASY, HARD};
        //     static int op = EASY;
        //     static int property = 20;

        //     nk_layout_row_static(ctx, 30, 80, 1);
        //     if (nk_button_label(ctx, "button"))
        //         fprintf(stdout, "button pressed\n");
        //     nk_layout_row_dynamic(ctx, 30, 2);
        //     if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        //     if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
        //     nk_layout_row_dynamic(ctx, 22, 1);
        //     nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
		// 	nk_layout_row_dynamic(ctx, 100, 2);
		// 	nk_labelf(ctx, NK_TEXT_LEFT, "%d", x);
		// 	nk_labelf(ctx, NK_TEXT_LEFT, "%d", y);
        // }
        // nk_end(ctx);
        /* Draw */
        nk_gdi_render(nk_rgb(30,30,30));
    }
	nk_gdi_shutdown();
    nk_gdifont_del(f.f14);
    nk_gdifont_del(f.f16);
    nk_gdifont_del(f.f18);
    nk_gdifont_del(f.custom);
    nk_gdifont_del(f.zc);
	ReleaseDC(hwnd, dc);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

