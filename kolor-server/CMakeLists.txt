
set(kolor_server_SRCS
    color-context.cpp
    display.cpp
    kolor-server.cpp
    output.cpp
    screen.cpp
)


kde4_add_plugin(kded_kolorserver ${kolor_server_SRCS})

target_link_libraries(kded_kolorserver
    ${KDE4_KDECORE_LIBS}
    ${OYRANOS_LIBRARIES}
    ${QT_LIBRARIES} ${QT_QTGUI_LIBRARIES}
    X11 Xrandr Xcm
)

install(TARGETS kded_kolorserver DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kolorserver.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kded)
