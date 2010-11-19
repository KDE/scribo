project(scriboservice)

include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${CMAKE_SOURCE_DIR}
  ${SOPRANO_INCLUDE_DIR}
  ${NEPOMUK_INCLUDE_DIR}
  )

set(scriboservice_SRC
  scriboservice.cpp
  scribosession.cpp
)

kde4_add_plugin(nepomukscriboservice ${scriboservice_SRC})

target_link_libraries(nepomukscriboservice
  ${SOPRANO_CLIENT_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${QT_QTCORE_LIBRARY}
  ${QT_QTDBUS_LIBRARY}
  ${KDE4_KDECORE_LIBS}
  ${NEPOMUK_LIBRARIES}
  scribo
  )

install(
  FILES nepomukscriboservice.desktop
  DESTINATION ${SERVICES_INSTALL_DIR})
install(
  TARGETS nepomukscriboservice
  DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES
  org.kde.nepomuk.Scribo.xml
  org.kde.nepomuk.ScriboSession.xml
  DESTINATION ${DBUS_INTERFACES_INSTALL_DIR}
  )
