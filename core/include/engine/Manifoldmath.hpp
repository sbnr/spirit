#pragma once
#ifndef MANIFOLDMATH_H
#define MANIFOLDMATH_H

#include <vector>
#include <memory>

#include <Eigen/Core>

#include <engine/Vectormath_Defines.hpp>

namespace Engine
{
	namespace Manifoldmath
	{
        // Get the norm of a vectorfield
        scalar norm(const vectorfield & vf);
        // Normalize a vectorfield
        void normalize(vectorfield & vf);

        // TODO: the following functions should maybe be characterised as vectorspace instead of manifold
        // Project v1 to be parallel to v2
        //    This assumes normalized vectorfields
        void project_parallel(vectorfield & vf1, const vectorfield & vf2);
        // Project v1 to be orthogonal to v2
        //    This assumes normalized vectorfields
        void project_orthogonal(vectorfield & vf1, const vectorfield & vf2);
        // Invert v1's component parallel to v2
        //    This assumes normalized vectorfields
        void invert_parallel(vectorfield & vf1, const vectorfield & vf2);
        // Invert v1's component orthogonal to v2
        //    This assumes normalized vectorfields
        void invert_orthogonal(vectorfield & vf1, const vectorfield & vf2);

        // TODO:
        // geodesic distance
        // tangent calculation for chains of vectorfields
    }
}

#endif