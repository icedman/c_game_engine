// sdl-config in the makefile should put SDL.h into the header search path
// see: https://nullprogram.com/blog/2023/01/08/
#include "SDL.h"

#include "engine.h"
#include "platform.h"
#include "utils.h"

static uint64_t perf_freq = 0;
static bool wants_to_exit = false;
static SDL_Window *window;
static void (*audio_callback)(float *buffer, uint32_t len) = NULL;
static char *path_assets = "";
static char *path_userdata = "";
static char *temp_path = NULL;

void platform_exit(void) { wants_to_exit = true; }

void platform_pump_events(void) {
  SDL_Event ev;
  while (SDL_PollEvent(&ev)) {
    // Detect ALT+Enter press to toggle fullscreen
    if (ev.type == SDL_KEYDOWN &&
        ev.key.keysym.scancode == SDL_SCANCODE_RETURN &&
        (ev.key.keysym.mod & (KMOD_LALT | KMOD_RALT))) {
      platform_set_fullscreen(!platform_get_fullscreen());
    }

    // Input Keyboard
    else if (ev.type == SDL_KEYDOWN || ev.type == SDL_KEYUP) {
      int code = ev.key.keysym.scancode;
      float state = ev.type == SDL_KEYDOWN ? 1.0 : 0.0;
      if (code >= SDL_SCANCODE_LCTRL && code <= SDL_SCANCODE_RALT) {
        int code_internal = code - SDL_SCANCODE_LCTRL + INPUT_KEY_L_CTRL;
        input_set_button_state(code_internal, state);
      } else if (code > 0 && code < INPUT_KEY_MAX) {
        input_set_button_state(code, state);
      }
    }

    else if (ev.type == SDL_TEXTINPUT) {
      input_textinput(ev.text.text[0]);
    }

    // Mouse buttons
    else if (ev.type == SDL_MOUSEBUTTONDOWN || ev.type == SDL_MOUSEBUTTONUP) {
      button_t button = INPUT_BUTTON_NONE;
      switch (ev.button.button) {
      case SDL_BUTTON_LEFT:
        button = INPUT_MOUSE_LEFT;
        break;
      case SDL_BUTTON_MIDDLE:
        button = INPUT_MOUSE_MIDDLE;
        break;
      case SDL_BUTTON_RIGHT:
        button = INPUT_MOUSE_RIGHT;
        break;
      default:
        break;
      }
      if (button != INPUT_BUTTON_NONE) {
        float state = ev.type == SDL_MOUSEBUTTONDOWN ? 1.0 : 0.0;
        // input_set_button_state(button, state);
      }
    }

    // Mouse wheel
    else if (ev.type == SDL_MOUSEWHEEL) {
      button_t button =
          ev.wheel.y > 0 ? INPUT_MOUSE_WHEEL_UP : INPUT_MOUSE_WHEEL_DOWN;
      // input_set_button_state(button, 1.0);
      // input_set_button_state(button, 0.0);
    }

    // Mouse move
    else if (ev.type == SDL_MOUSEMOTION) {
      // input_set_mouse_pos(ev.motion.x, ev.motion.y);
    }

    // Window Events
    if (ev.type == SDL_QUIT) {
      wants_to_exit = true;
    } else if (ev.type == SDL_WINDOWEVENT &&
               (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
                ev.window.event == SDL_WINDOWEVENT_RESIZED)) {
      // engine_resize(platform_screen_size());
    }
  }
}

double platform_now(void) {
  uint64_t perf_counter = SDL_GetPerformanceCounter();
  return (double)perf_counter / (double)perf_freq;
}

bool platform_get_fullscreen(void) {
  return SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN;
}

void platform_set_fullscreen(bool fullscreen) {
  if (fullscreen) {
    int32_t display = SDL_GetWindowDisplayIndex(window);

    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(display, &mode);
    SDL_SetWindowDisplayMode(window, &mode);
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    SDL_ShowCursor(SDL_DISABLE);
  } else {
    SDL_SetWindowFullscreen(window, 0);
    SDL_ShowCursor(SDL_ENABLE);
  }
}

uint8_t *platform_load_asset(const char *name, uint32_t *bytes_read) {
  return NULL;
}

#if defined(RENDER_GL) // ------------------------------------------------------
#define PLATFORM_WINDOW_FLAGS SDL_WINDOW_OPENGL
SDL_GLContext platform_gl;

void platform_video_init(void) {
#if defined(__EMSCRIPTEN__) || defined(USE_GLES2)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

  platform_gl = SDL_GL_CreateContext(window);
  SDL_GL_SetSwapInterval(PLATFORM_VSYNC);
}

void platform_prepare_frame(void) {
  // nothing
}

void platform_video_cleanup(void) { SDL_GL_DeleteContext(platform_gl); }

void platform_end_frame(void) { SDL_GL_SwapWindow(window); }

vec2i_t platform_screen_size(void) {
  int width, height;
  SDL_GL_GetDrawableSize(window, &width, &height);
  return vec2i(width, height);
}

