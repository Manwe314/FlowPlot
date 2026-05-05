# FlowPlot

FlowPlot is a header-only plotting library that:

- compiles a JSON template into an internal spec
- binds runtime data (`withData("dataset.field", ...)`)
- resolves layout/axes/layers into a render command stream
- optionally rasterizes to PNG with the built-in CPU renderer

It supports scatter and histogram layers, automatic axis-domain inference, legends/titles, and pluggable text measurement/layout through `ITextEngine`.
Text styling supports font family, numeric weight, and style (`normal`, `italic`, `oblique`).

## Quick Start

### 1. Create a template (`ScatterTemplate.json`)

```json
{
  "version": "1.0",
  "fonts": [
    {
      "family": "Inter",
      "weight": 400,
      "style": "normal",
      "path": "/absolute/path/to/Inter-Regular.ttf"
    },
    {
      "family": "Inter",
      "weight": 700,
      "style": "normal",
      "path": "/absolute/path/to/Inter-Bold.ttf"
    }
  ],
  "figure": {
    "width": 1200,
    "height": 800,
    "title": {
      "visible": true,
      "text": "Scatter Plot",
      "fontFamily": "Inter",
      "fontWeight": 700,
      "fontStyle": "normal"
    }
  },
  "datasets": [
    {
      "name": "main",
      "schema": {
        "x": "number",
        "y": "number"
      }
    }
  ],
  "panels": [
    {
      "layers": [
        {
          "type": "scatter",
          "dataset": "main",
          "mapping": {
            "x": { "field": "x" },
            "y": { "field": "y" }
          }
        }
      ]
    }
  ]
}
```

### 2. Render from C++

```cpp
#define FLOW_PLOT_RENDERER
#define FLOW_PLOT_IMPLEMENTATION
#define FLOW_PLOT_DEFAULT_FONT_PATH "./FacultyGlyphic-Regular.ttf"
#include "FlowPlot_Mega.hpp"

#include <vector>

int main()
{
    std::vector<int> x{1, 2, 3, 4, 5};
    std::vector<int> y{3, 4, 1, 4, 6};

    FlowPlot::plot("./ScatterTemplate.json")
        .withData("main.x", x)
        .withData("main.y", y)
        .writePng("./out.png");
}
```

### 3. Compile

```bash
g++ -std=c++20 -I. -IFlowPlot main.cpp -o plot
```

## Public API

- `FlowPlot::plot(path)` loads template JSON (`.json` extension optional).
- `PlotBuilder::set("path.to.prop", value)` mutates template JSON before compile.
  - `value` supports: `int`, `float`, `double`, `bool`, `const char*`, `std::string`, `std::string_view`.
- `PlotBuilder::setJsonRaw("path.to.prop", jsonText)` sets a property from raw JSON text (object/array/primitive).
- `PlotBuilder::withData("dataset.field", std::span<const T>)`
- `PlotBuilder::withData("dataset.field", const std::vector<T>&)`
- `PlotBuilder::useTextEngine(engine)` injects a custom text engine.
- `PlotBuilder::getCommands()` returns `RenderPlot` (resolved command stream).
- `PlotBuilder::writePng(path)` (only when `FLOW_PLOT_RENDERER` is enabled).
- `FlowPlot::registerFonts(textEngine, templateJsonText)` registers every root-level `fonts[]` entry with an `ITextEngine`.
- `FlowPlot::getCompleteJson(templateJsonText, pretty = true)` (only when `FLOW_PLOT_COMPLETE_JSON` is enabled).

## Defines

### Required in specific modes

- `FLOW_PLOT_RENDERER`
  - Enables renderer integration (`CpuRenderer`, `StbTextEngine`, `PlotBuilder::writePng`).
  - Define before including headers (or pass as compiler define).

- `FLOW_PLOT_IMPLEMENTATION`
  - Required in exactly one translation unit if you use built-in stb implementations.
  - Without this (and without external stb implementation), renderer builds can compile but will fail to link when stb symbols are needed.

### Optional

- `FLOW_PLOT_DEFAULT_FONT_PATH`
  - String literal path to a default TTF file auto-registered by `StbTextEngine`.
  - `PlotBuilder::writePng` and renderer-enabled `PlotBuilder::getCommands` auto-create a fallback `StbTextEngine` if none is set; this default font path makes that work out of the box.

- `FLOW_PLOT_COMPLETE_JSON`
  - Enables full-template normalization helper: `getCompleteJson(templateJsonText, pretty)`.

- `FLOW_PLOT_STB_EXTERNAL_IMPLEMENTATION`
  - Use when you provide stb implementations externally.
  - Disables FlowPlot’s internal stb implementation emission.

### Internal (do not set manually)

Examples: `FLOW_PLOT_HPP_INCLUDED`, `FLOW_PLOT_RENDERER_HPP_INCLUDED`, `FLOW_PLOT_DEFINED_STBTT_IMPLEMENTATION`, etc.

