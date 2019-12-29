#pragma once

#include "updatable.hpp"
#include "drawable.hpp"
#include "vol_sphere.hpp"
#include "audio_source.hpp"

class Spaghetti:
  public Updatable,
  public Glome::Drawable,
  public Audio::Source,
  public VolSphere
{
public:
  Spaghetti(Audio::World &audio_world);
  Spaghetti(Spaghetti&& other) = default;

  virtual ~Spaghetti();

  void draw(Camera& s) override;
  void update() override;

private:

  Vector4 position() const override {
    return Object::position();
  }

  struct Vertex {
    Vector4 pos;
    Vector4 color;
  };

  Matrix4 velo;
  GLuint vbo;
  GLsizei count;
};
