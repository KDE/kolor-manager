
 
find_package(KDE4 REQUIRED)
include_directories(${KDE4_INCLUDES} ${kolor_manager_SOURCE_DIR})

INCLUDE( "../settings")
 
set(kminfo_PART_SRCS 
      kminfo.cpp    
      )
set(kmdevices_RCCS
      icons.qrc
      )

QT4_ADD_RESOURCES(kminfo_RCC_SRCS ${kmdevices_RCCS})

kde4_add_ui_files(kminfo_PART_SRCS kminfo.ui)

kde4_add_plugin(kminfo ${kminfo_PART_SRCS} ${kminfo_RCC_SRCS})

target_link_libraries(kminfo ${KDE4_KFILE_LIBS} ${KDE4_KNEWSTUFF2_LIBS} ${KDE4_KDEUI_LIBS} ${OYRANOS_LIBRARIES} ${ELEKTRA_LIBRARIES} ${QT_LIBRARIES})

install(TARGETS kminfo DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kminfo.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )



