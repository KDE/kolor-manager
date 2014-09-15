include_directories(${CMAKE_CURRENT_BINARY_DIR})
 
set(kmsettings_PART_SRCS 
      kmsettings.cpp
      )

kde4_add_plugin(kmsettings ${kmsettings_PART_SRCS})

target_link_libraries(kmsettings ${SYNNEFO_LIBRARIES} ${KDE4_KDEUI_LIBS})

install(TARGETS kmsettings DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kmsettings.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

