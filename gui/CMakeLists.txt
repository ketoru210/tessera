# gui - user interface layer (executable)
# Only this layer depends on Widgets; serial logic comes from linking core.
qt_add_executable(tessera
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp mainwindow.h mainwindow.ui
    portpopup.cpp  portpopup.h  portpopup.ui
)

target_link_libraries(tessera
    PRIVATE
        core
        Qt::Widgets
)

install(TARGETS tessera
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET tessera
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
