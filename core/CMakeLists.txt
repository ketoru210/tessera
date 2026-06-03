# core - serial communication logic layer (static library)
# Key point: link only Core and SerialPort, never Widgets, to stay decoupled from the UI.
add_library(core STATIC
    core.cpp
    core.h
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
