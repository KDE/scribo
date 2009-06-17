/*
   Copyright (c) 2009 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _SCRIBO_TEXT_MATCHES_MODEL_P_H_
#define _SCRIBO_TEXT_MATCHES_MODEL_P_H_

#include "textmatch.h"

#include <QtCore/QList>

namespace Scribo {
    class TextMatchesModelPrivate
    {
    public:
        QList<TextMatch> m_matches;

        Scribo::TextOccurrence getOccurrenceForIndex( const QModelIndex& index );
    };
}

#endif

