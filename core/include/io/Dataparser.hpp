
#pragma once
#ifndef IO_DATAPARSER_H
#define IO_DATAPARSER_H

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
    void Read_From_OVF( vectorfield & vf, const Data::Geometry & geometry, std::string inputfilename );
    void Read_Spin_Configuration_CSV(std::shared_ptr<Data::Spin_System> s, const std::string file);
    void Read_Spin_Configuration(std::shared_ptr<Data::Spin_System> s, const std::string file, VF_FileFormat format = VF_FileFormat::CSV_POS_SPIN);
    void Read_SpinChain_Configuration(std::shared_ptr<Data::Spin_System_Chain> c, const std::string file);
    
    void External_Field_from_File(const std::string externalFieldFile, const std::shared_ptr<Data::Geometry> geometry, int & n_indices,
        intfield & external_field_index, scalarfield & external_field_magnitude, vectorfield & external_field_normal);
    void Anisotropy_from_File(const std::string anisotropyFile, const std::shared_ptr<Data::Geometry> geometry, int & n_indices,
        intfield & anisotropy_index, scalarfield & anisotropy_magnitude, vectorfield & anisotropy_normal);
    void Pairs_from_File(const std::string pairsFile, const std::shared_ptr<Data::Geometry> geometry, int & nop,
        pairfield & exchange_pairs, scalarfield & exchange_magnitudes,
        pairfield & dmi_pairs, scalarfield & dmi_magnitudes, vectorfield & dmi_normals);
    void Quadruplets_from_File(const std::string quadrupletsFile, const std::shared_ptr<Data::Geometry> geometry, int & noq,
        quadrupletfield & quadruplets, scalarfield & quadruplet_magnitudes);
    void Defects_from_File(const std::string defectsFile, int & n_defects,
        intfield & defect_indices, intfield & defect_types);
    void Pinned_from_File(const std::string pinnedFile, int & n_pinned,
        intfield & pinned_indices, vectorfield & pinned_spins);
};// end namespace IO
#endif