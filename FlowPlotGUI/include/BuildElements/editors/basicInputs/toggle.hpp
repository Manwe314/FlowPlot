#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"

struct toggleSwitchParams {
	bool value = false;
	float width = 44.0f;
	Clay_Color enabledColor = FlowUi::Flow_Color("#3fbf77ff");
	Clay_Color disabledColor = FlowUi::Flow_Color("#5e646eff");
	Clay_Color nobColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_Padding togglePadding = CLAY_PADDING_ALL(3);
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color borderColor = FlowUi::Flow_Color("#1f2329ff");
	float aspectRatio = 1.8f;

	std::function<void(bool)> onChange = nullptr;
};

FLOWUI_DEV_REGISTER_STRUCT(
	toggleSwitchParams,
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, value),
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, width),
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, enabledColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, disabledColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, nobColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, togglePadding),
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchParams, aspectRatio));

struct toggleSwitchState {
	bool isPressed = false;
};

FLOWUI_DEV_REGISTER_STRUCT(
	toggleSwitchState,
	FLOWUI_DEV_REFLECT_FIELD(toggleSwitchState, isPressed));

using ToggleSwitchDef = FlowUi::ElementDefinition<
	toggleSwitchParams,
	toggleSwitchState,
	void,
	FLOW_DEF_ID("ToggleSwitch")>;
using ToggleSwitchInteractionContext = ToggleSwitchDef::InteractionContext;

inline const ToggleSwitchDef kToggleSwitch = {
	nullptr,
	+[](ToggleSwitchDef::InteractionContext& context) {
		toggleSwitchState& state = ToggleSwitchDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		state.isPressed = true;
		if (context.params.onChange != nullptr)
		{
			context.params.onChange(!context.params.value);
		}
	},
	nullptr,
	nullptr,
	+[](ToggleSwitchDef::InteractionContext& context) {
		toggleSwitchState& state = ToggleSwitchDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const FlowUi::FrameInput& input = context.uiManager.getCurrentFrameInput();
		if (!input.mouseDown[0])
		{
			state.isPressed = false;
		}
	},
	nullptr,
	+[](ToggleSwitchDef::BuildContext& context) {
		(void)ToggleSwitchDef::getOrCreateState(FlowUi::toFlowId(context.elementID));

		const float width = std::max(std::round(context.params.width), 1.0f);
		const float aspectRatio = std::max(context.params.aspectRatio, 0.001f);
		const float height = std::max(std::round(width / aspectRatio), 1.0f);
		const float knobDiameter = std::max(
			std::round(height - static_cast<float>(context.params.togglePadding.top + context.params.togglePadding.bottom)),
			1.0f);

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId knobId = context.uiManager.toClayEID(context.createChildElementId("nob"));
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = {
			.width = CLAY_SIZING_FIXED(width),
			.height = CLAY_SIZING_FIXED(height),
		};
		root.layout.padding = context.params.togglePadding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.layout.childGap = 0;
		root.backgroundColor = context.params.value ? context.params.enabledColor : context.params.disabledColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(height * 0.5f);
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_ElementDeclaration knob{};
		knob.layout.sizing = {
			.width = CLAY_SIZING_FIXED(knobDiameter),
			.height = CLAY_SIZING_FIXED(knobDiameter),
		};
		knob.backgroundColor = context.params.nobColor;
		knob.cornerRadius = CLAY_CORNER_RADIUS(knobDiameter * 0.5f);

		Clay_ElementDeclaration spacer{};
		spacer.layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		CLAY(rootId, root)
		{
			if (context.params.value)
			{
				CLAY(spacerId, spacer){};
				CLAY(knobId, knob){};
			}
			else
			{
				CLAY(knobId, knob){};
				CLAY(spacerId, spacer){};
			}
		};
	},
};

struct toggleCardParams {
	std::string hintText = "Toggle";
	bool value = false;
	float width = 44.0f;
	Clay_Color enabledColor = FlowUi::Flow_Color("#3fbf77ff");
	Clay_Color disabledColor = FlowUi::Flow_Color("#5e646eff");
	Clay_Color nobColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_Padding togglePadding = CLAY_PADDING_ALL(3);
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color borderColor = FlowUi::Flow_Color("#1f2329ff");
	float aspectRatio = 1.8f;
	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_LayoutDirection cardLayout = CLAY_LEFT_TO_RIGHT;
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	std::function<void(bool)> onChange = nullptr;
};

FLOWUI_DEV_REGISTER_STRUCT(
	toggleCardParams,
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, value),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, width),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, enabledColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, disabledColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, nobColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, togglePadding),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, aspectRatio),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, cardLayout),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(toggleCardParams, textAlignment));

using ToggleCardDef = FlowUi::ElementDefinition<
	toggleCardParams,
	void,
	void,
	FLOW_DEF_ID("ToggleCard")>;

inline const ToggleCardDef kToggleCard = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ToggleCardDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId textId = context.uiManager.toClayEID(context.createChildElementId("hint-text"));
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));
		const std::string togglePath = context.createChildElementId("toggle-switch");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = context.params.cardLayout;
		root.layout.sizing = context.params.cardSizing;
		root.layout.padding = context.params.cardPadding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		root.layout.childGap = 0;
		root.backgroundColor = FlowUi::Flow_Color("#00000000");
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = context.params.textWrapMode;
		textConfig.textAlignment = context.params.textAlignment;
		textConfig.fontId = context.params.fontId;

		Clay_ElementDeclaration spacer{};
		spacer.layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		toggleSwitchParams switchParams{};
		switchParams.value = context.params.value;
		switchParams.width = context.params.width;
		switchParams.enabledColor = context.params.enabledColor;
		switchParams.disabledColor = context.params.disabledColor;
		switchParams.nobColor = context.params.nobColor;
		switchParams.togglePadding = context.params.togglePadding;
		switchParams.borderWidth = context.params.borderWidth;
		switchParams.borderColor = context.params.borderColor;
		switchParams.aspectRatio = context.params.aspectRatio;
		switchParams.onChange = context.params.onChange;

		CLAY(rootId, root)
		{
			CLAY(textId, {})
			{
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.hintText),
					CLAY_TEXT_CONFIG(textConfig)
				);
			};
			CLAY(spacerId, spacer){};
			context.uiManager.createElement(kToggleSwitch, togglePath)
				.setParameters(std::move(switchParams))
				.draw();
		};
	},
};
