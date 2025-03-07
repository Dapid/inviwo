/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2014-2021 Inviwo Foundation
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

#include <modules/basegl/processors/multichannelraycaster.h>
#include <inviwo/core/io/serialization/versionconverter.h>
#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/image/layergl.h>
#include <modules/opengl/texture/textureunit.h>

#include <modules/opengl/shader/shaderutils.h>
#include <modules/opengl/volume/volumeutils.h>
#include <modules/opengl/texture/textureutils.h>

#include <inviwo/core/datastructures/volume/volume.h>
#include <inviwo/core/algorithm/boundingbox.h>
#include <inviwo/core/util/zip.h>
#include <inviwo/core/util/stringconversion.h>

#include <fmt/format.h>

namespace inviwo {

const ProcessorInfo MultichannelRaycaster::processorInfo_{
    "org.inviwo.MultichannelRaycaster",  // Class identifier
    "Multichannel Raycaster",            // Display name
    "Volume Rendering",                  // Category
    CodeState::Experimental,             // Code state
    Tags::GL,                            // Tags
};
const ProcessorInfo MultichannelRaycaster::getProcessorInfo() const { return processorInfo_; }

MultichannelRaycaster::MultichannelRaycaster()
    : Processor()
    , shader_("multichannelraycaster.frag", Shader::Build::No)
    , volumePort_("volume")
    , entryPort_("entry")
    , exitPort_("exit")
    , backgroundPort_("bg")
    , outport_("outport")
    , transferFunctions_("transfer-functions", "Transfer functions")
    , tfs_{{{"transferFunction1", "Channel 1", &volumePort_},
            {"transferFunction2", "Channel 2", &volumePort_},
            {"transferFunction3", "Channel 3", &volumePort_},
            {"transferFunction4", "Channel 4", &volumePort_}}}
    , raycasting_("raycaster", "Raycasting")
    , camera_("camera", "Camera", util::boundingBox(volumePort_))
    , lighting_("lighting", "Lighting", &camera_)
    , positionIndicator_("positionindicator", "Position Indicator") {

    transferFunctions_.addProperties(tfs_[0], tfs_[1], tfs_[2], tfs_[3]);
    for (auto&& [i, tf] : util::enumerate(tfs_)) {
        HistogramSelection selection{};
        selection[i] = true;
        tf.setHistogramSelection(selection);
        tf.setCurrentStateAsDefault();
    }

    shader_.onReload([this]() { invalidate(InvalidationLevel::InvalidResources); });

    addPort(volumePort_, "VolumePortGroup");
    addPort(entryPort_, "ImagePortGroup1");
    addPort(exitPort_, "ImagePortGroup1");
    addPort(outport_, "ImagePortGroup1");
    addPort(backgroundPort_, "ImagePortGroup1");

    backgroundPort_.setOptional(true);

    addProperties(raycasting_, camera_, lighting_, positionIndicator_, transferFunctions_);

    volumePort_.onChange([this]() { initializeResources(); });

    backgroundPort_.onConnect([&]() { this->invalidate(InvalidationLevel::InvalidResources); });
    backgroundPort_.onDisconnect([&]() { this->invalidate(InvalidationLevel::InvalidResources); });
}

void MultichannelRaycaster::initializeResources() {
    utilgl::addShaderDefines(shader_, raycasting_);
    utilgl::addShaderDefines(shader_, camera_);
    utilgl::addShaderDefines(shader_, lighting_);
    utilgl::addShaderDefines(shader_, positionIndicator_);
    utilgl::addShaderDefinesBGPort(shader_, backgroundPort_);

    if (volumePort_.hasData()) {
        size_t channels = volumePort_.getData()->getDataFormat()->getComponents();
        for (auto&& [i, tf] : util::enumerate(tfs_)) {
            tf.setVisible(i < channels);
        }

        shader_.getFragmentShaderObject()->addShaderDefine("NUMBER_OF_CHANNELS",
                                                           fmt::format("{}", channels));
        StrBuffer str;
        for (size_t i = 0; i < channels; ++i) {
            str.append(
                "color[{0}] = APPLY_CHANNEL_CLASSIFICATION(transferFunction{1}, voxel, {0});", i,
                i + 1);
        }
        shader_.getFragmentShaderObject()->addShaderDefine("SAMPLE_CHANNELS", str.view());

        shader_.build();
    }
}

void MultichannelRaycaster::process() {
    utilgl::activateAndClearTarget(outport_);
    shader_.activate();

    TextureUnitContainer units;
    utilgl::bindAndSetUniforms(shader_, units, volumePort_);
    utilgl::bindAndSetUniforms(shader_, units, entryPort_, ImageType::ColorDepthPicking);
    utilgl::bindAndSetUniforms(shader_, units, exitPort_, ImageType::ColorDepth);
    if (backgroundPort_.hasData()) {
        utilgl::bindAndSetUniforms(shader_, units, backgroundPort_, ImageType::ColorDepthPicking);
    }

    size_t channels = volumePort_.getData()->getDataFormat()->getComponents();
    for (size_t channel = 0; channel < channels; channel++) {
        utilgl::bindAndSetUniforms(shader_, units, tfs_[channel]);
    }
    utilgl::setUniforms(shader_, outport_, camera_, lighting_, raycasting_, positionIndicator_);

    utilgl::singleDrawImagePlaneRect();

    shader_.deactivate();
    utilgl::deactivateCurrentTarget();
}

void MultichannelRaycaster::deserialize(Deserializer& d) {
    util::renamePort(d, {{&entryPort_, "entry-points"}, {&exitPort_, "exit-points"}});

    NodeVersionConverter vc([](TxElement* node) {
        if (TxElement* p1 = xml::getElement(
                node, "InPorts/InPort&type=org.inviwo.ImageMultiInport&identifier=exit-points")) {
            p1->SetAttribute("identifier", "exit");
        }
        if (TxElement* p2 = xml::getElement(
                node, "InPorts/InPort&type=org.inviwo.ImageMultiInport&identifier=entry-points")) {
            p2->SetAttribute("identifier", "entry");
        }
        return true;
    });

    d.convertVersion(&vc);
    Processor::deserialize(d);
}

}  // namespace inviwo
