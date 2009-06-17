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

#include "mainwindow.h"
#include "entity.h"
#include "textmatchesmodel.h"
#include "textmatcher.h"

#include <KTextEdit>
#include <KToolBar>
#include <KIcon>
#include <KLocale>
#include <KDebug>
#include <KDialog>
#include <KPluginSelector>
#include <KStandardAction>
#include <KServiceTypeTrader>
#include <KPluginInfo>
#include <KAction>
#include <KSharedConfig>

#include <Nepomuk/Types/Class>

#include <QtGui/QTreeView>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QHeaderView>
#include <QtGui/QTextCursor>
#include <QtGui/QTextDocument>
#include <QtGui/QItemSelectionModel>

#include <Soprano/Model>
#include <Soprano/Graph>
#include <Soprano/QueryResultIterator>
#include <Soprano/Serializer>
#include <Soprano/StatementIterator>
#include <Soprano/PluginManager>


MainWindow::MainWindow()
    : KMainWindow()
{
    QWidget* w = new QWidget( this );
    m_textEdit = new KTextEdit( w );
    m_resultView = new QTreeView( w );
    m_resultModel = new Scribo::TextMatchesModel( this );
    QSortFilterProxyModel* sortModel = new QSortFilterProxyModel( this );
    sortModel->setSortRole( Scribo::TextMatchesModel::RelevanceRole );
    sortModel->setSourceModel( m_resultModel );
    m_resultView->setModel( sortModel );
    m_resultView->setHeaderHidden( true );
    m_resultView->setSortingEnabled( true );
    m_resultView->header()->setResizeMode( QHeaderView::ResizeToContents );
    QHBoxLayout* layout = new QHBoxLayout( w );
    layout->addWidget( m_textEdit );
    layout->addWidget( m_resultView );

    setCentralWidget( w );

    toolBar()->addAction( KIcon( "nepomuk" ), i18n( "Start" ), this, SLOT( slotStart() ) );
    toolBar()->addAction( KStandardAction::preferences( this, SLOT( slotPreferences() ), this ) );

    m_matcher = new Scribo::TextMatcher( this );
    connect( m_matcher, SIGNAL( newMatch( Scribo::TextMatch ) ),
             m_resultModel, SLOT( addMatch( Scribo::TextMatch ) ) );
    connect( m_matcher, SIGNAL( finished() ),
             this, SLOT( slotMatchingFinished() ) );
    connect( m_resultView->selectionModel(), SIGNAL( currentChanged(const QModelIndex&, const QModelIndex&) ),
             this, SLOT( slotSelectionChanged(const QModelIndex&, const QModelIndex&) ) );
}


MainWindow::~MainWindow()
{
}


void MainWindow::slotStart()
{
    m_resultModel->clear();
    m_matcher->getPossibleMatches( m_textEdit->toPlainText() );
}


void MainWindow::slotPreferences()
{
    KDialog dlg( this );
    dlg.setCaption( i18n( "Configure plugins" ) );
    dlg.setButtons( KDialog::Ok );
    KPluginSelector* ps = new KPluginSelector( &dlg );
    ps->addPlugins( KPluginInfo::fromServices( KServiceTypeTrader::self()->query("Scribo/TextMatchPlugin") ),
                    KPluginSelector::IgnoreConfigFile,
                    i18n( "Scribo Text analysis plugins" ),
                    QString(),
                    KSharedConfig::openConfig( "scriborc" ) );
    ps->load();
    dlg.setMainWidget( ps );
    dlg.exec();
    ps->save();
}


void MainWindow::slotMatchingFinished()
{
    slotSelectionChanged( QModelIndex(), QModelIndex() );
}


namespace {
    QList<QTextEdit::ExtraSelection> createSelectionsForMatch( const QModelIndex& index, QTextDocument* doc )
    {
        QList<QTextEdit::ExtraSelection> selections;

        int start = index.data( Scribo::TextMatchesModel::StartPosRole ).toInt();
        int len = index.data( Scribo::TextMatchesModel::LengthRole ).toInt();

        QTextEdit::ExtraSelection selection;
        selection.cursor = QTextCursor( doc );
        selection.cursor.setPosition( start );
        selection.cursor.movePosition( QTextCursor::Right, QTextCursor::KeepAnchor, len );
        selection.format.setBackground( Qt::green );
        selections << selection;

        return selections;
    }
}

void MainWindow::slotSelectionChanged( const QModelIndex& current, const QModelIndex& )
{
    if ( current.isValid() ) {
        m_textEdit->setExtraSelections( createSelectionsForMatch( current, m_textEdit->document() ) );
    }
    else {
        // mark them all
        QList<QTextEdit::ExtraSelection> selections;
        for ( int i = 0; i < m_resultModel->rowCount(); ++i ) {
            selections << createSelectionsForMatch( m_resultModel->index( i, 0 ), m_textEdit->document() );
        }
        m_textEdit->setExtraSelections( selections );
    }
}

#include "mainwindow.moc"
