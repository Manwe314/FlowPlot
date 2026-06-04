cmake_minimum_required(VERSION 3.20)

project(FlowPlotGUI VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
option(FLOWPLOTGUI_ENABLE_DEV_MODE "Enable FlowUi developer mode in FlowPlotGUI" OFF)

set(FLOWUI_INCLUDE_ICON_MANAGER ON CACHE BOOL "Enable FlowUi Icon manager" FORCE)
set(FLOWUI_PUBLIC_VULKAN_INTEROP ON CACHE BOOL "Enable FlowUi Vulkan interop API" FORCE)
set(FLOWUI_ENABLE_RUNTIME_FONT_BAKING ON CACHE BOOL "Enable FlowUi runtime font baking" FORCE)
set(FLOWUI_INSTALL OFF CACHE BOOL "Disable FlowUi install rules for FlowPlotGUI app packages" FORCE)
set(FLOWUI_BUILD_FONT_BAKER OFF CACHE BOOL "Disable FlowUi font baker tool for FlowPlotGUI app packages" FORCE)
set(FLOW_UI_DEV_MODE ${FLOWPLOTGUI_ENABLE_DEV_MODE} CACHE BOOL "Enable FlowUi developer mode APIs/UI" FORCE)

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
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_box.vert"
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_box.frag"
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_marker.vert"
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_marker.frag"
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_text.vert"
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_text.frag"
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_polyline.vert"
    "${FLOWPLOTGUI_SHADER_SOURCE_DIR}/plot_viewport_polyline.frag"
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
        FLOWPLOTGUI_SOURCE_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
        FLOWPLOTGUI_DEV_MODE=$<BOOL:${FLOWPLOTGUI_ENABLE_DEV_MODE}>
        $<$<PLATFORM_ID:Windows>:NOMINMAX>
        $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
)

install(TARGETS FlowPlotGUI
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
    BUNDLE DESTINATION . COMPONENT Runtime
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    DESTINATION .
    COMPONENT Runtime
)

install(DIRECTORY "${FLOWPLOTGUI_SHADER_OUTPUT_DIR}/"
    DESTINATION shaders
    COMPONENT Runtime
    FILES_MATCHING PATTERN "*.spv"
)

set(CPACK_PACKAGE_NAME "FlowPlotGUI")
set(CPACK_PACKAGE_VENDOR "FlowPlot")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "FlowPlotGUI-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "FlowPlotGUI")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_COMPONENTS_ALL Runtime)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "FlowPlotGUI Runtime")

if(APPLE)
    set(CPACK_DMG_VOLUME_NAME "FlowPlotGUI")
endif()

include(CPack)
