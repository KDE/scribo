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
#include "worker.h"

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

#include <QtCore/QCoreApplication>

Q_DECLARE_METATYPE( Scribo::TextMatch )


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

    m_worker = new Worker( this );

    qRegisterMetaType<Scribo::TextMatch>();
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
    if ( Soprano::Model* model = static_cast<OpenCalais::LookupJob*>( job )->resultModel() ) {
        // cancel previous worker start
        m_worker->disconnect( this );
        m_worker->cancel();

        // restart the worker
        m_worker->setModel( model );
        connect( m_worker, SIGNAL( finished() ), this, SLOT( emitFinished() ) );
        connect( m_worker, SIGNAL( newMatch( Scribo::TextMatch ) ),
                 this, SLOT( addNewMatch( Scribo::TextMatch ) ),
                 Qt::QueuedConnection );
        m_worker->start();
    }
    else {
        emitFinished();
    }
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
