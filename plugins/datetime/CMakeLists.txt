include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${NEPOMUK_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${libscribo_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

set(datetimetextmatchplugin_SRCS
  datetimetextmatchplugin.cpp
  )

kde4_add_plugin(scribo_datetimetextmatchplugin ${datetimetextmatchplugin_SRCS})
target_link_libraries(scribo_datetimetextmatchplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDECORE_LIBS}
  scribo
  nepomukutils
)
install(TARGETS scribo_datetimetextmatchplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES scribo_datetimetextmatchplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/scribotextmatchplugins)
