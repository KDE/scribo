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

#ifndef _PIMO_TEXT_MATCH_PLUGIN_H_
#define _PIMO_TEXT_MATCH_PLUGIN_H_

#include "textmatchplugin.h"

#include <QtCore/QVariant>
#include <QtCore/QStringList>

/**
 * TextMatchPlugin that simply looks through the text, looking for each word
 * longer than N chars in the nao:prefLabel of each thing in the Nepomuk db.
 */
class PimoTextMatchPlugin : public Scribo::TextMatchPlugin
{
    Q_OBJECT

public:
    PimoTextMatchPlugin( QObject* parent, const QVariantList& );
    ~PimoTextMatchPlugin();

protected:
    void doGetPossibleMatches( const QString& text );

private Q_SLOTS:
    void scanText();
    bool queryWord( const QString& word );

private:
    QString m_text;
    int m_pos;
    QStringList m_stopWords;
};

#endif
