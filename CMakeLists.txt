cmake_minimum_required(VERSION 3.50)
project(Slime C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") 
set(CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_C_FLAGS_DEBUG "-O0" CACHE STRING "" FORCE)

set(BUILD_SHARED_LIBS ON)
set(SDL3IMAGE_INSTALL OFF)

add_subdirectory(vendored/SDL)
add_subdirectory(vendored/SDL_image)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    vendored/cJSON/*.c
    src/**/*.c
    src/*.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/vendored/SDL/include
    ${CMAKE_SOURCE_DIR}/vendored/SDL_image/include
    ${CMAKE_SOURCE_DIR}/vendored/cJSON
    ${CMAKE_SOURCE_DIR}/vendored/Nuklear
    ${CMAKE_SOURCE_DIR}/vendored/cute_tiled
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/Assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"
)
