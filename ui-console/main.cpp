#include "Threading.h"
#include "Handle_Signal.h"

#include "Interface_State.h"
#include "Interface_Chain.h"
#include "Interface_Configurations.h"
#include "Interface_Transitions.h"
#include "Interface_Simulation.h"

// Use Core Namespaces
using namespace Data;
using namespace Engine;
using namespace Utility;

// Initialise Global Variables
std::shared_ptr<State> state;
//Utility::LoggingHandler Utility::Log = Utility::LoggingHandler(Log_Level::WARNING, Log_Level::DEBUG, ".", "Log_" + Timing::CurrentDateTime() + ".txt");
//std::map<std::shared_ptr<Data::Spin_System>, std::thread> Utility::Threading::llg_threads = std::map<std::shared_ptr<Data::Spin_System>, std::thread>();
//std::map<std::shared_ptr<Data::Spin_System_Chain>, std::thread> Utility::Threading::gneb_threads = std::map<std::shared_ptr<Data::Spin_System_Chain>, std::thread>();

// Main
int main(int argc, char ** argv)
{

	//--- Register SigInt
	signal(SIGINT, Handle_Signal::Handle_SigInt);
	
	//---------------------- file names ---------------------------------------------
	//--- Config Files
	// const char * cfgfile = "input/markus-paper.cfg";
	const char * cfgfile = "input/gideon-master-thesis-isotropic.cfg";
	// const char * cfgfile = "input/daniel-master-thesis-isotropic.cfg";
	//--- Data Files
	// std::string spinsfile = "input/anisotropic/achiral.txt";
	// std::string chainfile = "input/chain.txt";
	//-------------------------------------------------------------------------------
	
	//--- Initialise State
	state = std::shared_ptr<State>(setupState(cfgfile));
	//---------------------- initialize spin_systems --------------------------------
	// Copy the system a few times
	Chain_Image_to_Clipboard(state.get());
	for (int i=1; i<7; ++i)
	{
		Chain_Insert_Image_After(state.get());
	}
	//-------------------------------------------------------------------------------
	
	//----------------------- spin_system_chain -------------------------------------
	// Parameters
	double dir[3] = { 0,0,1 };
	double pos[3] = { 14.5, 14.5, 0 };

	// Read Image from file
	//Configuration_from_File(state.get(), spinsfile, 0);
	// Read Chain from file
	//Chain_from_File(state.get(), chainfile);

	// First image is homogeneous with a Skyrmion at pos
	Configuration_Homogeneous(state.get(), dir, 0);
	Configuration_Skyrmion(state.get(), pos, 6.0, 1.0, -90.0, false, false, false, 0);
	// Last image is homogeneous
	Configuration_Homogeneous(state.get(), dir, state->noi-1);

	// Create transition of images between first and last
	Transition_Homogeneous(state.get(), 0, state->noi-1);
	//-------------------------------------------------------------------------------

	//----------------------- LLG Iterations ----------------------------------------
	Simulation_PlayPause(state.get(), "LLG", "SIB");
	//-------------------------------------------------------------------------------


	// Finish
	Log.Send(Log_Level::ALL, Log_Sender::ALL, "=====================================================");
	Log.Send(Log_Level::ALL, Log_Sender::ALL, "================= MonoSpin Finished =================");
	Log.Send(Log_Level::ALL, Log_Sender::ALL, "=====================================================");
	Log.Append_to_File();
	return 0;
}