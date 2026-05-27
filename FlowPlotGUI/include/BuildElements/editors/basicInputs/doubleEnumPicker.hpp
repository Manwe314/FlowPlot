#pragma once

#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "FlowPlotGui.hpp"

struct doubleEnumPickerCardParams {
	std::string hintText = "Options";

	std::string firstHintText = "First";
	std::span<const std::string> firstOptions{};
	std::string firstValue = "";
	std::string firstDefaultValue = "";
	std::function<void(std::string_view)> onFirstChange = nullptr;

	std::string secondHintText = "Second";
	std::span<const std::string> secondOptions{};
	std::string secondValue = "";
	std::string secondDefaultValue = "";
	std::function<void(std::string_view)> onSecondChange = nullptr;

	std::function<void(std::string_view, std::string_view)> onChange = nullptr;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};
	uint16_t cardChildGap = 6;
	Clay_Color cardBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color cardBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth cardBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	Clay_Sizing pickerRowSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding pickerRowPadding = CLAY_PADDING_ALL(0);
	uint16_t pickerRowChildGap = 0;
	Clay_Color pickerRowBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color pickerRowBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth pickerRowBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	Clay_Sizing firstPickerSizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Sizing secondPickerSizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	uint16_t pickerCardChildGap = 8;

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	enumPickerCardParams firstPicker{};
	enumPickerCardParams secondPicker{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	doubleEnumPickerCardParams,
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, firstHintText),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, firstValue),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, firstDefaultValue),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, secondHintText),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, secondValue),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, secondDefaultValue),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, pickerRowSizing),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, pickerRowPadding),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, pickerRowChildGap),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, pickerRowBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, pickerRowBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, pickerRowBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, firstPickerSizing),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, secondPickerSizing),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, pickerCardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, firstPicker),
	FLOWUI_DEV_REFLECT_FIELD(doubleEnumPickerCardParams, secondPicker));

using DoubleEnumPickerCardDef = FlowUi::ElementDefinition<
	doubleEnumPickerCardParams,
	void,
	void,
	FLOW_DEF_ID("DoubleEnumPickerCard")>;

