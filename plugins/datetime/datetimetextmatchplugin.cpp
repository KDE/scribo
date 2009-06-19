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

#include "datetimetextmatchplugin.h"
#include "textoccurrence.h"
#include "entity.h"
#include "statement.h"
#include "pimo.h"

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Node>
#include <Soprano/Graph>

#include <Nepomuk/Types/Class>
#include <Nepomuk/Types/Property>
#include <Nepomuk/Variant>

#include <KPluginFactory>
#include <KDebug>

#include <QtCore/QRegExp>


DateTimeTextMatchPlugin::DateTimeTextMatchPlugin( QObject* parent, const QVariantList& )
    : TextMatchPlugin( parent ),
      m_enLocale( QLocale::English )
{
}


DateTimeTextMatchPlugin::~DateTimeTextMatchPlugin()
{
}


void DateTimeTextMatchPlugin::doGetPossibleMatches( const QString& text )
{
    //
    // check for years (numbers between 1900 and 2020 for example)
    // check for stuff like "June 22" or even "June 22-26th" (the latter can be used as start and end time already)
    // check for "2 o'clock"
    // check for "14:23"
    // check for 12.6.2009 and 6/12/2009 and 6/12/09 and 6.12.09
    //

    m_years.clear();
    m_dates.clear();
    m_times.clear();
    m_dateRanges.clear();
    m_text = text;

    // we are english-only here!
    QStringList longMonthNames;
    QStringList shortMonthNames;
    for ( int i = 1; i <= 12; ++i ) {
        longMonthNames << m_enLocale.monthName( i, QLocale::LongFormat );
        shortMonthNames << m_enLocale.monthName( i, QLocale::ShortFormat );
    }

    //
    // most of the dates and times can be checked with QRegExp
    //

    // DD.MM.YYYY
    QRegExp date1( "\\b\\d{1,2}\\.\\d{1,2}\\.\\d{4,4}\\b" );

    // DD.MM.YY
    QRegExp date2( "\\b\\d{1,2}\\.\\d{1,2}\\.\\d{2,2}\\b" );

    // MM/DD/YYYY
    QRegExp date3( "\\b\\d{1,2}/\\d{1,2}/\\d{4,4}\\b" );

    // MM/DD/YY
    QRegExp date4( "\\b\\d{1,2}/\\d{1,2}/\\d{2,2}\\b" );

    // January MM [YYYY] (no word boundry at the end for 'st' or 'nd' or 'th') (also excluding ranges)
    QRegExp date5( QString( "\\b(%1)\\s\\d{1,2}(?!(\\d|\\s?-\\s?\\d))(\\s\\d{4,4})?" ).arg( longMonthNames.join( "|" ) ) );

    // January, MM [YYYY] (no word boundry at the end for 'st' or 'nd' or 'th') (also excluding ranges)
    QRegExp date6( QString( "\\b(%1),\\s?\\d{1,2}(?!(\\d|\\s?-\\s?\\d))(\\s\\d{4,4})?" ).arg( longMonthNames.join( "|" ) ) );

    // FIXME: date ranges 1-4

    QRegExp dateRange5( QString( "(\\b(?:%1)\\s\\d{1,2})\\s?-\\s?(\\d{1,2})(\\s\\d{4,4})?" ).arg( longMonthNames.join( "|" ) ) );

    QRegExp dateRange6( QString( "(\\b(?:%1),\\s?\\d{1,2})\\s?-\\s?(\\d{1,2})(\\s\\d{4,4})?" ).arg( longMonthNames.join( "|" ) ) );

    // YYYY (exclude those in full dates matched by the above)
    QRegExp year( "[^\\./]\\d{4,4}\\b" );

    // hh:mm[pm|am]
    QRegExp time1( "\\b\\d{1,2}\\:\\d{2,2}\\s?(pm|am|AM|PM)?\\b" );

    // hh:mm
    QRegExp time2( "\\b\\d{1,2}\\:\\d{2,2}\\b(?!\\s?(pm|am|AM|PM))\\b" );

    // hh o'clock
//    QRegExp time3( "\\b\\d{1,2}\\so'clock\\b" );

    lookForYears( year );

    lookForDates( date1, "d.M.yyyy" );
    lookForDates( date2, "d.M.yy" );
    lookForDates( date3, "M/d/yyyy" );
    lookForDates( date4, "M/d/yy" );
    lookForDates( date5, "MMMM d", true );
    lookForDates( date5, "MMMM d yyyy" );
    lookForDates( date6, "MMMM, d", true );
    lookForDates( date6, "MMMM,d", true );
    lookForDates( date6, "MMMM, d yyyy" );
    lookForDates( date6, "MMMM,d yyyy" );

    lookForDateRanges( dateRange5, "MMMM d", 1, 2, 3, true );
    lookForDateRanges( dateRange5, "MMMM d yyyy", 1, 2, 3, false );
    lookForDateRanges( dateRange6, "MMMM,d", 1, 2, 3, true );
    lookForDateRanges( dateRange6, "MMMM, d", 1, 2, 3, true );
    lookForDateRanges( dateRange6, "MMMM,d yyyy", 1, 2, 3, false );
    lookForDateRanges( dateRange6, "MMMM, d yyyy", 1, 2, 3, false );

    lookForTimes( time1, "h:map" );
    lookForTimes( time1, "h:m ap" );
    lookForTimes( time2, "h:m" );

    // FIXME: do a date and time proximity search to create combined datetime objects

    //
    // Now use the dates and times to create statements
    //
    for ( QHash<int, QPair<QDate, int> >::const_iterator it = m_dates.constBegin();
          it != m_dates.constEnd(); ++it ) {
        // FIXME: this is not great: 1. dtstart has range dateTime, 2. we do not know that it is a start
        //        better use something else or even create Scribo::Literal as alternative to Entity
        Scribo::Statement s( Nepomuk::Vocabulary::PIMO::dtstart(), Nepomuk::Variant( it.value().first ), Soprano::Graph() );
        Scribo::TextOccurrence oc;
        oc.setStartPos( it.key() );
        oc.setLength( it.value().second );
//        oc.setRelevance( 0.9 ); ?????????
        s.addOccurrence( oc );
        addNewMatch( s );
    }

    for ( QHash<int, QPair<QTime, int> >::const_iterator it = m_times.constBegin();
          it != m_times.constEnd(); ++it ) {
        // FIXME: this is not great: 1. dtstart has range dateTime, 2. we do not know that it is a start
        //        better use something else or even create Scribo::Literal as alternative to Entity
        Scribo::Statement s( Nepomuk::Vocabulary::PIMO::dtstart(), Nepomuk::Variant( it.value().first ), Soprano::Graph() );
        Scribo::TextOccurrence oc;
        oc.setStartPos( it.key() );
        oc.setLength( it.value().second );
//        oc.setRelevance( 0.9 ); ?????????
        s.addOccurrence( oc );
        addNewMatch( s );
    }

    for ( QHash<int, QPair<QPair<QDate, QDate>, int> >::const_iterator it = m_dateRanges.constBegin();
          it != m_dateRanges.constEnd(); ++it ) {
        // FIXME: this is not great: 1. dtstart has range dateTime, 2. we do not know that it is a start
        //        better use something else or even create Scribo::Literal as alternative to Entity
        Scribo::Statement s1( Nepomuk::Vocabulary::PIMO::dtstart(), Nepomuk::Variant( it.value().first.first ), Soprano::Graph() );
        Scribo::TextOccurrence oc1;
        oc1.setStartPos( it.key() );
        oc1.setLength( it.value().second );
//        oc.setRelevance( 0.9 ); ?????????
        s1.addOccurrence( oc1 );
        addNewMatch( s1 );

        Scribo::Statement s2( Nepomuk::Vocabulary::PIMO::dtend(), Nepomuk::Variant( it.value().first.second ), Soprano::Graph() );
        Scribo::TextOccurrence oc2;
        oc2.setStartPos( it.key() );
        oc2.setLength( it.value().second );
//        oc.setRelevance( 0.9 ); ?????????
        s2.addOccurrence( oc2 );
        addNewMatch( s2 );
    }

    emitFinished();
}


