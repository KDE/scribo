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

#include "pimotextmatchplugin.h"
#include "pimo.h"
#include "textoccurrence.h"
#include "entity.h"

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Node>
#include <Soprano/Index/IndexFilterModel>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Graph>

#include <Nepomuk/ResourceManager>
#include <Nepomuk/Resource>
#include <Nepomuk/Types/Class>

#include <KPluginFactory>
#include <KDebug>


namespace {
    const int s_minLength = 3;
}

PimoTextMatchPlugin::PimoTextMatchPlugin( QObject* parent, const QVariantList& )
    : TextMatchPlugin( parent )
{
    m_stopWords << QLatin1String( "and" )
                << QLatin1String( "or" )
                << QLatin1String( "the" )
                << QLatin1String( "that" )
                << QLatin1String( "this" )
                << QLatin1String( "there" )
                << QLatin1String( "for" )
                << QLatin1String( "with" );
}


PimoTextMatchPlugin::~PimoTextMatchPlugin()
{
}


void PimoTextMatchPlugin::doGetPossibleMatches( const QString& text )
{
    m_text = text;
    m_pos = 0;
    scanText();
}


void PimoTextMatchPlugin::scanText()
{
    // extract next word
    int pos = m_text.indexOf( QRegExp( "\\W" ), m_pos );
    if ( pos != -1 ) {
        QString word = m_text.mid( m_pos, pos-m_pos ).simplified();
        queryWord( word );

        // scan for next word without blocking
        m_pos = pos+1;
        QMetaObject::invokeMethod( this, "scanText", Qt::QueuedConnection );
    }
    else {
        if ( m_text.length() > m_pos ) {
            QString word = m_text.mid( m_pos );
            queryWord( word );
        }
        emitFinished();
    }
}


bool PimoTextMatchPlugin::queryWord( const QString& word )
{
    if ( word.length() < s_minLength ) {
//        kDebug() << word << "too short";
        return false;
    }
    else if ( m_stopWords.contains( word.toLower() ) ) {
        return false;
    }

//    kDebug() << "checking word" << word;

    QString query
        = Soprano::Index::IndexFilterModel::encodeUriForLuceneQuery( Soprano::Vocabulary::NAO::prefLabel() )
        + ':'
        + Soprano::Index::IndexFilterModel::encodeStringForLuceneQuery( word );
    Soprano::QueryResultIterator it
        = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery( query,
                                                                           Soprano::Query::QueryLanguageUser,
                                                                           "lucene" );
    while ( it.next() ) {
        Nepomuk::Resource res( it[0].uri() );
        if ( res.hasType( Nepomuk::Vocabulary::PIMO::Thing() ) ) {
            Scribo::Entity entity( res.genericLabel(), res.resourceType(), Soprano::Graph(), res );

            Scribo::TextOccurrence oc;
            oc.setStartPos( m_pos );
            oc.setLength( word.length() );
            oc.setRelevance( it[1].literal().toDouble() );
            entity.addOccurrence( oc );

            addNewMatch( entity );
        }
    }

    return true;
}


SCRIBO_EXPORT_TEXTMATCH_PLUGIN( PimoTextMatchPlugin, "scribo_pimotextmatchplugin" )

#include "pimotextmatchplugin.moc"
