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

#ifndef _DATETIME_TEXT_MATCH_PLUGIN_H_
#define _DATETIME_TEXT_MATCH_PLUGIN_H_

#include "textmatchplugin.h"

#include <QtCore/QVariant>
#include <QtCore/QHash>
#include <QtCore/QPair>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QLocale>


/**
 * Extracts dates and times from a text. This plugin does only work on
 * english text (except for typically formatted times and dates maybe).
 *
 * \author Sebastian Trueg <trueg@kde.org>
 */
class DateTimeTextMatchPlugin : public Scribo::TextMatchPlugin
{
    Q_OBJECT

public:
    DateTimeTextMatchPlugin( QObject* parent, const QVariantList& );
    ~DateTimeTextMatchPlugin();

protected:
    void doGetPossibleMatches( const QString& text );

private:
    void lookForYears( const QRegExp& regExp );
    /**
     * \param forceCurrentYear Use this for pattern that do not contain a year, otherwise we will
     * end up with 1900.
     */
    void lookForDates( const QRegExp& regExp, const QString& format, bool forceCurrentYear = false );
    void lookForTimes( const QRegExp& regExp, const QString& format );
    void lookForDateRanges( const QRegExp& regExp, const QString& format, int dateCap, int rangeEndCap, int yearCap, bool forceCurrentYear = false );

    QString m_text;

    // we cache the found dates and times at their text position and
    // store the length with them
    QHash<int, int> m_years;
    QHash<int, QPair<QDate, int> > m_dates;
    QHash<int, QPair<QTime, int> > m_times;
    QHash<int, QPair<QPair<QDate, QDate>, int> > m_dateRanges;

    QLocale m_enLocale;
};

#endif