void DateTimeTextMatchPlugin::lookForYears( const QRegExp& regExp )
{
//    kDebug() << regExp.pattern();
    int pos = 0;
    while ( ( pos = regExp.indexIn( m_text, pos ) ) != -1 ) {
        int year = regExp.cap( 0 ).toInt();
        // This is a rough check which will become more and more invalid over the years ;-)
        if ( year > 1901 && year < 2050 ) {
            kDebug() << "Found year" << year << "at" << pos;
            m_years.insert( pos, year );
        }

        pos += regExp.matchedLength();
    }
}


void DateTimeTextMatchPlugin::lookForDates( const QRegExp& regExp, const QString& format, bool forceCurrentYear )
{
//    kDebug() << regExp.pattern() << format << forceCurrentYear;
    int pos = 0;
    while ( ( pos = regExp.indexIn( m_text, pos ) ) != -1 ) {
        QDate date = m_enLocale.toDate( regExp.cap( 0 ), format );
        if ( date.isValid() ) {
            if ( forceCurrentYear )
                date = QDate( QDate::currentDate().year(), date.month(), date.day() );
            kDebug() << "Found date" << date << "at" << pos;
            m_dates.insert( pos, qMakePair( date, regExp.matchedLength() ) );
        }
        else {
            kDebug() << "Failed to parse date" << regExp.cap( 0 );
        }

        pos += regExp.matchedLength();
    }
}


