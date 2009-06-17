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

#ifndef _SCRIBO_TEXT_ENTITY_H_
#define _SCRIBO_TEXT_ENTITY_H_

#include "textmatch.h"

#include "scribo_export.h"

namespace Nepomuk {
    namespace Types {
        class Class;
    }
}

namespace Scribo {

    class EntityPrivate;

    class SCRIBO_EXPORT Entity : public TextMatch
    {
    public:
        Entity();
        Entity( const Entity& );
        Entity( const QString& label, const Nepomuk::Types::Class& type, const Soprano::Graph& rdf );
        ~Entity();

        Entity& operator=( const Entity& );

        Nepomuk::Types::Class type() const;
    };
}

#endif
