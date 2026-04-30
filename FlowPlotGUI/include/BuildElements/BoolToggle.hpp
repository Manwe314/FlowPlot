#pragma once

#include <functional>
#include <string>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

struct boolToggleParams;
using BoolToggleDef = FlowUi::ElementDefinition<boolToggleParams, struct boolToggleState, void, FLOW_DEF_ID("BoolToggle")>;
using BoolToggleInteractionContext = BoolToggleDef::InteractionContext;

struct boolToggleParams {
	bool value = false;
	std::function<void(bool)> onChange = nullptr;

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Padding padding = Clay_Padding{8, 8, 4, 4};
	Clay_Color falseColor = FlowUi::Flow_Color("#3f3f46ff");
	Clay_Color trueColor = FlowUi::Flow_Color("#0f766eff");
	Clay_Color hoverColor = FlowUi::Flow_Color("#52525bff");
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(5);
	uint16_t fontId = 0;
	uint16_t fontSize = 13;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f4f5ff");
};

struct boolToggleState {
	bool value = false;
};

FLOWUI_DEV_REGISTER_STRUCT(
	boolToggleParams,
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, value),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, falseColor),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, trueColor),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, hoverColor),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, cornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(boolToggleParams, textColor));

FLOWUI_DEV_REGISTER_STRUCT(
	boolToggleState,
	FLOWUI_DEV_REFLECT_FIELD(boolToggleState, value));

inline const BoolToggleDef kBoolToggle = {
	nullptr,
	+[](BoolToggleInteractionContext& context) {
		boolToggleState& state = BoolToggleDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		state.value = !state.value;
		if (context.params.onChange != nullptr)
		{
			context.params.onChange(state.value);
		}
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BoolToggleDef::BuildContext& context) {
		boolToggleState& state = BoolToggleDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		state.value = context.params.value;

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId textId = context.uiManager.toClayEID(context.createChildElementId("text"));
		const bool hovered = context.uiManager.getPreviousFramesInteraction().isHovered(rootId);

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = context.params.sizing;
		root.layout.padding = context.params.padding;
		root.layout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		root.backgroundColor = hovered ? context.params.hoverColor : (state.value ? context.params.trueColor : context.params.falseColor);
		root.cornerRadius = context.params.cornerRadius;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = CLAY_TEXT_WRAP_NONE;
		textConfig.textAlignment = CLAY_TEXT_ALIGN_CENTER;
		textConfig.fontId = context.params.fontId;

		CLAY(rootId, root){
			CLAY(textId, {}){
				CLAY_TEXT(context.uiManager.toClayString(state.value ? "True" : "False"), CLAY_TEXT_CONFIG(textConfig));
			};
		};
	},
};