#elif defined(RENDER_METAL) // ----------------------------------------------
#define PLATFORM_WINDOW_FLAGS 0
static SDL_MetalView *metal_view;
static SDL_Renderer *renderer;
static void *metal_layer;

void platform_video_init() {
  metal_view = SDL_Metal_CreateView(window);
  metal_layer = SDL_Metal_GetLayer(metal_view);
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void platform_video_cleanup() {
  SDL_Metal_DestroyView(metal_view);
  metal_view = NULL;
  metal_layer = NULL;
  SDL_DestroyRenderer(renderer);
  renderer = NULL;
}

void platform_prepare_frame() {}

void platform_end_frame() {}

vec2i_t platform_screen_size() {
  int width, height;
  SDL_Metal_GetDrawableSize(window, &width, &height);
  vec2i_t screen_size = vec2i(width, height);
  return screen_size;
}

void *platform_get_metal_layer() { return metal_layer; }

#elif defined(RENDER_SOFTWARE) // ----------------------------------------------
#define PLATFORM_WINDOW_FLAGS 0
static SDL_Renderer *renderer;
static SDL_Texture *screenbuffer = NULL;
static void *screenbuffer_pixels = NULL;
static int screenbuffer_pitch;
static vec2i_t screenbuffer_size = vec2i(0, 0);
static vec2i_t screen_size = vec2i(0, 0);

void platform_video_init(void) {
  renderer =
      SDL_CreateRenderer(window, -1,
                         SDL_RENDERER_ACCELERATED |
                             (PLATFORM_VSYNC ? SDL_RENDERER_PRESENTVSYNC : 0));
  SDL_GL_SetSwapInterval(PLATFORM_VSYNC);
}

void platform_video_cleanup(void) {
  if (screenbuffer) {
    SDL_DestroyTexture(screenbuffer);
  }
  SDL_DestroyRenderer(renderer);
}

void platform_prepare_frame(void) {
  if (screen_size.x != screenbuffer_size.x ||
      screen_size.y != screenbuffer_size.y) {
    if (screenbuffer) {
      SDL_DestroyTexture(screenbuffer);
    }
    screenbuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                                     SDL_TEXTUREACCESS_STREAMING, screen_size.x,
                                     screen_size.y);
    screenbuffer_size = screen_size;
  }
  SDL_LockTexture(screenbuffer, NULL, &screenbuffer_pixels,
                  &screenbuffer_pitch);
}

void platform_end_frame(void) {
  screenbuffer_pixels = NULL;
  SDL_UnlockTexture(screenbuffer);
  SDL_RenderCopy(renderer, screenbuffer, NULL, NULL);
  SDL_RenderPresent(renderer);
}

rgba_t *platform_get_screenbuffer(int32_t *pitch) {
  *pitch = screenbuffer_pitch;
  return screenbuffer_pixels;
}

vec2i_t platform_screen_size(void) {
  int width, height;
  SDL_GetWindowSize(window, &width, &height);
  screen_size = vec2i(width, height);
  return screen_size;
}

#else
#error "Unsupported renderer for platform SDL"
#endif

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK |
           SDL_INIT_GAMECONTROLLER);

  // Figure out the absolute asset and userdata paths. These may either be
  // supplied at build time through -DPATH_ASSETS=.. and -DPATH_USERDATA=..
  // or received at runtime from SDL. Note that SDL may return NULL for these.
  // We fall back to the current directory (i.e. just "") in this case.

  char *sdl_path_assets = NULL;
#ifdef PATH_ASSETS
  path_assets = TOSTRING(PATH_ASSETS);
#else
  sdl_path_assets = SDL_GetBasePath();
  if (sdl_path_assets) {
    path_assets = sdl_path_assets;
  }
#endif

  char *sdl_path_userdata = NULL;
#ifdef PATH_USERDATA
  path_userdata = TOSTRING(PATH_USERDATA);
#else
  sdl_path_userdata = SDL_GetPrefPath(GAME_VENDOR, GAME_NAME);
  if (sdl_path_userdata) {
    path_userdata = sdl_path_userdata;
  }
#endif

  // Reserve some space for concatenating the asset and userdata paths with
  // local filenames.
  temp_path = malloc(max(strlen(path_assets), strlen(path_userdata)) +
                     PLATFORM_MAX_PATH);

  perf_freq = SDL_GetPerformanceFrequency();

  window =
      SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE |
                           PLATFORM_WINDOW_FLAGS | SDL_WINDOW_ALLOW_HIGHDPI);

  platform_video_init();

  engine_init();

  while (!wants_to_exit) {
    platform_pump_events();
    platform_prepare_frame();
    engine_update();
    platform_end_frame();
  }

  engine_cleanup();
  platform_video_cleanup();

  SDL_DestroyWindow(window);

  if (sdl_path_assets) {
    SDL_free(sdl_path_assets);
  }
  if (sdl_path_userdata) {
    SDL_free(sdl_path_userdata);
  }

  SDL_Quit();
  return 0;
}
