cmake_minimum_required(VERSION 3.18)

project(${SKBUILD_PROJECT_NAME} VERSION ${SKBUILD_PROJECT_VERSION} LANGUAGES CXX)

set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(_flowplot flowplot_py.cpp)

target_compile_features(_flowplot PRIVATE cxx_std_20)

target_include_directories(_flowplot PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_compile_definitions(_flowplot PRIVATE
	FLOW_PLOT_IMPLEMENTATION
	FLOW_PLOT_RENDERER
	FLOW_PLOT_COMPLETE_JSON
)

install(TARGETS _flowplot LIBRARY DESTINATION flowplot)
