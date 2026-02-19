cmake_minimum_required(VERSION 3.15)

project(
    RxNet
    VERSION 1.0.0
    LANGUAGES C CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ────────────────────────────────────────────────────────────────
#  Helper to collect sources
# ────────────────────────────────────────────────────────────────
function(collect_sources root_dir out_var)
    file(GLOB_RECURSE sources
        "${root_dir}/*.c"
        "${root_dir}/*.cpp"
        "${root_dir}/*.cxx"
    )
    set(${out_var} ${sources} PARENT_SCOPE)
endfunction()

# ────────────────────────────────────────────────────────────────
#  Library target
# ────────────────────────────────────────────────────────────────
add_library(${PROJECT_NAME})

collect_sources("${CMAKE_CURRENT_SOURCE_DIR}/src" LIB_SOURCES)
target_sources(${PROJECT_NAME}
    PRIVATE
        ${LIB_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32)
endif()

# C/C++ standard
set_target_properties(${PROJECT_NAME} PROPERTIES
    C_STANDARD          11
    C_STANDARD_REQUIRED ON
    CXX_STANDARD        17
    CXX_STANDARD_REQUIRED ON
)

# ────────────────────────────────────────────────────────────────
#  Testing executable that links against the library
# ────────────────────────────────────────────────────────────────
add_executable(server-test)
target_compile_options(server-test PRIVATE
    $<$<CONFIG:Debug>:-g>
)

collect_sources("${CMAKE_CURRENT_SOURCE_DIR}/server-test" TEST_SOURCES)

target_sources(server-test
    PRIVATE
        ${TEST_SOURCES}
)

target_link_libraries(server-test
    PRIVATE
        ${PROJECT_NAME}
)

target_include_directories(server-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(server-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_executable(client-test)
target_compile_options(client-test PRIVATE
    $<$<CONFIG:Debug>:-g>
)

collect_sources("${CMAKE_CURRENT_SOURCE_DIR}/client-test" TEST_SOURCES)

target_sources(client-test
    PRIVATE
        ${TEST_SOURCES}
)

target_link_libraries(client-test
    PRIVATE
        ${PROJECT_NAME}
)

target_include_directories(client-test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(client-test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ────────────────────────────────────────────────────────────────
#  Installation rules
# ────────────────────────────────────────────────────────────────
install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include/${PROJECT_NAME}/
    DESTINATION include/${PROJECT_NAME}
)

install(
    EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION lib/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
)
