#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicInputField.hpp"
#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "FlowPlotGui.hpp"

enum class twoColumnInputValueKind : std::uint8_t {
	String,
	Numeric,
	Enum,
	Color,
};

FLOWUI_DEV_REGISTER_ENUM(
	twoColumnInputValueKind,
	FLOWUI_DEV_ENUM_VALUE(twoColumnInputValueKind::String),
	FLOWUI_DEV_ENUM_VALUE(twoColumnInputValueKind::Numeric),
	FLOWUI_DEV_ENUM_VALUE(twoColumnInputValueKind::Enum),
	FLOWUI_DEV_ENUM_VALUE(twoColumnInputValueKind::Color));

inline std::string twoColumnInputSpanValue(std::span<const std::string> values, std::size_t index)
{
	if (index >= values.size())
	{
		return "";
	}
	return values[index];
}

inline Clay_Vector2 twoColumnInputScrollOffsetForElementId(
	FlowUi::UiManager& uiManager,
	std::string_view elementId)
{
	const Clay_ScrollContainerData data =
		Clay_GetScrollContainerData(uiManager.toClayEID(elementId));
	if (!data.found || data.scrollPosition == nullptr)
	{
		return Clay_Vector2{0.0f, 0.0f};
	}
	return *data.scrollPosition;
}

inline double twoColumnInputParseDoubleOrZero(std::string_view text)
{
	double parsed = 0.0;
	if (!numericInputTryParseDouble(text, parsed))
	{
		return 0.0;
	}
	return parsed;
}

struct twoColumnInputTableParams {
	std::span<const std::string> categories{};
	std::span<const std::string> values{};
	std::string defaultCategory = "";
	std::string defaultValue = "";
	std::function<void(std::string_view, std::string_view)> onAddRow = nullptr;
	std::function<void(std::size_t)> onDeleteRow = nullptr;
	std::function<void(std::size_t, std::string_view)> onCategoryChange = nullptr;
	std::function<void(std::size_t, std::string_view)> onValueChange = nullptr;

	twoColumnInputValueKind valueKind = twoColumnInputValueKind::String;
	std::span<const std::string> enumOptions{};

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	uint16_t childGap = 6;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	std::string categoryHeaderText = "Category";
	std::string valueHeaderText = "Value";
	Clay_Sizing headerSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding headerPadding = CLAY_PADDING_ALL(0);
	uint16_t headerChildGap = 8;
	Clay_Color headerTextColor = FlowUi::Flow_Color("#aeb2b8ff");

	Clay_Sizing rowsContainerSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0, 260)};
	Clay_Padding rowsContainerPadding = CLAY_PADDING_ALL(0);
	uint16_t rowsChildGap = 4;
	Clay_Color rowsBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color rowsBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth rowsBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	Clay_Sizing rowSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding rowPadding = CLAY_PADDING_ALL(0);
	uint16_t rowChildGap = 8;
	Clay_Color rowBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color rowHoverBackgroundColor = FlowUi::Flow_Color("#20252dff");

	Clay_Sizing firstColumnSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(180), .height = CLAY_SIZING_FIT(0)};
	Clay_Sizing secondColumnSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(180), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding cellPadding = CLAY_PADDING_ALL(0);

	Clay_Sizing deleteSlotSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(26), .height = CLAY_SIZING_FIXED(26)};
	Clay_Sizing deleteButtonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_FIXED(24)};
	Clay_Padding deleteButtonPadding = CLAY_PADDING_ALL(4);
	Clay_Color deleteButtonBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color deleteButtonHoverBackgroundColor = FlowUi::Flow_Color("#3a2529ff");
	Clay_Color deleteButtonIconTintColor = FlowUi::Flow_Color("#ffffffff");

	Clay_Sizing addButtonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(28), .height = CLAY_SIZING_FIXED(28)};
	Clay_Padding addButtonPadding = CLAY_PADDING_ALL(5);
	Clay_Color addButtonBackgroundColor = FlowUi::Flow_Color("#20252dff");
	Clay_Color addButtonHoverBackgroundColor = FlowUi::Flow_Color("#2c333dff");
	Clay_Color addButtonBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth addButtonBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_CornerRadius addButtonCornerRadius = CLAY_CORNER_RADIUS(5);

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color inputTextColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_Padding inputPadding = Clay_Padding{8, 8, 6, 6};
	Clay_Color inputBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth inputBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color inputBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_CornerRadius inputCornerRadius = CLAY_CORNER_RADIUS(5);

	numericInputFieldParams numericValue{};
	enumPickerMenuParams enumValue{};
	colorPickerSwatchParams colorValue{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	twoColumnInputTableParams,
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, defaultCategory),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, defaultValue),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, valueKind),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, categoryHeaderText),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, valueHeaderText),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, headerSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, headerPadding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, headerChildGap),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, headerTextColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowsContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowsContainerPadding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowsChildGap),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowsBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowsBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowsBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowPadding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowChildGap),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, rowHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, firstColumnSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, secondColumnSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, cellPadding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, deleteSlotSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, deleteButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, deleteButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, deleteButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, deleteButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, deleteButtonIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, addButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, addButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, addButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, addButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, addButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, addButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, addButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, inputTextColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, inputPadding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, inputBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, inputBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, inputBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, inputCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, numericValue),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, enumValue),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputTableParams, colorValue));

