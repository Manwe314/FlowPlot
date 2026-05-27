#pragma once

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"

struct propsPanelParams {
	int minWidth = 436;
	int maxWidth = 520;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	FlowPlotGui::state* guiState = nullptr;
};

struct propsPanelState {
	int minWidth = 480;
};

FLOWUI_DEV_REGISTER_STRUCT(
	propsPanelParams,
	FLOWUI_DEV_REFLECT_FIELD(propsPanelParams, minWidth),
	FLOWUI_DEV_REFLECT_FIELD(propsPanelParams, maxWidth),
	FLOWUI_DEV_REFLECT_FIELD(propsPanelParams, backgroundColor));

FLOWUI_DEV_REGISTER_STRUCT(
	propsPanelState,
	FLOWUI_DEV_REFLECT_FIELD(propsPanelState, minWidth));

using PropsPanelDef = FlowUi::ElementDefinition<
	propsPanelParams,
	propsPanelState,
	void,
	FLOW_DEF_ID("PropsPanel")>;

inline const PropsPanelDef kPropsPanel = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](PropsPanelDef::InteractionContext& context) {
		(void)PropsPanelDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
	},
	+[](PropsPanelDef::BuildContext& context) -> Clay_ElementDeclaration {
		propsPanelState& state = PropsPanelDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		int width = state.minWidth;
		int widthMin = context.params.minWidth;
		int widthMax = context.params.maxWidth;
		if (widthMin < 0)
		{
			widthMin = 0;
		}
		if (widthMax < widthMin)
		{
			widthMax = widthMin;
		}
		if (width < widthMin)
		{
			width = widthMin;
		}
		else if (width > widthMax)
		{
			width = widthMax;
		}
		state.minWidth = width;

		Clay_LayoutConfig layout{};
		layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		layout.childGap = 0;
		layout.padding = CLAY_PADDING_ALL(0);
		layout.sizing = {
			.width = CLAY_SIZING_FIXED(static_cast<float>(width)),
			.height = CLAY_SIZING_GROW(0, 100000),
		};
		layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};

		Clay_ElementDeclaration root{};
		root.layout = layout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
		return root;
	},
	nullptr,
};
