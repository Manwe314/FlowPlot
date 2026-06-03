#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace FlowPlot
{
	namespace Spec
	{
		struct BoxSpec
		{
			std::optional<float> x = std::nullopt;
			std::optional<float> y = std::nullopt;
			std::optional<float> width = std::nullopt;
			std::optional<float> height = std::nullopt;
		};

		struct PaddingSpec
		{
			float left = 0.0f;
			float right = 0.0f;
			float top = 0.0f;
			float bottom = 0.0f;
		};

		struct TextSpec
		{
			bool visible = false;
			std::string text{};
			std::string fontFamily = "Default";
			float fontSize = 16.0f;
			std::uint16_t fontWeight = 400;
			std::string fontStyle = "normal";
			std::string color = "#111111";
			std::string overflow = "clip";
			bool clip = true;
			std::string hAlign = "left";
			std::string vAlign = "top";
			BoxSpec box{};
		};

		inline TextSpec makeFigureTitleTextSpec()
		{
			TextSpec title{};
			title.visible = true;
			title.text = "Untitled Plot";
			title.fontSize = 24.0f;
			title.fontWeight = 700;
			title.color = "#111111";
			title.box.height = 40.0f;
			return title;
		}

		inline TextSpec makePanelTitleTextSpec()
		{
			TextSpec title{};
			title.visible = false;
			title.text.clear();
			title.fontSize = 16.0f;
			title.color = "#111111";
			title.box.height = 24.0f;
			return title;
		}

		inline TextSpec makeXAxisTitleTextSpec()
		{
			TextSpec title{};
			title.visible = true;
			title.text = "X";
			title.fontSize = 14.0f;
			title.color = "#222222";
			title.box.height = 20.0f;
			return title;
		}

		inline TextSpec makeYAxisTitleTextSpec()
		{
			TextSpec title{};
			title.visible = true;
			title.text = "Y";
			title.fontSize = 14.0f;
			title.fontWeight = 500;
			title.color = "#222222";
			title.hAlign = "center";
			title.vAlign = "middle";
			title.box.height = 20.0f;
			return title;
		}

		struct LegendElementSpec
		{
			std::string id = "legend_element_1";
			std::string text = "Untitled Plot";
			std::string fontFamily = "Default";
			float fontSize = 24.0f;
			std::uint16_t fontWeight = 700;
			std::string fontStyle = "normal";
			std::string color = "#111111";
			std::string overflow = "clip";
			bool clip = true;
			BoxSpec box{std::nullopt, std::nullopt, std::nullopt, 40.0f};
			std::string iconShape = "square";
			std::string iconColor = "#0d37f0ff";
		};

		struct LegendSpec
		{
			std::string id = "legend_1";
			bool visible = false;
			std::string background = "#ffffff";
			std::string borderColor = "#cccccc";
			float borderWidth = 0.0f;
			PaddingSpec padding{8.0f, 8.0f, 8.0f, 8.0f};
			float gap = 12.0f;
			BoxSpec box{};
			std::vector<LegendElementSpec> legendElements{};
		};

		struct ScatterColorMappingSpec
		{
			std::string type = "bins";
			std::vector<std::string> categories{};
			std::vector<std::string> values{};
			std::string minValue = "#150353ff";
			std::string maxValue = "#f3f717ff";
		};

		struct ScatterSizeMappingSpec
		{
			std::string type = "contiguous";
			std::vector<std::string> categories{};
			std::vector<float> values{};
			float minValue = 3.0f;
			float maxValue = 12.0f;
		};

		struct ScatterLabelMappingSpec
		{
			std::vector<std::string> categories{};
			std::vector<std::string> values{};
		};

		enum class AxisDataRole : std::uint8_t
		{
			Null = 0,
			Primary,
			Secondary
		};

		struct LayerAxisDataSpec
		{
			AxisDataRole x = AxisDataRole::Primary;
			AxisDataRole y = AxisDataRole::Primary;
		};

		struct ScatterMappingSpec
		{
			std::string xField{};
			std::string yField{};
			std::string colorField{};
			ScatterColorMappingSpec colorMapping{};
			std::string sizeField{};
			ScatterSizeMappingSpec sizeMapping{};
			std::string labelField{};
			ScatterLabelMappingSpec labelMapping{};
		};

		struct ScatterStyleSpec
		{
			std::string markerShape = "circle";
			std::string fillColor = "#3366cc";
			std::string strokeColor = "#1f3f7f";
			float strokeWidth = 0.0f;
			float markerSize = 12.0f;
		};

		struct ScatterStatsSpec
		{
			bool showMeanPoint = false;
			std::string meanPointShape = "diamond";
			std::string meanPointColor = "#ff6633";
			std::string meanPointStrokeColor = "#aa3311";
			float meanPointStrokeWidth = 1.0f;
			float meanPointSize = 10.0f;
		};

		struct ScatterConfigSpec
		{
			float jitterX = 0.0f;
			float jitterY = 0.0f;
			float domainPadding = 0.05f;
		};

		struct HistogramColorMappingSpec
		{
			std::string type = "bins";
			std::vector<std::string> categories{};
			std::vector<std::string> values{};
			std::string minValue = "#3355aaff";
			std::string maxValue = "#88aaffff";
		};

		enum class HistogramDataAxis : std::uint8_t
		{
			X = 0,
			Y
		};

		struct HistogramMappingSpec
		{
			std::string dataField{};
			HistogramDataAxis axis = HistogramDataAxis::X;
			std::string colorField{};
			HistogramColorMappingSpec colorMapping{};
		};

		struct HistogramStyleSpec
		{
			std::string fillColor = "#88aaffb4";
			std::string strokeColor = "#3355aa";
			float strokeWidth = 1.0f;
		};

		struct HistogramStatsSpec
		{
			bool showMeanLine = false;
			std::string meanLineColor = "#ff6633";
			float meanLineWidth = 2.0f;
			bool showMedianLine = false;
			std::string medianLineColor = "#33aa66";
			float medianLineWidth = 2.0f;
		};

		struct HistogramConfigSpec
		{
			std::uint32_t binCount = 20;
			bool normalize = false;
			bool cumulative = false;
			bool showEmptyBins = false;
			float domainPadding = 0.05f;
		};

		struct LayerSpec
		{
			std::string id = "layer_1";
			std::string type = "scatter";
			bool visible = true;
			float opacity = 1.0f;
			std::int32_t zIndex = 0;
			std::string dataset{};
			LayerAxisDataSpec axisData{};

			ScatterMappingSpec scatterMapping{};
			ScatterStyleSpec scatterStyle{};
			ScatterStatsSpec scatterStats{};
			ScatterConfigSpec scatterConfig{};

			HistogramMappingSpec histogramMapping{};
			HistogramStyleSpec histogramStyle{};
			HistogramStatsSpec histogramStats{};
			HistogramConfigSpec histogramConfig{};
		};

		struct AxisSpec
		{
			bool visible = true;
			TextSpec title{};
			std::string scale = "linear";
			std::optional<float> min = std::nullopt;
			std::optional<float> max = std::nullopt;
			bool invert = false;
			bool grid = true;
			std::string gridColor = "#e0e0e0";
			float gridWidth = 1.0f;
			std::string lineColor = "#666666";
			float lineWidth = 1.0f;
			std::string tickColor = "#666666";
			float tickWidth = 1.0f;
			float tickLength = 6.0f;
			std::uint32_t tickCount = 5;
			std::vector<double> tickValues{};
			float tickValueGap = 2.0f;
			std::string tickLabelFormat = "auto";
			std::string tickLabelFontFamily = "Default";
			float tickLabelFontSize = 12.0f;
			std::uint16_t tickLabelFontWeight = 400;
			std::string tickLabelFontStyle = "normal";
			std::string tickLabelColor = "#333333";
			bool showMinorTicks = false;
			std::uint32_t minorTickCount = 0;
		};

		inline AxisSpec makeXAxisSpec()
		{
			AxisSpec axis{};
			axis.title = makeXAxisTitleTextSpec();
			return axis;
		}

		inline AxisSpec makeYAxisSpec()
		{
			AxisSpec axis{};
			axis.title = makeYAxisTitleTextSpec();
			return axis;
		}

		inline AxisSpec makeXSecondaryAxisSpec()
		{
			AxisSpec axis = makeXAxisSpec();
			axis.visible = false;
			return axis;
		}

		inline AxisSpec makeYSecondaryAxisSpec()
		{
			AxisSpec axis = makeYAxisSpec();
			axis.visible = false;
			return axis;
		}

		struct PanelSpec
		{
			std::string id = "panel_1";
			bool visible = true;
			std::string background = "#f8f8f8";
			std::string borderColor = "#cccccc";
			float borderWidth = 1.0f;
			PaddingSpec padding{12.0f, 12.0f, 12.0f, 12.0f};
			bool clipContent = true;
			TextSpec title = makePanelTitleTextSpec();
			AxisSpec xAxis = makeXAxisSpec();
			AxisSpec yAxis = makeYAxisSpec();
			AxisSpec xSecondary = makeXSecondaryAxisSpec();
			AxisSpec ySecondary = makeYSecondaryAxisSpec();
			std::vector<LayerSpec> layers{};
		};

		struct FigureSpec
		{
			std::uint32_t width = 1200;
			std::uint32_t height = 800;
			std::uint32_t dpi = 96;
			std::string background = "#ffffff";
			PaddingSpec padding{24.0f, 24.0f, 24.0f, 24.0f};
			TextSpec title = makeFigureTitleTextSpec();
			std::vector<LegendSpec> legends{};
		};

		struct LayoutSpec
		{
			std::uint32_t rows = 1;
			std::uint32_t cols = 1;
			float gap = 16.0f;
		};

		struct DatasetSpec
		{
			enum class FieldType : std::uint8_t
			{
				Number,
				String,
				Boolean
			};

			std::string name{};
			std::unordered_map<std::string, FieldType> schema{};
		};

		struct MasterTemplateSpec
		{
			std::string version = "1.0";
			FigureSpec figure{};
			std::vector<DatasetSpec> datasets{};
			LayoutSpec layout{};
			std::vector<PanelSpec> panels{};
		};
	} // namespace Spec
} // namespace FlowPlot

