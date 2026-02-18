#include "fenster.h"
#include <assert.h>
#include <stdbool.h>
#include "renderer.h"
#include "atlas.inl"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" 

#define BUFFER_SIZE 16384

typedef uint8_t byte;
static mu_Rect tex_buf[BUFFER_SIZE];
static mu_Rect src_buf[BUFFER_SIZE];
static mu_Color color_buf[BUFFER_SIZE];

static int buf_idx;

static struct fenster window = {.title="A window", .width=800, .height=600};

r_Image load_image(const char *filename) {
    int w, h, n;
    // n — это количество каналов, нам нужно 4 (RGBA)
    unsigned char *data = stbi_load(filename, &w, &h, &n, 4);
    if (!data) return (r_Image){0};

    uint32_t *pixels = malloc(w * h * sizeof(uint32_t));
    for (int i = 0; i < w * h; i++) {
        // Конвертация в формат fenster
        pixels[i] = (data[i*4] << 16) | (data[i*4+1] << 8) | data[i*4+2];
    }
    stbi_image_free(data);
    return (r_Image){w, h, pixels};
}

void r_draw_image(mu_Context *ctx, mu_Rect rect, r_Image *img) {
    mu_Rect clip = mu_get_clip_rect(ctx);
    
    // Используем -> для img, так как это указатель
    int sx_step = (img->w << 16) / rect.w;
    int sy_step = (img->h << 16) / rect.h;

    for (int y = 0; y < rect.h; y++) {
        int dst_y = rect.y + y;
        // Для глобальной переменной используем точку: window.height
        if (dst_y < clip.y || dst_y >= clip.y + clip.h || dst_y >= window.height) continue;

        int src_y = (y * sy_step) >> 16;
        uint32_t *src_row = &img->pixels[src_y * img->w];

        for (int x = 0; x < rect.w; x++) {
            int dst_x = rect.x + x;
            if (dst_x < clip.x || dst_x >= clip.x + clip.w || dst_x >= window.width) continue;

            int src_x = (x * sx_step) >> 16;
            
            // ВАЖНО: передаем &window (указатель), чтобы макрос сработал
            fenster_pixel(&window, dst_x, dst_y) = src_row[src_x];
        }
    }
}

static mu_Rect clip_rect;

void r_init(void) {
  /* init SDL window */
  window.buf = malloc(window.width * window.height * sizeof(*window.buf));
  r_clear(mu_color(0, 0, 0, 255));
  fenster_open(&window);

  /* init texture */
  clip_rect = mu_rect(0, 0, window.width, window.height);
}

void r_close(void){
    fenster_close(&window);
}

int r_loop(void) {
    return fenster_loop(&window);  
}

static inline bool within(int c, int lo, int hi) {
  return c >= lo && c < hi;
}

static inline bool within_rect(mu_Rect rect, int x, int y) {
  return within(x, rect.x, rect.x+rect.w)
        && within(y, rect.y, rect.y+rect.h);
}

static inline bool same_size(const mu_Rect* a, const mu_Rect* b) {
  return a->w == b->w && a->h == b->h;
}

static inline byte texture_color(const mu_Rect* tex, int x, int y) {
  assert(x < tex->w);
  x = x + tex->x;
  assert(y < tex->h);
  y = y + tex->y;
  return atlas_texture[y*ATLAS_WIDTH + x];
}

static inline uint32_t r_color(mu_Color clr) {
  return ((uint32_t)clr.a << 24) | ((uint32_t)clr.r << 16) | ((uint32_t)clr.g << 8) | clr.b;
}

mu_Color mu_color_argb(uint32_t clr) {
  return mu_color((clr >> 16) & 0xff, (clr >> 8) & 0xff, clr & 0xff, (clr >> 24) & 0xff);
}

static inline int greyscale(byte c) {
  return r_color(mu_color(c, c, c, 255));
}


static inline mu_Color blend_pixel(mu_Color dst, mu_Color src) {
  int ia = 0xff - src.a;
  dst.r = ((src.r * src.a) + (dst.r * ia)) >> 8;
  dst.g = ((src.g * src.a) + (dst.g * ia)) >> 8;
  dst.b = ((src.b * src.a) + (dst.b * ia)) >> 8;
  return dst;
}

