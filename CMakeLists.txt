project(scribo)

##################  Scribo version  ################################
set(CMAKE_SCRIBO_VERSION_MAJOR 0)
set(CMAKE_SCRIBO_VERSION_MINOR 2)
set(CMAKE_SCRIBO_VERSION_RELEASE 0)
set(CMAKE_SCRIBO_VERSION "${CMAKE_SCRIBO_VERSION_MAJOR}.${CMAKE_SCRIBO_VERSION_MINOR}.${CMAKE_SCRIBO_VERSION_RELEASE}")

# search packages used by KDE
set(KDE_MIN_VERSION "4.5.00")
find_package(KDE4 4.5.00 REQUIRED)
include(KDE4Defaults)
find_package(KDE4Workspace)
include(MacroLibrary)

find_package(SharedDesktopOntologies 0.5)
set(SOPRANO_MIN_VERSION "2.5.0")
find_package(Soprano)
find_package(Nepomuk)
find_package(NepomukExtras)

macro_log_feature(SHAREDDESKTOPONTOLOGIES_FOUND "Shared desktop ontologies" "Desktop ontologies" "http://oscaf.sourceforge.net" TRUE "0.5" "Ontologies necessary for the Nepomuk semantic desktop.")
macro_log_feature(Soprano_FOUND "Soprano" "Soprano RDF storage libraries" "soprano" TRUE "${SOPRANO_MIN_VERSION}" "Required to build anything.")
macro_log_feature(Nepomuk_FOUND "Nepomuk" "Nepomuk semantic desktop libraries" "kdelibs" TRUE "" "Required to build anything.")
macro_log_feature(NepomukExtras_FOUND "Nepomukextras" "Nepomuk extras - a set of convinient classes" "nepomukextras" TRUE "" "Required for text extraction.")

include_directories(${scribo_SOURCE_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(shell)
add_subdirectory(plugins)
add_subdirectory(service)

macro_display_feature_log()
