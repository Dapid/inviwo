/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2015 Inviwo Foundation
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

#ifndef IVW_IMAGEPORT_H
#define IVW_IMAGEPORT_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/ports/datainport.h>
#include <inviwo/core/ports/dataoutport.h>
#include <inviwo/core/datastructures/image/image.h>
#include <inviwo/core/interaction/events/eventhandler.h>
#include <inviwo/core/interaction/events/resizeevent.h>
#include <inviwo/core/util/imagecache.h>

namespace inviwo {

class ImageOutport;

class ImagePortBase {
public:
    virtual uvec2 getDimensions() const = 0;
    virtual void changeDataDimensions(ResizeEvent* resizeEvent,
                                      ImageOutport* target = nullptr) = 0;
    virtual bool isOutportDeterminingSize() const = 0;
    virtual void setOutportDeterminesSize(bool outportDeterminesSize) = 0;
};

template <size_t N = 1>
class BaseImageInport : public DataInport<Image, N>, public ImagePortBase {
public:
    BaseImageInport(std::string identifier, bool outportDeterminesSize = false);
    virtual ~BaseImageInport();

    /**
     * Connects this inport to the outport. Propagates the inport size to the outport if the
     * processor is an end processor (Canvas) or any of the dependent outports of this inport are
     * connected.
     *
     * @note Does not check if the outport is an ImageOutport
     * @param Outport * outport ImageOutport to connect
     */
    virtual void connectTo(Outport* outport) override;
    virtual const Image* getData() const override;
    virtual std::vector<const Image*> getVectorData() const override;
    virtual std::vector<std::pair<Outport*, const Image*>> getSourceVectorData()
        const override;
    virtual std::string getContentInfo() const override;

    // Actually returns the requested size... not size of the data.
    virtual uvec2 getDimensions() const override;

    /**
     * Handle resize event
     */
    virtual void changeDataDimensions(ResizeEvent* resizeEvent,
                                      ImageOutport* target = nullptr) override;

    virtual bool isOutportDeterminingSize() const override;
    virtual void setOutportDeterminesSize(bool outportDeterminesSize) override;

    void passOnDataToOutport(ImageOutport* outport) const;

private:
    uvec2 requestedDimensions_;
    bool outportDeterminesSize_;
};

using ImageInport = BaseImageInport<1>;
using ImageMultiInport = BaseImageInport<0>;

class IVW_CORE_API ImageOutport : public DataOutport<Image>, public EventHandler {
    template <size_t N> friend class BaseImageInport;

public:
    ImageOutport(std::string identifier, const DataFormatBase* format = DataVec4UINT8::get(),
                 bool handleResizeEvents = true);

    virtual ~ImageOutport();

    /**
     *	We will not handle resize event if we are not the data owner
     */
    virtual void setData(Image* data, bool ownsData = true) override;
    virtual void setConstData(const Image* data) override;
    const Image* getResizedImageData(uvec2 dimensions) const;

    /**
     * Handle resize event
     */
    void changeDataDimensions(ResizeEvent* resizeEvent);
    uvec2 getDimensions() const;
    /**
     * Set the dimensions of this port without propagating the size
     * through the network. Will resize the image contained within the port.
     */
    void setDimensions(const uvec2& newDimension);

    bool addResizeEventListener(EventListener*);
    bool removeResizeEventListener(EventListener*);

    /**
     * Determine if the image data should be resized during a resize event.
     * We will only resize if we own the data in the port.
     * @param handleResizeEvents True if data should be resized during a resize propagation,
     * otherwise false
     */
    void setHandleResizeEvents(bool handleResizeEvents);
    bool isHandlingResizeEvents() const;

protected:
    virtual void invalidate(InvalidationLevel invalidationLevel) override;

private:
    void updateImageFromInputSource();

    uvec2 dimensions_;
    bool handleResizeEvents_;  // True if data should be resized during a resize propagation,
                               // otherwise false

