#include <Spirit/State.h>
#include <Spirit/IO.h>
#include <Spirit/Chain.h>
#include <Spirit/Configurations.h>

#include <data/State.hpp>
#include <data/Spin_System.hpp>
#include <data/Spin_System_Chain.hpp>
#include <io/IO.hpp>
#include <utility/Logging.hpp>
#include <utility/Exception.hpp>

#include <fmt/format.h>

#include <memory>
#include <string>

/////// TODO: make use of file format specifications
/////// TODO: implement remaining functions

/*------------------------------------------------------------------------------------------------------ */
/*--------------------------------- From Config File --------------------------------------------------- */
/*------------------------------------------------------------------------------------------------------ */

int IO_System_From_Config(State * state, const char * file, int idx_image, int idx_chain)
{
    try
    {
    	std::shared_ptr<Data::Spin_System> image;
    	std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
    	// Create System (and lock it)
    	std::shared_ptr<Data::Spin_System> system = IO::Spin_System_from_Config(std::string(file));
    	system->Lock();
    	
    	// Filter for unacceptable differences to other systems in the chain
    	for (int i = 0; i < chain->noi; ++i)
    	{
    		if (state->active_chain->images[i]->nos != system->nos) return 0;
    		// Currently the SettingsWidget does not support different images being isotropic AND 
            // anisotropic at the same time
    		if (state->active_chain->images[i]->hamiltonian->Name() != system->hamiltonian->Name()) 
                return 0;
    	}
        
    	// Set System
    	image->Lock();
    	*image = *system;
    	image->Unlock();
        
    	// Initial configuration
    	float defaultPos[3] = {0,0,0}; 
    	float defaultRect[3] = {-1,-1,-1};
    	Configuration_Random(state, defaultPos, defaultRect, -1, -1, false, false, idx_image, idx_chain);
    	
    	// Return success
    	return 1;
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
        return 0;
    }
}

/*------------------------------------------------------------------------------------------------------ */
/*-------------------------------------- Images -------------------------------------------------------- */
/*------------------------------------------------------------------------------------------------------ */

void IO_Image_Read(State * state, const char * file, int format, int idx_image, int idx_chain)
{
    try
    {
        std::shared_ptr<Data::Spin_System> image;
        std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
        // Read the data
        image->Lock();
        IO::Read_Spin_Configuration(image, std::string(file), IO::VF_FileFormat(format));
        image->Unlock();

        Log( Utility::Log_Level::Info, Utility::Log_Sender::API,
            fmt::format("Read spins from file {} with format {}", file, format),
            idx_image, idx_chain );
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}

void IO_Image_Write(State * state, const char * file, int format, int idx_image, int idx_chain)
{
    try
    {
    	std::shared_ptr<Data::Spin_System> image;
    	std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
    	// Write the data
        image->Lock();
        if (format != IO_Fileformat_OVF)
            IO::Write_Spin_Configuration(image, 0, std::string(file), false);
        else
            IO::Save_To_OVF(*image->spins, *image->geometry, std::string(file));
    	image->Unlock();

    	Log( Utility::Log_Level::Info, Utility::Log_Sender::API,
            fmt::format("Wrote spins to file {} with format {}", file, format), 
            idx_image, idx_chain );
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}

void IO_Image_Append(State * state, const char * file, int iteration, int format, int idx_image, int idx_chain)
{
    try
    {
        std::shared_ptr<Data::Spin_System> image;
        std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
        // Write the data
        image->Lock();
        IO::Write_Spin_Configuration(image, 0, std::string(file), true);
        image->Unlock();

        Log( Utility::Log_Level::Info, Utility::Log_Sender::API,
            fmt::format("Appended spins to file {} with format {}", file, format),
            idx_image, idx_chain );
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}


/*------------------------------------------------------------------------------------------------------ */
/*-------------------------------------- Chains -------------------------------------------------------- */
/*------------------------------------------------------------------------------------------------------ */

void IO_Chain_Read(State * state, const char * file, int format, int idx_chain)
{
	int idx_image = -1;

    try
    {
    	std::shared_ptr<Data::Spin_System> image;
    	std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
    	// Read the data
    	chain->Lock();
    	IO::Read_SpinChain_Configuration(chain, std::string(file));
    	chain->Unlock();

    	// Update llg simulation information array size
    	if ((int)state->method_image[idx_chain].size() < chain->noi)
    	{
    		for (int i=state->method_image[idx_chain].size(); i < chain->noi; ++i)
    			state->method_image[idx_chain].push_back( 
                    std::shared_ptr<Engine::Method>( ) );
    	}

    	// Update state
    	State_Update(state);

    	// Update array lengths
    	Chain_Setup_Data(state, idx_chain);

    	Log( Utility::Log_Level::Info, Utility::Log_Sender::API,
            fmt::format("Read chain from file {} with format {}", file, format), 
             idx_image, idx_chain );
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}

void IO_Chain_Write(State * state, const char * file, int format, int idx_chain)
{
    int idx_image = -1;

    try
    {
        std::shared_ptr<Data::Spin_System> image;
        std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
        // Read the data
        chain->Lock();
        IO::Save_SpinChain_Configuration(chain, 0, std::string(file));
        chain->Unlock();

        Log( Utility::Log_Level::Info, Utility::Log_Sender::API,
                fmt::format("Wrote chain to file {} with format {}", file, format), 
                idx_image, idx_chain );
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}

/*------------------------------------------------------------------------------------------------------ */
/*------------------------------------ Collection ------------------------------------------------------ */
/*------------------------------------------------------------------------------------------------------ */

void IO_Collection_Read(State * state, const char * file, int idx_image, int idx_chain)
{

}

void IO_Collection_Write(State * state, const char * file, int idx_image, int idx_chain)
{

}

/*------------------------------------------------------------------------------------------------------ */
/*--------------------------------------- Data --------------------------------------------------------- */
/*------------------------------------------------------------------------------------------------------ */

//IO_Energies_Spins_Save
void IO_Write_System_Energy_per_Spin(State * state, const char * file, int idx_chain)
{
    int idx_image = -1;

    try
    {
    	std::shared_ptr<Data::Spin_System> image;
    	std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
        // Write the data
        IO::Write_System_Energy_per_Spin(*image, std::string(file));
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}

//IO_Energy_Spins_Save
void IO_Write_System_Energy(State * state, const char * file, int idx_image, int idx_chain)
{
    try
    {
    	std::shared_ptr<Data::Spin_System> image;
    	std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
    	// Write the data
    	IO::Write_System_Energy(*image, std::string(file));
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}

//IO_Energies_Save
void IO_Write_Chain_Energies(State * state, const char * file, int idx_chain)
{
    int idx_image = -1;
    
    try
    {
    	std::shared_ptr<Data::Spin_System> image;
    	std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
        // Write the data
        IO::Write_Chain_Energies(*chain, idx_chain, std::string(file));
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}

//IO_Energies_Interpolated_Save
void IO_Write_Chain_Energies_Interpolated(State * state, const char * file, int idx_chain)
{
    int idx_image = -1;
    
    try
    {    
        std::shared_ptr<Data::Spin_System> image;
        std::shared_ptr<Data::Spin_System_Chain> chain;
        
        // Fetch correct indices and pointers
        from_indices( state, idx_image, idx_chain, image, chain );
        
        // Write the data
        IO::Write_Chain_Energies_Interpolated(*chain, std::string(file));
    }
    catch( ... )
    {
        Utility::Handle_Exception( idx_image, idx_chain );
    }
}