## Template Schema (Overview)

Root object:

- `version` (`"1.0"`)
- `fonts` optional array of font variants
- `figure`
- `datasets`
- `layout`
- `panels`

### `fonts`

Optional root-level font manifest. Each entry describes one concrete font face:

```json
{
  "family": "Inter",
  "weight": 400,
  "style": "normal",
  "path": "/absolute/path/to/Inter-Regular.ttf"
}
```

- `family`: family name referenced by text specs.
- `weight`: numeric font weight, usually `100..900`.
- `style`: `normal`, `italic`, or `oblique`.
- `path`: absolute path to a `.ttf`/`.ttc` font file.

The built-in fallback `StbTextEngine` automatically registers these entries for `writePng()` and renderer-enabled `getCommands()`. Custom text engines can opt in by calling `FlowPlot::registerFonts(engine, templateJsonText)`.

### `figure`

Includes width/height/dpi/background/padding/title/legends.

### `datasets`

Each dataset has:

- `name`
- `schema` map where values are `number`, `string`, or `boolean`

Runtime data binding uses:

- `withData("datasetName.fieldName", data)`

### `panels`

Each panel has:

- style/frame/title
- axes: `xAxis`, `yAxis`, `xSecondary`, `ySecondary`
- `layers`

### `layers`

Common layer fields:

- `type`: `scatter` or `histogram`
- `dataset`: dataset name
- `axisData`: `{ "x": "primary|secondary|null", "y": "primary|secondary|null" }`
- `mapping`, `style`, `stats`, `config`

Scatter mapping (important fields):

- `mapping.x.field` (required)
- `mapping.y.field` (required)
- optional: `mapping.color.field`, `mapping.size.field`, `mapping.label.field`

Histogram mapping (important fields):

- `mapping.data.field` (required)
- `mapping.data.axis`: `"x"` or `"y"` (which axis receives input data)
- optional color field/mapping

Notes:

- If no layer contributes data to an axis and no explicit `min/max` is provided, axis domain falls back to `0..1`.
- You must define panels/layers/mappings if you want automatic domain inference from data.

## Renderer Pipeline

High-level flow:

1. Template JSON -> compiled spec
2. Spec + data views -> bound IR
3. Bound IR + text metrics -> resolved IR
4. Resolved IR -> `RenderPlot` command stream
5. `CpuRenderer` rasterizes commands to RGBA8 image / PNG

Command variants include:

- `BoxCommand`
- `PolylineCommand`
- `TextCommand`
- `MarkersCommand`
- clip stack commands (`PushClipRectCommand`, `PopClipRectCommand`)

## Text Engine Pluggability

`ITextEngine` is pluggable for text metrics/layout:

- `registerFont(...)`
- `hasFont(...)`
- `measureText(...)`
- `layoutText(...)`

Built-in `StbTextEngine` provides UTF-8 layout and glyph bitmap raster support used by the CPU renderer.

Important current behavior:

- `resolvePlotIR` needs a text engine for auto-sized text boxes.
- `PlotBuilder::writePng` and renderer-enabled `PlotBuilder::getCommands` auto-fall back to `StbTextEngine` when no engine is explicitly set.
- The fallback `StbTextEngine` automatically registers root `fonts[]` entries before resolving text.
- If no default font is available, `writePng` throws with guidance (`useTextEngine(...)` or set `FLOW_PLOT_DEFAULT_FONT_PATH`).
- Font lookup uses `family + weight + style`, with fallback to normal style/default weight/default family where possible.
- Non-`StbTextEngine` custom engines can drive measurement/layout, but glyph rasterization in the built-in renderer is currently specialized for `StbTextEngine`.

## Header Options

You can use modular headers (`FlowPlot/FlowPlot.hpp`) or one of four generated amalgamated headers.

Regenerate all amalgamated variants with:

```bash
python3 tools/generate_flowplot_mega.py
```

Generated outputs:

- `FlowPlot_Mega_Core.hpp`
  - Inlines FlowPlot headers only.
  - Leaves RapidJSON and stb external (`-IFlowPlot` needed for bundled deps).
- `FlowPlot_Mega_Stb.hpp`
  - Inlines FlowPlot + stb headers.
  - Leaves RapidJSON external (`-IFlowPlot` needed for bundled deps).
- `FlowPlot_Mega_Json.hpp`
  - Inlines FlowPlot + used RapidJSON subset.
  - Leaves stb external (`-IFlowPlot` needed only if renderer/stb paths are used).
- `FlowPlot_Mega.hpp`
  - Inlines FlowPlot + used RapidJSON subset + stb.
  - Fully self-contained single header (copy one file and include it).

All variants support the same feature macros and runtime API.

## JSON Backend

FlowPlot uses RapidJSON. The JSON-inlined mega variants include only the RapidJSON subset reachable from FlowPlot's actual include usage.

## Documentation Status

This is a temporary README. FlowPlot will get full documentation and a pre-v1.0.0 release soon.
