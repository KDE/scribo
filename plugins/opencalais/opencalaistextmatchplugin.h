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

#ifndef _OPEN_CALAIS_TEXT_MATCH_PLUGIN_H_
#define _OPEN_CALAIS_TEXT_MATCH_PLUGIN_H_

#include "textmatchplugin.h"

#include <QtCore/QVariant>
#include <QtCore/QHash>

#include <Soprano/Node> // for qHash(QUrl)


namespace OpenCalais {
    class LookupJob;
}
class KJob;
class Worker;

class OpenCalaisTextMatchPlugin : public Scribo::TextMatchPlugin
{
    Q_OBJECT

public:
    OpenCalaisTextMatchPlugin( QObject* parent, const QVariantList& );
    ~OpenCalaisTextMatchPlugin();

    QUrl matchPimoType( const QUrl& openCalaisType );

public Q_SLOTS:
    void slotConfigure();

protected:
    void doGetPossibleMatches( const QString& text );

private Q_SLOTS:
    void slotResult( KJob* );

private:
    OpenCalais::LookupJob* m_lookupJob;
    QHash<QUrl, QUrl> m_typeMap;

    Worker* m_worker;
};

#endif