struct twoColumnInputTableResources {
	twoColumnInputTableResources() = default;
	explicit twoColumnInputTableResources(FlowUi::App& app) :
		plusIcon(app.icons().textureRef("Plus")),
		trashIcon(app.icons().textureRef("Trash"))
	{}

	FlowUi::TextureRef plusIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef trashIcon = FlowUi::TextureRef{};
};

using TwoColumnInputTableDef = FlowUi::ElementDefinition<
	twoColumnInputTableParams,
	void,
	twoColumnInputTableResources,
	FLOW_DEF_ID("TwoColumnInputTable")>;

inline basicInputFieldParams twoColumnInputBaseTextInputParams(
	std::string fieldId,
	std::string value,
	Clay_Sizing sizing,
	const twoColumnInputTableParams& params,
	std::function<void(std::string_view)> onChange)
{
	basicInputFieldParams input{};
	input.fieldId = std::move(fieldId);
	input.value = std::move(value);
	input.syncValueFromParams = true;
	input.onTextChangedCallback = std::move(onChange);
	input.padding = params.inputPadding;
	input.sizing = sizing;
	input.borderColor = params.inputBorderColor;
	input.borderWidth = params.inputBorderWidth;
	input.backgroundColor = params.inputBackgroundColor;
	input.cornerRadius = params.inputCornerRadius;
	input.childTextAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	input.textWrapMode = CLAY_TEXT_WRAP_NONE;
	input.textAlignment = CLAY_TEXT_ALIGN_LEFT;
	input.fontId = params.fontId;
	input.fontSize = params.fontSize;
	input.textColor = params.inputTextColor;
	return input;
}

inline void twoColumnInputDrawTextCell(
	TwoColumnInputTableDef::BuildContext& context,
	std::string_view cellName,
	Clay_Sizing sizing,
	std::function<void()> drawChild)
{
	const Clay_ElementId cellId = context.uiManager.toClayEID(context.createChildElementId(std::string(cellName)));
	Clay_ElementDeclaration cell{};
	cell.layout.sizing = sizing;
	cell.layout.padding = context.params.cellPadding;
	cell.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	CLAY(cellId, cell)
	{
		drawChild();
	};
}