static void flush(void) {
  // draw things based on texture, vertex, color
  for (int i = 0; i < buf_idx; i++) {
    mu_Rect* src = &src_buf[i];
    mu_Rect* tex = &tex_buf[i];
    // draw
    int ystart = mu_max(src->y, clip_rect.y);
    int yend = mu_min(src->y+src->h, clip_rect.y+clip_rect.h);
    int xstart = mu_max(src->x, clip_rect.x);
    int xend = mu_min(src->x+src->w, clip_rect.x+clip_rect.w);
    // hacky but sufficient for us
    if (same_size(src, tex)) {
      for (int y = ystart; y < yend; y++) {
        for (int x = xstart; x < xend; x++) {
          assert(within_rect(*src, x, y));
          assert(within_rect(clip_rect, x, y));
          // read color from texture
          byte tc = texture_color(tex, x-src->x, y-src->y);
          fenster_pixel(&window, x, y) |= greyscale(tc);
        }
      }
    } else {
      mu_Color new_color = color_buf[i];
      for (int y = ystart; y < yend; y++) {
        for (int x = xstart; x < xend; x++) {
          assert(within_rect(*src, x, y));
          assert(within_rect(clip_rect, x, y));
          // blend color from operation
          mu_Color existing_color = mu_color_argb(fenster_pixel(&window, x, y));
          mu_Color result = blend_pixel(existing_color, new_color);
          fenster_pixel(&window, x, y) = r_color(result);
        }
      }
    }
  }

  buf_idx = 0;
}


static void push_quad(mu_Rect src, mu_Rect tex, mu_Color color) {
  if (buf_idx == BUFFER_SIZE) { flush(); }

  tex_buf[buf_idx] = tex;
  src_buf[buf_idx] = src;
  color_buf[buf_idx] = color;

  buf_idx++;
}


void r_draw_rect(mu_Rect rect, mu_Color color) {
  push_quad(rect, atlas[ATLAS_WHITE], color);
}


void r_draw_text(const char *text, mu_Vec2 pos, mu_Color color) {
  mu_Rect dst = { pos.x, pos.y, 0, 0 };
  for (const char *p = text; *p; p++) {
    if ((*p & 0xc0) == 0x80) { continue; }
    int chr = mu_min((unsigned char) *p, 127);
    mu_Rect src = atlas[ATLAS_FONT + chr];
    dst.w = src.w;
    dst.h = src.h;
    push_quad(dst, src, color);
    dst.x += dst.w;
  }
}


void r_draw_icon(int id, mu_Rect rect, mu_Color color) {
  mu_Rect src = atlas[id];
  int x = rect.x + (rect.w - src.w) / 2;
  int y = rect.y + (rect.h - src.h) / 2;
  push_quad(mu_rect(x, y, src.w, src.h), src, color);
}


int r_get_text_width(const char *text, int len) {
  int res = 0;
  for (const char *p = text; *p && len--; p++) {
    if ((*p & 0xc0) == 0x80) { continue; }
    int chr = mu_min((unsigned char) *p, 127);
    res += atlas[ATLAS_FONT + chr].w;
  }
  return res;
}


int r_get_text_height(void) {
  return 18;
}


void r_set_clip_rect(mu_Rect rect) {
  flush();
  int ystart = mu_max(0, rect.y);
  int yend = mu_min(window.height, rect.y+rect.h);
  int xstart = mu_max(0, rect.x);
  int xend = mu_min(window.width, rect.x+rect.w);
  clip_rect = mu_rect(xstart, ystart, xend-xstart, yend-ystart);
}

void r_clear(mu_Color clr) {
  flush();
  for (int i = 0; i < window.width * window.height; i++) {
    window.buf[i] = r_color(clr);
  }
}


void r_present(void) {
  flush();
  fenster_loop(&window);
}

int mouse_down = 0;

int r_mouse_down(void) {
  if (window.mouse && !mouse_down) {
    mouse_down = 1;
    return 1;
  }
  return 0;
}

int r_mouse_up(void) {
  if (!window.mouse && mouse_down) {
    mouse_down = 0;
    return 1;
  }
  return 0;
}

int r_mouse_moved(int *mousex, int *mousey) {
  if (window.x != *mousex || window.y != *mousey) {
    *mousex = window.x;
    *mousey = window.y;
    return 1;
  }
  return 0;
}

int r_ctrl_pressed(void) { return window.mod & 1; }

int r_shift_pressed(void) { return window.mod & 2; }

int r_alt_pressed(void) { return window.mod & 4; }

int r_key_down(int key) {
  if (window.keys[key] == 1) {
    window.keys[key]++;
    return 1;
  }
  return 0;
}

int r_key_up(int key) {
  if (window.keys[key] < 1) {
    return 1;
  }
  return 0;
}

int64_t r_get_time(void) {
  return fenster_time();
}

void r_sleep(int64_t ms) {
  fenster_sleep(ms);
}
