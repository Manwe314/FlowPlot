#pragma once

#include <cstdint>
#include "FlowPlot_Defaults.hpp"

namespace FlowPlotGui {
enum class GlobalState : uint8_t
{
	working,
	exporting,
};

struct state {
	GlobalState globalAppState = GlobalState::working;
	FlowPlot::Spec::MasterTemplateSpec activeTemplate{};
	/*some sort o pointer visual handle*/
};

} // namespace FlowPlotGui