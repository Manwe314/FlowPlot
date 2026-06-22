# Getting Started with Python

This tutorial creates a PNG from a FlowPlot template using Python. You need Python 3.10 or later and either an existing template JSON file or FlowPlotGUI to create one.

## 1. Install the package

```bash
python -m pip install FlowPlotPy
```

## 2. Get a template

For a starting point, copy `Examples/ScatterTemplate.json` into your project, for example as `ScatterTemplate.json`. Alternatively, create a template in [FlowPlotGUI](gui-install.md) and export it into your project directory.

The template declares the data fields it expects. The sample scatter template uses the dataset `main` with numeric `x` and `y` fields.

## 3. Bind data and write the plot

Create `plot.py` next to the template:

```python
import flowplot

x = [1, 2, 3, 4, 5]
y = [3, 4, 1, 4, 6]

(
    flowplot.plot("ScatterTemplate")
    .with_data("main.x", x)
    .with_data("main.y", y)
    .write_png("scatter.png")
)
```

Run it:

```bash
python plot.py
```

`scatter.png` is written to the current directory. The Python package includes a default font, so basic PNG rendering does not require manual font registration.

## Next steps

Use FlowPlotGUI to edit titles, colors, axes, and layers without changing this Python code. Your code only needs to keep binding the fields required by the exported template. See the [template workflow](template-workflow.md) for that handoff.
