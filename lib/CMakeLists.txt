project(libscribo)

include_directories(
  /home/kde4dev/kde/dev/kde/inst/kde4/include
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${SOPRANO_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}/nepomuk
  ${CMAKE_CURRENT_SOURCE_DIR}
)

configure_file(scribo_version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/scribo_version.h)

set(libopencalais_SRC
  textmatch.cpp
  textoccurrence.cpp
  entity.cpp
  statement.cpp
  textmatchesmodel.cpp
  textmatchplugin.cpp
  textmatcher.cpp
  scribo_version.cpp
)

kde4_add_library(scribo SHARED ${libopencalais_SRC})

target_link_libraries(
  scribo
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${KDE4_KIO_LIBS}
)

set_target_properties(scribo PROPERTIES VERSION ${CMAKE_SCRIBO_VERSION} SOVERSION ${CMAKE_SCRIBO_VERSION_MAJOR})

install(TARGETS scribo DESTINATION ${LIB_INSTALL_DIR})
install(FILES
  entity.h
  textoccurrence.h
  textmatch.h
  statement.h
  textmatchesmodel.h
  textmatchplugin.h
  textmatcher.h
  scribo_export.h
  ${CMAKE_CURRENT_BINARY_DIR}/scribo_version.h
  DESTINATION include/scribo)
install(FILES scribo-textmatchplugin.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})

find_package(Doxygen)

if(DOXYGEN_EXECUTABLE)
  configure_file(${libscribo_SOURCE_DIR}/Doxyfile.cmake ${libscribo_BINARY_DIR}/Doxyfile)

  if(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "${QT_DOC_DIR}/html")
  else(EXISTS ${QT_DOC_DIR}/html)
    set(QTDOCS "http://doc.trolltech.com/4.3/")
  endif(EXISTS ${QT_DOC_DIR}/html)

  add_custom_target(
    apidox
    COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND docs/html/installdox -l qt4.tag@${QTDOCS} docs/html/*.html)
endif(DOXYGEN_EXECUTABLE)
