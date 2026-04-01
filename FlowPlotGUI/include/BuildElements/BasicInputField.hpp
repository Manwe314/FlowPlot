#pragma once

#include <FlowUi/Flow.hpp>

#include "FlowPlotGui.hpp"

struct basicInputFieldParams {
	std::string defaultText = "...";
	Clay_Padding padding = CLAY_PADDING_ALL(10);
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(30, 90), .height = CLAY_SIZING_FIT(0)};
	Clay_Color borderColor = FlowUi::Flow_Color("#8f8d8dff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#cfcfcfff");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(6);
	Clay_ChildAlignment childTextAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t fontId = 0;
	uint16_t fontSize = 16;
	Clay_Color textColor = FlowUi::Flow_Color("#000000ff");
};

using BasicInputFieldDef = FlowUi::ElementDefinition<basicInputFieldParams, void, void, FLOW_DEF_ID("Basic input field")>;

inline const BasicInputFieldDef kBasicInputField = {
	+[](BasicInputFieldDef::InteractionContext& context) {
		(void)context;
	},
	+[](BasicInputFieldDef::InteractionContext& context) {
		context.uiManager.inputFields().requestCaret(
			context.elementID,
			FlowUi::InputFieldManager::CaretRequestKind::SetPrimary);
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BasicInputFieldDef::BuildContext& context) {
		const Clay_ElementId contentId = context.uiManager.toClayEID(context.elementID);
		const std::string textElementPath = context.createChildElementId("text");
		const Clay_ElementId textId = context.uiManager.toClayEID(textElementPath);

		auto result = context.uiManager.inputFields().requestField({
			.fieldId = context.elementID,
			.initialText = context.params.defaultText,
			.config = FlowUi::InputFieldManager::FieldConfig{
				.readOnly = false,
				.allowNewline = false,
				.allowArrowNavigation = true,
			},
			.textElementId = textId,
			.contentElementId = contentId,
		});
		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childTextAlignment;

		Clay_ElementDeclaration root{};
		root.id = contentId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.clip = {.horizontal = true, .vertical = true};
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = context.params.textWrapMode;
		textConfig.textAlignment = context.params.textAlignment;
		textConfig.fontId = context.params.fontId;

		CLAY(root){
			CLAY({.id = textId}){
				CLAY_TEXT(
					context.uiManager.toClayString(result.text),
					CLAY_TEXT_CONFIG(textConfig)
				);
			};
		};
	},
};
