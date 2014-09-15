INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE(   ${QT_USE_FILE}   )

set(kmdevices_PART_SRCS 
      kmdevices.cpp   
      )
set(kmdevices_RCCS
      icons.qrc
      )

QT4_ADD_RESOURCES(kmdevices_RCC_SRCS ${kmdevices_RCCS})

kde4_add_plugin(kmdevices ${kmdevices_PART_SRCS} ${kmdevices_RCC_SRCS})

target_link_libraries(kmdevices ${SYNNEFO_LIBRARIES} ${KDE4_KDEUI_LIBS} ${QT_LIBRARIES} )

install(TARGETS kmdevices DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kmdevices.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

