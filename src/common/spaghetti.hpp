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

  struct Vertex {
    Vector4 pos;
    Vector4 color;
  };

private:
  static constexpr uint16_t separator = std::numeric_limits<uint16_t>::max();

  // Number of line segments used to draw each curve
  static constexpr uint8_t SEGMENTS = 20;

  static unsigned filter_IBO_segments(std::vector<uint16_t>& idata);
  bool chip(UpdatableAdder& adder, const Vector4& impact_point);

  void explode(UpdatableAdder& adder,
      const std::vector<uint16_t>& idata,
      const std::vector<Vertex>& vdata) const;

  static std::normal_distribution<> bullet_damage;

  Vector3 rot_axis;
  float angular_speed;
  float speed;

  std::normal_distribution<> frailty;

  BufferObject vbo;
  GLsizei vbuf_size;

  BufferObject ibo{0};
  GLsizei count;

  std::vector<Vector4> impact;
};
