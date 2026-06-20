cmake_minimum_required(VERSION 3.19)
project(tessera VERSION 1.0.0 LANGUAGES CXX)

# Export compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find Qt once at the top level; declare every component used by subprojects:
#   core uses Core + SerialPort, gui uses Widgets.
# Note: keeping core free of Widgets is enforced by target_link_libraries in
# core/CMakeLists (it links only Core/SerialPort), not by where find_package lives.
find_package(Qt6 6.5 REQUIRED COMPONENTS Core SerialPort Widgets)


# Qt-wide setup: enable automatic moc/uic/rcc, set the C++ standard, etc.
qt_standard_project_setup()

# Layered layout:
#   core - serial communication logic (static library), no Widgets dependency
#   gui  - user interface (executable), links against core
add_subdirectory(core)
add_subdirectory(gui)

# tools - console drivers to exercise core standalone (no GUI). Optional.
add_subdirectory(tools)