inline void twoColumnInputDrawSecondColumnInput(
	TwoColumnInputTableDef::BuildContext& context,
	std::size_t rowIndex,
	std::string value)
{
	const std::string rowPrefix = "rows/row-" + std::to_string(rowIndex);
	switch (context.params.valueKind)
	{
	case twoColumnInputValueKind::String:
	{
		basicInputFieldParams inputParams = twoColumnInputBaseTextInputParams(
			context.createChildElementId(rowPrefix + "/value-input"),
			std::move(value),
			Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
			context.params,
			[rowIndex, callback = context.params.onValueChange](std::string_view changed) {
				if (callback != nullptr)
				{
					callback(rowIndex, changed);
				}
			});
		context.uiManager.createElement(kBasicInputField, context.createChildElementId(rowPrefix + "/value"))
			.setParameters(std::move(inputParams))
			.draw();
		break;
	}
	case twoColumnInputValueKind::Numeric:
	{
		numericInputFieldParams numericParams = context.params.numericValue;
		numericParams.value = twoColumnInputParseDoubleOrZero(value);
		numericParams.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
		numericParams.onChange = [
			rowIndex,
			valueType = numericParams.valueType,
			callback = context.params.onValueChange
		](double changed) {
			if (callback != nullptr)
			{
				callback(rowIndex, numericInputValueToText(valueType, changed));
			}
		};
		context.uiManager.createElement(kNumericInputField, context.createChildElementId(rowPrefix + "/numeric-value"))
			.setParameters(std::move(numericParams))
			.draw();
		break;
	}
	case twoColumnInputValueKind::Enum:
	{
		enumPickerMenuParams enumParams = context.params.enumValue;
		enumParams.options = context.params.enumOptions;
		enumParams.value = std::move(value);
		enumParams.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
		enumParams.onChange = [rowIndex, callback = context.params.onValueChange](std::string_view changed) {
			if (callback != nullptr)
			{
				callback(rowIndex, changed);
			}
		};
		context.uiManager.createElement(kEnumPickerMenu, context.createChildElementId(rowPrefix + "/enum-value"))
			.setParameters(std::move(enumParams))
			.draw();
		break;
	}
	case twoColumnInputValueKind::Color:
	{
		colorPickerSwatchParams colorParams = context.params.colorValue;
		colorParams.value = std::move(value);
		colorParams.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
		colorParams.onChange = [rowIndex, callback = context.params.onValueChange](std::string_view changed) {
			if (callback != nullptr)
			{
				callback(rowIndex, changed);
			}
		};
		context.uiManager.createElement(kColorPickerSwatch, context.createChildElementId(rowPrefix + "/color-value"))
			.setParameters(std::move(colorParams))
			.draw();
		break;
	}
	}
}

