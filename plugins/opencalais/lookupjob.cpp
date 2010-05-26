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

#include "lookupjob.h"
#include "opencalaisconfig.h"

#include <Soprano/Graph>
#include <Soprano/Parser>
#include <Soprano/PluginManager>
#include <Soprano/Statement>
#include <Soprano/Node>
#include <Soprano/StatementIterator>

#include <KLocale>
#include <KDebug>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QFile>


class OpenCalais::LookupJob::Private
{
public:
    QNetworkAccessManager* networkAccessManager;
    QNetworkReply* lastReply;

    QString content;
    Soprano::Graph resultGraph;
};


OpenCalais::LookupJob::LookupJob( QObject* parent )
    : KJob( parent ),
      d( new Private() )
{
    d->networkAccessManager = new QNetworkAccessManager( this );
}


OpenCalais::LookupJob::~LookupJob()
{
    delete d;
}


Soprano::Graph OpenCalais::LookupJob::resultGraph() const
{
    return d->resultGraph;
}


void OpenCalais::LookupJob::setContent( const QString& s )
{
    d->content = s;
}


void OpenCalais::LookupJob::start()
{
    kDebug();
    QUrl url( "http://api.opencalais.com/enlighten/rest/" );

    QString key = Config::licenseKey();
    if ( key.isEmpty() ) {
        kDebug() << "no key";
        setErrorText( i18n( "No OpenCalais API key configured." ) );
        emitResult();
        return;
    }

    QString paramsXML = "<c:params xmlns:c=\"http://s.opencalais.com/1/pred/\" "
                        "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">"
                        "<c:processingDirectives "
                        "c:contentType=\"TEXT/RAW\" "
                        "c:enableMetadataType=\"GenericRelations\" "
                        "c:outputFormat=\"xml/rdf\" "
                        "c:calculateRelevanceScore=\"true\" "
                        "c:omitOutputtingOriginalText=\"true\" "
                        ">"
                        "</c:processingDirectives>"
                        "<c:userDirectives "
                        "c:allowDistribution=\"false\" "
                        "c:allowSearch=\"false\" "
                        "c:externalID=\"17cabs901\" "
                        "c:submitter=\"ABC\""
                        ">"
                        "</c:userDirectives>"
                        "<c:externalMetadata>"
                        "</c:externalMetadata>"
                        "</c:params>";
//    kDebug() << paramsXML;

    QUrl dummy;
    dummy.addQueryItem( "licenseID", key );
    dummy.addQueryItem( "content", d->content );
    dummy.addQueryItem( "paramsXML", paramsXML );
//    kDebug() << dummy.encodedQuery();

    QByteArray data = dummy.encodedQuery();

    QNetworkRequest request( url );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QLatin1String( "application/x-www-form-urlencoded" ) );
    request.setHeader( QNetworkRequest::ContentLengthHeader, data.length() );
    d->lastReply = d->networkAccessManager->post( request, data );
    connect( d->lastReply, SIGNAL( finished() ), this, SLOT( slotTransferResult() ) );
}


void OpenCalais::LookupJob::slotTransferResult()
{
    d->resultGraph.removeAllStatements();

    // FIXME: error handling
    QByteArray data = d->lastReply->readAll();
//    kDebug() << d->lastReply->error() << d->lastReply->rawHeaderList() << data;
    d->lastReply->deleteLater();

    const Soprano::Parser* parser = Soprano::PluginManager::instance()->discoverParserForSerialization( Soprano::SerializationRdfXml );
    if ( parser ) {
        Soprano::StatementIterator it = parser->parseString( QString::fromUtf8( data ), QUrl(), Soprano::SerializationRdfXml );
        while ( it.next() ) {
            d->resultGraph.addStatement( *it );
        }
    }

    emitResult();
}

#include "lookupjob.moc"
