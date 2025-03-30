#include "render.h"
#include "pd_api.h"

static texture_t texture;
static int textures_len = 0;
static vec2i_t texture_sizes[RENDER_TEXTURES_MAX];
static LCDBitmap *textures[RENDER_TEXTURES_MAX];

extern PlaydateAPI *playdate;

static const LCDPattern intensity[] = {
  /* black */
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
  /* darkgray_2 */
  {0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
  /* gray_3 */
  {0x55, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
  /* gray */
  {0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
  /* white */
  {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
};

LCDColor get_pattern(int p) { return (LCDColor)&intensity[p]; }

float rgbToLuminance(unsigned int red, unsigned int green, unsigned int blue) {
  float luminance = 0.299f * red + 0.587f * green + 0.114f * blue;
  float normalizedLuminance = luminance / 255.0f;
  return normalizedLuminance;
}

LCDColor rgbToColor(unsigned int red, unsigned int green, unsigned int blue) {
  float luminance = rgbToLuminance(red, green, blue);
  int color_range = 5;
  int index = roundf(luminance * color_range);
  if (index >= color_range) {
    index = color_range - 1;
  }
  return (LCDColor)&intensity[index];
}

static vec2i_t screen_size;

void render_backend_init(void) {
	printf("render_backend_init\n");
}

void render_backend_cleanup(void) {}

void render_set_screen(vec2i_t size) {
	screen_size = size;
}

void render_set_blend_mode(render_blend_mode_t mode) {}
void render_set_post_effect(render_post_effect_t post) {}

void render_frame_prepare(void) {
  PlaydateAPI *pd = playdate;
	pd->graphics->fillRect(0, 0, 400, 240, kColorWhite);
}

void render_frame_end(void) {}

void render_draw_quad(quadverts_t *quad, texture_t texture_handle) {
	PlaydateAPI *pd = playdate;

	LCDBitmap *texture = textures[texture_handle.index];
	vec2i_t src_size = texture_sizes[texture_handle.index];

	vertex_t *v = quad->vertices;
	rgba_t color = v[0].color;
	int dx = v[0].pos.x;
	int dy = v[0].pos.y;

	float dist1 = vec2_dist(vec2(0,0), vec2(src_size.x, src_size.y));
	float dist2 = vec2_dist(v[0].pos, v[2].pos);
	double sz = dist2/dist1;
  	pd->graphics->drawScaledBitmap(texture, dx, dy, sz, sz);

	// int coords[8] = {
	// 	v[0].pos.x, v[0].pos.y,
	// 	v[1].pos.x, v[1].pos.y,
	// 	v[2].pos.x, v[2].pos.y,
	// 	v[3].pos.x, v[3].pos.y,
	// };
	// LCDColor clr = get_pattern(texture_handle.index % 4);
	// pd->graphics->fillPolygon(4, coords, clr, kPolygonFillNonZero);

	// for(int i=0; i<4; i++) {
	// 	vertex_t v1 = v[i];
	// 	vertex_t v2 = v[(i+1)%4];
	// 	int x1 = v1.pos.x;
	// 	int y1 = v1.pos.y;
	// 	int x2 = v2.pos.x;
	// 	int y2 = v2.pos.y;
	// 	pd->graphics->drawLine(x1, y1, x2, y2, 2, kColorBlack);
	// }
}

texture_mark_t textures_mark(void) {
	return (texture_mark_t){.index = textures_len};
}
void textures_reset(texture_mark_t mark) {}

texture_t texture_create(vec2i_t size, rgba_t *pixels) {
	texture.index = textures_len;
	texture_sizes[textures_len] = size;
	textures[textures_len] = (void*)pixels;
	textures_len++;
	return texture;
}
void texture_replace_pixels(texture_t texture_handle, vec2i_t size, rgba_t *pixels) {}
