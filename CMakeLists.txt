cmake_minimum_required(VERSION 3.11)

#names the project hello
# sets variable PROJECT_NAME to "hello"
project(graphics_utility_notes)

# tells CMake to compile main.c into an executable named hello
add_executable(${PROJECT_NAME} main.c)

# links executable against PSP system libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    pspdebug
    pspdisplay
    pspge
    pspgu
    pspgum
    pspkernel
)

create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)
