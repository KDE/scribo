project(scribo)

##################  Scribo version  ################################
set(CMAKE_SCRIBO_VERSION_MAJOR 0)
set(CMAKE_SCRIBO_VERSION_MINOR 2)
set(CMAKE_SCRIBO_VERSION_RELEASE 0)
set(CMAKE_SCRIBO_VERSION_STRING "${CMAKE_SCRIBO_VERSION_MAJOR}.${CMAKE_SCRIBO_VERSION_MINOR}.${CMAKE_SCRIBO_VERSION_RELEASE}")

# search packages used by KDE
set(KDE_MIN_VERSION "4.5.00")
find_package(KDE4 4.5.00 REQUIRED)
include(KDE4Defaults)
find_package(KDE4Workspace)
include(MacroLibrary)

find_package(SharedDesktopOntologies 0.5 REQUIRED)
set(SOPRANO_MIN_VERSION "2.5.0")
find_package(Soprano REQUIRED)
find_package(Nepomuk REQUIRED)

include_directories(${scribo_SOURCE_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(shell)
add_subdirectory(plugins)
add_subdirectory(service)
