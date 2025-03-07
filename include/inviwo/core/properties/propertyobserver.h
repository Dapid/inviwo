/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2015-2021 Inviwo Foundation
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
#include <inviwo/core/util/observer.h>
#include <inviwo/core/properties/propertysemantics.h>
#include <inviwo/core/properties/propertyvisibility.h>

namespace inviwo {

class Property;

class IVW_CORE_API PropertyObserver : public Observer {
public:
    PropertyObserver() = default;
    virtual ~PropertyObserver() = default;

    virtual void onSetIdentifier(Property* property, const std::string& identifier);
    virtual void onSetDisplayName(Property* property, const std::string& displayName);
    virtual void onSetSemantics(Property* property, const PropertySemantics& semantics);
    virtual void onSetReadOnly(Property* property, bool readonly);
    virtual void onSetVisible(Property* property, bool visible);
    virtual void onSetUsageMode(Property* property, UsageMode usageMode);
};

class IVW_CORE_API PropertyObservable : public Observable<PropertyObserver> {
protected:
    PropertyObservable() = default;

    void notifyObserversOnSetIdentifier(Property* property, const std::string& identifier);
    void notifyObserversOnSetDisplayName(Property* property, const std::string& displayName);
    void notifyObserversOnSetSemantics(Property* property, const PropertySemantics& semantics);
    void notifyObserversOnSetReadOnly(Property* property, bool readonly);
    void notifyObserversOnSetVisible(Property* property, bool visible);
    void notifyObserversOnSetUsageMode(Property* property, UsageMode usageMode);
};

struct IVW_CORE_API PropertyObserverDelegate : public PropertyObserver {
    std::function<void(Property*, const std::string&)> onIdentifierChange;
    std::function<void(Property*, const std::string&)> onDisplayNameChange;
    std::function<void(Property*, const PropertySemantics&)> onSemanticsChange;
    std::function<void(Property*, bool)> onReadOnlyChange;
    std::function<void(Property*, bool)> onVisibleChange;
    std::function<void(Property*, UsageMode)> onUsageModeChange;

    virtual void onSetIdentifier(Property* property, const std::string& identifier) override {
        if (onIdentifierChange) onIdentifierChange(property, identifier);
    }
    virtual void onSetDisplayName(Property* property, const std::string& displayName) override {
        if (onDisplayNameChange) onDisplayNameChange(property, displayName);
    }
    virtual void onSetSemantics(Property* property, const PropertySemantics& semantics) override {
        if (onSemanticsChange) onSemanticsChange(property, semantics);
    }
    virtual void onSetReadOnly(Property* property, bool readonly) override {
        if (onReadOnlyChange) onReadOnlyChange(property, readonly);
    }
    virtual void onSetVisible(Property* property, bool visible) override {
        if (onVisibleChange) onVisibleChange(property, visible);
    }
    virtual void onSetUsageMode(Property* property, UsageMode usageMode) override {
        if (onUsageModeChange) onUsageModeChange(property, usageMode);
    }
};

}  // namespace inviwo
