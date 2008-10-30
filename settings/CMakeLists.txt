
 
find_package(KDE4 REQUIRED)
include_directories(${KDE4_INCLUDES})
 
set(kmsettings_PART_SRCS 
      kmsettings.cpp      
      )

kde4_add_ui_files(kmsettings_PART_SRCS kmsettings.ui)

kde4_add_plugin(kmsettings ${kmsettings_PART_SRCS})

target_link_libraries(kmsettings ${KDE4_KFILE_LIBS} ${KDE4_KNEWSTUFF2_LIBS} ${KDE4_KDEUI_LIBS} ${OYRANOS_LIBRARIES} ${ELEKTRA_LIBRARIES})

install(TARGETS kmsettings DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kmsettings.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )



