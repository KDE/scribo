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

#include "textmatcher.h"
#include "textmatch.h"
#include "textmatchplugin.h"

#include <KServiceTypeTrader>
#include <KService>
#include <KDebug>
#include <KPluginInfo>
#include <KSharedConfig>

#include <QtCore/QEventLoop>


class Scribo::TextMatcher::Private
{
public:
    void _k_newMatch( const TextMatch& match );
    void _k_finished( TextMatchPlugin* plugin );

    QList<TextMatchPlugin*> m_plugins;

    QEventLoop* m_loop;
    QList<TextMatch> m_matchesCache;

    TextMatcher* q;
};


void Scribo::TextMatcher::Private::_k_newMatch( const TextMatch& match )
{
    m_matchesCache.append( match );
    emit q->newMatch( match );
}


void Scribo::TextMatcher::Private::_k_finished( TextMatchPlugin* plugin )
{
    m_plugins.removeAll( plugin );
    if ( m_plugins.isEmpty() ) {
        emit q->finished();
        if ( m_loop )
            m_loop->exit();
    }
}



Scribo::TextMatcher::TextMatcher( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    d->m_loop = 0;
    d->q = this;
}


Scribo::TextMatcher::~TextMatcher()
{
    delete d;
}


void Scribo::TextMatcher::getPossibleMatches( const QString& text )
{
    // the ugly way
    qDeleteAll( d->m_plugins );
    d->m_plugins.clear();

    // fetch all existing plugins
    KService::List services = KServiceTypeTrader::self()->query("Scribo/TextMatchPlugin");

    foreach( KPluginInfo info, KPluginInfo::fromServices( services, KSharedConfig::openConfig( "scriborc" )->group( "Plugins" ) ) ) {
        info.load();
        if ( info.isPluginEnabled() ) {
            TextMatchPlugin* plugin = info.service()->createInstance<TextMatchPlugin>( this, QVariantList(), 0 );
            if ( plugin )
                d->m_plugins.append( plugin );
            else
                kDebug() << "Failed to create plugin from service" << info.name();
        }
        else
            kDebug() << "Plugin" << info.name() << "disabled";
    }

    foreach( TextMatchPlugin* plugin, d->m_plugins ) {
        connect( plugin, SIGNAL( newMatch( Scribo::TextMatch ) ),
                 this, SLOT( _k_newMatch( Scribo::TextMatch ) ) );
        connect( plugin, SIGNAL( finished( Scribo::TextMatchPlugin* ) ),
                 this, SLOT( _k_finished( Scribo::TextMatchPlugin* ) ) );
        plugin->getPossibleMatches( text );
    }
}


QList<Scribo::TextMatch> Scribo::TextMatcher::getAllPossibleMatches( const QString& text )
{
    QEventLoop loop;
    d->m_loop = &loop;

    getPossibleMatches( text );

    loop.exec();
    d->m_loop = 0;
    return matches();
}


QList<Scribo::TextMatch> Scribo::TextMatcher::matches() const
{
    return d->m_matchesCache;
}

#include "textmatcher.moc"
