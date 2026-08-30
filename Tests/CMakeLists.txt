add_executable(6502Tests)

set(TESTS_SOURCES
    "src/pch.h"
    "src/CPU/CPUTests.h"
    "src/CPU/LDATest.cpp"
    "src/CPU/LDXTest.cpp"
    "src/CPU/LDYTest.cpp"
    "src/CPU/STATest.cpp"
    "src/CPU/STXTest.cpp"
    "src/CPU/STYTest.cpp"
    "src/CPU/TSXTest.cpp"
    "src/CPU/TXSTest.cpp"
    "src/CPU/PHATest.cpp"
    "src/CPU/PHPTest.cpp"
    "src/CPU/PLATest.cpp"
    "src/CPU/PLPTest.cpp"
    "src/CPU/JMPTest.cpp"
    "src/CPU/JSRTest.cpp"
    "src/CPU/RTSTest.cpp"
    "src/CPU/DavePoo Tests/DavePooTests.h"
    "src/CPU/DavePoo Tests/LoadRegisterTests.cpp"
    "src/CPU/DavePoo Tests/StoreRegisterTests.cpp"
    "src/MemoryTests.cpp"
)

option(ENABLE_DAVEPOO_TESTS "Compile the DavePoo's tests" OFF)

set(BASE_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set_target_properties(6502Tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BASE_OUTPUT_DIR}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "" FILES ${TESTS_SOURCES})
source_group("Precompiled Headers" REGULAR_EXPRESSION ".*pch.*")

target_sources(6502Tests PRIVATE ${TESTS_SOURCES})

target_compile_features(6502Tests PRIVATE cxx_std_20)
target_include_directories(6502Tests PRIVATE src)
target_precompile_headers(6502Tests PRIVATE src/pch.h)
target_compile_definitions(6502Tests PRIVATE ENABLE_DAVEPOO_TESTS=$<BOOL:${ENABLE_DAVEPOO_TESTS}>)

target_link_libraries(6502Tests PRIVATE 6502 GTest::gtest_main ProjectWarnings)

include(GoogleTest)
gtest_discover_tests(6502Tests)
