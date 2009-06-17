/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _SCRIBO_TEXT_MATCH_H_
#define _SCRIBO_TEXT_MATCH_H_

#include <QtCore/QList>

#include "textoccurrence.h"

#include "scribo_export.h"

class QString;

namespace Soprano {
    class Graph;
}

namespace Scribo {

    class TextMatchPrivate;
    class Entity;
    class Statement;

    class SCRIBO_EXPORT TextMatch
    {
    public:
        TextMatch();
        TextMatch( const TextMatch& );
        virtual ~TextMatch();

        TextMatch& operator=( const TextMatch& );

        QList<TextOccurrence> occurrences() const;

        QString label() const;
        QString comment() const;

        bool isEntity() const;
        bool isStatement() const;

        Soprano::Graph rdf() const;

        void addOccurrence( const TextOccurrence& o );

        Entity toEntity() const;
        Statement toStatement() const;

        bool isValid() const;
        bool isEmpty() const;

    private:
        TextMatch( TextMatchPrivate* );

        void detach();

        TextMatchPrivate* d;

        friend class Entity;
        friend class Statement;
    };
}

#endif
