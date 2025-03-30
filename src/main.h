#ifndef MAIN_H
#define MAIN_H

#include "engine.h"
#include "entity.h"
#include "input.h"
#include "types.h"
#include "utils.h"
#include "platform.h"
#include "render.h"

// -----------------------------------------------------------------------------
// Button actions

typedef enum {
  A_LEFT,
  A_RIGHT,
  A_START,
} action_t;

// -----------------------------------------------------------------------------
// Global data

typedef struct {
  // font_t *font;
  float score;
  float speed;
} global_t;

extern global_t g;

// -----------------------------------------------------------------------------
// Scenes

extern scene_t scene_game;

#endif // MAIN_H