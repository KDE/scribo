/*
   Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _SCRIBO_TEXT_MATCHES_MODEL_H_
#define _SCRIBO_TEXT_MATCHES_MODEL_H_

#include <QtCore/QAbstractItemModel>

#include "scribo_export.h"


namespace Scribo {

    class TextMatchesModelPrivate;
    class TextMatch;

    class SCRIBO_EXPORT TextMatchesModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        TextMatchesModel( QObject* parent = 0 );
        ~TextMatchesModel();

        int columnCount( const QModelIndex& parent = QModelIndex() ) const;
        QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
        QModelIndex parent( const QModelIndex& index ) const;
        int rowCount( const QModelIndex& parent = QModelIndex() ) const;
        QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const;

        TextMatch matchForIndex( const QModelIndex& index ) const;
        void clear();

        enum Roles {
            RelevanceRole = 6876879,
            StartPosRole = 875957,
            LengthRole = 5764876
        };

    public Q_SLOTS:
        void addMatch( const Scribo::TextMatch& match );

    private:
        TextMatchesModelPrivate* const d;
    };
}

#endif
