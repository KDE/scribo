/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>
   Based on TokenTree.java (C) 2010 Mikhail Kotelnikov <mikhail.kotelnikov@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) version 3, or any
   later version accepted by the membership of KDE e.V. (or its
   successor approved by the membership of KDE e.V.), which shall
   act as a proxy defined in Section 6 of version 3 of the license.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TOKENTREE_H
#define TOKENTREE_H

#include <QtCore/QVariant>

class TokenNode;

class TokenTree
{
public:
    TokenTree();
    ~TokenTree();

    void add(const QString& token, const QVariant& value);
    TokenNode* findNode(const QString& token, bool create = false);

    TokenNode* root() const { return m_root; }

private:
    TokenNode* m_root;
};

#endif // TOKENTREE_H
