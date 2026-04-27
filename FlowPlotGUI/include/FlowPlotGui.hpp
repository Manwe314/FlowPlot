#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "FlowPlot_Defaults.hpp"

namespace FlowPlotGui {
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

struct state {
	GlobalState globalAppState = GlobalState::working;
	FlowPlot::Spec::MasterTemplateSpec activeTemplate{};
	std::optional<TemplateNodeKey> selectedNode{};
};

} // namespace FlowPlotGui
