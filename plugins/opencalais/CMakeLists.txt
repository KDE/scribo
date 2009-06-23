include(SopranoAddOntology)

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

find_file(NFO_TRIG_SOURCE 
  nfo.trig
  PATHS "${KDE4_DATA_INSTALL_DIR}/.." "${KDE4_DATA_INSTALL_DIR}" ENV XDG_DATA_DIRS
  PATH_SUFFIXES "apps/nepomuk/ontologies"
  )
if(NOT NFO_TRIG_SOURCE)
  message(ERROR_FATAL "Could not find nfo.trig NFO source file. Is kdebase-runtime installed?")
endif(NOT NFO_TRIG_SOURCE)

# plugin
set(OCTMP_SCRS
  lookupjob.cpp
  opencalaistextmatchplugin.cpp
  )

soprano_add_ontology(OCTMP_SCRS ${CMAKE_CURRENT_SOURCE_DIR}/opencalais.rdfs "OpenCalais" "Nepomuk::Vocabulary" "rdfxml")
soprano_add_ontology(OCTMP_SCRS ${NFO_TRIG_SOURCE} "NFO" "Nepomuk::Vocabulary" "trig")

kde4_add_kcfg_files(OCTMP_SCRS opencalaisconfig.kcfgc)

kde4_add_plugin(scribo_opencalaistextmatchplugin ${OCTMP_SCRS})
target_link_libraries(scribo_opencalaistextmatchplugin
  ${NEPOMUK_LIBRARIES}
  ${SOPRANO_LIBRARIES}
  ${KDE4_KDECORE_LIBS}
  ${KDE4_KDEUI_LIBS}
  ${QT_QTNETWORK_LIBRARY}
  scribo
  pimo
)
install(TARGETS scribo_opencalaistextmatchplugin DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES scribo_opencalaistextmatchplugin.desktop DESTINATION ${SERVICES_INSTALL_DIR}/scribotextmatchplugins)


# KCM
set(KCM_SRCS
  opencalaiskcm.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/opencalaisconfig.cpp
)

kde4_add_ui_files(KCM_SRCS
  opencalaisconfig.ui
)

kde4_add_plugin(kcm_kopencalais ${KCM_SRCS})
target_link_libraries(kcm_kopencalais
  ${KDE4_KDEUI_LIBS}
)
install(TARGETS kcm_kopencalais DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kcm_kopencalais.desktop DESTINATION ${SERVICES_INSTALL_DIR})
