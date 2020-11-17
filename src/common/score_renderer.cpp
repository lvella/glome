#include "score_renderer.hpp"


void ScoreRenderer::update(float dt)
{
	SpaceViewRenderer::update(dt);

	// Scale of the score text fades after 1 second:
	score_anim_effect = std::max(0.0f, score_anim_effect - dt);

	if(auto ptr = target.lock()) {
		set_score_if_different(ptr->ctrl->get_points());
	}
}

void ScoreRenderer::draw(ObjSet& objs)
{
	SpaceViewRenderer::draw(objs);

	// Draw score text:
	gltBeginDraw();
	gltColor(1.0f, 0.5f, std::min(1.0f, 0.5f + score_anim_effect), 1.0f);
	gltDrawText2D(score.get(), score_offset, score_offset,
		3.0f + score_anim_effect);
	gltEndDraw();
	glBindVertexArray(VertexArrayID);
}

void
ScoreRenderer::set_score(uint64_t points)
{
	char score_text[64];
	snprintf(score_text, sizeof score_text, "Score: %lu", points);

	gltSetText(score.get(), score_text);
	glBindVertexArray(VertexArrayID);

	last_set_score = points;
}

void
ScoreRenderer::set_score_if_different(uint64_t points)
{
	if(points != last_set_score) {
		set_score(points);
		// Animate text at every score change:
		score_anim_effect += 0.7;
	}
}

ScoreGameOverRenderer::ScoreGameOverRenderer(
	std::shared_ptr<ShipController> c, ScoreRenderer&& sr
):
	SpaceViewRenderer(std::move(sr)),
	score(std::move(sr.score)),
	game_over(gltCreateText(), gltDeleteText),
	restart_msg(gltCreateText(), gltDeleteText),
	x{ScoreRenderer::score_offset, final_offset},
	y{ScoreRenderer::score_offset, 80.0f},
	scale{3.0f + sr.score_anim_effect, 5.0f},
	score_g{0.5f, 1.0f},
	score_b{std::min(1.0f, 0.5f + sr.score_anim_effect), 0.2f},
	controller{std::move(c)}
{
	gltSetText(game_over.get(), "Game over.");
	gltSetText(restart_msg.get(), "Press \"Fire\" to restart");

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Screen size:
	width = viewport[2];
	height =  viewport[3];

	// Limit maximum score width to screen visibility:
	const float max_width = viewport[2] - 2.0f * final_offset;
	const auto score_width = gltGetTextWidth(score.get(), scale.to);
	if(score_width > max_width) {
		scale.to *= max_width / score_width;
	}
}

void ScoreGameOverRenderer::update(float dt)
{
	SpaceViewRenderer::update(dt);

	time = time + dt;

	t = std::pow(std::min(1.0f, time / anim_duration), 3.0f);
}

void ScoreGameOverRenderer::draw(ObjSet& objs)
{
	SpaceViewRenderer::draw(objs);

	// Draw texts:
	gltBeginDraw();

	gltColor(1.0f, score_g(t), score_b(t), 1.0f);
	gltDrawText2D(score.get(), x(t), y(t), scale(t));

	gltColor(1.0f, 0.8f, 0.8f, t);
	gltDrawText2DAligned(
		game_over.get(), width * 0.5f, height * 0.5f, 6.0f,
		GLT_CENTER, GLT_CENTER
	);

	const float post_anim_time = time - anim_duration;
	if(post_anim_time > 0.0f)
	{
		gltColor(1.0f, 0.8f, 0.8f, std::min(1.0f, post_anim_time));
		gltDrawText2DAligned(
			restart_msg.get(),
			width-5.0f, height, 3.0f,
			GLT_RIGHT, GLT_BOTTOM
		);
	}

	gltEndDraw();
	glBindVertexArray(VertexArrayID);
}

float ScoreGameOverRenderer::Interpolator::operator()(float t)
{
	return from + t * (to - from);
}

bool ScoreGameOverRenderer::done()
{
	return time > anim_duration && controller->get_shot();
}
