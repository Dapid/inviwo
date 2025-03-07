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

#include <modules/plotting/plottingmodule.h>
#include <modules/plotting/processors/dataframecolumntocolorvector.h>
#include <modules/plotting/properties/axisproperty.h>
#include <modules/plotting/properties/axisstyleproperty.h>
#include <modules/plotting/properties/categoricalaxisproperty.h>
#include <modules/plotting/properties/marginproperty.h>
#include <modules/plotting/properties/plottextproperty.h>
#include <modules/plotting/properties/tickproperty.h>

namespace inviwo {

PlottingModule::PlottingModule(InviwoApplication* app) : InviwoModule(app, "Plotting") {

    registerProcessor<plot::DataFrameColumnToColorVector>();
    registerProperty<plot::AxisProperty>();
    registerProperty<plot::AxisStyleProperty>();
    registerProperty<plot::CategoricalAxisProperty>();
    registerProperty<plot::MajorTickProperty>();
    registerProperty<plot::MarginProperty>();
    registerProperty<plot::MinorTickProperty>();
    registerProperty<plot::PlotTextProperty>();
    registerProperty<plot::TickProperty>();
}

int PlottingModule::getVersion() const { return 1; }

std::unique_ptr<VersionConverter> PlottingModule::getConverter(int version) const {
    return std::make_unique<Converter>(version);
}

PlottingModule::Converter::Converter(int version) : version_(version) {}

bool PlottingModule::Converter::convert(TxElement* root) {
    bool res = false;
    switch (version_) {
        case 0: {
            res |= xml::changeAttribute(
                root, {{xml::Kind::processor("org.inviwo.VolumToDataFrame")}}, "type",
                "org.inviwo.VolumeToDataFrame", "org.inviwo.VolumeSequenceToDataFrame");

            return res;
        }
        default:
            return false;  // No changes
    }
}

}  // namespace inviwo
