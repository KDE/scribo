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

#include "statement.h"
#include "statement_p.h"


Scribo::TextMatchPrivate* Scribo::StatementPrivate::clone() const
{
    StatementPrivate* s = new StatementPrivate();
    s->m_rdf = m_rdf;
    s->m_occurrences = m_occurrences;
    s->m_verb = m_verb;
    s->m_entity = m_entity;
    return s;
}


QString Scribo::StatementPrivate::label() const
{
    return m_label;
}


Scribo::Statement::Statement()
    : TextMatch( new StatementPrivate() )
{
}


Scribo::Statement::Statement( const QString& verb, const Entity& entity, const QString& label, const Soprano::Graph& rdf )
    : TextMatch( new StatementPrivate() )
{
    StatementPrivate* p = static_cast<StatementPrivate*>( d );
    p->m_rdf = rdf;
    p->m_verb = verb;
    p->m_entity = entity;
    p->m_label = label;
}


Scribo::Statement::~Statement()
{
}


Scribo::Statement& Scribo::Statement::operator=( const Statement& other )
{
    d = other.d;
    d->ref.ref();
    return *this;
}


QString Scribo::Statement::verb() const
{
    return static_cast<StatementPrivate*>( d )->m_verb;
}


Scribo::Entity Scribo::Statement::entity() const
{
    return static_cast<StatementPrivate*>( d )->m_entity;
}
