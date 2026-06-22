# Use FlowPlotGUI

FlowPlotGUI is for authoring the static part of a plot: layout, panel configuration, layers, labels, axes, colors, and template defaults. Runtime application code remains responsible for binding the data declared by the template.

## Typical workflow

1. Create a template or open an existing JSON template.
2. Define the datasets and fields that application code will provide.
3. Add panels, axes, and layers such as scatter or histogram layers.
4. Adjust visual properties until the preview matches the intended chart.
5. Export the JSON template into the C++ or Python project.
6. Bind values using the dataset and field names from the template.

For example, a template with a dataset named `main` and fields `x` and `y` is used by either interface as follows:

```cpp
FlowPlot::plot("ScatterTemplate")
    .withData("main.x", x)
    .withData("main.y", y)
    .writePng("scatter.png");
```

```python
(
    flowplot.plot("ScatterTemplate")
    .with_data("main.x", x)
    .with_data("main.y", y)
    .write_png("scatter.png")
)
```

Changing colors, labels, or layout in the template normally requires no code change. Renaming a dataset or field does require updating the corresponding binding in code.
