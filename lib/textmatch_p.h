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

#ifndef _SCRIBO_TEXT_MATCH_P_H_
#define _SCRIBO_TEXT_MATCH_P_H_

#include "textoccurrence.h"

#include <Soprano/Graph>

#include <QtCore/QList>
#include <QtCore/QAtomicInt>


namespace Scribo {
    class TextMatchPrivate
    {
    public:
        TextMatchPrivate();
        virtual ~TextMatchPrivate();

        virtual TextMatchPrivate* clone() const = 0;

        Soprano::Graph m_rdf;
        QList<TextOccurrence> m_occurrences;

        QAtomicInt ref;

        virtual QString label() const { return QString(); }
        virtual QString comment() const { return QString(); }

        virtual bool isEntity() const { return false; }
    };
}

#endif
