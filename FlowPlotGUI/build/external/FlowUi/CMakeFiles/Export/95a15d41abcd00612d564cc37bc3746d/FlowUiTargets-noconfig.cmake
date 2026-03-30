#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "FlowUi::FlowUi" for configuration ""
set_property(TARGET FlowUi::FlowUi APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(FlowUi::FlowUi PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libflowui.a"
  )

list(APPEND _cmake_import_check_targets FlowUi::FlowUi )
list(APPEND _cmake_import_check_files_for_FlowUi::FlowUi "${_IMPORT_PREFIX}/lib/libflowui.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
