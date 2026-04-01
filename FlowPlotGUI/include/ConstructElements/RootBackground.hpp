#pragma once

#include <FlowUi/Flow.hpp>

#include "FlowPlotGui.hpp"

struct rootBackgroundParams {
	Clay_Color backgroundColor = FlowUi::Flow_Color("#1a1a1aff");
};

using RootBackgroundDef = FlowUi::ElementDefinition<rootBackgroundParams, void, void, FLOW_DEF_ID("RootBackground")>;

inline const RootBackgroundDef kRootBackground = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](RootBackgroundDef::BuildContext& context) -> Clay_ElementDeclaration {
		Clay_LayoutConfig layout{};
		layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		layout.childGap = 0;
		layout.sizing = {
			.width = CLAY_SIZING_GROW(0, 100000),
			.height = CLAY_SIZING_GROW(0, 100000),
		};

		Clay_ElementDeclaration root{};
		root.id = context.uiManager.toClayEID(context.elementID);
		root.layout = layout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
		return root;
	},
	nullptr,
};
