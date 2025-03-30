#include "engine.h"
#include "input.h"
#include "platform.h"

#include "main.h"

global_t g;

void main_init(void) {
  // Gamepad
  input_bind(INPUT_GAMEPAD_DPAD_LEFT, A_LEFT);
  input_bind(INPUT_GAMEPAD_DPAD_RIGHT, A_RIGHT);
  input_bind(INPUT_GAMEPAD_L_STICK_LEFT, A_LEFT);
  input_bind(INPUT_GAMEPAD_L_STICK_RIGHT, A_RIGHT);
  input_bind(INPUT_GAMEPAD_X, A_START);
  input_bind(INPUT_GAMEPAD_A, A_START);

  // Keyboard
  input_bind(INPUT_KEY_LEFT, A_LEFT);
  input_bind(INPUT_KEY_RIGHT, A_RIGHT);
  input_bind(INPUT_KEY_RETURN, A_START);

  engine_set_scene(&scene_game);

  // sound_t ss = sound(sound_source_synth_song(&song));
  // sound_set_volume(ss, 0.8);
  // sound_set_loop(ss, true);
  // sound_unpause(ss);
}

void main_cleanup(void) {}