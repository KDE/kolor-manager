set(kminfo_PART_SRCS
      kminfo.cpp
      )
SET(kminfo_RCCS
      icons.qrc
      )

QT5_ADD_RESOURCES(kcm_kminfo_RCC_SRCS ${kminfo_RCCS})
ADD_LIBRARY( kcm_kminfo MODULE ${kminfo_PART_SRCS} ${kminfo_RCC_SRCS} )
TARGET_LINK_LIBRARIES( kcm_kminfo
                       KF5::CoreAddons
                       KF5::ConfigWidgets
                       KF5::I18n
                       ${SYNNEFO_LIBRARIES}
                       Qt5::Core
                       Qt5::Widgets
                     )


INSTALL( TARGETS kcm_kminfo          DESTINATION  ${PLUGIN_INSTALL_DIR} )
INSTALL( FILES   kcm_kminfo.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

