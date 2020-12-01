#pragma once

#include <memory>

#include <openvr.h>
#include "runcontext.hpp"
#include "world_spaghetti_hunt.hpp"
#include "fatal_error.hpp"

/** Controls high level game state. */
class GameStateMachine
{
public:
	virtual ~GameStateMachine() = default;

	virtual RunContext *get_next_context() = 0;
};

class SpaghettiHuntStateMachine final: public GameStateMachine
{
public:
	SpaghettiHuntStateMachine()
	{
		if ( Options::vr_enable ) {

			// Loading the SteamVR Runtime
			vr::EVRInitError peError = vr::VRInitError_None;
			hmd = vr::VR_Init( &peError, vr::VRApplication_Scene );

			if ( peError == vr::VRInitError_None )
			{
				std::cout << "Launching glome in VR mode" << std::endl;
			}
			else
			{
				std::stringstream ss;
				ss << "OpenVR error: ";

				if ( !vr::VR_IsRuntimeInstalled() ) {
					ss << "OpenVR Runtime not detected on the system";
				}
				else if ( !vr::VR_IsHmdPresent() ) {
					ss << "HMD not detected on the system";
				}
				else {
					ss << vr::VR_GetVRInitErrorAsEnglishDescription(peError);
				}
				fatal_user_error(ss.str().c_str());
			}
		} else {
			std::cout << "Launching in non-VR mode" << std::endl;
		}
	}

	~SpaghettiHuntStateMachine()
	{
		if (hmd)
		{
			vr::VR_Shutdown();
			hmd = nullptr;
		}
	}

	RunContext *get_next_context()
	{
		world = std::make_unique<WorldSpaghettiHunt>(hmd);
		return world.get();
	}

private:
	std::unique_ptr<WorldSpaghettiHunt> world;
	vr::IVRSystem* hmd = nullptr;
};
