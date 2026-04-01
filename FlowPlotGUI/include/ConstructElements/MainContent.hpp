#pragma once

#include <FlowUi/Flow.hpp>

#include "FlowPlotGui.hpp"

struct mainContentParams {
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
};

using MainContentDef = FlowUi::ElementDefinition<mainContentParams, void, void, FLOW_DEF_ID("MainContent")>;

inline const MainContentDef kMainContent = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](MainContentDef::BuildContext& context) -> Clay_ElementDeclaration {
		Clay_LayoutConfig layout{};
		layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		layout.childGap = 0;
		layout.padding = CLAY_PADDING_ALL(0);
		layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_GROW(0),
		};
		layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};

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
