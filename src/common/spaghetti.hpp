#pragma once

#include <random>
#include <memory>
#include "updatable.hpp"
#include "drawable.hpp"
#include "collidable.hpp"
#include "audio_source.hpp"
#include "buffer_object.hpp"

class Spaghetti final:
  public Updatable,
  public Glome::Drawable,
  public Audio::Source,
  public Collidable
{
public:
  Spaghetti();

  void draw(Camera& s) override;
  bool update(float dt, UpdatableAdder& adder) override;

  void collided_with(const Collidable& other, float) override;

private:
  struct Vertex {
    Vector4 pos;
    Vector4 color;
  };

  void chip(const Vector4& impact_point);
  static std::normal_distribution<> bullet_damage;

  Vector3 rot_axis;
  float angular_speed;
  float speed;

  std::normal_distribution<> frailty;

  std::shared_ptr<BufferObject> p_vbo;
  GLsizei vbuf_size;

  BufferObject ibo{0};
  GLsizei count;

  bool dead = false;

  std::vector<std::shared_ptr<Updatable>> spawn;
};
