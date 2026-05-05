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
set(FLOWUI_INCLUDE_ICON_MANAGER ON CACHE BOOL "Enable FlowUi Icon manager" FORCE)
set(FLOWUI_PUBLIC_VULKAN_INTEROP ON CACHE BOOL "Enable FlowUi Vulkan interop API" FORCE)
set(FLOWUI_ENABLE_RUNTIME_FONT_BAKING OFF CACHE BOOL "Disable FlowUi runtime font baking" FORCE)
set(FLOW_UI_DEV_MODE ON CACHE BOOL "Enable FlowUi developer mode APIs/UI" FORCE)

add_subdirectory(external/FlowUi)

set(NFD_BUILD_TESTS OFF CACHE BOOL "Disable NFD tests" FORCE)
set(NFD_BUILD_SDL2_TESTS OFF CACHE BOOL "Disable NFD SDL tests" FORCE)
set(NFD_BUILD_GLFW3_TESTS OFF CACHE BOOL "Disable NFD GLFW tests" FORCE)
set(NFD_INSTALL OFF CACHE BOOL "Disable NFD install rules" FORCE)
set(NFD_WAYLAND OFF CACHE BOOL "Disable NFD Wayland support until vendored protocols are initialized" FORCE)
add_subdirectory(external/Nfd)

find_package(Vulkan REQUIRED)

set(FLOWPLOTGUI_SHADER_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")
set(FLOWPLOTGUI_SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/shaders")
set(FLOWPLOTGUI_SHADER_SOURCES
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_solid.vert"
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_solid.frag"
)

if(Vulkan_GLSLC_EXECUTABLE)
    set(FLOWPLOTGUI_SHADER_COMPILER "${Vulkan_GLSLC_EXECUTABLE}")
    set(FLOWPLOTGUI_SHADER_COMPILE_ARGS -O)
elseif(Vulkan_GLSLANG_VALIDATOR_EXECUTABLE)
    set(FLOWPLOTGUI_SHADER_COMPILER "${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}")
    set(FLOWPLOTGUI_SHADER_COMPILE_ARGS -V)
    message(WARNING "Vulkan glslc was not found; using glslangValidator to compile FlowPlotGUI shaders.")
else()
    message(FATAL_ERROR "No shader compiler found. Install Vulkan glslc (preferred) or glslangValidator.")
endif()

set(FLOWPLOTGUI_SHADER_SPIRV_FILES)
foreach(FLOWPLOTGUI_SHADER_SOURCE IN LISTS FLOWPLOTGUI_SHADER_SOURCES)
    get_filename_component(FLOWPLOTGUI_SHADER_FILE_NAME "${FLOWPLOTGUI_SHADER_SOURCE}" NAME)
    set(FLOWPLOTGUI_SHADER_SPIRV "${FLOWPLOTGUI_SHADER_OUTPUT_DIR}/${FLOWPLOTGUI_SHADER_FILE_NAME}.spv")

    add_custom_command(
        OUTPUT "${FLOWPLOTGUI_SHADER_SPIRV}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${FLOWPLOTGUI_SHADER_OUTPUT_DIR}"
        COMMAND "${FLOWPLOTGUI_SHADER_COMPILER}" ${FLOWPLOTGUI_SHADER_COMPILE_ARGS} "${FLOWPLOTGUI_SHADER_SOURCE}" -o "${FLOWPLOTGUI_SHADER_SPIRV}"
        DEPENDS "${FLOWPLOTGUI_SHADER_SOURCE}"
        COMMENT "Compiling FlowPlotGUI shader ${FLOWPLOTGUI_SHADER_FILE_NAME}"
        VERBATIM
    )

    list(APPEND FLOWPLOTGUI_SHADER_SPIRV_FILES "${FLOWPLOTGUI_SHADER_SPIRV}")
endforeach()

add_custom_target(flowplotgui_shaders DEPENDS ${FLOWPLOTGUI_SHADER_SPIRV_FILES})

file(GLOB_RECURSE FLOWPLOTGUI_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(FlowPlotGUI
    main.cpp
    ${FLOWPLOTGUI_SOURCES}
)

add_dependencies(FlowPlotGUI flowplotgui_shaders)

target_include_directories(FlowPlotGUI
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
	PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/../FlowPlot"
)

target_link_libraries(FlowPlotGUI
    PRIVATE
        FlowUi::FlowUi
        nfd::nfd
)

target_compile_definitions(FlowPlotGUI
    PRIVATE
        FLOWPLOTGUI_SHADER_OUTPUT_DIR="${FLOWPLOTGUI_SHADER_OUTPUT_DIR}"
)
