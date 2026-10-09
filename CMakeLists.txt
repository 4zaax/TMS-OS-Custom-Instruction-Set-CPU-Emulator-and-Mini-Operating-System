cmake_minimum_required(VERSION 3.10)
project(tmsos C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(tmsos src/os.c src/handlers.c src/helpers.c)

if(MSVC)
    target_compile_options(tmsos PRIVATE /W3)
    target_compile_definitions(tmsos PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(tmsos PRIVATE -Wall)
endif()
