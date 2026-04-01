#pragma once

#include <FlowUi/Flow.hpp>

#include "FlowPlotGui.hpp"

struct dataInputParams {
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#7a7a7aff");
};

using DataInputDef = FlowUi::ElementDefinition<dataInputParams, void, void, FLOW_DEF_ID("DataInput")>;

inline const DataInputDef kDataInput = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DataInputDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);

		Clay_LayoutConfig rootLayout{};
		rootLayout.sizing = context.params.sizing;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(root){};
	},
};
