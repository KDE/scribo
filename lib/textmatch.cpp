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

#include "textmatch.h"
#include "textmatch_p.h"
#include "entity.h"
#include "statement.h"

#include <KDebug>


namespace {
    class TextMatchSharedNull : public Scribo::TextMatchPrivate
    {
    public:
        TextMatchSharedNull() {
            ref.ref();
        }

        Scribo::TextMatchPrivate* clone() const { return const_cast<TextMatchSharedNull*>( this ); }
    };

    Q_GLOBAL_STATIC( TextMatchSharedNull, sharedNull )
}

Scribo::TextMatchPrivate::TextMatchPrivate()
{
}


Scribo::TextMatchPrivate::~TextMatchPrivate()
{
}


Scribo::TextMatch::TextMatch()
    : d( sharedNull() )
{
    d->ref.ref();
}


Scribo::TextMatch::TextMatch( const TextMatch& other )
    : d( other.d )
{
    d->ref.ref();
}


Scribo::TextMatch::TextMatch( TextMatchPrivate* p )
    : d( p )
{
    d->ref.ref();
}


Scribo::TextMatch::~TextMatch()
{
    if ( !d->ref.deref() )
        delete d;
}


void Scribo::TextMatch::detach()
{
    TextMatchPrivate* p = d->clone();
    d->ref.deref();
    p->ref.ref();
    d = p;
}


Scribo::TextMatch& Scribo::TextMatch::operator=( const TextMatch& other )
{
    d = other.d;
    d->ref.ref();
    return *this;
}


QList<Scribo::TextOccurrence> Scribo::TextMatch::occurrences() const
{
    return d->m_occurrences;
}


Soprano::Graph Scribo::TextMatch::rdf() const
{
    return d->m_rdf;
}


void Scribo::TextMatch::addOccurrence( const TextOccurrence& o )
{
    if ( d->ref != 1 )
        detach();
    d->m_occurrences << o;
}


bool Scribo::TextMatch::isEntity() const
{
    return d->isEntity();
}


bool Scribo::TextMatch::isStatement() const
{
    return !d->isEntity();
}


Scribo::Entity Scribo::TextMatch::toEntity() const
{
    if ( isEntity() ) {
        Entity e;
        e.d = d;
        d->ref.ref();
        return e;
    }
    else {
        return Entity();
    }
}


Scribo::Statement Scribo::TextMatch::toStatement() const
{
    if ( isStatement() ) {
        Statement s;
        s.d = d;
        d->ref.ref();
        return s;
    }
    else {
        return Statement();
    }
}


QString Scribo::TextMatch::label() const
{
    return d->label();
}


QString Scribo::TextMatch::comment() const
{
    return d->comment();
}


bool Scribo::TextMatch::isValid() const
{
    // FIXME
    return true;
}


bool Scribo::TextMatch::isEmpty() const
{
    // FIXME
    return false;
}
