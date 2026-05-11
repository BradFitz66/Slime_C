cmake_minimum_required(VERSION 3.50)
project(Slime C)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "") 
set(CMAKE_C_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_C_FLAGS_DEBUG "-O0" CACHE STRING "" FORCE)

include(FetchContent)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-3.4.8 
)

FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
        SDL3_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-3.4.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

set(SDL3IMAGE_INSTALL OFF)
set(BUILD_SHARED_LIBS FALSE)

FetchContent_MakeAvailable(SDL3_image)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    vendored/cJSON/*.c
    src/**/*.c
    src/*.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)
target_include_directories(${PROJECT_NAME} PUBLIC ${SDL3_SOURCE_DIR}/include ${SDL3_IMAGE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/vendored/cJSON ${CMAKE_SOURCE_DIR}/vendored/Nuklear)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/Assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Assets"
)
