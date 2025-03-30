#ifndef PLATFORM_H
#define PLATFORM_H

#include "input.h"
#include "types.h"
#include "utils.h"

// The window title, if applicable
#if !defined(WINDOW_TITLE)
#define WINDOW_TITLE "High Impact Game"
#endif

// The default window size, if applicable
#if !defined(WINDOW_WIDTH) || !defined(WINDOW_HEIGHT)
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
#endif

// The name of your company or handle. This may be used for the userdata
// directory, so it should not contain any special characters
#if !defined(GAME_VENDOR)
#define GAME_VENDOR "phoboslab"
#endif

// The name of your game. This may be used for the userdata directory, so it
// should not contain any special characters
#if !defined(GAME_NAME)
#define GAME_NAME "high_impact_game"
#endif

#if !defined(PLATFORM_VSYNC)
#define PLATFORM_VSYNC 1
#endif

// The max path length when loading/storing files
#if !defined(PLATFORM_MAX_PATH)
#define PLATFORM_MAX_PATH 512
#endif

double platform_now(void);
bool platform_get_fullscreen(void);
void platform_set_fullscreen(bool fullscreen);
void platform_exit(void);
uint8_t *platform_load_asset(const char *name, uint32_t *bytes_read);
void platform_video_init(void);
void platform_video_cleanup(void);
void platform_prepare_frame(void);
void platform_end_frame(void);
vec2i_t platform_screen_size(void);

#endif // PLATFORM_H