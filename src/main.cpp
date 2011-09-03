#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "main.hpp"
#include "menu.hpp"
#include "options.hpp"

using namespace std;
using namespace boost::asio::ip;

boost::asio::io_service gIOService;
World* world;

int main(int argc, char **argv)
{

	if (Options::parse_args(argc, argv))
		return 1;
	/*
	 * TODO: Parser for variables (doing).
	 * TODO: Network in game archive.
	 */
  srand(time(NULL));

	try
	{
		Menu::menu_initialize();
		Menu::menu_run();
		Menu::menu_halt();
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

