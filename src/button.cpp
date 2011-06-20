#include "button.hpp"
#include "menu.hpp"
#include "main.hpp"
#include "game.hpp"

namespace Button
{
	gcn::Button* singleplay_button;
	class ButtonActionListener : public gcn::ActionListener
	{
		public:
		void action(const gcn::ActionEvent& actionEvent)
		{
			if (actionEvent.getSource() == singleplay_button)
			{
				Game::init_game();
				Game::main_loop();
			}
		}
	};
	ButtonActionListener* buttonActionListener;
	/*
		* Init SinglePlayer Button
		* TODO: Organize button files better.
		* TODO: Enhance the default ugly button (in progress).
	*/
	void init_singleplay_button()
	{
		singleplay_button = new gcn::Button("Singleplayer");
		singleplay_button->setPosition(250, 300);
		singleplay_button->setActionEventId("singleplay_button");
		Menu::top->add(Button::singleplay_button);
		buttonActionListener = new ButtonActionListener();
		singleplay_button->addActionListener(buttonActionListener);
		
	}
}


