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

#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <KMainWindow>

class KTextEdit;
class KJob;
class QTreeView;
namespace Scribo {
    class TextMatchesModel;
    class TextMatcher;
}
class QModelIndex;

class MainWindow : public KMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private Q_SLOTS:
    void slotStart();
    void slotMatchingFinished();
    void slotSelectionChanged( const QModelIndex& current, const QModelIndex& );
    void slotPreferences();

private:
    KTextEdit* m_textEdit;
    QTreeView* m_resultView;
    Scribo::TextMatchesModel* m_resultModel;
    Scribo::TextMatcher* m_matcher;
};

#endif
