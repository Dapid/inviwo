/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2021 Inviwo Foundation
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

#include <inviwo/volume/processors/volumeregionmap.h>
#include <modules/base/algorithm/volume/volumevoronoi.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>

namespace inviwo {

// The Class Identifier has to be globally unique. Use a reverse DNS naming scheme
const ProcessorInfo VolumeRegionMap::processorInfo_{
    "org.inviwo.VolumeRegionMap",                               // Class identifier
    "Volume Region Map",                                        // Display name
    "Volume Operation",                                         // Category
    CodeState::Stable,                                          // Code state
    Tag::CPU | Tag{"Volume"} | Tag{"Atlas"} | Tag{"DataFrame"}  // Tags
};
const ProcessorInfo VolumeRegionMap::getProcessorInfo() const { return processorInfo_; }

VolumeRegionMap::VolumeRegionMap()
    : Processor()
    , dataFrame_("mapping_indexes")
    , inport_("inport")
    , outport_("outport")
    , from_{"from", "From", dataFrame_, ColumnOptionProperty::AddNoneOption::No, 1}
    , to_{"to", "to", dataFrame_, ColumnOptionProperty::AddNoneOption::No, 2} {

    addPort(inport_);
    addPort(dataFrame_);
    addPort(outport_);
    addProperties(from_, to_);
}

namespace {
constexpr auto copyColumn = [](const Column& col, auto& dstContainer, auto assign) {
    col.getBuffer()->getRepresentation<BufferRAM>()->dispatch<void, dispatching::filter::Scalars>(
        [&](auto buf) {
            const auto& data = buf->getDataContainer();
            for (auto&& [src, dst] : util::zip(data, dstContainer)) {
                std::invoke(assign, src, dst);
            }
        });
};
}

void VolumeRegionMap::process() {
    // Get columns
    auto csv = dataFrame_.getData();
    auto oldIdx = csv->getColumn(from_.getSelectedValue());
    auto newIdx = csv->getColumn(to_.getSelectedValue());
    auto nrows = csv->getNumberOfRows();

    // Store in vectors
    std::vector<uint32_t> sourceIndices(nrows);
    std::vector<uint32_t> destinationIndices(nrows);

    copyColumn(*oldIdx, sourceIndices,
               [](const auto& src, auto& dst) { dst = static_cast<uint32_t>(src); });
    copyColumn(*newIdx, destinationIndices,
               [](const auto& src, auto& dst) { dst = static_cast<uint32_t>(src); });

    // Volume
    auto inVolume = inport_.getData();
    auto newVolume = std::shared_ptr<Volume>(inVolume->clone());
    auto volRep = newVolume->getEditableRepresentation<VolumeRAM>();

    volRep->dispatch<void, dispatching::filter::Scalars>([&](auto volram) {
        using ValueType = util::PrecisionValueType<decltype(volram)>;
        ValueType* dataPtr = volram->getDataTyped();

        const auto& dim = volram->getDimensions();

        std::transform(dataPtr, dataPtr + dim.x * dim.y * dim.z, dataPtr, [&](const ValueType& v) {
            for (size_t i = 0; i < sourceIndices.size(); ++i) {
                if (static_cast<uint32_t>(v) == sourceIndices[i]) {
                    return static_cast<ValueType>(destinationIndices[i]);
                }
            }
            return ValueType{0};
        });
    });

    outport_.setData(newVolume);
}

}  // namespace inviwo