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

#include "tokennode.h"

#include <QtCore/QChar>

#include <KDebug>

TokenNode::TokenNode(const QChar& c, TokenNode* parent)
    : m_ch(c),
      m_parent(parent)
{
}

TokenNode::~TokenNode()
{
    qDeleteAll(m_children);
}

TokenNode * TokenNode::findNode(const QChar &ch, bool create)
{
    int pos = findChildPos(ch);
    TokenNode* child = 0;
    if(pos < 0) {
        if(create){
            child = new TokenNode(ch, this);
            addChild(-(pos+1), child);
        }
    }
    else {
        child = childAt(pos);
    }
    return child;
}

int TokenNode::findChildPos(const QChar &c) const
{
    int low = 0;
    int high = m_children.count() - 1;
    while (low <= high) {
        const int mid = low + (high-low+1)/2;
        TokenNode* midVal = m_children[mid];
        if (midVal->ch() < c) {
            low = mid + 1;
        }
        else if (midVal->ch() > c) {
            high = mid - 1;
        }
        else {
            return mid; // key found
        }
    }
    return -(low + 1); // key not found
}

TokenNode* TokenNode::childAt(int pos) const
{
    return pos >= 0 && pos < m_children.count() ? m_children[pos] : 0;
}

void TokenNode::addChild(int pos, TokenNode *child)
{
    if (child) {
        m_children.insert(pos, child);
    }
}

QDebug operator<<(QDebug& dbg, const TokenNode& node)
{
    return dbg << QString::fromLatin1("TokenNode(%1)").arg(node.ch());
}

QDebug operator<<(QDebug& dbg, TokenNode* node)
{
    if(node)
        return operator<<(dbg, *node);
    else
        return dbg << "(null)";
}
