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

#include "entity.h"
#include "entity_p.h"

#include <Soprano/Graph>

#include <KDebug>


Scribo::TextMatchPrivate* Scribo::EntityPrivate::clone() const
{
    EntityPrivate* ep = new EntityPrivate;
    ep->m_rdf = m_rdf;
    ep->m_occurrences = m_occurrences;
    ep->m_name = m_name;
    ep->m_type = m_type;
    return ep;
}


QString Scribo::EntityPrivate::comment() const
{
    return m_type.label() + ": " + label();
}


Scribo::Entity::Entity()
    : TextMatch( new EntityPrivate() )
{
}


Scribo::Entity::Entity( const Entity& other )
    : TextMatch( other.d )
{
}


Scribo::Entity::Entity( const QString& label, const Nepomuk::Types::Class& type, const Soprano::Graph& rdf )
    : TextMatch( new EntityPrivate() )
{
    EntityPrivate* p = static_cast<EntityPrivate*>( d );

    p->m_name = label;
    p->m_rdf = rdf;
    p->m_type = type;
}


Scribo::Entity::~Entity()
{
}


Scribo::Entity& Scribo::Entity::operator=( const Entity& other )
{
    d = other.d;
    d->ref.ref();
    return *this;
}


Nepomuk::Types::Class Scribo::Entity::type() const
{
    return static_cast<EntityPrivate*>( d )->m_type;
}
