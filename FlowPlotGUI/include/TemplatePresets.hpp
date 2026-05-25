#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "FlowPlotGui.hpp"

namespace FlowPlotGui {

struct TemplatePreset {
	std::string name{};
	FlowPlot::Spec::MasterTemplateSpec activeTemplate{};
	std::vector<RunningDataset> datasets{};
};

inline TemplatePreset makeScatterPlotPreset()
{
	TemplatePreset preset{};
	preset.name = "Scatter Plot";
	preset.activeTemplate = FlowPlot::Spec::MasterTemplateSpec{};

	FlowPlot::Spec::MasterTemplateSpec& spec = preset.activeTemplate;
	spec.version = "1.0";
	spec.figure.width = 1200;
	spec.figure.height = 800;
	spec.figure.dpi = 96;
	spec.figure.background = "#ffffff";
	spec.figure.padding = FlowPlot::Spec::PaddingSpec{
		.left = 24.0f,
		.right = 24.0f,
		.top = 24.0f,
		.bottom = 24.0f,
	};
	spec.figure.title.visible = true;
	spec.figure.title.text = "Scatter Plot";
	spec.figure.title.fontFamily = "Default";
	spec.figure.title.fontSize = 24.0f;
	spec.figure.title.fontWeight = 700;
	spec.figure.title.color = "#111111";
	spec.figure.title.overflow = "clip";
	spec.figure.title.clip = true;
	spec.figure.title.box = FlowPlot::Spec::BoxSpec{};
	spec.figure.legends.clear();

	FlowPlot::Spec::DatasetSpec datasetSpec{};
	datasetSpec.name = "main";
	datasetSpec.schema["x"] = FlowPlot::Spec::DatasetSpec::FieldType::Number;
	datasetSpec.schema["y"] = FlowPlot::Spec::DatasetSpec::FieldType::Number;
	spec.datasets.push_back(std::move(datasetSpec));

	FlowPlot::Spec::LayerSpec scatterLayer{};
	scatterLayer.id = "layer_1";
	scatterLayer.type = "scatter";
	scatterLayer.dataset = "main";
	scatterLayer.scatterMapping.xField = "x";
	scatterLayer.scatterMapping.yField = "y";

	FlowPlot::Spec::PanelSpec panel{};
	panel.id = "panel_1";
	panel.layers.clear();
	panel.layers.push_back(std::move(scatterLayer));
	spec.panels.clear();
	spec.panels.push_back(std::move(panel));

	const std::vector<int> x{1, 2, 3, 4, 5};
	const std::vector<int> y{3, 4, 1, 4, 6};

	FlowPlotGui::numericColumn xColumn{};
	xColumn.name = "x";
	xColumn.data.assign(x.begin(), x.end());

	FlowPlotGui::numericColumn yColumn{};
	yColumn.name = "y";
	yColumn.data.assign(y.begin(), y.end());

	FlowPlotGui::RunningDataset runningDataset{};
	runningDataset.name = "main";
	runningDataset.numericColumns.push_back(std::move(xColumn));
	runningDataset.numericColumns.push_back(std::move(yColumn));
	preset.datasets.push_back(std::move(runningDataset));

	return preset;
}

inline TemplatePreset makeHistogramChartPreset()
{
	TemplatePreset preset{};
	preset.name = "Histogram chart";
	preset.activeTemplate = FlowPlot::Spec::MasterTemplateSpec{};

	FlowPlot::Spec::MasterTemplateSpec& spec = preset.activeTemplate;
	spec.version = "1.0";
	spec.figure.width = 1200;
	spec.figure.height = 800;
	spec.figure.dpi = 96;
	spec.figure.background = "#ffffff";
	spec.figure.padding = FlowPlot::Spec::PaddingSpec{
		.left = 24.0f,
		.right = 24.0f,
		.top = 24.0f,
		.bottom = 24.0f,
	};
	spec.figure.title.visible = true;
	spec.figure.title.text = "Histogram chart";
	spec.figure.title.fontFamily = "Default";
	spec.figure.title.fontSize = 24.0f;
	spec.figure.title.fontWeight = 700;
	spec.figure.title.color = "#111111";
	spec.figure.title.overflow = "clip";
	spec.figure.title.clip = true;
	spec.figure.title.box = FlowPlot::Spec::BoxSpec{};
	spec.figure.legends.clear();

	FlowPlot::Spec::DatasetSpec datasetSpec{};
	datasetSpec.name = "resonance";
	datasetSpec.schema["frequency_hz"] = FlowPlot::Spec::DatasetSpec::FieldType::Number;
	datasetSpec.schema["response"] = FlowPlot::Spec::DatasetSpec::FieldType::Number;
	spec.datasets.push_back(std::move(datasetSpec));

	FlowPlot::Spec::LayerSpec histogramLayer{};
	histogramLayer.id = "layer_1";
	histogramLayer.type = "histogram";
	histogramLayer.dataset = "resonance";
	histogramLayer.histogramMapping.dataField = "response";
	histogramLayer.histogramMapping.axis = FlowPlot::Spec::HistogramDataAxis::X;
	histogramLayer.histogramStyle.fillColor = "#6fb6ffb4";
	histogramLayer.histogramStyle.strokeColor = "#235d9f";
	histogramLayer.histogramConfig.binCount = 12;
	histogramLayer.histogramConfig.showEmptyBins = true;
	histogramLayer.histogramStats.showMeanLine = true;
	histogramLayer.histogramStats.showMedianLine = true;

	FlowPlot::Spec::PanelSpec panel{};
	panel.id = "panel_1";
	panel.xAxis.title.text = "Response";
	panel.yAxis.title.text = "Count";
	panel.layers.clear();
	panel.layers.push_back(std::move(histogramLayer));
	spec.panels.clear();
	spec.panels.push_back(std::move(panel));

	const std::vector<double> frequencies{
		80.0, 90.0, 100.0, 110.0, 120.0, 130.0, 140.0, 150.0,
		160.0, 170.0, 180.0, 190.0, 200.0, 210.0, 220.0, 230.0,
	};
	const std::vector<double> responses{
		0.12, 0.18, 0.27, 0.42, 0.68, 0.91, 1.08, 1.0,
		0.82, 0.61, 0.45, 0.31, 0.24, 0.19, 0.15, 0.11,
	};

	FlowPlotGui::numericColumn frequencyColumn{};
	frequencyColumn.name = "frequency_hz";
	frequencyColumn.data = frequencies;

	FlowPlotGui::numericColumn responseColumn{};
	responseColumn.name = "response";
	responseColumn.data = responses;

	FlowPlotGui::RunningDataset runningDataset{};
	runningDataset.name = "resonance";
	runningDataset.numericColumns.push_back(std::move(frequencyColumn));
	runningDataset.numericColumns.push_back(std::move(responseColumn));
	preset.datasets.push_back(std::move(runningDataset));

	return preset;
}

inline std::vector<std::string> templatePresetNames()
{
	return {
		"Scatter Plot",
		"Histogram chart",
	};
}

inline TemplatePreset makeTemplatePreset(std::string_view name)
{
	if (name == "Histogram chart")
	{
		return makeHistogramChartPreset();
	}
	return makeScatterPlotPreset();
}

inline void replaceWithTemplatePreset(state& guiState, TemplatePreset preset, bool markChanged = true)
{
	guiState.activeTemplate = std::move(preset.activeTemplate);
	guiState.datasets = std::move(preset.datasets);
	guiState.selectedNode.reset();

	if (markChanged)
	{
		++guiState.templateRevision;
		++guiState.datasetRevision;
		markViewportChanged(guiState);
		resetDocumentSession(guiState);
	}
}

} // namespace FlowPlotGui
