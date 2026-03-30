#pragma once

#include <FlowUi/Flow.hpp>
#include "FlowPlotGui.hpp"


inline const FlowUi::ElementDefinition kRootBackground = {
	// elementTypeName: unique element type key (used for registration/lookups).
	"RootBackground",

	// initializeDefaultParameters: optional defaults merged with per-instance overrides.
	[](FlowUi::ElementParameters& defaults) {
		defaults.setValue("background color", FlowUi::Flow_Color("#1a1a1aff"));
	},

	// onHovered: optional callback when this element was hovered in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onPressed: optional callback when this element was pressed in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onHeld: optional callback when this element was held in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onReleased: optional callback when this element was released in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// runLogic: optional per-frame logic callback before buildElement executes.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// constructElment: this is the primary path for this element.
	[](FlowUi::ElementBuildContext& context) -> Clay_ElementDeclaration {
		Clay_LayoutConfig layout{};
		layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		layout.childGap = 0;
		layout.sizing = {
			.width = CLAY_SIZING_GROW(0, 100000),
			.height = CLAY_SIZING_GROW(0, 100000),
		};

		Clay_ElementDeclaration root{};
		root.id = context.elementId;
		root.layout = layout;
		root.backgroundColor = context.parameters.getValue<Clay_Color>("background color");
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};
		return root;
	},

	// buildElement: intentionally empty; use construct() for this element.
	[](FlowUi::ElementBuildContext& context) {
		(void)context;
	},
};

inline const FlowUi::ElementDefinition kMainContent = {
	// elementTypeName: unique element type key (used for registration/lookups).
	"MainContent",

	// initializeDefaultParameters: optional defaults merged with per-instance overrides.
	[](FlowUi::ElementParameters& defaults) {
		defaults.setValue("background color", FlowUi::Flow_Color("#00000000"));
	},

	// onHovered: optional callback when this element was hovered in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onPressed: optional callback when this element was pressed in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onHeld: optional callback when this element was held in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onReleased: optional callback when this element was released in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// runLogic: optional per-frame logic callback before buildElement executes.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// constructElment: this is the primary path for this element.
	[](FlowUi::ElementBuildContext& context) -> Clay_ElementDeclaration {
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
		root.id = context.elementId;
		root.layout = layout;
		root.backgroundColor = context.parameters.getValue<Clay_Color>("background color");
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};
		return root;
	},

	// buildElement: intentionally empty; use construct() for this element.
	[](FlowUi::ElementBuildContext& context) {
		(void)context;
	},
};

inline const FlowUi::ElementDefinition kTemplatePanel = {
	// elementTypeName: unique element type key (used for registration/lookups).
	"TemplatePanel",

	// initializeDefaultParameters: optional defaults merged with per-instance overrides.
	[](FlowUi::ElementParameters& defaults) {
		defaults.setValue("width min", 220);
		defaults.setValue("width max", 520);
		defaults.setValue("background color", FlowUi::Flow_Color("#00000000"));
	},

	// onHovered: optional callback when this element was hovered in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onPressed: optional callback when this element was pressed in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onHeld: optional callback when this element was held in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// onReleased: optional callback when this element was released in the previous frame.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// runLogic: optional per-frame logic callback before buildElement executes.
	[](FlowUi::ElementInteractionContext& context) {
		(void)context;
	},

	// constructElment: this is the primary path for this element.
	[](FlowUi::ElementBuildContext& context) -> Clay_ElementDeclaration {
		int widthMin = context.parameters.getValue<int>("width min", 0);
		int widthMax = context.parameters.getValue<int>("width max", widthMin);
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
		root.id = context.elementId;
		root.layout = layout;
		root.backgroundColor = context.parameters.getValue<Clay_Color>("background color");
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0,0,0,0,0}};
		return root;
	},

	// buildElement: intentionally empty; use construct() for this element.
	[](FlowUi::ElementBuildContext& context) {
		(void)context;
	},
};
