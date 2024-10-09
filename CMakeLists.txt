cmake_minimum_required(VERSION 3.14)
project(FileMoverShellExtension)

set(CMAKE_CXX_STANDARD 17)

set(CONTEXT_MENU_SOURCE_FILES
    src/contextMenu/dll.cpp src/contextMenu/dll.def
    src/contextMenu/contextMenu.h src/contextMenu/contextMenu.cpp
    src/contextMenu/contextMenuFactory.h src/contextMenu/contextMenuFactory.cpp
)
add_library(fileMover SHARED ${CONTEXT_MENU_SOURCE_FILES})
target_link_libraries(fileMover shlwapi)

set(SETUP_SOURCE_FILES
    src/setup.cpp
    src/config.h
    src/utils.h src/utils.cpp
)
add_executable(setup ${SETUP_SOURCE_FILES})