# tools - manual console harnesses for verifying core without the GUI.
#
# coretest: hand-driven console to exercise core (port enumeration now, more later).
# Plain add_executable (no WIN32) -> console subsystem, so stdout/qDebug show in
# the terminal. Links core ONLY (not Widgets) -> proves core stands alone.
add_executable(coretest
    coretest.cpp
)

target_link_libraries(coretest
    PRIVATE
        core
)
