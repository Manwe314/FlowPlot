#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <string_view>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"

struct basicInputFieldParams {
	std::string fieldId = "";
	std::string defaultText = "...";
	std::string value = "";
	bool syncValueFromParams = false;
	std::function<void(std::string_view)> onTextChangedCallback = nullptr;

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

FLOWUI_DEV_REGISTER_STRUCT(
	basicInputFieldParams,
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, fieldId),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, defaultText),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, value),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, syncValueFromParams),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, cornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, childTextAlignment),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldParams, textColor));

struct basicInputFieldState {
	bool hasLastObservedText = false;
	bool hadPrimaryCaretLastFrame = false;
	std::string lastObservedText{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	basicInputFieldState,
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldState, hasLastObservedText),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldState, hadPrimaryCaretLastFrame),
	FLOWUI_DEV_REFLECT_FIELD(basicInputFieldState, lastObservedText));

using BasicInputFieldDef = FlowUi::ElementDefinition<
	basicInputFieldParams,
	basicInputFieldState,
	void,
	FLOW_DEF_ID("Basic input field")>;

inline const BasicInputFieldDef kBasicInputField = {
	+[](BasicInputFieldDef::InteractionContext& context) {
		(void)context;
	},
	+[](BasicInputFieldDef::InteractionContext& context) {
		const std::string_view fieldId =
			context.params.fieldId.empty()
			? context.elementID
			: std::string_view(context.params.fieldId);
		context.uiManager.inputFields().requestCaret(
			fieldId,
			FlowUi::CaretRequestKind::SetPrimary);
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BasicInputFieldDef::BuildContext& context) {
		basicInputFieldState& state = BasicInputFieldDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const std::string_view fieldIdView =
			context.params.fieldId.empty()
			? context.elementID
			: std::string_view(context.params.fieldId);
		const std::string fieldId(fieldIdView);
		const std::string_view requestedText =
			context.params.syncValueFromParams
			? std::string_view(context.params.value)
			: std::string_view(context.params.defaultText);

		const bool needsExternalValueSync =
			context.params.syncValueFromParams &&
			state.hasLastObservedText &&
			!state.hadPrimaryCaretLastFrame &&
			state.lastObservedText != requestedText;
		if (needsExternalValueSync)
		{
			(void)context.uiManager.inputFields().removeField(fieldId);
			state.hasLastObservedText = false;
			state.lastObservedText.clear();
		}

		const Clay_ElementId contentId = context.uiManager.toClayEID(context.elementID);
		const std::string textElementPath = context.createChildElementId("text");
		const Clay_ElementId textId = context.uiManager.toClayEID(textElementPath);

		const FlowUi::FieldQueryResult result =
			context.uiManager.inputFields().requestField({
			.fieldId = fieldId,
			.initialText = requestedText,
			.config = FlowUi::FieldConfig{
				.readOnly = false,
				.allowNewline = false,
				.allowArrowNavigation = true,
			},
			.textElementId = textId,
			.contentElementId = contentId,
		});

		const std::string resultText(result.text);
		if (!state.hasLastObservedText)
		{
			state.hasLastObservedText = true;
			state.lastObservedText = resultText;
		}
		else if (state.lastObservedText != resultText)
		{
			state.lastObservedText = resultText;
			if (context.params.onTextChangedCallback != nullptr)
			{
				context.params.onTextChangedCallback(resultText);
			}
		}
		state.hadPrimaryCaretLastFrame = result.hasPrimaryCaret;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.params.sizing;
		if (rootLayout.sizing.height.type == CLAY__SIZING_TYPE_FIT)
		{
			const float minTextHeight = static_cast<float>(context.params.fontSize)
				+ static_cast<float>(context.params.padding.top + context.params.padding.bottom)
				+ static_cast<float>(context.params.borderWidth.top + context.params.borderWidth.bottom);
			rootLayout.sizing.height = CLAY_SIZING_FIT(std::max(minTextHeight, 1.0f), 100000.0f);
		}
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childTextAlignment;

		Clay_ElementDeclaration root{};
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

		CLAY(contentId, root){
			CLAY(textId, {}){
				CLAY_TEXT(
					context.uiManager.toClayString(result.text),
					CLAY_TEXT_CONFIG(textConfig)
				);
			};
		};
	},
};
