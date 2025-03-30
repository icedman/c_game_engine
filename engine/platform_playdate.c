#include "engine.h"
#include "platform.h"

#include "pd_api.h"

#define WIDTH 400
#define HEIGHT 240

const char *systemFontPath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
LCDFont *systemFont = NULL;

PlaydateAPI *playdate = NULL;

static int platform_update(void *userdata) {
  PlaydateAPI *pd = playdate;

  // platform_pump_events();
  PDButtons current;
  pd->system->getButtonState(&current, NULL, NULL);

  input_set_button_state(INPUT_KEY_LEFT, current & kButtonLeft);
  input_set_button_state(INPUT_KEY_RIGHT, current & kButtonRight);
  input_set_button_state(INPUT_KEY_UP, current & kButtonUp);
  input_set_button_state(INPUT_KEY_DOWN, current & kButtonDown);
  input_set_button_state(INPUT_GAMEPAD_A, current & kButtonA);
  input_set_button_state(INPUT_GAMEPAD_B, current & kButtonB);

  platform_prepare_frame();
  engine_update();
  platform_end_frame();

  return 1;
}

int eventHandler(PlaydateAPI *pd, PDSystemEvent event, uint32_t arg) {
  (void)arg; // arg is currently only used for event = kEventKeyPressed

  if (event == kEventInit) {
    playdate = pd;

    const char *err;
    systemFont = pd->graphics->loadFont(systemFontPath, &err);
    if (systemFont == NULL)
      pd->system->error("%s:%i Couldn't load font %s: %s", __FILE__, __LINE__,
                        systemFontPath, err);

    // Note: If you set an update callback in the kEventInit handler, the system
    // assumes the game is pure C and doesn't run any Lua code in the game

    engine_init();
    pd->system->setUpdateCallback(platform_update, pd);
  }

  return 0;
}

// Return the current time in seconds since program start
double platform_now(void) {
  return (double)playdate->system->getCurrentTimeMilliseconds() / 1000;
}

vec2i_t platform_screen_size() {
  vec2i_t screen_size = vec2i(400, 240);
  return screen_size;
}

void platform_prepare_frame(void) {
  // nothing
}

void platform_end_frame(void) {
  PlaydateAPI *pd = playdate;
  pd->system->drawFPS(0, 0);
}

// Load a file into temp memory. Must be freed via temp_free()
uint8_t *platform_load_asset(const char *name, uint32_t *bytes_read) {
  PlaydateAPI *pd = playdate;
  SDFile *fp = pd->file->open(name, kFileRead);
  if (!fp) {
    printf("%s\n", pd->file->geterr());
    return NULL;
  }

  pd->file->seek(fp, 0, SEEK_END);
  unsigned int size = pd->file->tell(fp);
  pd->file->seek(fp, 0, SEEK_SET);

  uint8_t *data = malloc(size);
  *bytes_read = pd->file->read(fp, data, size);
  pd->file->close(fp);

  return data;
}

void exit(int) {}
void abort(void) {}
void _close(void) {}
void _read(void) {}
void _write(void) {}
void _fstat(void) {}
void _lseek_r(void) {}
void _isatty(void) {}