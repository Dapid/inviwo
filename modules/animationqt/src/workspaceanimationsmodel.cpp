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

#include <modules/animationqt/workspaceanimationsmodel.h>

#include <modules/qtwidgets/inviwoqtutils.h>

#include <inviwo/core/util/zip.h>

namespace inviwo {

namespace animation {

AnimationsModel::AnimationsModel(WorkspaceAnimations& animations, QObject* parent)
    : QAbstractListModel(parent), animations_{animations} {

    onChangedHandle_ = animations_.onChanged_.add([this](size_t index, Animation& anim) {
        auto it = animations_.find(&anim);
        if (it != animations_.end()) {
            anim.addObserver(this);
        } else {
            anim.removeObserver(this);
        }
        auto idx = createIndex(static_cast<int>(index), 0);
        emit dataChanged(idx, idx);
    });
}

Qt::ItemFlags AnimationsModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) {
        return QAbstractListModel::flags(index) | Qt::ItemIsDropEnabled;
    } else {
        return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
    }
}
QVariant AnimationsModel::headerData([[maybe_unused]] int section,
                                     [[maybe_unused]] Qt::Orientation orientation,
                                     [[maybe_unused]] int role) const {
    return "Animations";
}
int AnimationsModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    } else {
        return static_cast<int>(animations_.size());
    }
}
QVariant AnimationsModel::data(const QModelIndex& index, int role) const {
    if (index.row() < 0 || index.row() >= static_cast<int>(animations_.size())) {
        return QVariant();
    } else if (role == Qt::DisplayRole || role == Qt::EditRole) {
        return QVariant(utilqt::toQString(animations_[index.row()].getName()));
    }

    return QVariant();
}

bool AnimationsModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (index.row() >= 0 && index.row() < static_cast<int>(animations_.size()) &&
        (role == Qt::EditRole || role == Qt::DisplayRole)) {
        auto valueString = utilqt::fromQString(value.toString());
        if (animations_[index.row()].getName() == valueString) {
            return true;
        } else {
            animations_[index.row()].setName(valueString);
            emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
            return true;
        }
    } else {
        return false;
    }
}

bool AnimationsModel::insertRows(int row, int count, const QModelIndex& parent) {
    if (count < 1 || row < 0 || row > rowCount(parent)) {
        return false;
    }
    beginInsertRows(QModelIndex(), row, row + count - 1);
    for (auto i = row; i < row + count; i++) {
        animations_.insert(static_cast<size_t>(i), fmt::format("Animaton {}", i + 1));
    }
    endInsertRows();
    return true;
}

bool AnimationsModel::removeRows(int row, int count, const QModelIndex& parent) {
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent)) {
        return false;
    }
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    for (int i = row + count - 1; i >= row; i--) {
        animations_.erase(i);
    }
    endRemoveRows();
    return true;
}

void AnimationsModel::onNameChanged(Animation* anim) {

    if (auto it = animations_.find(anim); it != animations_.end()) {
        auto index = static_cast<int>(std::distance(animations_.begin(), it));
        auto fromI = createIndex(static_cast<int>(index), 0);
        auto toI = createIndex(static_cast<int>(index), 0);
        emit dataChanged(fromI, toI);
    }
}

}  // namespace animation

}  // namespace inviwo
