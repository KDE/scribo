include_directories(
  ${QT_INCLUDES}
  ${KDE4_INCLUDES}
  ${NEPOMUK_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}
  ${libscribo_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/qtsoap
  ${CMAKE_CURRENT_BINARY_DIR}
  )

# plugin
set(webc_SCRS
  webcontenttextmatchplugin.cpp
#  qtsoap/qtsoap.cpp
  )

#kde4_add_kcfg_files(webc_SCRS webcontentconfig.kcfgc)

kde4_add_plugin(scribo_webcontenttextmatchplugin ${webc_SCRS})
target_link_libraries(scribo_webcontenttextmatchplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${QT_QTNETWORK_LIBRARY}
  ${QT_QTXML_LIBRARY}
  scribo
)
install(TARGETS scribo_webcontenttextmatchplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES scribo_webcontenttextmatchplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/scribotextmatchplugins)
