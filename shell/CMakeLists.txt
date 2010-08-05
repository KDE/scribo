project(scriboshell)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${NEPOMUK_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}/nepomuk
  ${CMAKE_SOURCE_DIR}
  ${libscribo_SOURCE_DIR}
)

set(scriboshell_SRCS
  mainwindow.cpp
  main.cpp
)

kde4_add_executable(scriboshell ${scriboshell_SRCS})

target_link_libraries(scriboshell
  scribo
  ${SOPRANO_LIBRARIES}
  ${QT_QTGUI_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KCMUTILS_LIBS}
)

install(TARGETS scriboshell DESTINATION ${BIN_INSTALL_DIR})
#install(FILES scriboshell.desktop DESTINATION ${SERVICES_INSTALL_DIR}/ServiceMenus)
#install(FILES scriboshell-app.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
