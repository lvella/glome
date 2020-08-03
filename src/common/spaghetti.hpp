#pragma once

#include <random>
#include <memory>
#include "scorer.hpp"
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

  void draw(Camera& c) override;
  bool update(float dt, UpdatableAdder& adder) override;

  void collided_with(const Collidable& other, float) override;

  struct Vertex {
    Vector4 pos;
    Vector4 color;
  };

private:
  struct Impact {
    std::shared_ptr<Scorer> scorer;
    Vector4 location;
  };

  struct Damager {
    std::shared_ptr<Scorer> scorer;
    unsigned damage;
  };

  static constexpr uint16_t separator = std::numeric_limits<uint16_t>::max();

  // Number of line segments used to draw each curve
  static constexpr uint8_t SEGMENTS = 20;

  static unsigned filter_IBO_segments(std::vector<uint16_t>& idata);

  Damager* get_damager(const std::shared_ptr<Scorer>& scorer);
  uint64_t compute_score(unsigned damage_done);

  bool chip(UpdatableAdder& adder, const Vector4& impact_point, unsigned& damage);

  void explode(UpdatableAdder& adder,
      const std::vector<uint16_t>& idata,
      const std::vector<Vertex>& vdata) const;

  static std::normal_distribution<> bullet_damage;

  Vector3 rot_axis;
  float angular_speed;
  float speed;

  std::normal_distribution<> fragility;

  BufferObject vbo;
  GLsizei vbuf_size;

  BufferObject ibo{0};
  GLsizei count;

  std::vector<Impact> impacts;
  std::vector<Damager> damage_log;
  unsigned total_damage = 0;
};
