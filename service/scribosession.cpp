/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "scribosession.h"
#include "textmatcher.h"
#include "textmatch.h"
#include "entity.h"

#include <KDebug>


ScriboSession::ScriboSession(QObject *parent)
    : QObject(parent)
{
    kDebug();
    m_matcher = new Scribo::TextMatcher( this );
    connect( m_matcher, SIGNAL( newMatch( Scribo::TextMatch ) ),
             this, SLOT( slotNewMatch( Scribo::TextMatch ) ) );
    connect( m_matcher, SIGNAL( finished() ),
             this, SLOT( slotFinished() ) );
}

ScriboSession::~ScriboSession()
{
    kDebug() << m_text;
}

QString ScriboSession::text() const
{
    return m_text;
}

void ScriboSession::fireAndForget()
{
    kDebug() << m_text;
    m_matcher->getPossibleMatches(m_text);
}

void ScriboSession::slotNewMatch(const Scribo::TextMatch &match)
{
    if( match.isEntity() ) {
        Scribo::Entity entity = match.toEntity();
        if(entity.localResource().isValid()) {
            emit newLocalEntity(KUrl(entity.localResource().resourceUri()).url(), match.occurrences());
        }
        else {
            emit newEntity(match.label(), match.comment(), match.occurrences());
        }
    }
}

void ScriboSession::slotFinished()
{
    kDebug() << m_text;
    emit finished();
    deleteLater();
}

void ScriboSession::setText(const QString &text)
{
    m_text = text;
}

void ScriboSession::close()
{
    kDebug() << m_text;
    emit finished();
    deleteLater();
}
