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

#include "opencalaiskcm.h"
#include "opencalaisconfig.h"

#include <KPluginFactory>
#include <KConfigDialogManager>


K_PLUGIN_FACTORY( OpenCalaisKcmFactory, registerPlugin<OpenCalais::Kcm>(); )
K_EXPORT_PLUGIN( OpenCalaisKcmFactory( "kopencalais", "kcm_kopencalais" ) )


OpenCalais::Kcm::Kcm( QWidget* parent, const QVariantList& args )
    : KCModule( OpenCalaisKcmFactory::componentData(), parent, args )
{
    setupUi( this );
    m_configManager = new KConfigDialogManager( this, Config::self() );
    connect( m_configManager, SIGNAL( widgetModified() ),
             this, SLOT( changed() ) );
}


OpenCalais::Kcm::~Kcm()
{
}


void OpenCalais::Kcm::load()
{
    m_configManager->updateWidgets();
}


void OpenCalais::Kcm::save()
{
    m_configManager->updateSettings();
}

#include "opencalaiskcm.moc"
