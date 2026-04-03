cmake_minimum_required(VERSION 3.20)

project(FlowPlotGUI LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Keep a compile database next to sources so IDEs (clangd, etc.) resolve
# FlowUi include paths instead of falling back to root-level compile_flags.txt.
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(FLOWPLOTGUI_COMPILE_COMMANDS_LINK
        "${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json")
    if(EXISTS "${FLOWPLOTGUI_COMPILE_COMMANDS_LINK}" OR IS_SYMLINK "${FLOWPLOTGUI_COMPILE_COMMANDS_LINK}")
        file(REMOVE "${FLOWPLOTGUI_COMPILE_COMMANDS_LINK}")
    endif()

    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E create_symlink
            "${CMAKE_BINARY_DIR}/compile_commands.json"
            "${FLOWPLOTGUI_COMPILE_COMMANDS_LINK}"
        RESULT_VARIABLE FLOWPLOTGUI_LINK_RESULT
        ERROR_QUIET
    )
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# FlowUi options
set(FLOWUI_INCLUDE_SVG_MANAGER ON CACHE BOOL "Enable FlowUi SVG manager" FORCE)
set(FLOWUI_PUBLIC_VULKAN_INTEROP ON CACHE BOOL "Enable FlowUi Vulkan interop API" FORCE)
set(FLOWUI_ENABLE_RUNTIME_FONT_BAKING OFF CACHE BOOL "Disable FlowUi runtime font baking" FORCE)
set(FLOW_UI_DEV_MODE ON CACHE BOOL "Enable FlowUi developer mode APIs/UI" FORCE)

add_subdirectory(external/FlowUi)

file(GLOB_RECURSE FLOWPLOTGUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(FlowPlotGUI
    main.cpp
    ${FLOWPLOTGUI_SOURCES}
)

target_include_directories(FlowPlotGUI
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/../FlowPlot"
)

target_link_libraries(FlowPlotGUI
    PRIVATE
        FlowUi::FlowUi
)
