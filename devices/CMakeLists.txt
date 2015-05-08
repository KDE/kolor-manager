INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

SET(kmdevices_PART_SRCS 
      kmdevices.cpp   
      )
SET(kmdevices_RCCS
      icons.qrc
      )

QT5_ADD_RESOURCES(kmdevices_RCC_SRCS ${kmdevices_RCCS})
ADD_LIBRARY( kmdevices MODULE ${kmdevices_PART_SRCS} ${kmdevices_RCC_SRCS})
TARGET_LINK_LIBRARIES( kmdevices 
                       KF5::CoreAddons
                       KF5::ConfigWidgets
                       KF5::I18n
                       ${SYNNEFO_LIBRARIES}
                       Qt5::Core
                       Qt5::Widgets )

INSTALL( TARGETS kmdevices DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL( FILES kmdevices.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

