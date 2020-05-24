#include "button.hpp"

NButton::NButton(const std::string& caption) :
	Button(caption),
	has_mouse(false)
{
	setFrameSize(0);
}

void NButton::draw(gcn::Graphics* graphics)
{
	if (has_mouse)
	{
		graphics->setFont(hl_font);
		graphics->drawText(getCaption(),0,0);
	}
	else
	{
		graphics->setFont(getFont());
		graphics->drawText(getCaption(),0,0);
	}
}

void NButton::setHL_font(gcn::Font* font)
{
	hl_font = font;
}

void NButton::mouseEntered(gcn::MouseEvent& mouseEvent)
{
	Button::mouseEntered(mouseEvent);
	has_mouse = true;
}

void NButton::mouseExited(gcn::MouseEvent& mouseEvent)
{
	Button::mouseExited(mouseEvent);
	has_mouse = false;
}
