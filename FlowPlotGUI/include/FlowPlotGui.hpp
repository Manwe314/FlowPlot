#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "FlowPlot_Defaults.hpp"

namespace FlowPlotGui {
inline constexpr std::size_t kInitialDatasetCapacity = 3;
inline constexpr std::size_t kInitialColumnsPerTypeCapacity = 4;
inline constexpr std::size_t kInitialRowsCapacity = 8;
inline constexpr std::size_t kMaxDatasetCount = 10;

enum class GlobalState : uint8_t
{
	working,
	exporting,
};

enum class TemplateNodeKind : uint8_t {
	Figure,
	FigureTitle,
	PanelsGroup,
	Panel,
	PanelTitle,
	XAxis,
	YAxis,
	XSecondaryAxis,
	YSecondaryAxis,
	AxisTitle,
	LayersGroup,
	Layer,
	LegendsGroup,
	Legend,
	LegendElement,
};

struct TemplateNodeKey {
	TemplateNodeKind kind = TemplateNodeKind::Figure;
	std::string outer{};
	std::string inner{};
	std::string flowElementId{};
};

struct numericColumn {
	numericColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<double> data;
	std::string name;
};

struct stringColumn {
	stringColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<std::string> data;
	std::string name;
};

struct boolColumn {
	boolColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<bool> data;
	std::string name;
};

struct RunningDataset {
	RunningDataset()
	{
		numericColumns.reserve(kInitialColumnsPerTypeCapacity);
		stringColumns.reserve(kInitialColumnsPerTypeCapacity);
		boolColumns.reserve(kInitialColumnsPerTypeCapacity);
	}

	std::vector<numericColumn> numericColumns;
	std::vector<stringColumn> stringColumns;
	std::vector<boolColumn> boolColumns; 

	std::string name;
};

struct state {
	state()
	{
		activeTemplate.datasets.reserve(kInitialDatasetCapacity);
		datasets.reserve(kInitialDatasetCapacity);
	}

	GlobalState globalAppState = GlobalState::working;
	FlowPlot::Spec::MasterTemplateSpec activeTemplate{};
	std::optional<TemplateNodeKey> selectedNode{};
	std::vector<RunningDataset> datasets;
};



} // namespace FlowPlotGui
