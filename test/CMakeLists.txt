enable_testing()
include(CTest)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(kongoc_test
    ../src/lib.cpp
    ./lib.test.cpp
)

target_include_directories(kongoc_test PRIVATE ../src)
target_compile_definitions(kongoc_test PRIVATE CATCH_CONFIG_MAIN)
add_test(NAME kongoc_test COMMAND sexpr_test WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})


