INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
 
SET(kmsettings_PART_SRCS
      kmsettings.cpp
      )

QT5_ADD_RESOURCES(kcm_kmsettings_RCC_SRCS ${kmsettings_RCCS})
ADD_LIBRARY( kcm_kmsettings MODULE ${kmsettings_PART_SRCS} )
TARGET_LINK_LIBRARIES( kcm_kmsettings
                       KF5::CoreAddons
                       KF5::ConfigWidgets
                       KF5::I18n
                       ${SYNNEFO_LIBRARIES}
                       Qt5::Core
                       Qt5::Widgets
                     )

INSTALL(TARGETS kcm_kmsettings          DESTINATION ${PLUGIN_INSTALL_DIR})
INSTALL(FILES   kcm_kmsettings.desktop  DESTINATION ${SERVICES_INSTALL_DIR} )

