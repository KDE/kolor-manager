#cmake file for building Kolor-Manager and related modules
 
PROJECT(kolor-manager)
SET( PACKAGE_NAME kolor-manager )

SET( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules )


# project version
SET( ${PROJECT_NAME}_MAJOR_VERSION 1)
SET( ${PROJECT_NAME}_MINOR_VERSION 1)
SET( ${PROJECT_NAME}_MICRO_VERSION 0)

SET( ${PROJECT_NAME}_VERSION ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_MICRO_VERSION} )

FIND_PACKAGE(Qt5Widgets)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
FIND_PACKAGE(ECM REQUIRED NO_MODULE)
SET(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_MODULE_PATH})
INCLUDE(KDEInstallDirs)
INCLUDE(KDECompilerSettings)
INCLUDE(KDECMakeSettings)
INCLUDE(FeatureSummary)
INCLUDE(GenerateExportHeader)

FIND_PACKAGE(Qt5 REQUIRED COMPONENTS Core Widgets)
FIND_PACKAGE(KF5 REQUIRED COMPONENTS CoreAddons I18n ConfigWidgets KDELibs4Support ConfigWidgets)

FIND_PACKAGE(Oyranos REQUIRED)
FIND_PACKAGE(Synnefo REQUIRED)
 

INCLUDE_DIRECTORIES(${KDE4_INCLUDES} ${OYRANOS_INCLUDE_DIRS} ${XCM_INCLUDE_DIRS} ${SYNNEFO_INCLUDE_DIR})
LINK_DIRECTORIES(
    ${KDE4_LIBRARY_DIRS}
    ${OYRANOS_LIBRARY_DIRS}
    ${XCM_LIBRARY_DIRS}
)


SET( ${CMAKE_BUILD_TYPE} Debug)

SUBDIRS( devices
         information
         #settings
       )


INSTALL( FILES settings-kolor-management.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

ADD_CUSTOM_TARGET( dist
                COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/ && git archive --prefix=${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}/ --format=tar HEAD > ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar
                COMMAND echo "git:       git clone git://anongit.kde.org/${PACKAGE_NAME}" > ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND echo "git sha1:  " | tr -d "\\n" >> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/ && git rev-parse HEAD >> ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND rm -rf ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION} && tar xf ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar
                COMMAND tar cf - ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION} | bzip2 > ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2
                COMMAND echo  "size:      " | tr -d "\\n" >> ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND du -b ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2 | sed s/[[:space:]][[:print:]]*// >> ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND echo  "md5sum:    " | tr -d "\\n" >> ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/ && md5sum ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2 >> ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND echo  "sha1sum:   " | tr -d "\\n" >> ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND sha1sum ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2 >> ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND echo  "sha256sum: " | tr -d "\\n" >> ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMAND sha256sum ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2 >> ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2.hashsum
                COMMENT Make a ${PACKAGE_NAME} source package by using git
                VERBATIM
                WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                )
 
ADD_CUSTOM_TARGET( debsrc
                COMMAND test -d ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}/ && rm -rf ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}/
                COMMAND tar xjf ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2
                COMMAND mv ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}.tar.bz2 ${PACKAGE_NAME}_${${PACKAGE_NAME}_VERSION}.orig.tar.bz2
                COMMAND cp -R ${CMAKE_SOURCE_DIR}/extras/debian ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}/
                COMMAND dpkg-source -b ${PACKAGE_NAME}-${${PACKAGE_NAME}_VERSION}/
                DEPENDS dist
                COMMENT Make a ${PROJECT_NAME} DEB source package
                VERBATIM
                )
