
find_package(XRandR REQUIRED)

set(kolor_server_SRCS
    color-context.cpp
    display.cpp
    kolor-server.cpp
    output.cpp
    screen.cpp
)

set(CMAKE_REQUIRED_INCLUDES ${X11_Xrandr_INCLUDE_PATH}/Xrandr.h)
set(CMAKE_REQUIRED_LIBRARIES ${X11_Xrandr_LIB})
check_function_exists(XRRGetScreenResourcesCurrent HAVE_RANDR_1_3)
check_function_exists(XRRGetScreenSizeRange HAVE_RANDR_1_2)

kde4_add_plugin(kded_kolorserver ${kolor_server_SRCS})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(kded_kolorserver
    ${KDE4_KDECORE_LIBS}
    ${OYRANOS_LIBRARIES}
    ${QT_LIBRARIES} ${QT_QTGUI_LIBRARIES}
    X11 Xrandr ${XCM_LDFLAGS}
)

install(TARGETS kded_kolorserver DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kolorserver.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
