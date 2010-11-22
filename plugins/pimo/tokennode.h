/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>
   Based on TokenNode.java (C) 2010 Mikhail Kotelnikov <mikhail.kotelnikov@gmail.com>

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

#ifndef TOKENNODE_H
#define TOKENNODE_H

#include <QtCore/QChar>
#include <QtCore/QVariant>
#include <QtCore/QList>

class TokenNode
{
public:
    TokenNode(const QChar& ch, TokenNode* parent = 0);
    ~TokenNode();

    QChar ch() const { return m_ch; }
    QVariant value() const { return m_value; }
    void setValue(const QVariant& v) { m_value = v; }

    TokenNode* findNode(const QChar& ch, bool create = false);
    TokenNode* childAt(int pos) const;

private:
    int findChildPos(const QChar& ch) const;
    void addChild(int pos, TokenNode*);

    QChar m_ch;
    QVariant m_value;
    TokenNode* m_parent;
    QList<TokenNode*> m_children;
};

QDebug operator<<(QDebug& dbg, const TokenNode& node);
QDebug operator<<(QDebug& dbg, TokenNode* node);

#endif // TOKENNODE_H
