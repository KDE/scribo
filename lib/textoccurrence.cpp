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

#include "textoccurrence.h"
#include "textoccurrence_p.h"

Scribo::TextOccurrencePrivate::TextOccurrencePrivate()
    : m_startPos( 0 ),
      m_length( 0 ),
      m_relevance( 0.0 )
{
}


Scribo::TextOccurrence::TextOccurrence()
    : d( new TextOccurrencePrivate() )
{
}


Scribo::TextOccurrence::TextOccurrence( const TextOccurrence& other )
{
    d = other.d;
}


Scribo::TextOccurrence::~TextOccurrence()
{
}


Scribo::TextOccurrence& Scribo::TextOccurrence::operator=( const TextOccurrence& other )
{
    d = other.d;
    return *this;
}


int Scribo::TextOccurrence::startPosition() const
{
    return d->m_startPos;
}


int Scribo::TextOccurrence::endPosition() const
{
    return d->m_startPos + d->m_length;
}


int Scribo::TextOccurrence::length() const
{
    return d->m_length;
}


double Scribo::TextOccurrence::relevance() const
{
    return d->m_relevance;
}


void Scribo::TextOccurrence::setStartPos( int i )
{
    d->m_startPos = i;
}


void Scribo::TextOccurrence::setLength( int len )
{
    d->m_length = len;
}


void Scribo::TextOccurrence::setRelevance( double r )
{
    d->m_relevance = r;
}
