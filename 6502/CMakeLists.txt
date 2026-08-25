add_library(6502 STATIC)

set(6502_SOURCES
    "src/pch.h"
    "src/CPU.h"
    "src/CPU.cpp"
    "src/Memory.h"
    "src/Memory.cpp"
    "src/Types.h"
)

set(BASE_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set_target_properties(6502 PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${BASE_OUTPUT_DIR}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "" FILES ${6502_SOURCES})
source_group("Precompiled Headers" REGULAR_EXPRESSION ".*pch.*")

target_sources(6502 PRIVATE ${6502_SOURCES})

target_compile_features(6502 PUBLIC cxx_std_20)
target_include_directories(6502 PUBLIC src)
target_precompile_headers(6502 PRIVATE src/pch.h)

target_link_libraries(6502 PRIVATE ProjectWarnings)
