
find_package(KDE4 REQUIRED)
include_directories(${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

INCLUDE(   ${QT_USE_FILE}   )
 
set(kmdevices_PART_SRCS 
      kmdevices.cpp   
      )
set(kmdevices_RCCS
      icons.qrc
      )

QT4_ADD_RESOURCES(kmdevices_RCC_SRCS ${kmdevices_RCCS})

kde4_add_ui_files(kmdevices_PART_SRCS kmdevices.ui)

kde4_add_plugin(kmdevices ${kmdevices_PART_SRCS} ${kmdevices_RCC_SRCS})

target_link_libraries(kmdevices ${KDE4_KFILE_LIBS} ${KDE4_KNEWSTUFF2_LIBS} ${KDE4_KDEUI_LIBS} ${OYRANOS_LIBRARIES} ${SANE_LIBRARY} ${CUPS_LIBRARIES} ${QT_LIBRARIES} )

install(TARGETS kmdevices DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kmdevices.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )



