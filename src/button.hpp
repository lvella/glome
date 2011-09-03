#pragma once
#include <guichan.hpp>
#include <iostream>

class NButton : public gcn::Button
{
public:
	NButton(const std::string& caption);
	void setHL_font(gcn::Font* font);
	void draw(gcn::Graphics* graphics);
	void mouseEntered(gcn::MouseEvent& mouseEvent);
	void mouseExited(gcn::MouseEvent& mouseEvent);
	
	void init_singleplay_button();
private:
	gcn::Font* hl_font;
	bool has_mouse;
};
