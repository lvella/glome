#pragma once

#include <guichan.hpp>

class NButton : public gcn::Button
{
public:
	NButton(const std::string& caption);
	void setHL_font(gcn::Font* font);
	void draw(gcn::Graphics* graphics);
	void mouseEntered(gcn::MouseEvent& mouseEvent);
	void mouseExited(gcn::MouseEvent& mouseEvent);

private:
	gcn::Font* hl_font;
	bool has_mouse;
};
