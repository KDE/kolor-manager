set(kminfo_PART_SRCS 
      kminfo.cpp    
      )
set(kmdevices_RCCS
      icons.qrc
      )

ADD_LIBRARY( kminfo MODULE ${kminfo_PART_SRCS} ${kminfo_RCC_SRCS} )
SET_TARGET_PROPERTIES( kminfo PROPERTIES PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}" )
TARGET_LINK_LIBRARIES( kminfo
                       KF5::CoreAddons
                       KF5::ConfigWidgets
                       KF5::I18n
                       ${SYNNEFO_LIBRARIES}
                       Qt5::Core
                       Qt5::Widgets
                     )


INSTALL( TARGETS kminfo DESTINATION ${PLUGIN_INSTALL_DIR} )
INSTALL( FILES kminfo.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

