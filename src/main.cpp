#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "main.hpp"
#include "menu.hpp"
#include "parser.hpp"

using namespace std;
using namespace boost::asio::ip;

int WIDTH = 800;
int HEIGHT = 600;
const float FOV = 45.0f;
bool FULLSCREEN = false;
bool SLOW = false;
bool isServer = false;
bool isClient = false;
bool isSplit = true;


boost::asio::io_service gIOService;
string host;
short port = 0;
World* world;

int main(int argc, char **argv)
{

	if (Parser::parse_args(argc, argv))
		return 1;
	/*
	 * TODO: Parser for variables.
	 * TODO: Network in game archive.
	 */
  srand(time(NULL));

	try
	{
		Menu::menu_initialize();
		Menu::menu_run();
	}
	
	catch (gcn::Exception e)
	{
 		cout << e.getMessage() << endl;
		return 1;
	}	
 	catch (std::exception e)
	{
 	  cout << "Std exception: " << e.what() << endl;
		return 1;
	}
	catch (...)
	{
		cout << "Unknown exception" << endl;
		return 1;
	}
	
}

