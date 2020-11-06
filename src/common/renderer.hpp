#pragma once

#include <deque>
#include <memory>
#include <unordered_map>
#include <vector>
#include <list>

#include "audio_world.hpp"
#include "audio_listener.hpp"
#include "drawable.hpp"
#include "shader.hpp"
#include "qrot.hpp"
#include "math.hpp"
#include "ship.hpp"
#include "frustum.hpp"
#include "gltext.hpp"

class Renderer
{
public:
	static void initialize();

	using ObjSet = std::unordered_multimap<
		DrawSpecsBase*,
		std::weak_ptr<Glome::Drawable>
	>;

	virtual ~Renderer() = default;

	virtual void update(float dt) = 0;
	virtual void draw(ObjSet& objs) = 0;

protected:
	static GLuint VertexArrayID;

	// Camera position relative to target...
	static const QRot cam_offset;

	static const Frustum frustum_at_origin;
};

class MapRenderer
{
public:
	virtual ~MapRenderer() = default;
	virtual void fill_minimap(const Renderer::ObjSet& objs, Camera& cam) = 0;
};

class MultiViewRenderer final: public Renderer
{
public:
	MultiViewRenderer(std::vector<std::unique_ptr<Renderer>>&& sub_renderers);

	void update(float dt) override;
	void draw(ObjSet& objs) override;

private:
	struct Viewport
	{
		std::unique_ptr<Renderer> renderer;
		int _x, _y, _w, _h;
	};

	std::vector<Viewport> viewports;
};

class PlayerScoreRenderer:
	public Renderer,
	public MapRenderer,
	public Audio::Listener,
	public NonCopyable
{
public:
	PlayerScoreRenderer(std::weak_ptr<Ship> player, Audio::World &audio_world);

	virtual void update(float dt) override;
	virtual void draw(ObjSet& objs) override;

	virtual void fill_minimap(const ObjSet& objs, Camera& cam) final;

	virtual const QRot &transformation() const override;
protected:

	void set_score(uint64_t points);
	void set_score_if_different(uint64_t points);
	void draw_objects(Camera& camera, ObjSet& objs);
	void draw_score();

	struct PathPoint {
		float dt;
		QRot t;
	};

	std::weak_ptr<Ship> target;
	std::deque<PathPoint> cam_hist;
	QRot curr_qrot;

	std::unique_ptr<GLTtext, decltype(&gltDeleteText)> score;
	uint64_t last_set_score;
	float score_anim_effect = 0.0f;
};
