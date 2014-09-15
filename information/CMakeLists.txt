INCLUDE( "../settings")
 
set(kminfo_PART_SRCS 
      kminfo.cpp    
      )
set(kmdevices_RCCS
      icons.qrc
      )

QT4_ADD_RESOURCES(kminfo_RCC_SRCS ${kmdevices_RCCS})

kde4_add_plugin(kminfo ${kminfo_PART_SRCS} ${kminfo_RCC_SRCS})

target_link_libraries(kminfo ${SYNNEFO_LIBRARIES} ${KDE4_KDEUI_LIBS})

install(TARGETS kminfo DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kminfo.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

