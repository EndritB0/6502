add_subdirectory(6502)
add_subdirectory(Sandbox)
add_subdirectory(Tests)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)