inline const TwoColumnInputTableDef kTwoColumnInputTable = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](TwoColumnInputTableDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId headerId = context.uiManager.toClayEID(context.createChildElementId("header"));
		const std::string rowsContainerPath = context.createChildElementId("rows-container");
		const Clay_ElementId rowsContainerId = context.uiManager.toClayEID(rowsContainerPath);
		const Clay_Vector2 rowsScrollOffset =
			twoColumnInputScrollOffsetForElementId(context.uiManager, rowsContainerPath);

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		root.layout.sizing = context.params.sizing;
		root.layout.padding = context.params.padding;
		root.layout.childGap = context.params.childGap;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		root.backgroundColor = context.params.backgroundColor;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_ElementDeclaration header{};
		header.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		header.layout.sizing = context.params.headerSizing;
		header.layout.padding = context.params.headerPadding;
		header.layout.childGap = context.params.headerChildGap;
		header.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

		Clay_TextElementConfig headerTextConfig{};
		headerTextConfig.textColor = context.params.headerTextColor;
		headerTextConfig.fontSize = context.params.fontSize;
		headerTextConfig.wrapMode = CLAY_TEXT_WRAP_NONE;
		headerTextConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		headerTextConfig.fontId = context.params.fontId;

		Clay_ElementDeclaration rowsContainer{};
		rowsContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		rowsContainer.layout.sizing = context.params.rowsContainerSizing;
		rowsContainer.layout.padding = context.params.rowsContainerPadding;
		rowsContainer.layout.childGap = context.params.rowsChildGap;
		rowsContainer.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		rowsContainer.backgroundColor = context.params.rowsBackgroundColor;
		rowsContainer.border = {.color = context.params.rowsBorderColor, .width = context.params.rowsBorderWidth};
		rowsContainer.clip = {.horizontal = false, .vertical = true, .childOffset = rowsScrollOffset};

		const std::size_t rowCount = std::max(context.params.categories.size(), context.params.values.size());

		CLAY(rootId, root)
		{
			CLAY(headerId, header)
			{
				twoColumnInputDrawTextCell(context, "header/category-cell", context.params.firstColumnSizing, [&]() {
					CLAY_TEXT(context.uiManager.toClayString(context.params.categoryHeaderText), CLAY_TEXT_CONFIG(headerTextConfig));
				});
				twoColumnInputDrawTextCell(context, "header/value-cell", context.params.secondColumnSizing, [&]() {
					CLAY_TEXT(context.uiManager.toClayString(context.params.valueHeaderText), CLAY_TEXT_CONFIG(headerTextConfig));
				});
				CLAY(context.uiManager.toClayEID(context.createChildElementId("header/delete-spacer")), {
					.layout = {.sizing = context.params.deleteSlotSizing},
				}){};
			};

			CLAY(rowsContainerId, rowsContainer)
			{
				for (std::size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
				{
					const std::string rowPath = context.createChildElementId("rows/row-" + std::to_string(rowIndex));
					const Clay_ElementId rowId = context.uiManager.toClayEID(rowPath);
					const bool rowHovered = context.uiManager.getPreviousFramesInteraction().isHovered(rowId);

					Clay_ElementDeclaration row{};
					row.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
					row.layout.sizing = context.params.rowSizing;
					row.layout.padding = context.params.rowPadding;
					row.layout.childGap = context.params.rowChildGap;
					row.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
					row.backgroundColor = rowHovered ? context.params.rowHoverBackgroundColor : context.params.rowBackgroundColor;

					CLAY(rowId, row)
					{
						const std::string category = twoColumnInputSpanValue(context.params.categories, rowIndex);
						twoColumnInputDrawTextCell(context, "rows/row-" + std::to_string(rowIndex) + "/category-cell", context.params.firstColumnSizing, [&]() {
							basicInputFieldParams inputParams = twoColumnInputBaseTextInputParams(
								context.createChildElementId("rows/row-" + std::to_string(rowIndex) + "/category-input"),
								category,
								Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)},
								context.params,
								[rowIndex, callback = context.params.onCategoryChange](std::string_view changed) {
									if (callback != nullptr)
									{
										callback(rowIndex, changed);
									}
								});
							context.uiManager.createElement(kBasicInputField, context.createChildElementId("rows/row-" + std::to_string(rowIndex) + "/category"))
								.setParameters(std::move(inputParams))
								.draw();
						});

						const std::string value = twoColumnInputSpanValue(context.params.values, rowIndex);
						twoColumnInputDrawTextCell(context, "rows/row-" + std::to_string(rowIndex) + "/value-cell", context.params.secondColumnSizing, [&]() {
							twoColumnInputDrawSecondColumnInput(context, rowIndex, value);
						});

						CLAY(context.uiManager.toClayEID(context.createChildElementId("rows/row-" + std::to_string(rowIndex) + "/delete-slot")), {
							.layout = {
								.sizing = context.params.deleteSlotSizing,
								.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
							},
						})
						{
							if (rowHovered)
							{
								basicButtonParams deleteParams{};
								deleteParams.contentMode = basicButtonParams::ContentMode::IconOnly;
								if (TwoColumnInputTableDef::resources.has_value())
								{
									deleteParams.icon = TwoColumnInputTableDef::resources->trashIcon;
								}
								deleteParams.onPressedCallback = [rowIndex, callback = context.params.onDeleteRow](BasicButtonInteractionContext) {
									if (callback != nullptr)
									{
										callback(rowIndex);
									}
								};
								deleteParams.padding = context.params.deleteButtonPadding;
								deleteParams.sizing = context.params.deleteButtonSizing;
								deleteParams.backgroundColor = context.params.deleteButtonBackgroundColor;
								deleteParams.hoverBackgroundColor = context.params.deleteButtonHoverBackgroundColor;
								deleteParams.cornerRadius = CLAY_CORNER_RADIUS(4);
								deleteParams.borderColor = FlowUi::Flow_Color("#00000000");
								deleteParams.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
								deleteParams.iconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(14), .height = CLAY_SIZING_FIXED(14)};
								deleteParams.iconTintColor = context.params.deleteButtonIconTintColor;

								context.uiManager.createElement(kBasicButton, context.createChildElementId("rows/row-" + std::to_string(rowIndex) + "/delete"))
									.setParameters(std::move(deleteParams))
									.draw();
							}
						};
					};
				}

				Clay_ElementDeclaration addRow{};
				addRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
				addRow.layout.sizing = context.params.rowSizing;
				addRow.layout.padding = context.params.rowPadding;
				addRow.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

				CLAY(context.uiManager.toClayEID(context.createChildElementId("rows/add-row")), addRow)
				{
					basicButtonParams addParams{};
					addParams.contentMode = basicButtonParams::ContentMode::IconOnly;
					if (TwoColumnInputTableDef::resources.has_value())
					{
						addParams.icon = TwoColumnInputTableDef::resources->plusIcon;
					}
					addParams.onPressedCallback = [
						defaultCategory = context.params.defaultCategory,
						defaultValue = context.params.defaultValue,
						callback = context.params.onAddRow
					](BasicButtonInteractionContext) {
						if (callback != nullptr)
						{
							callback(defaultCategory, defaultValue);
						}
					};
					addParams.padding = context.params.addButtonPadding;
					addParams.sizing = context.params.addButtonSizing;
					addParams.backgroundColor = context.params.addButtonBackgroundColor;
					addParams.hoverBackgroundColor = context.params.addButtonHoverBackgroundColor;
					addParams.cornerRadius = context.params.addButtonCornerRadius;
					addParams.borderColor = context.params.addButtonBorderColor;
					addParams.borderWidth = context.params.addButtonBorderWidth;
					addParams.iconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(16), .height = CLAY_SIZING_FIXED(16)};
					addParams.iconTintColor = FlowUi::Flow_Color("#ffffffff");
					context.uiManager.createElement(kBasicButton, context.createChildElementId("rows/add-button"))
						.setParameters(std::move(addParams))
						.draw();
				};
			};
		};
	},
};

