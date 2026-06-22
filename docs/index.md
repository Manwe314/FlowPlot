# FlowPlot Documentation {#mainpage}

FlowPlot is a template-driven plotting library that turns JSON plot definitions and runtime data into reusable charts. It is built for visual template authoring with FlowPlotGUI, then lightweight rendering from C++ or Python.

## Table of Contents

- [Getting started with Python](getting-started-python.md)
- [Getting started with C++](getting-started-cpp.md)
- [Install FlowPlotGUI](gui-install.md)
- [Use FlowPlotGUI](gui-usage.md)
- [Template workflow](template-workflow.md)
- [C++ API reference](annotated.html)

## Library interfaces

FlowPlot is available as the `FlowPlotPy` Python package and as a header-only C++ library. Both interfaces consume the same JSON templates, so a template exported by the GUI can be shared between Python and C++ applications.

## FlowPlotGUI

FlowPlotGUI is the desktop editor for creating and refining plot templates. Use it to arrange figures, panels, axes, layers, and visual styling, then export the template and bind the live data in your application code.

Start with [Getting started with Python](getting-started-python.md) or [Getting started with C++](getting-started-cpp.md). If you need to create a template first, install the GUI and follow the [template workflow](template-workflow.md).
