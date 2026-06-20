# core - serial communication logic layer (static library)
# Key point: link only Core and SerialPort, never Widgets, to stay decoupled from the UI.
add_library(core STATIC
    core.cpp
    core.h
    portinfo.cpp
    portinfo.h
    serialport.cpp
    serialport.h
)

target_link_libraries(core
    PUBLIC
        Qt::Core
        Qt::SerialPort
)

# Let targets that link core (e.g. gui) #include "core.h" directly
target_include_directories(core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Inject the single source-of-truth version (root project() VERSION) into the
# binary, so version() never has to be hand-edited on release.
target_compile_definitions(core PRIVATE TESSERA_VERSION="${PROJECT_VERSION}")
