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

#ifndef _SCRIBO_TEXT_MATCH_PLUGIN_H_
#define _SCRIBO_TEXT_MATCH_PLUGIN_H_

#include <QtCore/QObject>

#include "scribo_export.h"

namespace Scribo {

    class TextMatchPluginPrivate;
    class TextMatch;

    class SCRIBO_EXPORT TextMatchPlugin : public QObject
    {
        Q_OBJECT

    public:
        TextMatchPlugin( QObject* parent = 0 );
        virtual ~TextMatchPlugin();

    public Q_SLOTS:
        void getPossibleMatches( const QString& text );

    Q_SIGNALS:
/** \cond hide_private_signals_from_moc */
#ifndef Q_MOC_RUN
    private: // don't tell moc, but this signal is in fact private
#endif
/** \endcond */
        void finished( Scribo::TextMatchPlugin* );
        void newMatch( const Scribo::TextMatch& );

    protected Q_SLOTS:
        /**
         * Emits the finished signal.
         */
        void emitFinished();

        /**
         * Subclasses need to call this for each new TextMatch.
         *
         * This will emit the newMatch signal.
         */
        void addNewMatch( const Scribo::TextMatch& );

        virtual void doGetPossibleMatches( const QString& ) = 0;

    private:
        TextMatchPluginPrivate* d;
    };
}

/**
 * Export a %Scribo text match plugin.
 *
 * \param classname The name of the Scribo::TextMatchPlugin subclass to export.
 * \param libname The name of the library which should export the plugin.
 */
#define SCRIBO_EXPORT_TEXTMATCH_PLUGIN( classname, libname )  \
K_PLUGIN_FACTORY(factory, registerPlugin<classname>();)     \
K_EXPORT_PLUGIN(factory(#libname))

#endif
