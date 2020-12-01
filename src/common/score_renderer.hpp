#pragma once

#include "renderer.hpp"

class ScoreRenderer:
	public SpaceViewRenderer
{
public:
	template<class... Args>
	ScoreRenderer(Args&&... args):
		SpaceViewRenderer(std::forward<Args>(args)...),
		score(gltCreateText(), gltDeleteText)
	{
		assert(score);

		set_score(0);

		// gltCreateText() messes with VAO, so we need to reset:
		glBindVertexArray(VertexArrayID);
	}

	virtual void update(float dt) override;
	virtual void draw(ObjSet& objs) override;

private:
	void set_score(uint64_t points);
	void set_score_if_different(uint64_t points);

	static constexpr float score_offset = 15.0f;

	std::unique_ptr<GLTtext, decltype(&gltDeleteText)> score;
	uint64_t last_set_score;
	float score_anim_effect = 0.0f;

	friend class ScoreGameOverRenderer;
};

class ScoreGameOverRenderer:
	public SpaceViewRenderer
{
public:
	ScoreGameOverRenderer(std::shared_ptr<ShipController> c, ScoreRenderer&& sr);

	virtual void update(float dt) override;
	virtual void draw(ObjSet& objs) override;

	bool done();

private:
	struct Interpolator
	{
		float from;
		float to;

		float operator()(float t);
	};

	std::unique_ptr<GLTtext, decltype(&gltDeleteText)> score;
	std::unique_ptr<GLTtext, decltype(&gltDeleteText)> game_over;
	std::unique_ptr<GLTtext, decltype(&gltDeleteText)> restart_msg;

	static constexpr float final_offset = 40.0f;
	static constexpr float anim_duration = 3.5f; // In seconds

	float time = 0.0f;
	float t = 0.0f;
	float width, height;
	Interpolator x, y, scale,
		score_g, score_b;

	std::shared_ptr<ShipController> controller;
};
