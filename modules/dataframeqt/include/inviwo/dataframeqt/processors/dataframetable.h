/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2019-2021 Inviwo Foundation
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

#include <inviwo/dataframeqt/dataframeqtmoduledefine.h>
#include <inviwo/core/processors/processor.h>
#include <inviwo/core/metadata/processorwidgetmetadata.h>
#include <inviwo/core/datastructures/bitset.h>

#include <inviwo/dataframe/datastructures/dataframe.h>
#include <inviwo/core/properties/ordinalproperty.h>
#include <inviwo/core/properties/boolproperty.h>
#include <inviwo/core/processors/exporter.h>

#include <modules/brushingandlinking/ports/brushingandlinkingports.h>

namespace inviwo {

class DataFrameTableProcessorWidget;

/** \docpage{org.inviwo.DataFrameTable, DataFrame Table}
 * ![](org.inviwo.DataFrameTable.png?classIdentifier=org.inviwo.DataFrameTable)
 * Shows the content of a DataFrame in a tabular view.
 *
 * ### Inports
 *   * __inport__               DataFrame contents to be shown in the processor widget
 *   * __brushingAndLinking__   inport for brushing & linking interactions
 *
 * ### Properties
 *   * __Show Index Column__             show/hide index column in table
 *   * __Show Category Indices__         show interal category indices for categorical columns
 *   * __Vector Components as Columns__  print glm::vec components in separate columns
 *
 */

class IVW_MODULE_DATAFRAMEQT_API DataFrameTable : public Processor,
                                                  public ProcessorWidgetMetaDataObserver,
                                                  public Exporter {
public:
    DataFrameTable();
    virtual ~DataFrameTable();

    virtual void process() override;

    void setWidgetSize(size2_t);
    size2_t getWidgetSize() const;

    virtual void setProcessorWidget(std::unique_ptr<ProcessorWidget> processorWidget) override;

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;

    /**
     * @see Exporter::exportFile
     */
    virtual std::optional<std::string> exportFile(
        std::string_view path, std::string_view name,
        const std::vector<FileExtension>& candidateExtensions, Overwrite overwrite) const override;

protected:
    virtual void onProcessorWidgetPositionChange(ProcessorWidgetMetaData*) override;
    virtual void onProcessorWidgetDimensionChange(ProcessorWidgetMetaData*) override;
    virtual void onProcessorWidgetVisibilityChange(ProcessorWidgetMetaData*) override;

    DataFrameTableProcessorWidget* getWidget() const;

private:
    DataFrameInport inport_;
    BrushingAndLinkingInport brushLinkPort_;

    IntSize2Property dimensions_;
    IntVec2Property position_;
    BoolProperty showIndexColumn_;
    BoolProperty showCategoryIndices_;
    BoolProperty showFilteredRowCols_;

    ProcessorWidgetMetaData* widgetMetaData_;
};

}  // namespace inviwo
