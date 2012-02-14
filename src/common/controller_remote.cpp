#include "controller_remote.hpp"

using namespace std;

ControllerRemote::ControllerRemote(vector<Ship*>* pp, unordered_map<int, Ship*>* prp) : ControllerLocal(pp, pp, ai_c)
{
	remote_players = prp;
}

