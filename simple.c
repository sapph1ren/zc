#include <raylib.h>
#include <stdbool.h>
#include <stdlib.h>

#include "microui.h"
#include "murl.h"

static void voice_wndw(mu_Context* ctx, int x, int y) {
	mu_Container *cnt = mu_get_container(ctx, "Voice");
    if (cnt) {
        cnt->rect = mu_rect(0, 0, x * 0.35, y * 0.7);
    }
	if (mu_begin_window_ex(ctx, "Voice", mu_rect(0,0,x*0.35,y*0.7), MU_OPT_NORESIZE | MU_OPT_NOCLOSE | MU_OPT_NOTITLE)){
		char b[64];
		sprintf(b, "%d %d", x, y);
		mu_label(ctx, b);
		
		mu_end_window(ctx);
	}	
}

static void chats_wndw(mu_Context* ctx, int x, int y) {
	mu_Container *cnt = mu_get_container(ctx, "chats");
    if (cnt) {
        cnt->rect = mu_rect(0,y*0.7,x*0.35,y*0.3);
    }
	if (mu_begin_window_ex(ctx, "chats", mu_rect(0,y*0.7,x*0.35,y*0.3), MU_OPT_NORESIZE | MU_OPT_NOCLOSE | MU_OPT_NOTITLE)){
		char b[64];
		sprintf(b, "%d %d", x, y);
		mu_label(ctx, b);
		
		
		
		mu_end_window(ctx);
	}	
	
}

static void chat_wndw(mu_Context* ctx, int x, int y) {
	mu_Container *cnt = mu_get_container(ctx, "chat");
    if (cnt) {
        cnt->rect = mu_rect(x*0.35,0,x*0.65,y);
    }
	if (mu_begin_window_ex(ctx, "chat", mu_rect(x*0.35,0,x*0.65,y), MU_OPT_NORESIZE | MU_OPT_NOCLOSE | MU_OPT_NOTITLE)){
		char b[64];
		sprintf(b, "%d %d", x, y);
		mu_label(ctx, b);
		
		
		
		mu_end_window(ctx);
	}	
	
}




int main(void) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 600, "microui + raylib");
	SetTargetFPS(30);

	mu_Context *ctx = malloc(sizeof(mu_Context));
	mu_init(ctx);
	murl_setup_font(ctx);
	int w, h;
	while (!WindowShouldClose()) {
		w = GetScreenWidth();
		h = GetScreenHeight();
		murl_handle_input(ctx);
		mu_begin(ctx);
		voice_wndw(ctx, w, h);
	    chats_wndw(ctx, w, h);
		chat_wndw(ctx, w, h);
		mu_end(ctx);
		
		BeginDrawing();
		ClearBackground(BLACK);
		murl_render(ctx);
		EndDrawing();
	}

	free(ctx);
	CloseWindow();
	return 0;
}
