/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2016-2021 Inviwo Foundation
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

#include <modules/base/algorithm/volume/volumecurl.h>

#include <inviwo/core/util/volumeramutils.h>
#include <inviwo/core/util/indexmapper.h>
#include <inviwo/core/util/templatesampler.h>
#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/datastructures/volume/volumeram.h>
#include <inviwo/core/datastructures/volume/volumeramprecision.h>

namespace inviwo {
namespace util {

std::unique_ptr<Volume> curlVolume(std::shared_ptr<const Volume> volume) {
    return curlVolume(*volume);
}

std::unique_ptr<Volume> curlVolume(const Volume& volume) {
    auto newVolume = std::make_unique<Volume>(volume, noData);
    auto newVolumeRep = std::make_shared<VolumeRAMPrecision<vec3>>(volume.getDimensions());
    newVolume->addRepresentation(newVolumeRep);

    const auto m = newVolume->getCoordinateTransformer().getDataToWorldMatrix();

    const auto a = m * vec4(0, 0, 0, 1);
    const auto b = m * vec4(1.0f / vec3(volume.getDimensions() - size3_t(1)), 1);
    const auto spacing = b - a;

    const vec3 ox(spacing.x, 0, 0);
    const vec3 oy(0, spacing.y, 0);
    const vec3 oz(0, 0, spacing.z);

    volume.getRepresentation<VolumeRAM>()->dispatch<void, dispatching::filter::Vec3s>([&](auto
                                                                                              vol) {
        using ValueType = util::PrecisionValueType<decltype(vol)>;
        using ComponentType = typename ValueType::value_type;
        using FloatType =
            typename std::conditional_t<std::is_same<float, ComponentType>::value, float, double>;
        using Sampler = TemplateVolumeSampler<ValueType, FloatType>;

        util::IndexMapper3D index(volume.getDimensions());
        auto data = newVolumeRep->getDataTyped();
        float minV = std::numeric_limits<float>::max();
        float maxV = std::numeric_limits<float>::lowest();

        const auto worldSpace = Sampler::Space::World;
        const Sampler sampler(volume, worldSpace);

        util::forEachVoxel(*vol, [&](const size3_t& pos) {
            const vec3 world{m * vec4(vec3(pos) / vec3(volume.getDimensions() - size3_t(1)), 1)};

            const auto Fxp = static_cast<vec3>(sampler.sample(world + ox));
            const auto Fxm = static_cast<vec3>(sampler.sample(world - ox));
            const auto Fyp = static_cast<vec3>(sampler.sample(world + oy));
            const auto Fym = static_cast<vec3>(sampler.sample(world - oy));
            const auto Fzp = static_cast<vec3>(sampler.sample(world + oz));
            const auto Fzm = static_cast<vec3>(sampler.sample(world - oz));

            const vec3 Fx = (Fxp - Fxm) / (2.0f * spacing.x);
            const vec3 Fy = (Fyp - Fym) / (2.0f * spacing.y);
            const vec3 Fz = (Fzp - Fzm) / (2.0f * spacing.z);

            const vec3 c{Fy.z - Fz.y, Fz.x - Fx.z, Fx.y - Fy.x};

            minV = std::min({minV, c.x, c.y, c.z});
            maxV = std::max({maxV, c.x, c.y, c.z});

            data[index(pos)] = c;
        });

        auto range = std::max(std::abs(minV), std::abs(maxV));
        newVolume->dataMap_.dataRange = dvec2(-range, range);
        newVolume->dataMap_.valueRange = dvec2(minV, maxV);
    });

    return newVolume;
}

}  // namespace util
}  // namespace inviwo
