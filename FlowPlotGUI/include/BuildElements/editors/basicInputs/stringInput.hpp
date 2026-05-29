#pragma once

#include <functional>
#include <string>
#include <string_view>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicInputField.hpp"
#include "FlowPlotGui.hpp"

struct stringInputCardParams {
	std::string hintText = "Text";
	std::string value = "";
	std::function<void(std::string_view)> onChange = nullptr;
	std::function<void()> onEditBegin = nullptr;
	std::function<void()> onEditEnd = nullptr;
	FlowPlotGui::state* propertyFocusState = nullptr;
	std::string propertyScrollContainerId{};
	std::size_t propertyTabOrder = 0;
	bool propertyTabStop = false;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};
	Clay_LayoutDirection cardLayout = CLAY_TOP_TO_BOTTOM;
	uint16_t cardChildGap = 6;

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	Clay_Padding inputPadding = Clay_Padding{8, 8, 6, 6};
	Clay_Color inputBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth inputBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color inputBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_CornerRadius inputCornerRadius = CLAY_CORNER_RADIUS(5);
	Clay_ChildAlignment inputTextAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t inputFontSize = 14;
	Clay_Color inputTextColor = FlowUi::Flow_Color("#f4f6f8ff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	stringInputCardParams,
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, value),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, cardLayout),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, inputPadding),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, inputBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, inputBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, inputBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, inputCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, inputTextAlignment),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, inputFontSize),
	FLOWUI_DEV_REFLECT_FIELD(stringInputCardParams, inputTextColor));

using StringInputCardDef = FlowUi::ElementDefinition<
	stringInputCardParams,
	void,
	void,
	FLOW_DEF_ID("StringInputCard")>;

inline const StringInputCardDef kStringInputCard = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](StringInputCardDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("hint-text"));
		const std::string inputPath = context.createChildElementId("input");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = context.params.cardLayout;
		root.layout.sizing = context.params.cardSizing;
		root.layout.padding = context.params.cardPadding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		root.layout.childGap = context.params.cardChildGap;
		root.backgroundColor = FlowUi::Flow_Color("#00000000");
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_TextElementConfig hintTextConfig{};
		hintTextConfig.textColor = context.params.textColor;
		hintTextConfig.fontSize = context.params.fontSize;
		hintTextConfig.wrapMode = context.params.textWrapMode;
		hintTextConfig.textAlignment = context.params.textAlignment;
		hintTextConfig.fontId = context.params.fontId;

		basicInputFieldParams inputParams{};
		inputParams.fieldId = inputPath;
		inputParams.value = context.params.value;
		inputParams.syncValueFromParams = true;
		inputParams.onTextChangedCallback = context.params.onChange;
		inputParams.onEditBegin = context.params.onEditBegin;
		inputParams.onEditEnd = context.params.onEditEnd;
		if (context.params.propertyTabStop && context.params.propertyFocusState != nullptr)
		{
			context.params.propertyFocusState->propertyInputFocusGrid.registerField(FlowPlotGui::PropertyInputFocus{
				.fieldId = inputPath,
				.elementId = inputPath,
				.scrollContainerId = context.params.propertyScrollContainerId,
				.order = context.params.propertyTabOrder,
			});
			FlowPlotGui::wirePropertyInputFocusCallbacks(
				*context.params.propertyFocusState,
				inputPath,
				inputParams.onEditBegin,
				inputParams.onEditEnd);
		}
		inputParams.padding = context.params.inputPadding;
		inputParams.sizing = Clay_Sizing{
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_FIT(0),
		};
		inputParams.borderColor = context.params.inputBorderColor;
		inputParams.borderWidth = context.params.inputBorderWidth;
		inputParams.backgroundColor = context.params.inputBackgroundColor;
		inputParams.cornerRadius = context.params.inputCornerRadius;
		inputParams.childTextAlignment = context.params.inputTextAlignment;
		inputParams.textWrapMode = CLAY_TEXT_WRAP_NONE;
		inputParams.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		inputParams.fontId = context.params.fontId;
		inputParams.fontSize = context.params.inputFontSize;
		inputParams.textColor = context.params.inputTextColor;

		CLAY(rootId, root)
		{
			CLAY(hintId, {})
			{
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.hintText),
					CLAY_TEXT_CONFIG(hintTextConfig)
				);
			};

			context.uiManager.createElement(kBasicInputField, inputPath)
				.setParameters(std::move(inputParams))
				.draw();
		};
	},
};
