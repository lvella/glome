#pragma once

#include "updatable.hpp"
#include "drawable.hpp"
#include "vol_sphere.hpp"
#include "audio_source.hpp"

class Spaghetti final:
  public Updatable,
  public Glome::Drawable,
  public Audio::Source,
  public VolSphere
{
public:
  Spaghetti(Audio::World &audio_world);
  Spaghetti(Spaghetti&& other) = default;

  ~Spaghetti();

  void draw(Camera& s) override;
  bool update(float dt, UpdatableAdder& adder) override;

  void collided_with(const VolSphere& other, float) override;

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
};
