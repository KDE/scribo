include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${NEPOMUK_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${libscribo_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${pimolib_BINARY_DIR}
  ${pimolib_SOURCE_DIR}
  )

set(pimotextmatchplugin_SRCS
  pimotextmatchplugin.cpp
  )

kde4_add_plugin(scribo_pimotextmatchplugin ${pimotextmatchplugin_SRCS})
target_link_libraries(scribo_pimotextmatchplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${SOPRANO_INDEX_LIBRARIES}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${QT_QTNETWORK_LIBRARY}
  scribo
  pimo
)
install(TARGETS scribo_pimotextmatchplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES scribo_pimotextmatchplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/scribotextmatchplugins)
