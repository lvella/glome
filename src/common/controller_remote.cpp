#include "controller_remote.hpp"

using namespace std;

controller_remote::controller_remote(vector<ship*>* pp, unordered_map<ship*>* prp) : controller(pp)
{
	remote_players = prp;
}

