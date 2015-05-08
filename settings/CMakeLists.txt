INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
 
SET(kmsettings_PART_SRCS 
      kmsettings.cpp
      )

ADD_LIBRARY( kmsettings MODULE ${kmsettings_PART_SRCS} )
SET_TARGET_PROPERTIES( kminfo PROPERTIES PREFIX "${CMAKE_SHARED_LIBRARY_PREFIX}" )
TARGET_LINK_LIBRARIES( kmsettings
                       KF5::CoreAddons
                       KF5::ConfigWidgets
                       KF5::I18n
                       ${SYNNEFO_LIBRARIES}
                       Qt5::Core
                       Qt5::Widgets
                     )

INSTALL(TARGETS kmsettings DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL( FILES kmsettings.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

