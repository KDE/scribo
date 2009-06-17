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

#include "opencalaistextmatchplugin.h"
#include "lookupjob.h"
#include "entity.h"
#include "statement.h"
#include "pimo.h"
#include "opencalais.h"
#include "nfo.h"

#include <Nepomuk/Types/Class>

#include <Soprano/Model>
#include <Soprano/Graph>
#include <Soprano/QueryResultIterator>
#include <Soprano/Serializer>
#include <Soprano/StatementIterator>
#include <Soprano/PluginManager>

#include <KDebug>
#include <KPluginFactory>


OpenCalaisTextMatchPlugin::OpenCalaisTextMatchPlugin( QObject* parent, const QVariantList& )
    : TextMatchPlugin( parent ),
      m_lookupJob( 0 )
{
    // build OpenCalais->PIMO type map
    // FIXME: complete the map
    m_typeMap.insert( Nepomuk::Vocabulary::OpenCalais::City(), Nepomuk::Vocabulary::PIMO::City() );
    m_typeMap.insert( Nepomuk::Vocabulary::OpenCalais::Country(), Nepomuk::Vocabulary::PIMO::Country() );
    m_typeMap.insert( Nepomuk::Vocabulary::OpenCalais::Company(), Nepomuk::Vocabulary::PIMO::Organization() );
    m_typeMap.insert( Nepomuk::Vocabulary::OpenCalais::Organization(), Nepomuk::Vocabulary::PIMO::Organization() );
    m_typeMap.insert( Nepomuk::Vocabulary::OpenCalais::Person(), Nepomuk::Vocabulary::PIMO::Person() );
    m_typeMap.insert( Nepomuk::Vocabulary::OpenCalais::Position(), Nepomuk::Vocabulary::PIMO::PersonRole() );
    m_typeMap.insert( Nepomuk::Vocabulary::OpenCalais::Continent(), Nepomuk::Vocabulary::PIMO::Location() );
    m_typeMap.insert( Nepomuk::Vocabulary::OpenCalais::URL(), Nepomuk::Vocabulary::NFO::Website() );
}


OpenCalaisTextMatchPlugin::~OpenCalaisTextMatchPlugin()
{
}


void OpenCalaisTextMatchPlugin::doGetPossibleMatches( const QString& text )
{
    // cancel previous jobs
    delete m_lookupJob;

    // do the lookup
    m_lookupJob = new OpenCalais::LookupJob( this );
    connect( m_lookupJob, SIGNAL( result( KJob* ) ),
             this, SLOT( slotResult( KJob* ) ) );
    m_lookupJob->setContent( text );
    m_lookupJob->start();
}


