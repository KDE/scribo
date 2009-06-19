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

#ifndef _SCRIBO_TEXT_MATCHER_H_
#define _SCRIBO_TEXT_MATCHER_H_

#include <QtCore/QObject>
#include <QtCore/QList>

#include "scribo_export.h"

namespace Scribo {

    class TextMatch;

    /**
     * A simple wrapper around TextMatchPlugin which allows
     * to use all available plugins at the same time.
     *
     * The text matcher reads its configuration from kopencalaisrc. This
     * mainly means enabled and disabled plugins.
     */
    class SCRIBO_EXPORT TextMatcher : public QObject
    {
        Q_OBJECT

    public:
        TextMatcher( QObject* parent = 0 );
        ~TextMatcher();

        /**
         * Blocks until all matches have been found.
         */
        QList<TextMatch> getAllPossibleMatches( const QString& text );

        /**
         * \return A list of all matches.
         * Makes only sense to call after finished has been emitted.
         */
        QList<TextMatch> matches() const;

    Q_SIGNALS:
        void newMatch( const Scribo::TextMatch& match );
        void finished();

    public Q_SLOTS:
        void getPossibleMatches( const QString& text );

    private:
        class Private;
        Private* const d;

        Q_PRIVATE_SLOT( d, void _k_newMatch( const Scribo::TextMatch& ) )
        Q_PRIVATE_SLOT( d, void _k_finished( Scribo::TextMatchPlugin* ) )
    };
}

#endif
