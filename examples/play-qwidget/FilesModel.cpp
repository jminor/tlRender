// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2021-2022 Darby Johnston
// All rights reserved.

#include "FilesModel.h"

#include <tlrCore/StringFormat.h>

#include <QApplication>
#include <QPalette>

namespace tlr
{
    FilesModel::FilesModel(QObject* parent) :
        QAbstractListModel(parent)
    {}

    void FilesModel::add(
        const std::string& fileName,
        const std::string& audioFileName)
    {
        beginInsertRows(QModelIndex(), _items.size(), _items.size());
        auto item = std::make_shared<FilesModelItem>();
        item->path = file::Path(fileName);
        item->audioPath = file::Path(audioFileName);
        _items.push_back(item);
        endInsertRows();
        _current = _items.size() - 1;
        Q_EMIT currentChanged(_items[_current]);
        Q_EMIT countChanged(_items.size());
        Q_EMIT dataChanged(
            index(_current, 0),
            index(_current, 0),
            { Qt::BackgroundRole, Qt::ForegroundRole });
    }

    void FilesModel::remove()
    {
        if (_current != -1)
        {
            beginRemoveRows(QModelIndex(), _current, _current);
            _items.erase(_items.begin() + _current);
            endRemoveRows();
            _current = std::min(_current, static_cast<int>(_items.size()) - 1);
            Q_EMIT currentChanged(_current != -1 ? _items[_current] : nullptr);
            Q_EMIT countChanged(_items.size());
            if (_current != -1)
            {
                Q_EMIT dataChanged(
                    index(_current, 0),
                    index(_current, 0),
                    { Qt::BackgroundRole, Qt::ForegroundRole });
            }
        }
    }

    void FilesModel::clear()
    {
        if (!_items.empty())
        {
            beginRemoveRows(QModelIndex(), 0, _items.size() - 1);
            _items.clear();
            endRemoveRows();
            _current = -1;
            Q_EMIT currentChanged(nullptr);
            Q_EMIT countChanged(_items.size());
        }
    }

    std::shared_ptr<FilesModelItem> FilesModel::current() const
    {
        return _current != -1 ? _items[_current] : nullptr;
    }

    void FilesModel::setCurrent(const QModelIndex& index)
    {
        if (index.isValid() &&
            index.row() >= 0 &&
            index.row() < _items.size() &&
            index.row() != _current)
        {
            _current = index.row();
            Q_EMIT currentChanged(_items[_current]);
            Q_EMIT dataChanged(
                this->index(_current, 0),
                this->index(_current, 0),
                { Qt::BackgroundRole, Qt::ForegroundRole });
        }
    }

    int FilesModel::rowCount(const QModelIndex&) const
    {
        return _items.size();
    }

    QVariant FilesModel::data(const QModelIndex& index, int role) const
    {
        QVariant out;
        if (index.isValid() &&
            index.row() >= 0 &&
            index.row() < _items.size())
        {
            switch (role)
            {
            case Qt::DisplayRole:
            {
                const auto& item = _items[index.row()];
                std::string s = string::Format(
                    "{0}\n"
                    "    Video: {1}\n"
                    "    Audio: {2}").
                    arg(item->path.get(-1, false)).
                    arg(!item->avInfo.video.empty() ? item->avInfo.video[0] : imaging::Info()).
                    arg(item->avInfo.audio);
                out.setValue(QString::fromUtf8(s.c_str()));
                break;
            }
            case Qt::BackgroundRole:
                out = qApp->palette().color(index.row() == _current ?
                    QPalette::Highlight :
                    QPalette::Base);
                break;
            case Qt::ForegroundRole:
                out = qApp->palette().color(index.row() == _current ?
                    QPalette::HighlightedText :
                    QPalette::Foreground);
                break;
            default: break;
            }
        }
        return out;
    }

    void FilesModel::first()
    {
        if (!_items.empty() && _current != 0)
        {
            _current = 0;
            Q_EMIT currentChanged(_items[_current]);
            if (_current != -1)
            {
                Q_EMIT dataChanged(
                    index(_current, 0),
                    index(_current, 0),
                    { Qt::BackgroundRole, Qt::ForegroundRole });
            }
        }
    }

    void FilesModel::last()
    {
        if (!_items.empty() && _current != _items.size() - 1)
        {
            _current = _items.size() - 1;
            Q_EMIT currentChanged(_items[_current]);
            if (_current != -1)
            {
                Q_EMIT dataChanged(
                    index(_current, 0),
                    index(_current, 0),
                    { Qt::BackgroundRole, Qt::ForegroundRole });
            }
        }
    }

    void FilesModel::next()
    {
        if (_items.size() > 1)
        {
            ++_current;
            if (_current >= _items.size())
            {
                _current = 0;
            }
            Q_EMIT currentChanged(_items[_current]);
            if (_current != -1)
            {
                Q_EMIT dataChanged(
                    index(_current, 0),
                    index(_current, 0),
                    { Qt::BackgroundRole, Qt::ForegroundRole });
            }
        }
    }

    void FilesModel::prev()
    {
        if (_items.size() > 1)
        {
            --_current;
            if (_current < 0)
            {
                _current = _items.size() - 1;
            }
            Q_EMIT currentChanged(_items[_current]);
            if (_current != -1)
            {
                Q_EMIT dataChanged(
                    index(_current, 0),
                    index(_current, 0),
                    { Qt::BackgroundRole, Qt::ForegroundRole });
            }
        }
    }
}
