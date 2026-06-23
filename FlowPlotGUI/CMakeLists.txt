cmake_minimum_required(VERSION 3.21)

if(POLICY CMP0207)
    cmake_policy(SET CMP0207 NEW)
endif()

project(FlowPlotGUI VERSION 0.9.10 LANGUAGES CXX)

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

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set(MSDF_ATLAS_DYNAMIC_RUNTIME ON CACHE BOOL "Link msdf-atlas-gen with the dynamic MSVC runtime" FORCE)
    set(MSDFGEN_DYNAMIC_RUNTIME ON CACHE BOOL "Link msdfgen with the dynamic MSVC runtime" FORCE)
endif()

# FlowUi options
option(FLOWPLOTGUI_ENABLE_DEV_MODE "Enable FlowUi developer mode in FlowPlotGUI" OFF)

if(WIN32)
    set(FLOWPLOTGUI_RUNTIME_INSTALL_DIR ".")
    set(FLOWPLOTGUI_RESOURCE_INSTALL_DIR ".")
    set(FLOWPLOTGUI_SHADER_INSTALL_DIR "shaders")
elseif(APPLE)
    set(FLOWPLOTGUI_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
    set(FLOWPLOTGUI_RESOURCE_INSTALL_DIR "FlowPlotGUI.app/Contents/Resources")
    set(FLOWPLOTGUI_SHADER_INSTALL_DIR "${FLOWPLOTGUI_RESOURCE_INSTALL_DIR}/shaders")
    set(FLOWPLOTGUI_FRAMEWORK_INSTALL_DIR "FlowPlotGUI.app/Contents/Frameworks")
else()
    set(FLOWPLOTGUI_RUNTIME_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}")
    set(FLOWPLOTGUI_RESOURCE_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/FlowPlotGUI")
    set(FLOWPLOTGUI_SHADER_INSTALL_DIR "${FLOWPLOTGUI_RESOURCE_INSTALL_DIR}/shaders")
endif()

set(FLOWUI_INCLUDE_ICON_MANAGER ON CACHE BOOL "Enable FlowUi Icon manager" FORCE)
set(FLOWUI_PUBLIC_VULKAN_INTEROP ON CACHE BOOL "Enable FlowUi Vulkan interop API" FORCE)
set(FLOWUI_ENABLE_RUNTIME_FONT_BAKING ON CACHE BOOL "Enable FlowUi runtime font baking" FORCE)
set(FLOWUI_INSTALL OFF CACHE BOOL "Disable FlowUi install rules for FlowPlotGUI app packages" FORCE)
set(FLOWUI_INSTALL_RUNTIME_ASSETS ON CACHE BOOL "Install FlowUi runtime assets for FlowPlotGUI app packages" FORCE)
set(FLOWUI_RUNTIME_ASSET_INSTALL_DIR "${FLOWPLOTGUI_SHADER_INSTALL_DIR}" CACHE PATH "FlowUi runtime asset install destination" FORCE)
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


if(WIN32)
    set_target_properties(FlowPlotGUI PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

if(APPLE)
    set(FLOWPLOTGUI_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_BINARY_DIR}/Info.plist")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/packaging/Info.plist.in"
        "${FLOWPLOTGUI_BUNDLE_INFO_PLIST}"
        @ONLY)

    set(FLOWPLOTGUI_BUNDLE_ICON "${CMAKE_CURRENT_BINARY_DIR}/FlowPlotGUI.icns")
    set(FLOWPLOTGUI_ICONSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/FlowPlotGUI.iconset")
    set(FLOWPLOTGUI_ICON_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/assets/Svgs/favicon/android-chrome-512x512.png")
    add_custom_command(
        OUTPUT "${FLOWPLOTGUI_BUNDLE_ICON}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${FLOWPLOTGUI_ICONSET_DIR}"
        COMMAND sips -z 16 16 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_16x16.png"
        COMMAND sips -z 32 32 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_16x16@2x.png"
        COMMAND sips -z 32 32 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_32x32.png"
        COMMAND sips -z 64 64 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_32x32@2x.png"
        COMMAND sips -z 128 128 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_128x128.png"
        COMMAND sips -z 256 256 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_128x128@2x.png"
        COMMAND sips -z 256 256 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_256x256.png"
        COMMAND sips -z 512 512 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_256x256@2x.png"
        COMMAND sips -z 512 512 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_512x512.png"
        COMMAND sips -z 1024 1024 "${FLOWPLOTGUI_ICON_SOURCE}" --out "${FLOWPLOTGUI_ICONSET_DIR}/icon_512x512@2x.png"
        COMMAND iconutil -c icns "${FLOWPLOTGUI_ICONSET_DIR}" -o "${FLOWPLOTGUI_BUNDLE_ICON}"
        DEPENDS "${FLOWPLOTGUI_ICON_SOURCE}"
        COMMENT "Creating FlowPlotGUI macOS application icon"
        VERBATIM)
    add_custom_target(flowplotgui_bundle_icon DEPENDS "${FLOWPLOTGUI_BUNDLE_ICON}")
    add_dependencies(FlowPlotGUI flowplotgui_bundle_icon)
    set_source_files_properties("${FLOWPLOTGUI_BUNDLE_ICON}" PROPERTIES
        GENERATED TRUE
        MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(FlowPlotGUI PRIVATE "${FLOWPLOTGUI_BUNDLE_ICON}")
    set_target_properties(FlowPlotGUI PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${FLOWPLOTGUI_BUNDLE_INFO_PLIST}"
        MACOSX_BUNDLE_ICON_FILE "FlowPlotGUI.icns"
        INSTALL_RPATH "@executable_path/../Frameworks")
endif()

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
        FLOWPLOTGUI_SOURCE_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
        FLOWPLOTGUI_SOURCE_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
        FLOWPLOTGUI_DEV_MODE=$<BOOL:${FLOWPLOTGUI_ENABLE_DEV_MODE}>
        $<$<PLATFORM_ID:Windows>:NOMINMAX>
        $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
)

if(WIN32)
    set(FLOWPLOTGUI_WINDOWS_RUNTIME_PRE_EXCLUDE_REGEXES
        "api-ms-.*"
        "ext-ms-.*"
        "[Aa][Dd][Vv][Aa][Pp][Ii]32\\.dll"
        "[Aa][Zz][Uu][Rr][Ee][Aa][Tt][Tt][Ee][Ss][Tt].*\\.dll"
        "[Cc][Oo][Mm][Cc][Tt][Ll]32\\.dll"
        "[Cc][Oo][Mm][Dd][Ll][Gg]32\\.dll"
        "[Cc][Rr][Yy][Pp][Tt]32\\.dll"
        "[Gg][Dd][Ii]32\\.dll"
        "[Hh][Vv][Ss][Ii][Ff][Ii][Ll][Ee][Tt][Rr][Uu][Ss][Tt]\\.dll"
        "[Ii][Mm][Mm]32\\.dll"
        "[Kk][Ee][Rr][Nn][Ee][Ll]32\\.dll"
        "[Mm][Ss][Vv][Cc][Pp]140.*\\.dll"
        "[Oo][Ll][Ee]32\\.dll"
        "[Oo][Ll][Ee][Aa][Uu][Tt]32\\.dll"
        "[Pp][Dd][Mm][Uu][Tt][Ii][Ll][Ii][Tt][Ii][Ee][Ss]\\.dll"
        "[Rr][Pp][Cc][Rr][Tt]4\\.dll"
        "[Ss][Ee][Cc][Uu][Rr][Ee]32\\.dll"
        "[Ss][Hh][Ee][Ll][Ll]32\\.dll"
        "[Ss][Hh][Ll][Ww][Aa][Pp][Ii]\\.dll"
        "[Uu][Ss][Ee][Rr]32\\.dll"
        "[Vv][Cc][Rr][Uu][Nn][Tt][Ii][Mm][Ee]140.*\\.dll"
        "[Vv][Uu][Ll][Kk][Aa][Nn]-1\\.dll"
        "[Ww][Pp][Aa][Xx][Hh][Oo][Ll][Dd][Ee][Rr]\\.dll"
        "[Ww][Tt][Dd].*\\.dll"
    )

    install(TARGETS FlowPlotGUI
        RUNTIME_DEPENDENCY_SET FlowPlotGUIRuntimeDeps
        RUNTIME DESTINATION ${FLOWPLOTGUI_RUNTIME_INSTALL_DIR} COMPONENT Runtime
        BUNDLE DESTINATION . COMPONENT Runtime
    )

    install(RUNTIME_DEPENDENCY_SET FlowPlotGUIRuntimeDeps
        DESTINATION ${FLOWPLOTGUI_RUNTIME_INSTALL_DIR}
        COMPONENT Runtime
        PRE_EXCLUDE_REGEXES ${FLOWPLOTGUI_WINDOWS_RUNTIME_PRE_EXCLUDE_REGEXES}
        POST_EXCLUDE_REGEXES
            ".*[Ww][Ii][Nn][Dd][Oo][Ww][Ss][/\\\\][Ss][Tt][Ee][Mm]32[/\\\\].*"
    )
elseif(APPLE)
    install(TARGETS FlowPlotGUI
        RUNTIME_DEPENDENCY_SET FlowPlotGUIRuntimeDeps
        BUNDLE DESTINATION . COMPONENT Runtime
    )

    install(RUNTIME_DEPENDENCY_SET FlowPlotGUIRuntimeDeps
        DESTINATION "${FLOWPLOTGUI_FRAMEWORK_INSTALL_DIR}"
        COMPONENT Runtime
        POST_EXCLUDE_REGEXES
            "^/System/Library/.*"
            "^/usr/lib/.*")

    get_filename_component(FLOWPLOTGUI_VULKAN_LIBRARY_DIR "${Vulkan_LIBRARY}" DIRECTORY)
    find_file(FLOWPLOTGUI_MOLTENVK_LIBRARY
        NAMES libMoltenVK.dylib
        HINTS "${FLOWPLOTGUI_VULKAN_LIBRARY_DIR}")
    find_file(FLOWPLOTGUI_MOLTENVK_ICD_MANIFEST
        NAMES MoltenVK_icd.json
        HINTS "${FLOWPLOTGUI_VULKAN_LIBRARY_DIR}/../share/vulkan/icd.d")
    if(NOT FLOWPLOTGUI_MOLTENVK_LIBRARY OR NOT FLOWPLOTGUI_MOLTENVK_ICD_MANIFEST)
        message(FATAL_ERROR "The macOS Vulkan SDK must provide libMoltenVK.dylib and MoltenVK_icd.json.")
    endif()

    file(READ "${FLOWPLOTGUI_MOLTENVK_ICD_MANIFEST}" FLOWPLOTGUI_MOLTENVK_ICD_CONTENTS)
    string(REGEX REPLACE
        "\"library_path\"[ \\t\\r\\n]*:[ \\t\\r\\n]*\"[^\"]*\""
        "\"library_path\": \"../../../Frameworks/libMoltenVK.dylib\""
        FLOWPLOTGUI_MOLTENVK_ICD_CONTENTS
        "${FLOWPLOTGUI_MOLTENVK_ICD_CONTENTS}")
    set(FLOWPLOTGUI_BUNDLED_ICD_MANIFEST "${CMAKE_CURRENT_BINARY_DIR}/MoltenVK_icd.json")
    file(WRITE "${FLOWPLOTGUI_BUNDLED_ICD_MANIFEST}" "${FLOWPLOTGUI_MOLTENVK_ICD_CONTENTS}")
    install(FILES "${Vulkan_LIBRARY}" "${FLOWPLOTGUI_MOLTENVK_LIBRARY}"
        DESTINATION "${FLOWPLOTGUI_FRAMEWORK_INSTALL_DIR}"
        COMPONENT Runtime)
    install(FILES "${FLOWPLOTGUI_BUNDLED_ICD_MANIFEST}"
        DESTINATION "${FLOWPLOTGUI_RESOURCE_INSTALL_DIR}/vulkan/icd.d"
        COMPONENT Runtime)

    set(FLOWPLOTGUI_BUNDLE_SEARCH_DIRS
        "${FLOWPLOTGUI_VULKAN_LIBRARY_DIR};/opt/homebrew/lib;/usr/local/lib")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/packaging/FixupBundle.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/FixupBundle.cmake"
        @ONLY)
    install(SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/FixupBundle.cmake" COMPONENT Runtime)
else()
    install(TARGETS FlowPlotGUI
        RUNTIME DESTINATION ${FLOWPLOTGUI_RUNTIME_INSTALL_DIR} COMPONENT Runtime
        BUNDLE DESTINATION . COMPONENT Runtime
    )
endif()

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    DESTINATION ${FLOWPLOTGUI_RESOURCE_INSTALL_DIR}
    COMPONENT Runtime
)

install(DIRECTORY "${FLOWPLOTGUI_SHADER_OUTPUT_DIR}/"
    DESTINATION ${FLOWPLOTGUI_SHADER_INSTALL_DIR}
    COMPONENT Runtime
    FILES_MATCHING PATTERN "*.spv"
)

if(UNIX AND NOT APPLE)
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/packaging/io.github.flowplot.FlowPlotGUI.desktop"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
        COMPONENT Runtime
    )

    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/Svgs/favicon/android-chrome-512x512.png"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/512x512/apps"
        RENAME FlowPlotGUI.png
        COMPONENT Runtime
    )
endif()

set(CPACK_PACKAGE_NAME "FlowPlotGUI")
set(CPACK_PACKAGE_VENDOR "FlowPlot")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "FlowPlotGUI-${PROJECT_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "FlowPlotGUI")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_COMPONENTS_ALL Runtime)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "FlowPlotGUI Runtime")
set(CPACK_VERBATIM_VARIABLES ON)

if(UNIX AND NOT APPLE)
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
    set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "FlowPlot")
    set(CPACK_DEBIAN_PACKAGE_SECTION "science")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libc6 (>= 2.35), libstdc++6, libgcc-s1, libvulkan1, libglfw3, libgtk-3-0, libfreetype6, libpng16-16, zlib1g")
    set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
    set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "FlowPlotGUI graphical template editor")
endif()

if(WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_COMPONENT Runtime)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${FLOWPLOTGUI_RUNTIME_INSTALL_DIR}")
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)

    set(CPACK_PACKAGE_FILE_NAME "FlowPlotGUI-${PROJECT_VERSION}-Windows")
    set(CPACK_NSIS_DISPLAY_NAME "FlowPlotGUI")
    set(CPACK_NSIS_PACKAGE_NAME "FlowPlotGUI")
    set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/Svgs/favicon/favicon.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/assets/Svgs/favicon/favicon.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "FlowPlotGUI.exe")
    set(CPACK_NSIS_MENU_LINKS "FlowPlotGUI.exe" "FlowPlotGUI")
    set(CPACK_NSIS_MODIFY_PATH OFF)
endif()

if(APPLE)
    set(CPACK_PACKAGE_FILE_NAME "FlowPlotGUI-${PROJECT_VERSION}-macOS")
    set(CPACK_DMG_VOLUME_NAME "FlowPlotGUI")
    set(CPACK_DMG_FORMAT "UDBZ")
endif()

include(CPack)
