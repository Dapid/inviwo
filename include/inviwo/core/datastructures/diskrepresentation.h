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

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/util/cloneableptr.h>
#include <inviwo/core/util/exception.h>

#include <string>
#include <string_view>
#include <memory>

namespace inviwo {

template <typename Repr>
class DiskRepresentationLoader;

/**
 * \ingroup datastructures
 * Base class for all DiskRepresentations \see Data, DataRepresentation
 */
template <typename Repr, typename Self>
class DiskRepresentation {
public:
    DiskRepresentation() = default;
    DiskRepresentation(std::string_view srcFile, DiskRepresentationLoader<Repr>* loader = nullptr);
    DiskRepresentation(const DiskRepresentation& rhs) = default;
    DiskRepresentation& operator=(const DiskRepresentation& that) = default;
    virtual ~DiskRepresentation() = default;
    virtual DiskRepresentation* clone() const;

    const std::string& getSourceFile() const;
    bool hasSourceFile() const;

    void setLoader(DiskRepresentationLoader<Repr>* loader);

    std::shared_ptr<Repr> createRepresentation() const;
    void updateRepresentation(std::shared_ptr<Repr> dest) const;

private:
    std::string sourceFile_;

    // DiskRepresentation owns a DataReader to be able to convert itself into RAM.
    util::cloneable_ptr<DiskRepresentationLoader<Repr>> loader_;
};

template <typename Repr, typename Self>
DiskRepresentation<Repr, Self>::DiskRepresentation(std::string_view srcFile,
                                                   DiskRepresentationLoader<Repr>* loader)
    : sourceFile_(srcFile), loader_(loader) {}

template <typename Repr, typename Self>
DiskRepresentation<Repr, Self>* DiskRepresentation<Repr, Self>::clone() const {
    return new DiskRepresentation<Repr, Self>(*this);
}

template <typename Repr, typename Self>
const std::string& DiskRepresentation<Repr, Self>::getSourceFile() const {
    return sourceFile_;
}

template <typename Repr, typename Self>
bool DiskRepresentation<Repr, Self>::hasSourceFile() const {
    return !sourceFile_.empty();
}

template <typename Repr, typename Self>
void DiskRepresentation<Repr, Self>::setLoader(DiskRepresentationLoader<Repr>* loader) {
    loader_.reset(loader);
}

template <typename Repr, typename Self>
std::shared_ptr<Repr> DiskRepresentation<Repr, Self>::createRepresentation() const {
    if (!loader_) throw Exception("No loader available to create representation", IVW_CONTEXT);
    return loader_->createRepresentation(*static_cast<const Self*>(this));
}

template <typename Repr, typename Self>
void DiskRepresentation<Repr, Self>::updateRepresentation(std::shared_ptr<Repr> dest) const {
    if (!loader_) throw Exception("No loader available to update representation", IVW_CONTEXT);
    loader_->updateRepresentation(dest, *static_cast<const Self*>(this));
}

template <typename Repr>
class DiskRepresentationLoader {
public:
    virtual ~DiskRepresentationLoader() = default;
    virtual DiskRepresentationLoader* clone() const = 0;
    virtual std::shared_ptr<Repr> createRepresentation(const Repr&) const = 0;
    virtual void updateRepresentation(std::shared_ptr<Repr> dest, const Repr&) const = 0;
};

}  // namespace inviwo
