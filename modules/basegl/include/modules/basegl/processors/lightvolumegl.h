/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2013-2021 Inviwo Foundation
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

#include <modules/basegl/baseglmoduledefine.h>
#include <modules/opengl/inviwoopengl.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/ports/volumeport.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/datastructures/light/baselightsource.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/transferfunctionproperty.h>
#include <modules/opengl/volume/volumegl.h>
#include <modules/opengl/buffer/framebufferobject.h>
#include <modules/opengl/shader/shader.h>
#include <modules/opengl/texture/texture3d.h>
#include <modules/opengl/glformats.h>

#include <array>

namespace inviwo {

/** \docpage{org.inviwo.LightVolumeGL, Light Volume}
 * ![](org.inviwo.LightVolumeGL.png?classIdentifier=org.inviwo.LightVolumeGL)
 *
 * ...
 *
 * ### Inports
 *   * __inport__ ...
 *
 * ### Outports
 *   * __outport__ ...
 *
 * ### Properties
 *   * __Light Volume Size__ ...
 *   * __Support Light Color__ ...
 *   * __Float Precision__ ...
 *   * __Transfer function__ ...
 *
 */
class IVW_MODULE_BASEGL_API LightVolumeGL : public Processor {
public:
    LightVolumeGL();
    virtual ~LightVolumeGL() = default;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

protected:
    void process() override;

    struct PropagationParameters {
        FrameBufferObject fbo;
        Texture3D tex;
        mat4 axisPermutation;
        mat4 axisPermutationINV;
        mat4 axisPermutationLight;
        vec4 permutedLightDirection;

        PropagationParameters(vec4 borderColor)
            : fbo{}, tex{makeTex(size3_t{1}, DataUInt8::id(), borderColor)} {}
        ~PropagationParameters() = default;

        static Texture3D makeTex(size3_t dim, DataFormatId format, vec4 borderColor) {
            Texture3D tex{dim, GLFormats::get(format), GL_LINEAR};
            tex.bind();
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
            glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));
            tex.initialize(nullptr);
            return tex;
        }
    };

    bool lightSourceChanged();
    bool volumeChanged(bool);

    void volumeSizeOptionChanged();
    void supportColoredLightChanged();
    void floatPrecisionChanged();

    void updatePermuationMatrices(const vec3&, PropagationParameters*, PropagationParameters*);

private:
    VolumeInport inport_;
    VolumeOutport outport_;
    std::shared_ptr<Volume> volume_;
    DataInport<LightSource> lightSource_;

    BoolProperty supportColoredLight_;
    OptionPropertyInt volumeSizeOption_;
    TransferFunctionProperty transferFunction_;
    BoolProperty floatPrecision_;

    Shader propagationShader_;
    Shader mergeShader_;

    static const vec4 borderColor_;
    vec4 lightColor_;

    std::array<PropagationParameters, 2> propParams_;

    FrameBufferObject mergeFBO_;
    float blendingFactor_;

    bool internalVolumesInvalid_;
    size3_t volumeDimOut_;
    vec3 volumeDimOutF_;
    vec3 volumeDimOutFRCP_;
    vec3 volumeDimInF_;
    vec3 volumeDimInFRCP_;
    vec3 lightDir_;
    vec3 lightPos_;
    LightSourceType lightType_;
    bool calculatedOnes_;
};

}  // namespace inviwo