inline const DoubleEnumPickerCardDef kDoubleEnumPickerCard = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DoubleEnumPickerCardDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("hint-text"));
		const Clay_ElementId pickerRowId = context.uiManager.toClayEID(context.createChildElementId("picker-row"));
		const Clay_ElementId leadingSpacerId = context.uiManager.toClayEID(context.createChildElementId("picker-row/leading-spacer"));
		const Clay_ElementId middleSpacerId = context.uiManager.toClayEID(context.createChildElementId("picker-row/middle-spacer"));
		const Clay_ElementId trailingSpacerId = context.uiManager.toClayEID(context.createChildElementId("picker-row/trailing-spacer"));
		const Clay_ElementId firstPickerContainerId = context.uiManager.toClayEID(context.createChildElementId("picker-row/first-picker-container"));
		const Clay_ElementId secondPickerContainerId = context.uiManager.toClayEID(context.createChildElementId("picker-row/second-picker-container"));
		const std::string firstPickerPath = context.createChildElementId("picker-row/first-picker-container/first-picker");
		const std::string secondPickerPath = context.createChildElementId("picker-row/second-picker-container/second-picker");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		root.layout.sizing = context.params.cardSizing;
		root.layout.padding = context.params.cardPadding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		root.layout.childGap = context.params.cardChildGap;
		root.backgroundColor = context.params.cardBackgroundColor;
		root.border = {.color = context.params.cardBorderColor, .width = context.params.cardBorderWidth};

		Clay_TextElementConfig hintTextConfig{};
		hintTextConfig.textColor = context.params.textColor;
		hintTextConfig.fontSize = context.params.fontSize;
		hintTextConfig.wrapMode = context.params.textWrapMode;
		hintTextConfig.textAlignment = context.params.textAlignment;
		hintTextConfig.fontId = context.params.fontId;

		Clay_ElementDeclaration pickerRow{};
		pickerRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		pickerRow.layout.sizing = context.params.pickerRowSizing;
		pickerRow.layout.padding = context.params.pickerRowPadding;
		pickerRow.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		pickerRow.layout.childGap = context.params.pickerRowChildGap;
		pickerRow.backgroundColor = context.params.pickerRowBackgroundColor;
		pickerRow.border = {.color = context.params.pickerRowBorderColor, .width = context.params.pickerRowBorderWidth};

		Clay_ElementDeclaration rowSpacer{};
		rowSpacer.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(0)};
		rowSpacer.backgroundColor = FlowUi::Flow_Color("#00000000");

		Clay_ElementDeclaration pickerContainer{};
		pickerContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		pickerContainer.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
		pickerContainer.layout.padding = CLAY_PADDING_ALL(0);
		pickerContainer.layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		pickerContainer.backgroundColor = FlowUi::Flow_Color("#00000000");
		pickerContainer.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		enumPickerCardParams firstPickerParams = context.params.firstPicker;
		firstPickerParams.hintText = context.params.firstHintText;
		firstPickerParams.options = context.params.firstOptions;
		firstPickerParams.value = context.params.firstValue;
		firstPickerParams.defaultValue = context.params.firstDefaultValue;
		firstPickerParams.cardSizing = context.params.firstPickerSizing;
		firstPickerParams.cardChildGap = context.params.pickerCardChildGap;
		firstPickerParams.fontId = context.params.fontId;
		firstPickerParams.fontSize = context.params.fontSize;
		firstPickerParams.textColor = context.params.textColor;
		firstPickerParams.textWrapMode = context.params.textWrapMode;
		firstPickerParams.textAlignment = context.params.textAlignment;
		firstPickerParams.onChange = [
			onFirstChange = context.params.onFirstChange,
			onChange = context.params.onChange,
			secondValue = context.params.secondValue
		](std::string_view value) {
			if (onFirstChange != nullptr)
			{
				onFirstChange(value);
			}
			if (onChange != nullptr)
			{
				onChange(value, secondValue);
			}
		};

		enumPickerCardParams secondPickerParams = context.params.secondPicker;
		secondPickerParams.hintText = context.params.secondHintText;
		secondPickerParams.options = context.params.secondOptions;
		secondPickerParams.value = context.params.secondValue;
		secondPickerParams.defaultValue = context.params.secondDefaultValue;
		secondPickerParams.cardSizing = context.params.secondPickerSizing;
		secondPickerParams.cardChildGap = context.params.pickerCardChildGap;
		secondPickerParams.fontId = context.params.fontId;
		secondPickerParams.fontSize = context.params.fontSize;
		secondPickerParams.textColor = context.params.textColor;
		secondPickerParams.textWrapMode = context.params.textWrapMode;
		secondPickerParams.textAlignment = context.params.textAlignment;
		secondPickerParams.onChange = [
			onSecondChange = context.params.onSecondChange,
			onChange = context.params.onChange,
			firstValue = context.params.firstValue
		](std::string_view value) {
			if (onSecondChange != nullptr)
			{
				onSecondChange(value);
			}
			if (onChange != nullptr)
			{
				onChange(firstValue, value);
			}
		};

		CLAY(rootId, root)
		{
			CLAY(hintId, {})
			{
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.hintText),
					CLAY_TEXT_CONFIG(hintTextConfig)
				);
			};

			CLAY(pickerRowId, pickerRow)
			{
				CLAY(leadingSpacerId, rowSpacer){};
				CLAY(firstPickerContainerId, pickerContainer)
				{
					context.uiManager.createElement(kEnumPickerCard, firstPickerPath)
						.setParameters(std::move(firstPickerParams))
						.draw();
				};
				CLAY(middleSpacerId, rowSpacer){};
				CLAY(secondPickerContainerId, pickerContainer)
				{
					context.uiManager.createElement(kEnumPickerCard, secondPickerPath)
						.setParameters(std::move(secondPickerParams))
						.draw();
				};
				CLAY(trailingSpacerId, rowSpacer){};
			};
		};
	},
};