struct twoColumnInputCardParams {
	std::string hintText = "Table";

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0, 360)};
	Clay_LayoutDirection cardLayout = CLAY_LEFT_TO_RIGHT;
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	twoColumnInputTableParams table{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	twoColumnInputCardParams,
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, cardLayout),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(twoColumnInputCardParams, table));

using TwoColumnInputCardDef = FlowUi::ElementDefinition<
	twoColumnInputCardParams,
	void,
	void,
	FLOW_DEF_ID("TwoColumnInputCard")>;

inline const TwoColumnInputCardDef kTwoColumnInputCard = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](TwoColumnInputCardDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("hint-text"));
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));
		const std::string tablePath = context.createChildElementId("table");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = context.params.cardLayout;
		root.layout.sizing = context.params.cardSizing;
		root.layout.padding = context.params.cardPadding;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		root.layout.childGap = 0;
		root.backgroundColor = FlowUi::Flow_Color("#00000000");
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_TextElementConfig hintTextConfig{};
		hintTextConfig.textColor = context.params.textColor;
		hintTextConfig.fontSize = context.params.fontSize;
		hintTextConfig.wrapMode = context.params.textWrapMode;
		hintTextConfig.textAlignment = context.params.textAlignment;
		hintTextConfig.fontId = context.params.fontId;

		Clay_ElementDeclaration spacer{};
		spacer.layout.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_PERCENT(1.0f)};

		twoColumnInputTableParams tableParams = context.params.table;
		tableParams.fontId = context.params.fontId;
		tableParams.fontSize = context.params.fontSize;
		tableParams.inputTextColor = context.params.textColor;

		CLAY(rootId, root)
		{
			CLAY(hintId, {})
			{
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.hintText),
					CLAY_TEXT_CONFIG(hintTextConfig)
				);
			};
			CLAY(spacerId, spacer){};
			context.uiManager.createElement(kTwoColumnInputTable, tablePath)
				.setParameters(std::move(tableParams))
				.draw();
		};
	},
};
