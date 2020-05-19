#pragma once

#include "updatable.hpp"
#include "drawable.hpp"
#include "collidable.hpp"
#include "audio_source.hpp"
#include <memory>

class Spaghetti final:
  public Updatable,
  public Glome::Drawable,
  public Audio::Source,
  public Collidable
{
public:
  Spaghetti(Audio::World &audio_world);

  ~Spaghetti();

  void draw(Camera& s) override;
  bool update(float dt, UpdatableAdder& adder) override;

  void collided_with(const Collidable& other, float) override;

private:

  Vector4 position() const override {
    return Object::position();
  }

  struct Vertex {
    Vector4 pos;
    Vector4 color;
  };

  Vector3 rot_axis;
  float angular_speed;
  float speed;
  GLuint vbo;
  GLsizei count;
  bool dead = false;

  std::shared_ptr<Spaghetti> spawn;
};
