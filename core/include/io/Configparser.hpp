#pragma once
#ifndef IO_CONFIGPARSER_H
#define IO_CONFIGPARSER_H

#include <data/Geometry.hpp>
#include <data/Spin_System.hpp>
#include <data/Spin_System_Chain.hpp>
#include <data/Parameters_Method_LLG.hpp>
#include <data/Parameters_Method_GNEB.hpp>
#include <data/Parameters_Method_MMF.hpp>
#include <engine/Hamiltonian_Heisenberg_Neighbours.hpp>
#include <engine/Hamiltonian_Heisenberg_Pairs.hpp>
#include <engine/Hamiltonian_Gaussian.hpp>

namespace IO
{
    // Note that due to the modular structure of the input parsers, input may be given in one or in separate files.
    // Input may be given incomplete. In this case a log entry is created and default values are used.
    void Log_from_Config(const std::string configFile, bool force_quiet=false);
    std::unique_ptr<Data::Spin_System> Spin_System_from_Config(const std::string configFile);
    std::shared_ptr<Data::Geometry> Geometry_from_Config(const std::string configFile);
    std::shared_ptr<Data::Pinning> Pinning_from_Config(const std::string configFile, const std::shared_ptr<Data::Geometry> geometry);
    std::unique_ptr<Data::Parameters_Method_LLG> Parameters_Method_LLG_from_Config(const std::string configFile, const std::shared_ptr<Data::Pinning> pinning);
    std::unique_ptr<Data::Parameters_Method_MC> Parameters_Method_MC_from_Config(const std::string configFile, const std::shared_ptr<Data::Pinning> pinning);
    std::unique_ptr<Data::Parameters_Method_GNEB> Parameters_Method_GNEB_from_Config(const std::string configFile, const std::shared_ptr<Data::Pinning> pinning);
    std::unique_ptr<Data::Parameters_Method_MMF> Parameters_Method_MMF_from_Config(const std::string configFile, const std::shared_ptr<Data::Pinning> pinning);
    std::unique_ptr<Engine::Hamiltonian> Hamiltonian_from_Config(const std::string configFile, const std::shared_ptr<Data::Geometry> geometry);
    std::unique_ptr<Engine::Hamiltonian_Heisenberg_Neighbours> Hamiltonian_Heisenberg_Neighbours_from_Config(const std::string configFile, const std::shared_ptr<Data::Geometry> geometry);
    std::unique_ptr<Engine::Hamiltonian_Heisenberg_Pairs> Hamiltonian_Heisenberg_Pairs_from_Config(const std::string configFile, const std::shared_ptr<Data::Geometry> geometry);
    std::unique_ptr<Engine::Hamiltonian_Gaussian> Hamiltonian_Gaussian_from_Config(const std::string configFile, const std::shared_ptr<Data::Geometry> geometry);
};// end namespace IO
#endif