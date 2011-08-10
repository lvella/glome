#include "button.hpp"
#include "menu.hpp"
#include "main.hpp"


NButton::NButton(const std::string& caption) :
	Button(caption)
{
	has_mouse = false;
}

void NButton::draw(gcn::Graphics* graphics)
{
	if (mHasMouse)
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

void NButton::mouse_entered(gcn::MouseEvent& mouseEvent)
{
	Button::mouseEntered(mouseEvent);
	has_mouse = true;
}

void NButton::mouse_exited(gcn::MouseEvent& mouseEvent)
{
	Button::mouseExited(mouseEvent);
	has_mouse = false;
}


