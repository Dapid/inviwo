/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2017 Inviwo Foundation
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

#ifndef IVW_PROPERTYWIDGETQT_H
#define IVW_PROPERTYWIDGETQT_H

#include <modules/qtwidgets/qtwidgetsmoduledefine.h>
#include <modules/qtwidgets/inviwodockwidget.h>
#include <inviwo/core/properties/propertyvisibility.h>
#include <inviwo/core/properties/propertywidget.h>
#include <inviwo/core/properties/propertyobserver.h>
#include <inviwo/core/util/observer.h>
#include <inviwo/core/properties/optionproperty.h>

#include <warn/push>
#include <warn/ignore/all>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <warn/pop>

class QMenu;
class QAction;
class QActionGroup;

namespace inviwo {

class PropertyListWidget;
class Property;
using BaseCallBack = std::function<void()>;


class IVW_MODULE_QTWIDGETS_API PropertyWidgetQt : public QWidget,
                                                  public PropertyWidget,
                                                  public PropertyObserver {
#include <warn/push>
#include <warn/ignore/all>
    Q_OBJECT
#include <warn/pop>
public:
    PropertyWidgetQt();
    PropertyWidgetQt(Property* property);
    virtual ~PropertyWidgetQt();

    virtual QMenu* getContextMenu();

    // Should be called first thing after the property has been added to a layout.
    virtual void initState();

    static int MINIMUM_WIDTH;
    static int SPACING;
    static int MARGIN;
    static void setSpacingAndMargins(QLayout* layout);

    virtual void onChildVisibilityChange(PropertyWidgetQt* child);

    // PropertyObservable overrides
    virtual void onSetSemantics(const PropertySemantics& semantics) override;
    virtual void onSetReadOnly(bool readonly) override;
    virtual void onSetVisible(bool visible) override;
    virtual void onSetUsageMode(UsageMode usageMode) override;

    // QWidget overrides
    virtual QSize sizeHint() const override;
    virtual QSize minimumSizeHint() const override;

    void setNestedDepth(int depth);
    int getNestedDepth() const;

    PropertyWidgetQt* getParentPropertyWidget() const;
    InviwoDockWidget* getBaseContainer() const;
    void setParentPropertyWidget(PropertyWidgetQt* parent, InviwoDockWidget* widget);

public slots:
    virtual void updateContextMenu();
    virtual void resetPropertyToDefaultState();
    virtual void showContextMenu(const QPoint& pos);
    virtual void setDeveloperUsageMode(bool value);
    virtual void setApplicationUsageMode(bool value);
    virtual void changeSemantics(QAction* action);

signals:
    void updateSemantics(PropertyWidgetQt*);

protected:
    virtual void setVisible(bool visible) override;
    UsageMode getApplicationUsageMode();

    // Context menu
    void generateContextMenu();
    void generatePresetMenuActions();
    void updatePresetMenuActions();

    void generateModuleMenuActions();
    void updateModuleMenuActions();

    virtual bool event(QEvent* event) override;  //< for custom tooltips.

    void paintEvent(QPaintEvent* pe) override;

    // Actions
    QMenu* usageModeItem_;
    QActionGroup* usageModeActionGroup_;
    QAction* developerUsageModeAction_;
    QAction* applicationUsageModeAction_;
    QAction* copyAction_;
    QAction* pasteAction_;
    QAction* copyPathAction_;

    QMenu* semanicsMenuItem_;
    QActionGroup* semanticsActionGroup_;

private:
    PropertyWidgetQt* parent_;
    InviwoDockWidget* baseContainer_;

    TemplateOptionProperty<UsageMode>* applicationUsageMode_;
    const BaseCallBack* appModeCallback_;
    QMenu* contextMenu_;
    std::unordered_map<std::string, std::unique_ptr<QMenu>> moduleSubMenus_;
    QMenu* appPresetMenu_;
    QMenu* workspacePresetMenu_;
    QMenu* propertyPresetMenu_;

    const int maxNumNestedShades_;  //< This number has do match the number of shades in the qss.
    int nestedDepth_;
};

}  // namespace inviwo

#endif  // IVW_PROPERTYWIDGETQT_H
