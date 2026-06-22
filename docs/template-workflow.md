# Template Workflow

A FlowPlot template is a JSON document that describes the reusable structure and appearance of a plot. It includes figure settings, datasets, panels, axes, layers, and optional font definitions; the application provides data at runtime.

## Start from an example or the GUI

For simple scatter plots, start from `Examples/ScatterTemplate.json`. For a custom layout, use [FlowPlotGUI](gui-usage.md) to construct and export a template.

## Keep the schema stable

Treat dataset and field names as the interface between a template and the application. A template that defines `main.x` and `main.y` requires both bindings before it can render. Visual edits are generally safe to make without changing code; schema changes must be mirrored in C++ or Python.

## Share templates across languages

The same exported template works with both bindings. This makes it practical to prototype a chart in Python, move it to a C++ application, or keep a common visual definition across tools.

Default templates for common cases will be provided over time, so the GUI is recommended for bespoke visuals rather than required for every plot.
