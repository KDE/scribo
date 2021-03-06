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

#ifndef _NEPOMUK_SCRIBO_SERVICE_H_
#define _NEPOMUK_SCRIBO_SERVICE_H_

#include <Nepomuk/Service>
#include <QtCore/QVariant>

class QDBusObjectPath;
class ScriboSession;

namespace Nepomuk {
    class ScriboService : public Service
    {
        Q_OBJECT
        Q_CLASSINFO( "D-Bus Interface", "org.kde.nepomuk.Scribo" )

    public:
        /**
         * both paramters are basically unused but forced by the KDE
         * plugin system.
         */
        ScriboService( QObject* parent, const QVariantList& );
        ~ScriboService();

    public Q_SLOTS:
        Q_SCRIPTABLE QDBusObjectPath analyzeText(const QString& text);
        Q_SCRIPTABLE QDBusObjectPath analyzeResource(const QString& uri);

    private:
        QDBusObjectPath registerSession(ScriboSession* session);

        int m_sessionCnt; // used for unique dbus object path names
    };
}

#endif