#ifdef FLOW_PLOT_COMPLETE_JSON

namespace FlowInternal
{
	namespace JsonDefaults
	{
		inline constexpr std::string_view kLegendElementDefaultsJson = R"flowplot(
{
  "text": "Untitled Plot",
  "fontFamily": "Default",
  "fontSize": 24,
  "fontWeight": 700,
  "fontStyle": "normal",
  "color": "#111111",
  "overflow": "clip",
  "clip": true,
  "box": {
    "x": null,
    "y": null,
    "width": null,
    "height": 40
  },
  "iconShape": "square",
  "iconColor": "#0d37f0ff"
}
)flowplot";

		inline constexpr std::string_view kLegendDefaultsJson = R"flowplot(
{
  "visible": false,
  "background": "#ffffff",
  "borderColor": "#cccccc",
  "borderWidth": 0,
  "padding": {
    "left": 8,
    "right": 8,
    "top": 8,
    "bottom": 8
  },
  "gap": 12,
  "box": {
    "x": null,
    "y": null,
    "width": null,
    "height": null
  },
  "legendElements": []
}
)flowplot";

		inline constexpr std::string_view kScatterMappingDefaultsJson = R"flowplot(
{
  "x": {
    "field": ""
  },
  "y": {
    "field": ""
  },
  "color": {
    "field": "",
    "colorMapping-scatter": {
      "type": "bins",
      "categories": [],
      "values": [],
      "minValue": "#150353ff",
      "maxValue": "#f3f717ff"
    }
  },
  "size": {
    "field": "",
    "sizeMapping-scatter": {
      "type": "contiguous",
      "categories": [],
      "values": [],
      "minValue": 3.0,
      "maxValue": 12.0
    }
  },
  "label": {
    "field": "",
    "labelMapping-scatter": {
      "categories": [],
      "values": []
    }
  }
}
)flowplot";

		inline constexpr std::string_view kScatterStyleDefaultsJson = R"flowplot(
{
  "markerShape": "circle",
  "fillColor": "#3366cc",
  "strokeColor": "#1f3f7f",
  "strokeWidth": 1.0,
  "markerSize": 6.0
}
)flowplot";

		inline constexpr std::string_view kScatterStatsDefaultsJson = R"flowplot(
{
  "showMeanPoint": false,
  "meanPointShape": "diamond",
  "meanPointColor": "#ff6633",
  "meanPointStrokeColor": "#aa3311",
  "meanPointStrokeWidth": 1.0,
  "meanPointSize": 10.0
}
)flowplot";

		inline constexpr std::string_view kScatterConfigDefaultsJson = R"flowplot(
{
  "jitterX": 0.0,
  "jitterY": 0.0,
  "domainPadding": 0.05
}
)flowplot";

		inline constexpr std::string_view kHistogramMappingDefaultsJson = R"flowplot(
{
  "data": {
    "field": "",
    "axis": "x"
  },
  "color": {
    "field": "",
    "colorMapping-histogram": {
      "type": "bins",
      "categories": [],
      "values": [],
      "minValue": "#3355aaff",
      "maxValue": "#88aaffff"
    }
  }
}
)flowplot";

		inline constexpr std::string_view kHistogramStyleDefaultsJson = R"flowplot(
{
  "fillColor": "#88aaff",
  "strokeColor": "#3355aa",
  "strokeWidth": 1.0
}
)flowplot";

		inline constexpr std::string_view kHistogramStatsDefaultsJson = R"flowplot(
{
  "showMeanLine": false,
  "meanLineColor": "#ff6633",
  "meanLineWidth": 2.0,
  "showMedianLine": false,
  "medianLineColor": "#33aa66",
  "medianLineWidth": 2.0
}
)flowplot";

		inline constexpr std::string_view kHistogramConfigDefaultsJson = R"flowplot(
{
  "binCount": 20,
  "normalize": false,
  "cumulative": false,
  "showEmptyBins": false,
  "domainPadding": 0.05
}
)flowplot";

		inline constexpr std::string_view kLayerDefaultsJson = R"flowplot(
{
  "id": "layer_1",
  "type": "scatter",
  "visible": true,
  "opacity": 1.0,
  "zIndex": 0,
  "dataset": "",
  "axisData": {
    "x": "primary",
    "y": "primary"
  },
  "mapping": {},
  "style": {},
  "stats": {},
  "config": {}
}
)flowplot";

		inline constexpr std::string_view kPanelDefaultsJson = R"flowplot(
{
  "id": "panel_1",
  "visible": true,
  "background": "#f8f8f8",
  "borderColor": "#cccccc",
  "borderWidth": 1,
  "padding": {
    "left": 56,
    "right": 24,
    "top": 24,
    "bottom": 56
  },
  "clipContent": true,
  "title": {
    "visible": false,
    "text": "",
    "fontFamily": "Default",
    "fontSize": 16,
    "fontStyle": "normal",
    "color": "#111111",
    "overflow": "clip",
    "clip": true,
    "box": {
      "x": null,
      "y": null,
      "width": null,
      "height": 24
    }
  },
  "xAxis": {
    "visible": true,
    "title": {
      "visible": true,
      "text": "X",
      "fontFamily": "Default",
      "fontSize": 14,
      "fontStyle": "normal",
      "color": "#222222",
      "overflow": "clip",
      "clip": true,
      "box": {
        "x": null,
        "y": null,
        "width": null,
        "height": 20
      }
    },
    "scale": "linear",
    "min": null,
    "max": null,
    "invert": false,
    "grid": true,
    "gridColor": "#e0e0e0",
    "gridWidth": 1,
    "lineColor": "#666666",
    "lineWidth": 1,
    "tickColor": "#666666",
    "tickWidth": 1,
    "tickLength": 6,
    "tickCount": 5,
    "tickValues": [],
    "tickValueGap": 2.0,
    "tickLabelFormat": "auto",
    "tickLabelFontFamily": "Default",
    "tickLabelFontSize": 12,
    "tickLabelFontWeight": 400,
    "tickLabelFontStyle": "normal",
    "tickLabelColor": "#333333",
    "showMinorTicks": false,
    "minorTickCount": 0
  },
  "xSecondary": {
    "visible": false,
    "title": {
      "visible": true,
      "text": "X",
      "fontFamily": "Default",
      "fontSize": 14,
      "fontStyle": "normal",
      "color": "#222222",
      "overflow": "clip",
      "clip": true,
      "box": {
        "x": null,
        "y": null,
        "width": null,
        "height": 20
      }
    },
    "scale": "linear",
    "min": null,
    "max": null,
    "invert": false,
    "grid": true,
    "gridColor": "#e0e0e0",
    "gridWidth": 1,
    "lineColor": "#666666",
    "lineWidth": 1,
    "tickColor": "#666666",
    "tickWidth": 1,
    "tickLength": 6,
    "tickCount": 5,
    "tickValues": [],
    "tickValueGap": 2.0,
    "tickLabelFormat": "auto",
    "tickLabelFontFamily": "Default",
    "tickLabelFontSize": 12,
    "tickLabelFontWeight": 400,
    "tickLabelFontStyle": "normal",
    "tickLabelColor": "#333333",
    "showMinorTicks": false,
    "minorTickCount": 0
  },
  "yAxis": {
    "visible": true,
    "title": {
      "visible": true,
      "text": "Y",
      "fontFamily": "Default",
      "fontSize": 14,
      "fontWeight": 500,
      "fontStyle": "normal",
      "color": "#222222",
      "hAlign": "center",
      "vAlign": "middle",
      "overflow": "clip",
      "clip": true,
      "box": {
        "x": null,
        "y": null,
        "width": null,
        "height": 20
      }
    },
    "scale": "linear",
    "min": null,
    "max": null,
    "invert": false,
    "grid": true,
    "gridColor": "#e0e0e0",
    "gridWidth": 1,
    "lineColor": "#666666",
    "lineWidth": 1,
    "tickColor": "#666666",
    "tickWidth": 1,
    "tickLength": 6,
    "tickCount": 5,
    "tickValues": [],
    "tickValueGap": 2.0,
    "tickLabelFormat": "auto",
    "tickLabelFontFamily": "Default",
    "tickLabelFontSize": 12,
    "tickLabelFontWeight": 400,
    "tickLabelFontStyle": "normal",
    "tickLabelColor": "#333333",
    "showMinorTicks": false,
    "minorTickCount": 0
  },
  "ySecondary": {
    "visible": false,
    "title": {
      "visible": true,
      "text": "Y",
      "fontFamily": "Default",
      "fontSize": 14,
      "fontWeight": 500,
      "fontStyle": "normal",
      "color": "#222222",
      "hAlign": "center",
      "vAlign": "middle",
      "overflow": "clip",
      "clip": true,
      "box": {
        "x": null,
        "y": null,
        "width": null,
        "height": 20
      }
    },
    "scale": "linear",
    "min": null,
    "max": null,
    "invert": false,
    "grid": true,
    "gridColor": "#e0e0e0",
    "gridWidth": 1,
    "lineColor": "#666666",
    "lineWidth": 1,
    "tickColor": "#666666",
    "tickWidth": 1,
    "tickLength": 6,
    "tickCount": 5,
    "tickValues": [],
    "tickValueGap": 2.0,
    "tickLabelFormat": "auto",
    "tickLabelFontFamily": "Default",
    "tickLabelFontSize": 12,
    "tickLabelFontWeight": 400,
    "tickLabelFontStyle": "normal",
    "tickLabelColor": "#333333",
    "showMinorTicks": false,
    "minorTickCount": 0
  },
  "layers": []
}
)flowplot";

		inline constexpr std::string_view kMasterTemplateJson = R"flowplot(
{
  "version": "1.0",
  "fonts": [],
  "figure": {
    "width": 1200,
    "height": 800,
    "dpi": 96,
    "background": "#ffffff",
    "padding": {
      "left": 24,
      "right": 24,
      "top": 24,
      "bottom": 24
    },
    "title": {
      "visible": true,
      "text": "Untitled Plot",
      "fontFamily": "Default",
      "fontSize": 24,
      "fontWeight": 700,
      "fontStyle": "normal",
      "color": "#111111",
      "overflow": "clip",
      "clip": true,
      "box": {
        "x": null,
        "y": null,
        "width": null,
        "height": 40
      }
    },
    "legends": []
  },
  "datasets": [],
  "layout": {
    "rows": 1,
    "cols": 1,
    "gap": 16
  },
  "panels": []
}
)flowplot";
	} // namespace JsonDefaults
} // namespace FlowInternal

#endif // FLOW_PLOT_COMPLETE_JSON