void OpenCalaisTextMatchPlugin::slotResult( KJob* job )
{
    if ( const Soprano::Model* model = static_cast<OpenCalais::LookupJob*>( job )->resultModel() ) {
        // select all instances that do not have a subject -> these should be the extracted entities
        Soprano::QueryResultIterator it = model->executeQuery( QString( "select * where { "
                                                                        "?r a ?type . "
                                                                        "OPTIONAL { ?r <http://s.opencalais.com/1/pred/subject> ?sub . } . "
                                                                        "FILTER(!bound(?sub)) . "
                                                                        "}" ),
                                                               Soprano::Query::QueryLanguageSparql );
        while ( it.next() ) {
            Soprano::Node r = it["r"];
            Soprano::Graph graph;
            Soprano::QueryResultIterator it2 = model->executeQuery( QString( "construct { ?entity ?ep ?eo . ?rel ?relp ?relo . } "
                                                                             "where { "
                                                                             "?entity <http://s.opencalais.com/1/pred/subject> %1 . "
                                                                             "?entity ?ep ?eo . "
                                                                             "?rel a <http://s.opencalais.com/1/type/sys/RelevanceInfo> . "
                                                                             "?rel <http://s.opencalais.com/1/pred/subject> %1 . "
                                                                             "?rel ?relp ?relo . "
                                                                             "}" )
                                                                    .arg( r.toN3() ),
                                                                    Soprano::Query::QueryLanguageSparql );
            while ( it2.next() ) {
                graph << it2.currentStatement();
            }

            it2 = model->executeQuery( QString( "select ?name ?rel ?type ?offset ?length where { "
                                                "%1 <http://s.opencalais.com/1/pred/name> ?name . "
                                                "%1 a ?type . "
                                                "?relInfo a <http://s.opencalais.com/1/type/sys/RelevanceInfo> . "
                                                "?relInfo <http://s.opencalais.com/1/pred/subject> %1 . "
                                                "?relInfo <http://s.opencalais.com/1/pred/relevance> ?rel . "
                                                "?info a <http://s.opencalais.com/1/type/sys/InstanceInfo> . "
                                                "?info <http://s.opencalais.com/1/pred/subject> %1 . "
                                                "?info <http://s.opencalais.com/1/pred/offset> ?offset . "
                                                "?info <http://s.opencalais.com/1/pred/length> ?length . "
                                                "}" )
                                       .arg( r.toN3() ),
                                       Soprano::Query::QueryLanguageSparql );
            if ( it2.next() ) {
                Nepomuk::Types::Class type( matchPimoType( it2["type"].uri() ) );
                QString name = it2["name"].toString();

                Scribo::Entity entity( name, type, graph );
                do {
                    double rel = it2["rel"].literal().toDouble();
                    int offset = it2["offset"].literal().toInt();
                    int length = it2["length"].literal().toInt();

                    Scribo::TextOccurrence oc;
                    oc.setStartPos( offset );
                    oc.setLength( length );
                    oc.setRelevance( rel );
                    entity.addOccurrence( oc );

                    kDebug() << type << type.label() << name << rel << offset << length;
                } while ( it2.next() );

                addNewMatch( entity );

                // find relations for the entity
                Soprano::QueryResultIterator it3 = model->executeQuery( QString( "select ?verb ?offset ?length ?exact where { "
                                                                                 "?s a <http://s.opencalais.com/1/type/em/r/GenericRelations> . "
                                                                                 "?s <http://s.opencalais.com/1/pred/relationsubject> %1 . "
                                                                                 "?s <http://s.opencalais.com/1/pred/verb> ?verb . "
                                                                                 "?info a <http://s.opencalais.com/1/type/sys/InstanceInfo> . "
                                                                                 "?info <http://s.opencalais.com/1/pred/subject> ?s . "
                                                                                 "?info <http://s.opencalais.com/1/pred/offset> ?offset . "
                                                                                 "?info <http://s.opencalais.com/1/pred/length> ?length . "
                                                                                 "?info <http://s.opencalais.com/1/pred/exact> ?exact . "
                                                                                 "}" )
                                                                        .arg( r.toN3() ),
                                                                        Soprano::Query::QueryLanguageSparql );
                if ( it3.next() ) {
                    QString verb = it3["verb"].toString();
                    QString exact = it3["exact"].toString();
                    int offset = it3["offset"].literal().toInt();
                    int length = it3["length"].literal().toInt();

                    // FIXME: get the graph
                    Scribo::Statement s( verb, entity, exact, Soprano::Graph() );

                    Scribo::TextOccurrence oc;
                    oc.setStartPos( offset );
                    oc.setLength( length );
                    s.addOccurrence( oc );

                    addNewMatch( s );
                }
            }
        }
    }
    else {
        kDebug() << "no result";
    }

    emitFinished();
}


QUrl OpenCalaisTextMatchPlugin::matchPimoType( const QUrl& openCalaisType )
{
    QHash<QUrl, QUrl>::const_iterator it = m_typeMap.constFind( openCalaisType );
    if ( it == m_typeMap.constEnd() )
        return Nepomuk::Vocabulary::PIMO::Thing();
    else
        return *it;
}

SCRIBO_EXPORT_TEXTMATCH_PLUGIN( OpenCalaisTextMatchPlugin, "scribo_opencalaistextmatchplugin" )

#include "opencalaistextmatchplugin.moc"
