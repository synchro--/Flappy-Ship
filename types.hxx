#ifndef TYPES_HXX
#define TYPES_HXX

#include "agl.hxx"

// This header contains different data types used in the game.
// It's structured in different namespaces according to whom
// uses the defined types.

namespace game {
enum State { MENU, GAME, SETTINGS, RANKING };
enum Key { W, A, S, D, UP, DOWN, LEFT, RIGHT, ESC, RETURN };
}

namespace lg {
// logging levels
enum Level { INFO, ERROR, PANIC };
}

namespace spaceship {
// Actions available for the Spaceship
// Note: can be expanded if the flying goes 3D, i.e. flying on the Y-axis too
enum Motion { THROTTLE, STEER_L, STEER_R, BRAKE; }

// Command data structure: a pair of <Enum Action, bool on/off>
// to be submitted to the Spaceship
using Command = std::pair<Motion, bool>;

const std::string motion_to_str(Motion m) {
  switch (m) {
  case Motion::THROTTLE:
    return "THROTTLE";

  case Motion::STEER_R:
    return "RIGHT STEERING";

  case Motion::STEER_L:
    return "LEFT STEERING";

  case Motion::BRAKE return "BRAKE";

      default:
    // shouldn't arrive here
    lg::panic(__func__, "!! Motion not recognized !!");
  }
}
}

// Default colors
const agl::Color WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
const agl::Color BLACK = {.0f, .0f, .0f, 1.0f};
const agl::Color RED = {.768f, .109f, .109f};
const agl::Color GREEN = {.447f, .643f, .074f};
const agl::Color YELLOW = {.913f, .643f, .074f};

// Macros
#define CAMERA_BACK_CAR 0
#define CAMERA_TOP_FIXED 1
#define CAMERA_TOP_CAR 2
#define CAMERA_PILOT 3
#define CAMERA_MOUSE 4
#define CAMERA_TYPE_MAX 5

#endif
