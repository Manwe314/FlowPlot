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

struct mainContentPanelParams {
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
};

using MainContentPanelDef = FlowUi::ElementDefinition<mainContentPanelParams, void, void, FLOW_DEF_ID("MainContentPanel")>;

inline const MainContentPanelDef kMainContentPanel = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](MainContentPanelDef::BuildContext& context) -> Clay_ElementDeclaration {
		Clay_LayoutConfig layout{};
		layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
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

struct templatePanelParams {
	int maxWidth = 520;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
};

struct templatePanelState {
	int minWidth = 220;
};

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
		int widthMin = state.minWidth;
		int widthMax = context.params.maxWidth;
		if (widthMax < widthMin)
		{
			widthMax = widthMin;
		}

		Clay_LayoutConfig layout{};
		layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		layout.childGap = 0;
		layout.padding = CLAY_PADDING_ALL(0);
		layout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(widthMin), static_cast<float>(widthMax)),
			.height = CLAY_SIZING_GROW(0, 100000),
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

struct propsPanelParams {
	int maxWidth = 520;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
};

struct propsPanelState {
	int minWidth = 220;
};

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
		int widthMin = state.minWidth;
		int widthMax = context.params.maxWidth;
		if (widthMax < widthMin)
		{
			widthMax = widthMin;
		}

		Clay_LayoutConfig layout{};
		layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		layout.childGap = 0;
		layout.padding = CLAY_PADDING_ALL(0);
		layout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(widthMin), static_cast<float>(widthMax)),
			.height = CLAY_SIZING_GROW(0, 100000),
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