    ImageCache cache_;
};

// Image Inport
template <size_t N>
BaseImageInport<N>::BaseImageInport(std::string identifier, bool outportDeterminesSize)
    : DataInport<Image, N>(identifier)
    , requestedDimensions_(8, 8)
    , outportDeterminesSize_(outportDeterminesSize) {}

template <size_t N>
BaseImageInport<N>::~BaseImageInport() {}

template <size_t N>
void BaseImageInport<N>::connectTo(Outport* outport) {
    if (getNumberOfConnections() + 1 > getMaxNumberOfConnections())
        throw Exception("Trying to connect to a full port.", IvwContext);

    ResizeEvent resizeEvent(requestedDimensions_);
    ImageOutport* imageOutport = dynamic_cast<ImageOutport*>(outport);
    imageOutport->changeDataDimensions(&resizeEvent);

    DataInport<Image, N>::connectTo(outport);
}

// set dimensions based on port groups
template <size_t N>
void BaseImageInport<N>::changeDataDimensions(ResizeEvent* resizeEvent, ImageOutport* target) {
    requestedDimensions_ = resizeEvent->size();

    if (target) {
        target->changeDataDimensions(resizeEvent);
    } else {
        for (auto outport : getConnectedOutports()) {
            if (auto imageOutport = static_cast<ImageOutport*>(outport)) {
                imageOutport->changeDataDimensions(resizeEvent);
            }
        }
    }
}

template <size_t N>
uvec2 BaseImageInport<N>::getDimensions() const {
    return requestedDimensions_;
}

template <size_t N>
const Image* BaseImageInport<N>::getData() const {
    if (isConnected()) {
        auto outport = static_cast<const ImageOutport*>(getConnectedOutport());
        if (isOutportDeterminingSize()) {
            return outport->getConstData();
        } else {
            return outport->getResizedImageData(requestedDimensions_);
        }
    } else {
        return nullptr;
    }
}

template <size_t N /*= 1*/>
std::vector<const Image*> inviwo::BaseImageInport<N>::getVectorData() const {
    std::vector<const Image*> res(N);

    for (auto outport : connectedOutports_) {
        // Safe to static cast since we are unable to connect other outport types.
        auto dataport = static_cast<const ImageOutport*>(outport);

        if (dataport->hasData()) {
            if (isOutportDeterminingSize()) {
                res.push_back(dataport->getConstData());
            } else {
                res.push_back(dataport->getResizedImageData(requestedDimensions_));
            }
        }
    }

    return res;
}

template <size_t N>
std::vector<std::pair<Outport*, const Image*>>
inviwo::BaseImageInport<N>::getSourceVectorData() const {
    std::vector<std::pair<Outport*, const Image*>> res(N);

    for (auto outport : connectedOutports_) {
        // Safe to static cast since we are unable to connect other outport types.
        auto dataport = static_cast<ImageOutport*>(outport);
        if (dataport->hasData()) {
            if (isOutportDeterminingSize()) {
                res.emplace_back(dataport, dataport->getConstData());
            } else {
                res.emplace_back(dataport, dataport->getResizedImageData(requestedDimensions_));
            }
        }
    }

    return res;
}

template <size_t N>
bool BaseImageInport<N>::isOutportDeterminingSize() const {
    return outportDeterminesSize_;
}

template <size_t N>
void BaseImageInport<N>::setOutportDeterminesSize(bool outportDeterminesSize) {
    outportDeterminesSize_ = outportDeterminesSize;
}

template <size_t N>
std::string BaseImageInport<N>::getContentInfo() const {
    if (hasData())
        return getData()->getDataInfo();
    else
        return getClassIdentifier() + " has no data.";
}

template <size_t N>
void BaseImageInport<N>::passOnDataToOutport(ImageOutport* outport) const {
    if (hasData()) {
        const Image* img = getData();
        Image* out = outport->getData();
        if (out) img->resizeRepresentations(out, out->getDimensions());
    }
}

}  // namespace

#endif  // IVW_IMAGEPORT_H
