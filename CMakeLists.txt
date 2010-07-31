#cmake file for building Kolor-Manager and related modules
 
PROJECT(kolor_manager)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

cmake_minimum_required(VERSION 2.4)

# project version
SET( ${PROJECT_NAME}_MAJOR_VERSION 0)
SET( ${PROJECT_NAME}_MINOR_VERSION 1)

find_package(KDE4 REQUIRED )
find_package(Oyranos REQUIRED)

include_directories(${KDE4_INCLUDES} ${OYRANOS_INCLUDE_DIR})


SET( ${CMAKE_BUILD_TYPE} Debug)

SUBDIRS(devices information settings)


#kde4_add_plugin(settings-kolor-management ${kolor_manager_PART_SRCS})
 
#install(TARGETS kmdevices DESTINATION /usr/lib64/kde4)
install( FILES settings-kolor-management.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
 

