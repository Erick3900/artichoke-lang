cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(
    artichoke-lang
    VERSION 0.0.1
    DESCRIPTION ""
    LANGUAGES CXX
)

set(PROJECT_AUTHOR "Erick Saúl")
set(PROJECT_AUTHOR_GITHUB "@Erick3900")

add_subdirectory(configured_files)

find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(docopt CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(tl-expected CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)

file(GLOB_RECURSE LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/lang/**.cpp")
file(GLOB_RECURSE CLI_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/cli/**.cpp")
file(GLOB_RECURSE TST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/tests/**.cpp")

add_library(
    ${PROJECT_NAME} STATIC
        ${LIB_SOURCES}
)

target_link_libraries(
    ${PROJECT_NAME} PUBLIC
        fmt::fmt
        tl::expected
)

target_include_directories(
    ${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include/lang
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common
        ${CMAKE_BINARY_DIR}/configured_files/include
)


add_executable(
    artichoke 
        main.cpp
        ${CLI_SOURCES}
)

target_link_libraries(
    artichoke PRIVATE
        docopt::docopt
        ${PROJECT_NAME}
)

target_include_directories(
    artichoke PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/cli
        ${CMAKE_CURRENT_SOURCE_DIR}/include/common
        ${CMAKE_BINARY_DIR}/configured_files/include
)

set_target_properties(
    artichoke PROPERTIES
        OUTPUT_NAME "arti-lang"
)


enable_testing()

foreach(FILE IN LISTS TST_SOURCES)
    cmake_path(GET FILE PARENT_PATH PARENT)
    cmake_path(GET PARENT STEM PARENT_STEM)
    cmake_path(GET FILE STEM FILE_STEM)

    if (${PARENT_STEM} MATCHES "tests")
        set(TestName ${FILE_STEM})
    else()
        set(TestName "${PARENT_STEM}.${FILE_STEM}")
    endif()

    add_executable(
        ${TestName}_test
            ${FILE}
    )

    target_include_directories(
        ${TestName}_test PRIVATE
            tests/include
    )

    target_link_libraries(
        ${TestName}_test PRIVATE
            GTest::gtest
            ${PROJECT_NAME}
    )

    add_test(${TestName} ${TestName}_test)
endforeach()
