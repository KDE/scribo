/*
   Copyright (C) 2007-2011 Sebastian Trueg <trueg@kde.org>

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

#ifndef _SCRIBO_VERSION_H_
#define _SCRIBO_VERSION_H_

#include "scribo_export.h"

/// @brief Scribo version as string at compile time.
#define SCRIBO_VERSION_STRING "${CMAKE_SCRIBO_VERSION}"

/// @brief The major Scribo version number at compile time
#define SCRIBO_VERSION_MAJOR ${CMAKE_SCRIBO_VERSION_MAJOR}

/// @brief The minor Scribo version number at compile time
#define SCRIBO_VERSION_MINOR ${CMAKE_SCRIBO_VERSION_MINOR}

/// @brief The Scribo release version number at compile time
#define SCRIBO_VERSION_RELEASE ${CMAKE_SCRIBO_VERSION_RELEASE}

/**
 * \brief Create a unique number from the major, minor and release number of a %Scribo version
 *
 * This function can be used for preprocessing. For version information at runtime
 * use the version methods in the Scribo namespace.
 */
#define SCRIBO_MAKE_VERSION( a,b,c ) (((a) << 16) | ((b) << 8) | (c))

/**
 * \brief %Scribo Version as a unique number at compile time
 *
 * This macro calculates the %Scribo version into a number. It is mainly used
 * through SCRIBO_IS_VERSION in preprocessing. For version information at runtime
 * use the version methods in the Scribo namespace.
 */
#define SCRIBO_VERSION \
    SCRIBO_MAKE_VERSION(SCRIBO_VERSION_MAJOR,SCRIBO_VERSION_MINOR,SCRIBO_VERSION_RELEASE)

/**
 * \brief Check if the %Scribo version matches a certain version or is higher
 *
 * This macro is typically used to compile conditionally a part of code:
 * \code
 * #if SCRIBO_IS_VERSION(2,1)
 * // Code for Scribo 2.1
 * #else
 * // Code for Scribo 2.0
 * #endif
 * \endcode
 *
 * For version information at runtime
 * use the version methods in the Scribo namespace.
 */
#define SCRIBO_IS_VERSION(a,b,c) ( SCRIBO_VERSION >= SCRIBO_MAKE_VERSION(a,b,c) )


namespace Scribo {
    /**
     * @brief Returns the major number of Scribo's version, e.g.
     * 1 for %Scribo 1.0.2.
     * @return the major version number at runtime.
     */
    SCRIBO_EXPORT unsigned int versionMajor();

    /**
     * @brief Returns the minor number of Scribo's version, e.g.
     * 0 for %Scribo 1.0.2.
     * @return the minor version number at runtime.
     */
    SCRIBO_EXPORT unsigned int versionMinor();

    /**
     * @brief Returns the release of Scribo's version, e.g.
     * 2 for %Scribo 1.0.2.
     * @return the release number at runtime.
     */
    SCRIBO_EXPORT unsigned int versionRelease();

    /**
     * @brief Returns the %Scribo version as string, e.g. "1.0.2".
     *
     * On contrary to the macro SCRIBO_VERSION_STRING this function returns
     * the version number of Scribo at runtime.
     * @return the %Scribo version. You can keep the string forever
     */
    SCRIBO_EXPORT const char* versionString();
}

#endif
