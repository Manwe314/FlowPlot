# FlowPlot

FlowPlot is a prerelease plotting library for C++ and Python. It renders plots from reusable JSON templates and runtime data, with a small chaining API for changing template values, binding data, and writing images.

The project is currently focused on scatter and histogram-style plots, template-driven layout, reusable visual defaults, and a GUI workflow for authoring templates.

## What Is FlowPlot?

FlowPlot is a plotting system built from a few programmatic parts rather than a single monolithic plotting call.

At the core is a compiler pipeline. It takes a plot template plus runtime data, normalizes the template with defaults, binds the supplied data fields, resolves layout and axes, and outputs renderer-independent drawing commands. Those commands can be consumed directly by an application, or passed to FlowPlot's built-in CPU renderer to produce a final image such as a PNG.

For C++, FlowPlot is header-only. You can include the modular headers or use one of the generated single-header `FlowPlot_Mega*.hpp` variants. For Python, FlowPlot is packaged as a native extension module built from the same C++ core.

FlowPlot also comes with FlowPlotGUI, a desktop app for visually authoring templates. The GUI is intended to make templates reusable and composable: instead of hard-coding every plot from scratch, users can build pluggable template blocks and feed them new data from C++ or Python. As a prerelease, customization is still limited in places, but the scaffolding for that template-first workflow is already in the project.

## Quick Start

### Python

Install the Python package:

```bash
pip install FlowPlotPy
```

Create or export a template, then bind data and write a PNG:

```python
import flowplot

plot = flowplot.plot("./ScatterTemplate")
plot.with_data("main.x", [1, 2, 3, 4, 5])
plot.with_data("main.y", [3, 4, 1, 4, 6])
plot.write_png("pythonPlot.png")
```

The Python package includes a default font, so basic PNG rendering works without manually registering one. NumPy arrays are recommended for larger data, but normal Python lists are supported.

### C++

Use the full single-header build when you want the simplest include path:

```cpp
#define FLOW_PLOT_RENDERER
#define FLOW_PLOT_IMPLEMENTATION
#define FLOW_PLOT_DEFAULT_FONT_PATH "./Inter.ttf"
#include "FlowPlot_Mega.hpp"

#include <vector>

int main()
{
    std::vector<int> x{1, 2, 3, 4, 5};
    std::vector<int> y{3, 4, 1, 4, 6};

    FlowPlot::plot("./ScatterTemplate")
        .withData("main.x", x)
        .withData("main.y", y)
        .writePng("./CppPlot.png");
}
```

Compile:

```bash
g++ -std=c++20 -I. main.cpp -o plot
```

For modular headers, include `FlowPlot/FlowPlot.hpp` and add the needed include paths for bundled dependencies.

### Templates

FlowPlotGUI is the visual template editor for FlowPlot. While templates are JSON files and can be written by hand, the library is designed with the GUI workflow in mind: build the layout and styling visually, export the completed template, then bind runtime data to it from C++ or Python. This keeps application code focused on data while the template owns the plot's reusable presentation.

Download the GUI package for your platform:

| Platform | Package | Download | Installation / use |
| --- | --- | --- | --- |
| 🍎 **macOS** | DMG | [Download for macOS](https://github.com/Manwe314/FlowPlot/releases/download/v0.9.9/FlowPlotGUI-0.9.9-macOS.dmg) | Open the DMG and drag `FlowPlotGUI.app` to Applications. See the macOS note below. |
| 🪟 **Windows** | Installer (recommended) | [Download installer](https://github.com/Manwe314/FlowPlot/releases/download/v0.9.9/FlowPlotGUI-0.9.9-Windows.exe) | Run the installer. |
| 🪟 **Windows** | Runtime ZIP | [Download ZIP](https://github.com/Manwe314/FlowPlot/releases/download/v0.9.9/FlowPlotGUI-0.9.9-Windows-Runtime.zip) | Unzip and run the included executable. |
| 🐧 **Linux** | AppImage (recommended) | [Download AppImage](https://github.com/Manwe314/FlowPlot/releases/download/v0.9.9/FlowPlotGUI-0.9.9-x86_64.AppImage) | Make it executable, then run it. |
| 🐧 **Linux** | Debian package | [Download `.deb`](https://github.com/Manwe314/FlowPlot/releases/download/v0.9.9/flowplotgui_0.9.9_amd64.deb) | Install it with your distribution's package installer. |
| 🐧 **Linux** | Tarball | [Download `.tar.gz`](https://github.com/Manwe314/FlowPlot/releases/download/v0.9.9/FlowPlotGUI-0.9.9-Linux.tar.gz) | Extract it and run the included executable. |

### macOS security notice

The macOS release is currently unsigned and not notarized. After moving `FlowPlotGUI.app` to Applications, macOS may block its first launch. In Finder, Control-click the app, choose **Open**, then confirm **Open**. If macOS still blocks it, go to **System Settings → Privacy & Security** and select **Open Anyway** for FlowPlotGUI. This creates an exception for this app only; do not disable Gatekeeper globally.

See the [FlowPlotGUI usage guide](docs/gui-usage.md) for the template-authoring workflow. Simple default templates will also be provided for common cases, so using the GUI will not be required for every plot.

## Tech Stack

FlowPlot is written in modern C++ and uses:

- RapidJSON for template parsing and normalization.
- stb libraries for the built-in CPU image/font path.
- pybind11, scikit-build-core, and CMake for the Python extension build.
- FlowUi, Vulkan, GLFW, and Native File Dialog for FlowPlotGUI.
- Doxygen for generated C++ API documentation.

The C++ library can be used as modular headers or as generated amalgamated headers. Regenerate the mega headers with:

```bash
python3 tools/generate_flowplot_mega.py
```

Available generated headers:

- `FlowPlot_Mega_Core.hpp`: FlowPlot headers only.
- `FlowPlot_Mega_Stb.hpp`: FlowPlot plus stb headers.
- `FlowPlot_Mega_Json.hpp`: FlowPlot plus the used RapidJSON subset.
- `FlowPlot_Mega.hpp`: FlowPlot plus RapidJSON subset plus stb, intended as the most self-contained option.

## Template Overview

FlowPlot templates are JSON documents that describe the static structure of a plot. Runtime code supplies data and may override selected template properties before rendering.

A typical template contains:

- `version`: template format version.
- `fonts`: optional font manifest for C++ rendering.
- `figure`: image size, background, padding, title, and figure-level layout.
- `datasets`: named dataset schemas.
- `panels`: plot panels with axes, titles, styles, and layers.
- `layers`: visual layers such as scatter or histogram mappings.

Runtime data is bound by dataset and field name:

```cpp
plot.withData("main.x", xValues);
plot.withData("main.y", yValues);
```

```python
plot.with_data("main.x", x_values)
plot.with_data("main.y", y_values)
```

Templates are meant to be reusable. You can author a template once, then use it with different data or small property overrides:

```cpp
FlowPlot::plot("./ScatterTemplate")
    .set("figure.title.text", "Experiment 12")
    .withData("main.x", x)
    .withData("main.y", y)
    .writePng("experiment-12.png");
```

Detailed API, schema, packaging, and GUI documentation will live in separate docs as the prerelease stabilizes.