void DateTimeTextMatchPlugin::lookForDateRanges( const QRegExp& regExp, const QString& format, int dateCap, int rangeEndCap, int yearCap, bool forceCurrentYear )
{
//    kDebug() << regExp.pattern() << format;
    int pos = 0;
    while ( ( pos = regExp.indexIn( m_text, pos ) ) != -1 ) {
        QDate startDate = m_enLocale.toDate( regExp.cap( dateCap ), format );
        if ( startDate.isValid() ) {
            if ( yearCap > 0 && regExp.cap( yearCap ).length() ) {
                startDate = QDate( regExp.cap( yearCap ).toInt(), startDate.month(), startDate.day() );
            }
            else if ( forceCurrentYear ) {
                startDate = QDate( QDate::currentDate().year(), startDate.month(), startDate.day() );
            }
            kDebug() << regExp.cap( dateCap ) << regExp.cap( rangeEndCap );
            QDate endDate( startDate.year(), startDate.month(), regExp.cap( rangeEndCap ).toInt() );
            kDebug() << "Found date range" << startDate << endDate;

            m_dateRanges.insert( pos, qMakePair( qMakePair( startDate, endDate ), regExp.matchedLength() ) );
        }
        else {
            kDebug() << "Failed to parse date range" << regExp.cap( 0 );
        }

        pos += regExp.matchedLength();
    }
}


void DateTimeTextMatchPlugin::lookForTimes( const QRegExp& regExp, const QString& format )
{
//    kDebug() << regExp.pattern() << format;
    int pos = 0;
    while ( ( pos = regExp.indexIn( m_text, pos ) ) != -1 ) {
        QTime time = QTime::fromString( regExp.cap( 0 ), format );
        if ( time.isValid() ) {
            kDebug() << "Found time" << time << "at" << pos;
            m_times.insert( pos, qMakePair( time, regExp.matchedLength() ) );
        }
        else {
            kDebug() << "Failed to parse time" << regExp.cap( 0 );
        }

        pos += regExp.matchedLength();
    }
}


SCRIBO_EXPORT_TEXTMATCH_PLUGIN( DateTimeTextMatchPlugin, "scribo_datetimetextmatchplugin" )

#include "datetimetextmatchplugin.moc"
