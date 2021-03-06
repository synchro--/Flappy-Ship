
#include "elements.h"
#include <cmath>

// Implementation of the objects in elements.h
namespace elements {

/*
 * Floor
 */
Floor::Floor(const char *texture_filename)
    : m_size(FLOOR_SIZE), m_height(0.0f), m_env(agl::get_env()),
      // repat = true, linear interpolation
      m_tex(m_env.loadTexture(texture_filename, true, false)) {}

void Floor::render() {
  // lg::i(__func__, "Rendering floor...");
  m_env.drawFloor(m_tex, m_size, m_height, 150);
}

Floor *get_floor(const char *texture_filename) {
  const static auto TAG = __func__;
  lg::i(TAG, "Loading floor texture from %s", texture_filename);

  static std::unique_ptr<Floor> s_floor(nullptr);
  if (!s_floor) {
    s_floor.reset(new Floor(texture_filename)); // Init
  }

  return s_floor.get();
}

/*
 * Sky
 */

Sky::Sky(const char *texture_filename)
    : m_radius(SKY_RADIUS), m_lats(20.0f), m_longs(20.0f),
      m_env(agl::get_env()), m_tex(m_env.loadTexture(texture_filename, false)) {
}

void Sky::render() {
  // lg::i(__func__, "Rendering Sky...");
  m_env.drawSky(m_tex, m_radius, m_lats, m_longs);
}

void Sky::set_params(double radius, int lats, int longs) {
  m_radius = radius;
  m_lats = lats;
  m_longs = longs;
}

Sky *get_sky(const char *texture_filename) {
  const static auto TAG = __func__;
  lg::i(TAG, "Loading Sky texture from %s", texture_filename);

  static std::unique_ptr<Sky> s_sky(nullptr);
  if (!s_sky) {
    s_sky.reset(new Sky(texture_filename)); // Init
  }

  return s_sky.get();
}

/*
 * Ring. See elements::Ring
 *
 */

Ring::Ring(float x, float y, float z, bool flight_mode, float angle)
    : m_ship_old_z(INFINITY), m_triggered(false), m_env(agl::get_env()) {
  m_3D_FLIGHT = flight_mode;
  m_px = x;
  m_py = m_3D_FLIGHT ? y : 1.5;
  m_pz = z;
  m_angle = angle;
}

// initaliazing static members of Ring class
// colors for when the ring is triggered or not
const agl::Color Ring::TRIGGERED = agl::RED; //{1.0f, .86f, .35f, .7f};
const agl::Color Ring::NOT_TRIGGERED = {.2f, .80f, .2f, .7f};
// view UP vector
const agl::Vec3 Ring::s_viewUP = agl::Vec3(0.0, 1.0, 0.0);
// radius values
const float Ring::s_r = 0.3; // inner radius
const float Ring::s_R = 2.5; // outer radius

void Ring::render() {
  m_env.mat_scope([&] {
    m_env.translate(m_px, m_py, m_pz);
    m_env.rotate(m_angle, s_viewUP);
    // set the proper color if triggered
    m_env.setColor(m_triggered ? TRIGGERED : NOT_TRIGGERED);

    if (m_env.isBlending()) {
      // maybe move this to Env helper function
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      m_env.drawTorus(s_r, s_R);

      glDisable(GL_BLEND);
    } else {
      m_env.drawTorus(s_r, s_R);
    }

  });
}

void Ring::checkCrossing(float x, float z) {
  // if the ring has already been crossed, nothing to do
  if (!m_triggered) {
    bool first_call = m_ship_old_z == INFINITY;

    // get distance wrt to ring center
    x -= m_px;
    z -= m_pz;

    float cos_phi = cosf(m_angle * M_PI / 180.0f);
    float sin_phi = sinf(m_angle * M_PI / 180.0f);

    // project coords of the ship to ring reference frame
    float x_ring = x * cos_phi - z * sin_phi;
    float z_ring = x * sin_phi + z * cos_phi;

    // X: is it inside the circle diameter?
    bool check_X = (x_ring < 2 * s_R) && (x_ring > -2 * s_R);
    // Z: sign changed? Then crossing happened
    bool check_Z =
        (z_ring >= 0 && m_ship_old_z < 0) || (z_ring <= 0 && m_ship_old_z > 0);

    if (!first_call && check_Z && check_X) {
      m_triggered = true;
    }

    m_ship_old_z = z_ring;
  }
}

// as above but checking on all 3Dimesionson for flappy flight mode
void Ring::checkCrossing(float x, float y, float z) {
  // if the ring has already been crossed, nothing to do
  if (!m_triggered) {
    bool first_call = m_ship_old_z == INFINITY;

    // get distance wrt to ring center
    x -= m_px;
    y -= m_py;
    z -= m_pz;

    float cos_phi = cosf(m_angle * M_PI / 180.0f);
    float sin_phi = sinf(m_angle * M_PI / 180.0f);

    // project coords of the ship to ring reference frame
    float x_ring = x * cos_phi - z * sin_phi;
    float y_ring = y;
    float z_ring = x * sin_phi + z * cos_phi;

    // X: is it inside the circle diameter?
    bool check_X = (x_ring < 2 * s_R) && (x_ring > -2 * s_R);
    bool check_Y = (y_ring < 2 * s_R) && (y_ring > -2 * s_R);
    // Z: sign changed? Then crossing happened
    bool check_Z =
        (z_ring >= 0 && m_ship_old_z < 0) || (z_ring <= 0 && m_ship_old_z > 0);

    if (!first_call && check_Z && check_Y && check_X) {
      m_triggered = true;
    }

    m_ship_old_z = z_ring;
  }
}

/*
 * BadCube. See elements::BadCube
 *
 */

BadCube::BadCube(float x, float y, float z, bool flight_mode, float angle)
    : m_ship_old_z(INFINITY), m_env(agl::get_env()) {
  m_3D_FLIGHT = flight_mode;
  m_px = x;
  m_py = m_3D_FLIGHT ? y : 2.5;
  m_pz = z;
  m_angle = angle;
}

// initaliazing static members of BadCube class
// view UP vector
const agl::Vec3 BadCube::s_viewUP = agl::Vec3(0.0, 1.0, 0.0);
const float BadCube::side = 2.5; // side of the cube

void BadCube::render() {
  m_env.mat_scope([&] {
    m_env.translate(m_px, m_py, m_pz);
    m_env.rotate(m_angle, s_viewUP);
    
    // if blending is not active the cubes will be just plain squares
    if (m_env.isBlending()) {
      // maybe move this to Env helper function
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      m_env.drawCube(side);

      glDisable(GL_BLEND);
    } else {
      m_env.setColor(agl::YELLOW);
      m_env.drawSquare(side);
    }

  });
}

bool BadCube::checkCrossing(float x, float z) {
  // get distance wrt to the cube center
  x -= m_px;
  z -= m_pz;

  float cos_phi = cosf(m_angle * M_PI / 180.0f);
  float sin_phi = sinf(m_angle * M_PI / 180.0f);

  // project coords of the ship to the cube reference frame
  float x_ring = x * cos_phi - z * sin_phi;
  float z_ring = x * sin_phi + z * cos_phi;

  // X: is it inside the square perimeter ?
  bool check_X = (x_ring < 2 * side) && (x_ring > -2 * side);
  // Z: sign changed? Then crossing happened
  bool check_Z =
      (z_ring >= 0 && m_ship_old_z < 0) || (z_ring <= 0 && m_ship_old_z > 0);

  // update last z
  m_ship_old_z = z_ring;

  return check_X && check_Z;
}

// as above but checking on all 3Dimesionson for flappy flight mode
bool BadCube::checkCrossing(float x, float y, float z) {
  // get distance wrt to the cube center
  x -= m_px;
  y -= m_py;
  z -= m_pz;

  float cos_phi = cosf(m_angle * M_PI / 180.0f);
  float sin_phi = sinf(m_angle * M_PI / 180.0f);

  // project coords of the ship to cube reference frame
  float x_ring = x * cos_phi - z * sin_phi;
  float y_ring = y;
  float z_ring = x * sin_phi + z * cos_phi;

  // X: is it inside the square perimeter?
  bool check_X = (x_ring < 2 * side) && (x_ring > -2 * side);
  bool check_Y = (y_ring < 2 * side) && (y_ring > -2 * side);
  // Z: sign changed? Then crossing happened
  bool check_Z =
      (z_ring >= 0 && m_ship_old_z < 0) || (z_ring <= 0 && m_ship_old_z > 0);

  // update last Z position
  m_ship_old_z = z_ring;
  return (check_Z && check_Y && check_X);
}

/*
 * The Final Door. See elements::Door
 *
 */

// get Door instance
std::unique_ptr<Door> get_door(const char *mesh_filename, const char *texture_filename) {
  const static auto TAG = __func__;
  lg::i(TAG, "Loading Final Door --> Mesh: %s", mesh_filename);
  // init
  return std::unique_ptr<Door>(new Door(mesh_filename, texture_filename));
}

Door::Door(const char *mesh_filename, const char *texture_filename)
    : m_px(0), m_py(6.0), m_pz(-(FLOOR_SIZE - 1.0)), m_scaleX(DOOR_SCALE),
      m_scaleY(DOOR_SCALE), m_scaleZ(DOOR_SCALE), m_angle(30),
      m_ship_old_z(INFINITY), m_env(agl::get_env()),
      m_mesh(agl::loadMesh(mesh_filename)), m_tex(m_env.loadTexture(texture_filename)) {}

// initaliazing static members of Door class
// view UP vector
const agl::Vec3 Door::s_viewUP = agl::Vec3(0.0, 1.0, 0.0);
const float Door::side = 2.5; // door side

void Door::render() {
    m_env.textureDrawing(m_tex, [&] {
  m_env.mat_scope([&] {
    m_env.translate(m_px, m_py, m_pz);
    // adjust mesh pre-defined angle
    m_env.rotate(m_angle, s_viewUP);
    m_env.rotate(90, agl::Vec3(1.0, 0.0, 0.0));
    m_env.rotate(45, agl::Vec3(0, 0, 1));
    // scale mesh
    m_env.scale(m_scaleX, m_scaleY, m_scaleZ);
    m_mesh->renderGouraud(m_env.isWireframe());
  });
    });
}

bool Door::checkCrossing(float x, float z) {
  // get distance wrt to the door center
  x -= m_px;
  z -= m_pz;

  float cos_phi = cosf(m_angle * M_PI / 180.0f);
  float sin_phi = sinf(m_angle * M_PI / 180.0f);

  // project coords of the ship to the door reference frame
  float x_ring = x * cos_phi - z * sin_phi;
  float z_ring = x * sin_phi + z * cos_phi;

  // X: is it inside the square perimeter ?
  bool check_X = (x_ring < 2 * side) && (x_ring > -2 * side);
  // Z: sign changed? Then crossing happened
  bool check_Z =
      (z_ring >= 0 && m_ship_old_z < 0) || (z_ring <= 0 && m_ship_old_z > 0);

  // update last z
  m_ship_old_z = z_ring;

  return check_X && check_Z;
}

} // namespace elements
