#include "ai_controller.hpp"

AiController::AiController()
{
}

AiController::~AiController()
{

}

void
AiController::act()
{
	ShipController::move(-0.1);
	ShipController::move_h(0.5);
	/*ShipController::move_s(0.5);
	ShipController::move_v(0.5);*/
}

