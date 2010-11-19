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

#include "scriboservice.h"
#include "scribosession.h"

#include <KPluginFactory>
#include <KDebug>

#include <Soprano/Model>
#include <Soprano/StatementIterator>
#include <Soprano/Statement>
#include <Soprano/Node>
#include <Soprano/NodeIterator>
#include <Soprano/QueryResultIterator>

#include <Nepomuk/Resource>
#include <Nepomuk/ResourceManager>
#include <Nepomuk/Variant>

#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusObjectPath>


/**
 * this marco creates a service factory which can then be found by the Qt/KDE
 * plugin system in the Nepomuk server.
 */
NEPOMUK_EXPORT_SERVICE( Nepomuk::ScriboService, "nepomukscriboservice")


Nepomuk::ScriboService::ScriboService( QObject* parent, const QList<QVariant>& )
    : Service( parent ),
      m_sessionCnt(0)
{
}


Nepomuk::ScriboService::~ScriboService()
{
}

QDBusObjectPath Nepomuk::ScriboService::analyzeText(const QString &text)
{
    ScriboSession* session = new ScriboSession(this);
    session->setText(text);

    const QString dbusObjectPath = QString( "/nepomukscriboservice/scribosession%1" ).arg( ++m_sessionCnt );
    QDBusConnection::sessionBus().registerObject( dbusObjectPath, session );

    QMetaObject::invokeMethod(session, "fireAndForget", Qt::QueuedConnection);

    return QDBusObjectPath(dbusObjectPath);
}

#include "scriboservice.moc"
