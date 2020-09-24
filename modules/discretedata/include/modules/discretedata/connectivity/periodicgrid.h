/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2020 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#pragma once

#include <modules/discretedata/connectivity/structuredgrid.h>

namespace inviwo {
namespace discretedata {

/**
 * \brief A curvilinear grid in nD, with some dimensions set to wrap
 * Assume first point in a dimension equals the last point in that dimension
 */
template <ind N>
class PeriodicGrid : public StructuredGrid<N> {
public:
    using StructuredGrid<N>::indexFromLinear;
    using StructuredGrid<N>::indexToLinear;
    // using StructuredGrid<N>::numPrimitives_;  //::NumVerticesPerDimension;
    /**
     * \brief Create an nD grid
     * @param gridDimension Dimension of grid (not vertices)
     * @param numCellsPerDim Number of cells in each dimension, expect size gridDimension+1
     */
    PeriodicGrid(const std::array<ind, N>& numCellsPerDim,
                 const std::array<bool, N>& isDimPeriodic);
    virtual ~PeriodicGrid() = default;

    virtual ind getNumVerticesInDimension(ind dim) const override;

    bool isPeriodic(ind dim) const { return isDimPeriodic_[dim]; }

    void setPeriodic(ind dim, bool periodic = true) { isDimPeriodic_[dim] = periodic; }

    virtual void getConnections(std::vector<ind>& result, ind index, GridPrimitive from,
                                GridPrimitive to, bool isPosition = false) const override;

protected:
    void sameLevelConnection(std::vector<ind>& result, ind idxLin,
                             const std::array<ind, N>& size) const;

protected:
    std::array<bool, N> isDimPeriodic_;
};

}  // namespace discretedata
}  // namespace inviwo

#include "periodicgrid.inl"
