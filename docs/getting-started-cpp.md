# Getting Started with C++

This tutorial renders a template to PNG from a C++ application. It uses the complete generated header because it is the shortest path to a working prototype.

## 1. Get FlowPlot

Clone the repository and make `FlowPlot_Mega.hpp` available to your project:

```bash
git clone https://github.com/Manwe314/FlowPlot.git
cd FlowPlot
```

For the first template, copy `Examples/ScatterTemplate.json` beside your source file. You can also build one in [FlowPlotGUI](gui-install.md) and export it there.

## 2. Create a plot program

Create `main.cpp`:

```cpp
#define FLOW_PLOT_RENDERER
#define FLOW_PLOT_IMPLEMENTATION
#define FLOW_PLOT_DEFAULT_FONT_PATH "./Examples/FacultyGlyphic-Regular.ttf"
#include "FlowPlot_Mega.hpp"

#include <vector>

int main()
{
    std::vector<int> x{1, 2, 3, 4, 5};
    std::vector<int> y{3, 4, 1, 4, 6};

    FlowPlot::plot("ScatterTemplate")
        .withData("main.x", x)
        .withData("main.y", y)
        .writePng("scatter.png");
}
```

`FlowPlot::plot()` returns a thread-local builder intended for one compact chain. Use `FlowPlot::makePlot()` when multiple builders need to coexist on the same thread.

## 3. Compile and run

From the repository root:

```bash
g++ -std=c++20 -I. -IFlowPlot main.cpp -o plot
./plot
```

The program writes `scatter.png`. The vectors must remain alive until `writePng()` returns because `withData()` keeps non-owning views of them.

## Next steps

The GUI owns reusable presentation; C++ supplies live data. Edit or build a template with FlowPlotGUI, export it, and retain the same data binding calls where the schema is unchanged. See the [template workflow](template-workflow.md) and the generated [C++ API reference](annotated.html).
