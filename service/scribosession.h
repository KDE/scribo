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

#ifndef SCRIBOSESSION_H
#define SCRIBOSESSION_H

#include <QObject>
#include <QtCore/QList>

#include "textoccurrence.h"

namespace Scribo {
class TextMatch;
class TextMatcher;
}

class ScriboSession : public QObject
{
    Q_OBJECT
    Q_CLASSINFO( "D-Bus Interface", "org.kde.nepomuk.ScriboSession" )

public:
    ScriboSession(QObject *parent = 0);
    ~ScriboSession();

public Q_SLOTS:
    void setText(const QString& text);
    Q_SCRIPTABLE void start();
    Q_SCRIPTABLE QString text() const;

Q_SIGNALS:
    /**
     * \param resource The resource URI encoded via KUrl::url()
     * \param occurrences The occurrences in the text
     */
    Q_SCRIPTABLE void newLocalEntity( const QString& resource, const QList<Scribo::TextOccurrence>& occurrences );
    Q_SCRIPTABLE void newEntity( const QString& label, const QString& description, const QList<Scribo::TextOccurrence>& occurrences );
    Q_SCRIPTABLE void finished();

public Q_SLOTS:
    void close();

private Q_SLOTS:
    void slotNewMatch( const Scribo::TextMatch& match );
    void slotFinished();

private:
    QString m_text;
    Scribo::TextMatcher* m_matcher;
};

#endif // SCRIBOSESSION_H
