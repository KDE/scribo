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

#include "textmatchesmodel.h"
#include "textmatchesmodel_p.h"
#include "entity.h"

#include <KLocale>
#include <KDebug>

#include <Nepomuk/Types/Class>


namespace {
    int getIndexId( const QModelIndex& index ) {
        return static_cast<int>( index.internalId() );
    }

    int getTopLevelIndexRow( const QModelIndex& index ) {
        return qAbs( getIndexId( index ) )-1;
    }

    quint32 getInternalIndexId( int row, bool topLevel ) {
        ++row;
        if ( !topLevel )
            row *= -1;
        return static_cast<quint32>( row );
    }
}


Scribo::TextOccurrence Scribo::TextMatchesModelPrivate::getOccurrenceForIndex( const QModelIndex& index )
{
    TextMatch match = m_matches[getTopLevelIndexRow( index )];
    return match.occurrences()[index.row()/3];
}


Scribo::TextMatchesModel::TextMatchesModel( QObject* parent )
    : QAbstractItemModel( parent ),
      d( new TextMatchesModelPrivate() )
{
}


Scribo::TextMatchesModel::~TextMatchesModel()
{
    delete d;
}


int Scribo::TextMatchesModel::columnCount( const QModelIndex& ) const
{
    return 1;
}


QVariant Scribo::TextMatchesModel::data( const QModelIndex& index, int role ) const
{
    if ( !index.isValid() ) {
        return QVariant();
    }

    TextMatch match = matchForIndex( index );

    if ( role == Qt::DisplayRole ) {
        // properties
        if ( getIndexId( index ) < 0 ) {
            TextOccurrence oc = d->getOccurrenceForIndex( index );
            switch( index.row()%3 ) {
            case 0:
                return i18n( "Relevance: %1", oc.relevance() );
            case 1:
                return i18n( "Start Position: %1", oc.startPosition() );
            case 2:
                return i18n( "Length: %1", oc.length() );
            }
        }

        // label
        else {
            return QString( "%1 (%2)" ).arg( match.label() ).arg( match.toEntity().type().label() );
        }
    }
    else if ( role == RelevanceRole ) {
        if ( getIndexId( index ) < 0 )
            return d->getOccurrenceForIndex( index ).relevance();
        else
            return match.occurrences().first().relevance();
    }
    else if ( role == StartPosRole ) {
        if ( getIndexId( index ) < 0 )
            return d->getOccurrenceForIndex( index ).startPosition();
        else
            return match.occurrences().first().startPosition();
    }
    else if ( role == LengthRole ) {
        if ( getIndexId( index ) < 0 )
            return d->getOccurrenceForIndex( index ).length();
        else
            return match.occurrences().first().length();
    }

    return QVariant();
}


QModelIndex Scribo::TextMatchesModel::parent( const QModelIndex& index ) const
{
    if ( index.isValid() &&
        getIndexId( index ) < 0 ) {
        return createIndex( getTopLevelIndexRow( index ), index.column(), getInternalIndexId( getTopLevelIndexRow( index ), true ) );
    }
    else {
        return QModelIndex();
    }
}


int Scribo::TextMatchesModel::rowCount( const QModelIndex& parent ) const
{
    if ( parent.isValid() ) {
        if ( getIndexId( parent ) > 0 ) {
            TextMatch match = matchForIndex( parent );
            if ( match.isEntity() ) {
                // start pos, length, relevance
                return match.occurrences().count()*3;
            }
            else {
                // FIXME
                return 0;
            }
        }
        else {
            return 0;
        }
    }
    else {
        return d->m_matches.count();
    }
}


QModelIndex Scribo::TextMatchesModel::index( int row, int column, const QModelIndex& parent ) const
{
    // evil hacking with internalPointer since internalId is unsigned
    if ( parent.isValid() ) {
        if ( getIndexId( parent ) > 0 )
            return createIndex( row, column, getInternalIndexId( getTopLevelIndexRow( parent ), false ) );
        else
            return QModelIndex();
    }
    else {
        return createIndex( row, column, getInternalIndexId( row, true ) );
    }
}


Scribo::TextMatch Scribo::TextMatchesModel::matchForIndex( const QModelIndex& index ) const
{
    if ( index.isValid() )
        return d->m_matches[getTopLevelIndexRow( index )];
    else
        return TextMatch();
}


void Scribo::TextMatchesModel::addMatch( const TextMatch& match )
{
    beginInsertRows( QModelIndex(), d->m_matches.count(), d->m_matches.count() );
    d->m_matches << match;
    endInsertRows();
}


void Scribo::TextMatchesModel::clear()
{
    d->m_matches.clear();
    reset();
}

#include "textmatchesmodel.moc"
