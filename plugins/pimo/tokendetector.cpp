/*
   This file is part of the Nepomuk KDE project.
   Copyright (C) 2010 Sebastian Trueg <trueg@kde.org>
   Based on TokenDetector.java (C) 2010 Mikhail Kotelnikov <mikhail.kotelnikov@gmail.com>

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

#include "tokendetector.h"
#include "tokentree.h"
#include "tokennode.h"

#include <QtCore/QPair>

#include <KDebug>

namespace {
const QChar END_OF_PHRASE('.');

QChar filterCharacter(const QChar& ch) {
    return ch.toLower();
}

bool isEndOfPhrase(const QChar& ch) {
    return ch == END_OF_PHRASE;
}
}

class TokenDetector::TokenTreePointer
{
public:
    TokenTreePointer(int pos, TokenNode* node);
    ~TokenTreePointer();

    int startPos() const { return m_startPos; }
    TokenNode* tokenNode() const { return m_node; }
    bool hasTokenValue() const { return m_node != 0; }
    bool shift(const QChar& ch);

private:
    int m_startPos;
    TokenNode* m_node;
};

TokenDetector::TokenTreePointer::TokenTreePointer(int pos, TokenNode *node)
    : m_startPos(pos),
      m_node(node)
{
}

TokenDetector::TokenTreePointer::~TokenTreePointer()
{
}

bool TokenDetector::TokenTreePointer::shift(const QChar &ch)
{
    if (m_node) {
        m_node = m_node->findNode(ch, false);
        return m_node;
    }
    else {
        return false;
    }
}


TokenDetector::TokenDetector(TokenTree* tree, QObject* parent)
    : QObject(parent),
      m_tree(tree),
      m_spaceCounter(0),
      m_pos(-1)
{
}

TokenDetector::~TokenDetector()
{
    qDeleteAll(m_pointers);
}

void TokenDetector::update(const QChar &ch)
{
    m_currentChar = m_nextChar;
    m_nextChar = filterCharacter(ch);

    QChar currentChar = m_currentChar;
    if (isEndOfPhrase(currentChar)) {
        m_pointers.clear();
        m_spaceCounter = 0;
    }
    else {
        if (currentChar.isSpace()) {
            m_spaceCounter++;
        }
        else {
            if (m_spaceCounter > 0 || m_pos == 0) {
                m_pointers.insert(new TokenTreePointer(m_pos, m_tree->root()));
            }
            m_spaceCounter = 0;
        }
        if (m_spaceCounter <= 1) {
            QSet<TokenTreePointer*> toRemove;
            foreach (TokenTreePointer* handler, m_pointers) {
                if (handler->shift(currentChar)) {
                    // We can report about a new token only at the end of
                    // the word or a phrase
                    if (isEndOfPhrase(m_nextChar) || m_nextChar.isSpace()) {
                        if (handler->hasTokenValue()) {
                            TokenNode* node = handler->tokenNode();
                            emit tokenFound(handler->startPos(), m_pos, node->value());
                        }
                    }
                }
                else {
                    toRemove.insert(handler);
                }
            }
            foreach (TokenTreePointer* handler, toRemove) {
                m_pointers.remove(handler);
                delete handler;
            }
        }
    }
    m_pos++;
}

void TokenDetector::finish()
{
    kDebug();
    update(END_OF_PHRASE);
}

#include "tokendetector.moc"
