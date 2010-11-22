/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (c) 2009-2010 Sebastian Trueg <trueg@kde.org>
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
#include "tokentree.h"
#include "tokendetector.h"

#include <Soprano/Model>
#include <Soprano/QueryResultIterator>
#include <Soprano/Node>
#include <Soprano/Vocabulary/NAO>
#include <Soprano/Graph>

#include <Nepomuk/ResourceManager>
#include <Nepomuk/Resource>
#include <Nepomuk/Types/Class>
#include <Nepomuk/Query/Query>
#include <Nepomuk/Query/ComparisonTerm>
#include <Nepomuk/Query/ResourceTypeTerm>
#include <Nepomuk/Query/LiteralTerm>
#include <Nepomuk/Query/AndTerm>
#include <Nepomuk/Query/OrTerm>

#include <KPluginFactory>
#include <KDebug>

#include <QtCore/QThread>

class PimoTextMatchPlugin::WorkThread : public QThread
{
public:
    WorkThread( QObject* parent );
    ~WorkThread();

    void start(const QString& text);
    void run();

private:
    void buildTokenTree();
    QString m_text;
    TokenTree* m_tokenTree;
    bool m_canceled;
};

PimoTextMatchPlugin::WorkThread::WorkThread( QObject* parent )
    : QThread(parent),
      m_tokenTree(0),
      m_canceled(false)
{
}

PimoTextMatchPlugin::WorkThread::~WorkThread()
{
    m_canceled = true;
    wait();
    delete m_tokenTree;
}

void PimoTextMatchPlugin::WorkThread::start(const QString &text)
{
    m_text = text;
    QThread::start();
}

void PimoTextMatchPlugin::WorkThread::run()
{
    buildTokenTree();
    TokenDetector* detector = new TokenDetector(m_tokenTree);
    connect(detector, SIGNAL(tokenFound(int,int,QVariant)),
            parent(), SLOT(slotTokenFound(int,int,QVariant)),
            Qt::QueuedConnection);
    foreach(const QChar& ch, m_text) {
        detector->update(ch);
        if(m_canceled)
            break;
    }
    if(!m_canceled)
        detector->finish();
    delete detector;
}

void PimoTextMatchPlugin::WorkThread::buildTokenTree()
{
    if(!m_tokenTree) {
        m_tokenTree = new TokenTree();
        // populatre tree with all resources that have a nao:prefLabel - urgh, this could be big!
        Nepomuk::Query::Query query(
                    Nepomuk::Query::ComparisonTerm( Soprano::Vocabulary::NAO::prefLabel(), Nepomuk::Query::Term())
                    );
        query.addRequestProperty(Nepomuk::Query::Query::RequestProperty(Soprano::Vocabulary::NAO::prefLabel(), false));

        kDebug() << query.toSparqlQuery();

        Soprano::QueryResultIterator it
                = Nepomuk::ResourceManager::instance()->mainModel()->executeQuery( query.toSparqlQuery(),
                                                                                  Soprano::Query::QueryLanguageSparql );
        while ( !m_canceled && it.next() ) {
            const QUrl res( it[0].uri() );
            const QString label( it[1].toString() );
            m_tokenTree->add(label, QVariant::fromValue(res));
        }
    }
}



PimoTextMatchPlugin::PimoTextMatchPlugin( QObject* parent, const QVariantList& )
    : TextMatchPlugin( parent ),
      m_workThread(0)
{
}


PimoTextMatchPlugin::~PimoTextMatchPlugin()
{
}


void PimoTextMatchPlugin::doGetPossibleMatches( const QString& text )
{
    if(!m_workThread) {
        m_workThread = new WorkThread(this);
        connect(m_workThread, SIGNAL(finished()),
                this, SLOT(emitFinished()));
    }
    m_workThread->start(text);
}


void PimoTextMatchPlugin::slotTokenFound(int pos, int endPos, const QVariant& value)
{
    kDebug() << pos << endPos << value;

    Nepomuk::Resource res( value.toUrl() );
    Scribo::Entity entity( res.genericLabel(), res.resourceType(), Soprano::Graph(), res );

    Scribo::TextOccurrence oc;
    oc.setStartPos( pos );
    oc.setLength( endPos-pos+1 );
    oc.setRelevance( 1.0 ); // TokenTree only produces perfect matches for now!
    entity.addOccurrence( oc );

    addNewMatch( entity );
}

SCRIBO_EXPORT_TEXTMATCH_PLUGIN( PimoTextMatchPlugin, "scribo_pimotextmatchplugin" )

#include "pimotextmatchplugin.moc"
