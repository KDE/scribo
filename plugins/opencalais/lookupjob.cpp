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

#include <Soprano/Model>
#include <Soprano/Parser>
#include <Soprano/PluginManager>
#include <Soprano/Statement>
#include <Soprano/Node>
#include <Soprano/StatementIterator>
#include <Soprano/Global>
#include <Soprano/BackendSettings>
#include <Soprano/Backend>
#include <Soprano/StorageModel>

#include <KLocale>
#include <KDebug>
#include <KNotification>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include <QtCore/QFile>
#include <QtCore/QProcess>


class OpenCalais::LookupJob::Private
{
public:
    QNetworkAccessManager* networkAccessManager;
    QNetworkReply* lastReply;

    QString content;
    Soprano::Model* resultModel;
    bool modelRetrieved;

    bool createModel();
};


bool OpenCalais::LookupJob::Private::createModel()
{
    delete resultModel;

    // sesame2 is waaaay faster then redland which is the default in Soprano
    if ( const Soprano::Backend* b = Soprano::PluginManager::instance()->discoverBackendByName( "sesame2" ) )
        resultModel = b->createModel( Soprano::BackendSettings() << Soprano::BackendSetting( Soprano::BackendOptionStorageMemory ) );
    else
        resultModel = Soprano::createModel( Soprano::BackendSettings() << Soprano::BackendSetting( Soprano::BackendOptionStorageMemory ) );

    return resultModel;
}


OpenCalais::LookupJob::LookupJob( QObject* parent )
    : KJob( parent ),
      d( new Private() )
{
    d->resultModel = 0;
    d->networkAccessManager = new QNetworkAccessManager( this );
}


OpenCalais::LookupJob::~LookupJob()
{
    // if no one took the model, we delete it
    if ( !d->modelRetrieved )
        delete d->resultModel;
    delete d;
}


Soprano::Model* OpenCalais::LookupJob::resultModel() const
{
    d->modelRetrieved = true;
    return d->resultModel;
}


void OpenCalais::LookupJob::setContent( const QString& s )
{
    d->content = s;
}


void OpenCalais::LookupJob::start()
{
    delete d->resultModel;
    d->resultModel = 0;
    d->modelRetrieved = false;

    QUrl url(  "http://api.opencalais.com/enlighten/rest/" );

    QString key = Config::licenseKey();
    if ( key.isEmpty() ) {
        kDebug() << "no key";
        setErrorText( i18n( "No OpenCalais API key configured." ) );
        KNotification* n = KNotification::event( KNotification::Warning,
                                                 i18n( "No OpenCalais API key configured." ) );
        n->setActions( QStringList() << i18n( "Configure..." ) );
        connect( n, SIGNAL( action1Activated() ), this, SLOT( slotConfigure() ) );
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
    // FIXME: error handling
    QByteArray data = d->lastReply->readAll();
//    kDebug() << d->lastReply->error() << d->lastReply->rawHeaderList() << data;
    delete d->lastReply;

    if ( !d->createModel() ) {
        setErrorText( i18n( "Failed to create Soprano memory model. Most likely the installation misses Soprano backend plugins" ) );
        emitResult();
        return;
    }

    const Soprano::Parser* parser = Soprano::PluginManager::instance()->discoverParserForSerialization( Soprano::SerializationRdfXml );
    if ( parser ) {
        Soprano::StatementIterator it = parser->parseString( QString::fromUtf8( data ), QUrl(), Soprano::SerializationRdfXml );
        while ( it.next() ) {
            Soprano::Statement s = *it;
            d->resultModel->addStatement( s );
        }
    }

    emitResult();
}


void OpenCalais::LookupJob::slotConfigure()
{
    QProcess::startDetached( "kcmshell4", QStringList() << "kcm_kopencalais" );
}

#include "lookupjob.moc"
