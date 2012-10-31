#include "controller_remote.hpp"

using namespace std;

ControllerRemote::ControllerRemote(vector<Ship*> pp, unordered_map<int, Ship*>* prp) : ControllerLocal(vector<Ship*>(pp), vector<Ship*>(), vector<AiController*>())
{
	remote_players = prp;
}

