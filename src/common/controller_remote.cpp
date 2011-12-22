#include "controller_remote.hpp"

using namespace std;

ControllerRemote::ControllerRemote(vector<Ship*>* pp, unordered_map<Ship*>* prp) : Controller(pp)
{
	remote_players = prp;
}

