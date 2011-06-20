#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include "main.hpp"
#include "menu.hpp"

using namespace std;
using namespace boost::asio::ip;

const int WIDTH = 800;
const int HEIGHT = 600;
const float FOV = 45.0f;
boost::asio::io_service gIOService;
bool isServer;
bool isClient;
string host;
short port;
World* world;
bool isSplit;

int main(int argc, char **argv)
{
	/*
	 * TODO: Parser for variables.
	 * TODO: Network in game archive.
	 */
  srand(time(NULL));
  
	// Configure network
	if(argc == 1)
	{
	  isServer = isClient = false;
	  isSplit = true;
	}
	else if(argc == 4)
	{
		host = argv[2];
	  port = atoi(argv[3]);
	  isSplit = false;
	  isServer = (atoi(argv[1]) == 0);
	  isClient = !isServer;
	}
	
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

