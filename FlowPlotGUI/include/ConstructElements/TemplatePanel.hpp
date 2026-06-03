#pragma once

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"

struct templatePanelParams {
	int minWidth = 360;
	int maxWidth = 520;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
};

struct templatePanelState {
	int minWidth = 480;
};

FLOWUI_DEV_REGISTER_STRUCT(
	templatePanelParams,
	FLOWUI_DEV_REFLECT_FIELD(templatePanelParams, minWidth),
	FLOWUI_DEV_REFLECT_FIELD(templatePanelParams, maxWidth),
	FLOWUI_DEV_REFLECT_FIELD(templatePanelParams, backgroundColor));

FLOWUI_DEV_REGISTER_STRUCT(
	templatePanelState,
	FLOWUI_DEV_REFLECT_FIELD(templatePanelState, minWidth));

using TemplatePanelDef = FlowUi::ElementDefinition<
	templatePanelParams,
	templatePanelState,
	void,
	FLOW_DEF_ID("TemplatePanel")>;

inline const TemplatePanelDef kTemplatePanel = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](TemplatePanelDef::InteractionContext& context) {
		(void)TemplatePanelDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
	},
	+[](TemplatePanelDef::BuildContext& context) -> Clay_ElementDeclaration {
		templatePanelState& state = TemplatePanelDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
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
