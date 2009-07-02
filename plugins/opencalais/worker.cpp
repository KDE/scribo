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

#include "worker.h"
#include "opencalaistextmatchplugin.h"
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



Worker::Worker( OpenCalaisTextMatchPlugin* plugin )
    : QThread( plugin ),
      m_model( 0 ),
      m_plugin( plugin ),
      m_canceled( false )
{
}


Worker::~Worker()
{
}


void Worker::run()
{
    m_canceled = false;

    // select all instances that do not have a subject -> these should be the extracted entities
    Soprano::QueryResultIterator it = m_model->executeQuery( QString( "select * where { "
                                                                      "?r a ?type . "
                                                                      "OPTIONAL { ?r <http://s.opencalais.com/1/pred/subject> ?sub . } . "
                                                                      "FILTER(!bound(?sub)) . "
                                                                      "}" ),
                                                             Soprano::Query::QueryLanguageSparql );
    while ( !m_canceled && it.next() ) {
        Soprano::Node r = it["r"];
        Soprano::Graph graph;
        Soprano::QueryResultIterator it2 = m_model->executeQuery( QString( "construct { ?entity ?ep ?eo . ?rel ?relp ?relo . } "
                                                                           "where { "
                                                                           "?entity <http://s.opencalais.com/1/pred/subject> %1 . "
                                                                           "?entity ?ep ?eo . "
                                                                           "?rel a <http://s.opencalais.com/1/type/sys/RelevanceInfo> . "
                                                                           "?rel <http://s.opencalais.com/1/pred/subject> %1 . "
                                                                           "?rel ?relp ?relo . "
                                                                           "}" )
                                                                  .arg( r.toN3() ),
                                                                  Soprano::Query::QueryLanguageSparql );
        while ( !m_canceled && it2.next() ) {
            graph << it2.currentStatement();
        }

        if ( m_canceled )
            break;

        it2 = m_model->executeQuery( QString( "select ?name ?rel ?type ?offset ?length where { "
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
        if ( !m_canceled && it2.next() ) {
            Nepomuk::Types::Class type( m_plugin->matchPimoType( it2["type"].uri() ) );
            QString name = it2["name"].toString();

            // FIXME: actually the opencalais resource should be used as the pimo:hasOtherRepresentation of the pimo thing
            //        but that would mean that Entity needs more information or the graph needs to be really used
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
            } while ( !m_canceled && it2.next() );

            if ( m_canceled )
                break;

            addNewMatch( entity );

            // find relations for the entity
            Soprano::QueryResultIterator it3 = m_model->executeQuery( QString( "select ?verb ?offset ?length ?exact where { "
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
            if ( !m_canceled && it3.next() ) {
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

    delete m_model;
}


void Worker::cancel()
{
    m_canceled = true;
    wait();
}


void Worker::addNewMatch( const Scribo::TextMatch& match )
{
    emit newMatch( match );
}

#include "worker.moc"
