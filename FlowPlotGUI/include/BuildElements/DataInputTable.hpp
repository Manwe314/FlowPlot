#pragma once

#include <cstddef>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "DatasetsHelpers.hpp"
#include "FlowPlotGui.hpp"
#include "BuildElements/BoolToggle.hpp"
#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicInputField.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"

inline Clay_Vector2 dataInputTableScrollOffsetForElementId(
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

struct dataInputColumnView {
	FlowPlotGui::DatasetFieldType type = FlowPlotGui::DatasetFieldType::Number;
	std::size_t typedColumnIndex = 0;
	std::string name{};
};

inline std::vector<dataInputColumnView> dataInputBuildColumnViews(const FlowPlotGui::RunningDataset& dataset)
{
	std::vector<dataInputColumnView> columns{};
	columns.reserve(FlowPlotGui::datasetColumnCount(dataset));
	for (std::size_t i = 0; i < dataset.numericColumns.size(); ++i)
	{
		columns.push_back({FlowPlotGui::DatasetFieldType::Number, i, dataset.numericColumns[i].name});
	}
	for (std::size_t i = 0; i < dataset.stringColumns.size(); ++i)
	{
		columns.push_back({FlowPlotGui::DatasetFieldType::String, i, dataset.stringColumns[i].name});
	}
	for (std::size_t i = 0; i < dataset.boolColumns.size(); ++i)
	{
		columns.push_back({FlowPlotGui::DatasetFieldType::Boolean, i, dataset.boolColumns[i].name});
	}

	return columns;
}

struct dataInputHeaderRowParams {
	FlowPlotGui::state* guiState = nullptr;
	std::size_t datasetIndex = 0;

	float rowHeight = 36.0f;
	float idColumnWidth = 44.0f;
	float actionColumnWidth = 40.0f;
	uint16_t columnGap = 0;

	Clay_Padding cellPadding = Clay_Padding{8, 8, 0, 0};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#202126ff");
	Clay_Color cellBorderColor = FlowUi::Flow_Color("#363943ff");
	Clay_BorderWidth cellBorderWidth = Clay_BorderWidth{0, 1, 0, 1, 0};
	uint16_t fontId = 0;
	uint16_t fontSize = 13;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f4f5ff");

	Clay_Sizing iconButtonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)};
	Clay_Color iconColor = FlowUi::Flow_Color("#a1a1aaff");
	Clay_Color iconHoverColor = FlowUi::Flow_Color("#ef4444ff");
	Clay_Color addIconHoverColor = FlowUi::Flow_Color("#67e8f9ff");
	Clay_Color addButtonHoverBackgroundColor = FlowUi::Flow_Color("#30363fff");

	int16_t outsideDismissZIndex = 220;
	int16_t menuZIndex = 221;
	uint16_t menuGapPx = 4;
	Clay_Sizing menuSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(140), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding menuPadding = Clay_Padding{0, 0, 4, 4};
	Clay_Color menuBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_Color menuBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth menuBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 1};
	Clay_CornerRadius menuCornerRadius = CLAY_CORNER_RADIUS(5);
	Clay_Padding optionPadding = Clay_Padding{10, 10, 7, 7};
	Clay_Color optionHoverBackgroundColor = FlowUi::Flow_Color("#2c333dff");
	Clay_Color separatorColor = FlowUi::Flow_Color("#303640ff");
};

struct dataInputHeaderRowState {
	bool addColumnMenuOpen = false;
};

FLOWUI_DEV_REGISTER_STRUCT(
	dataInputHeaderRowParams,
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, rowHeight),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, idColumnWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, actionColumnWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, columnGap),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, cellPadding),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, cellBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, cellBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, iconButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, iconColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, iconHoverColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, addIconHoverColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, addButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, outsideDismissZIndex),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, menuZIndex),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, menuGapPx),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, menuSizing),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, menuPadding),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, menuBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, menuBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, menuBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, menuCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, optionPadding),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, optionHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowParams, separatorColor));

FLOWUI_DEV_REGISTER_STRUCT(
	dataInputHeaderRowState,
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderRowState, addColumnMenuOpen));

struct dataInputHeaderRowResources {
	dataInputHeaderRowResources() = default;
	explicit dataInputHeaderRowResources(FlowUi::App& app) :
		plusIcon(app.icons().textureRef("Plus")),
		closeIcon(app.icons().textureRef("X")),
		trashIcon(app.icons().textureRef("Trash"))
	{}

	FlowUi::TextureRef plusIcon{};
	FlowUi::TextureRef closeIcon{};
	FlowUi::TextureRef trashIcon{};
};

using DataInputHeaderRowDef = FlowUi::ElementDefinition<
	dataInputHeaderRowParams,
	dataInputHeaderRowState,
	dataInputHeaderRowResources,
	FLOW_DEF_ID("DataInputHeaderRow")>;

inline basicButtonParams dataInputTransparentButton(
	Clay_Sizing sizing,
	Clay_Padding padding,
	std::function<void(BasicButtonInteractionContext)> onPressed)
{
	basicButtonParams params{};
	params.contentMode = basicButtonParams::ContentMode::None;
	params.onPressedCallback = std::move(onPressed);
	params.padding = padding;
	params.sizing = sizing;
	params.backgroundColor = FlowUi::Flow_Color("#00000000");
	params.hoverBackgroundColor = FlowUi::Flow_Color("#00000000");
	params.cornerRadius = CLAY_CORNER_RADIUS(0);
	params.borderColor = FlowUi::Flow_Color("#00000000");
	params.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	return params;
}

inline basicButtonParams dataInputColumnMenuOptionParams(
	std::string text,
	Clay_Padding padding,
	Clay_Color textColor,
	Clay_Color hoverBackgroundColor,
	uint16_t fontId,
	uint16_t fontSize,
	std::function<void(BasicButtonInteractionContext)> onPressed)
{
	basicButtonParams params{};
	params.text = std::move(text);
	params.contentMode = basicButtonParams::ContentMode::TextOnly;
	params.onPressedCallback = std::move(onPressed);
	params.onHoveredCallback = [hoverBackgroundColor](BasicButtonInteractionContext buttonContext) {
		buttonContext.params.backgroundColor = hoverBackgroundColor;
	};
	params.padding = padding;
	params.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	params.backgroundColor = FlowUi::Flow_Color("#00000000");
	params.hoverBackgroundColor = hoverBackgroundColor;
	params.cornerRadius = CLAY_CORNER_RADIUS(0);
	params.borderColor = FlowUi::Flow_Color("#00000000");
	params.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	params.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	params.fontId = fontId;
	params.fontSize = fontSize;
	params.textColor = textColor;
	return params;
}

inline const DataInputHeaderRowDef kDataInputHeaderRow = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DataInputHeaderRowDef::BuildContext& context) {
		dataInputHeaderRowState& state = DataInputHeaderRowDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId idCellId = context.uiManager.toClayEID(context.createChildElementId("id-cell"));
		const Clay_ElementId columnsAreaId = context.uiManager.toClayEID(context.createChildElementId("columns"));
		const Clay_ElementId actionCellId = context.uiManager.toClayEID(context.createChildElementId("action-cell"));
		const std::string addButtonPath = context.createChildElementId("action-cell/add-column");

		std::vector<dataInputColumnView> columns{};
		if (context.params.guiState != nullptr && context.params.datasetIndex < context.params.guiState->datasets.size())
		{
			columns = dataInputBuildColumnViews(context.params.guiState->datasets[context.params.datasetIndex]);
		}

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(context.params.rowHeight)};
		root.layout.childGap = context.params.columnGap;
		root.backgroundColor = context.params.backgroundColor;
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		auto makeCell = [&](Clay_Sizing sizing) {
			Clay_ElementDeclaration cell{};
			cell.layout.sizing = sizing;
			cell.layout.padding = context.params.cellPadding;
			cell.layout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
			cell.backgroundColor = FlowUi::Flow_Color("#00000000");
			cell.border = {.color = context.params.cellBorderColor, .width = context.params.cellBorderWidth};
			return cell;
		};

		Clay_ElementDeclaration idCell = makeCell(
			Clay_Sizing{.width = CLAY_SIZING_FIXED(context.params.idColumnWidth), .height = CLAY_SIZING_GROW(0)});
		Clay_ElementDeclaration columnsArea{};
		columnsArea.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		columnsArea.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
		columnsArea.layout.childGap = context.params.columnGap;
		columnsArea.backgroundColor = FlowUi::Flow_Color("#00000000");
		columnsArea.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
		Clay_ElementDeclaration actionCell = makeCell(
			Clay_Sizing{.width = CLAY_SIZING_FIXED(context.params.actionColumnWidth), .height = CLAY_SIZING_GROW(0)});
		actionCell.layout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = CLAY_TEXT_WRAP_NONE;
		textConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		textConfig.fontId = context.params.fontId;

		CLAY(rootId, root){
			CLAY(idCellId, idCell){
				CLAY_TEXT(context.uiManager.toClayString("id"), CLAY_TEXT_CONFIG(textConfig));
			};

			CLAY(columnsAreaId, columnsArea){
				for (std::size_t columnIndex = 0; columnIndex < columns.size(); ++columnIndex)
				{
					const dataInputColumnView& column = columns[columnIndex];
					const std::string cellPath = context.createChildElementId("columns/column-" + std::to_string(columnIndex));
					const Clay_ElementId cellId = context.uiManager.toClayEID(cellPath);
					const Clay_ElementId closePlaceholderId = context.uiManager.toClayEID(cellPath + "/close-placeholder");
					const bool cellHovered = context.uiManager.getPreviousFramesInteraction().isHovered(cellId);

					Clay_ElementDeclaration columnCell = makeCell(
						Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)});
					columnCell.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
					columnCell.layout.childGap = 6;

					CLAY(cellId, columnCell){
						basicInputFieldParams nameInput{};
						nameInput.fieldId = cellPath + "/name-input";
						nameInput.value = column.name;
						nameInput.syncValueFromParams = true;
						nameInput.padding = CLAY_PADDING_ALL(0);
						nameInput.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
						nameInput.borderColor = FlowUi::Flow_Color("#00000000");
						nameInput.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
						nameInput.backgroundColor = FlowUi::Flow_Color("#00000000");
						nameInput.cornerRadius = CLAY_CORNER_RADIUS(0);
						nameInput.fontId = context.params.fontId;
						nameInput.fontSize = context.params.fontSize;
						nameInput.textColor = context.params.textColor;
						nameInput.onTextChangedCallback = [
							guiState = context.params.guiState,
							datasetIndex = context.params.datasetIndex,
							type = column.type,
							typedColumnIndex = column.typedColumnIndex
						](std::string_view changed) {
							if (guiState != nullptr)
							{
								FlowPlotGui::renameColumn(*guiState, datasetIndex, type, typedColumnIndex, changed);
							}
						};
						context.uiManager.createElement(kBasicInputField, cellPath + "/name-input")
							.setParameters(std::move(nameInput))
							.draw();

						if (cellHovered)
						{
							basicButtonParams closeButton{};
							closeButton.icon = DataInputHeaderRowDef::resources->closeIcon;
							closeButton.contentMode = basicButtonParams::ContentMode::IconOnly;
							closeButton.padding = CLAY_PADDING_ALL(0);
							closeButton.sizing = context.params.iconButtonSizing;
							closeButton.backgroundColor = FlowUi::Flow_Color("#00000000");
							closeButton.hoverBackgroundColor = FlowUi::Flow_Color("#00000000");
							closeButton.borderColor = FlowUi::Flow_Color("#00000000");
							closeButton.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
							closeButton.iconContainerSizing = context.params.iconButtonSizing;
							closeButton.iconTintColor = context.params.iconColor;
							closeButton.onHoveredCallback = [hoverColor = context.params.iconHoverColor](BasicButtonInteractionContext buttonContext) {
								buttonContext.params.iconTintColor = hoverColor;
							};
							closeButton.onPressedCallback = [
								guiState = context.params.guiState,
								datasetIndex = context.params.datasetIndex,
								type = column.type,
								typedColumnIndex = column.typedColumnIndex
							](BasicButtonInteractionContext) {
								if (guiState != nullptr)
								{
									FlowPlotGui::removeColumn(*guiState, datasetIndex, type, typedColumnIndex);
								}
							};
							context.uiManager.createElement(kBasicButton, cellPath + "/remove")
								.setParameters(std::move(closeButton))
								.draw();
						}
						else
						{
							Clay_ElementDeclaration placeholder{};
							placeholder.layout.sizing = context.params.iconButtonSizing;
							placeholder.backgroundColor = FlowUi::Flow_Color("#00000000");
							placeholder.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
							CLAY(closePlaceholderId, placeholder){};
						}
					};
				}
			};

			CLAY(actionCellId, actionCell){
				basicButtonParams addButton{};
				addButton.icon = DataInputHeaderRowDef::resources->plusIcon;
				addButton.contentMode = basicButtonParams::ContentMode::IconOnly;
				addButton.padding = CLAY_PADDING_ALL(0);
				addButton.sizing = context.params.iconButtonSizing;
				addButton.backgroundColor = FlowUi::Flow_Color("#00000000");
				addButton.hoverBackgroundColor = context.params.addButtonHoverBackgroundColor;
				addButton.borderColor = FlowUi::Flow_Color("#00000000");
				addButton.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
				addButton.cornerRadius = CLAY_CORNER_RADIUS(4);
				addButton.iconContainerSizing = context.params.iconButtonSizing;
				addButton.iconTintColor = context.params.iconColor;
				addButton.onHoveredCallback = [hoverColor = context.params.addIconHoverColor](BasicButtonInteractionContext buttonContext) {
					buttonContext.params.iconTintColor = hoverColor;
				};
				addButton.onPressedCallback = [elementFlowId = FlowUi::toFlowId(context.elementID)](BasicButtonInteractionContext) {
					if (dataInputHeaderRowState* latestState = DataInputHeaderRowDef::tryGetState(elementFlowId))
					{
						latestState->addColumnMenuOpen = !latestState->addColumnMenuOpen;
					}
				};
				context.uiManager.createElement(kBasicButton, addButtonPath)
					.setParameters(std::move(addButton))
					.draw();

				if (state.addColumnMenuOpen)
				{
					const FlowUi::FlowElementId elementFlowId = FlowUi::toFlowId(context.elementID);

					Clay_ElementDeclaration dismissLayer{};
					const Clay_ElementId dismissLayerId = context.uiManager.toClayEID(context.createChildElementId("dismiss-layer"));
					dismissLayer.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
					dismissLayer.backgroundColor = FlowUi::Flow_Color("#00000000");
					dismissLayer.floating = {
						.zIndex = context.params.outsideDismissZIndex,
						.attachPoints = {.element = CLAY_ATTACH_POINT_LEFT_TOP, .parent = CLAY_ATTACH_POINT_LEFT_TOP},
						.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
						.attachTo = CLAY_ATTACH_TO_ROOT,
					};
					CLAY(dismissLayerId, dismissLayer){
						context.uiManager.createElement(kBasicButton, context.createChildElementId("dismiss-layer/button"))
							.setParameters(dataInputTransparentButton(
								Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
								CLAY_PADDING_ALL(0),
								[elementFlowId](BasicButtonInteractionContext) {
									if (dataInputHeaderRowState* latestState = DataInputHeaderRowDef::tryGetState(elementFlowId))
									{
										latestState->addColumnMenuOpen = false;
									}
								}))
							.draw();
					};

					Clay_ElementDeclaration menu{};
					const Clay_ElementId menuId = context.uiManager.toClayEID(context.createChildElementId("add-column-menu"));
					menu.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
					menu.layout.sizing = context.params.menuSizing;
					menu.layout.padding = context.params.menuPadding;
					menu.backgroundColor = context.params.menuBackgroundColor;
					menu.cornerRadius = context.params.menuCornerRadius;
					menu.border = {.color = context.params.menuBorderColor, .width = context.params.menuBorderWidth};
					menu.floating = {
						.offset = {.x = 0.0f, .y = static_cast<float>(context.params.menuGapPx)},
						.parentId = context.uiManager.toClayEID(addButtonPath).id,
						.zIndex = context.params.menuZIndex,
						.attachPoints = {.element = CLAY_ATTACH_POINT_CENTER_TOP, .parent = CLAY_ATTACH_POINT_CENTER_BOTTOM},
						.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
						.attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID,
					};

					auto addOption = [&](std::string label, FlowPlotGui::DatasetFieldType type, std::size_t optionIndex) {
						context.uiManager.createElement(kBasicButton, context.createChildElementId("add-column-menu/option-" + std::to_string(optionIndex)))
							.setParameters(dataInputColumnMenuOptionParams(
								std::move(label),
								context.params.optionPadding,
								context.params.textColor,
								context.params.optionHoverBackgroundColor,
								context.params.fontId,
								context.params.fontSize,
								[
									elementFlowId,
									guiState = context.params.guiState,
									datasetIndex = context.params.datasetIndex,
									type
								](BasicButtonInteractionContext) {
									if (dataInputHeaderRowState* latestState = DataInputHeaderRowDef::tryGetState(elementFlowId))
									{
										latestState->addColumnMenuOpen = false;
									}
									if (guiState != nullptr)
									{
										FlowPlotGui::addColumn(*guiState, datasetIndex, type);
									}
								}))
							.draw();
					};

					auto separator = [&](std::size_t separatorIndex) {
						Clay_ElementDeclaration sep{};
						sep.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(1)};
						sep.backgroundColor = context.params.separatorColor;
						CLAY(context.uiManager.toClayEID(context.createChildElementId("add-column-menu/separator-" + std::to_string(separatorIndex))), sep){};
					};

					CLAY(menuId, menu){
						addOption("numeric", FlowPlotGui::DatasetFieldType::Number, 0);
						separator(0);
						addOption("string", FlowPlotGui::DatasetFieldType::String, 1);
						separator(1);
						addOption("boolean", FlowPlotGui::DatasetFieldType::Boolean, 2);
					};
				}
			};
		};
	},
};

struct dataInputDataRowParams {
	FlowPlotGui::state* guiState = nullptr;
	std::size_t datasetIndex = 0;
	std::size_t rowIndex = 0;
	std::string rowsContainerId{};
	float rowHeight = 34.0f;
	float rowGap = 0.0f;
	float idColumnWidth = 44.0f;
	float actionColumnWidth = 40.0f;
	uint16_t columnGap = 0;
	Clay_Padding cellPadding = Clay_Padding{8, 8, 4, 4};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#2d2d32ff");
	Clay_Color hoverBackgroundColor = FlowUi::Flow_Color("#33333aff");
	Clay_Color cellBorderColor = FlowUi::Flow_Color("#3a3a40ff");
	Clay_BorderWidth cellBorderWidth = Clay_BorderWidth{0, 1, 0, 1, 0};
	uint16_t fontId = 0;
	uint16_t fontSize = 13;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f4f5ff");
	Clay_Sizing iconButtonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)};
	Clay_Color iconColor = FlowUi::Flow_Color("#a1a1aaff");
	Clay_Color trashIconHoverColor = FlowUi::Flow_Color("#ef4444ff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	dataInputDataRowParams,
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, rowIndex),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, rowsContainerId),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, rowHeight),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, rowGap),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, idColumnWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, actionColumnWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, columnGap),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, cellPadding),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, hoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, cellBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, cellBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, iconButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, iconColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputDataRowParams, trashIconHoverColor));

using DataInputDataRowDef = FlowUi::ElementDefinition<dataInputDataRowParams, void, void, FLOW_DEF_ID("DataInputDataRow")>;

inline const DataInputDataRowDef kDataInputDataRow = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DataInputDataRowDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId idCellId = context.uiManager.toClayEID(context.createChildElementId("id-cell"));
		const Clay_ElementId columnsAreaId = context.uiManager.toClayEID(context.createChildElementId("columns"));
		const Clay_ElementId actionCellId = context.uiManager.toClayEID(context.createChildElementId("action-cell"));
		const Clay_ElementId trashPlaceholderId = context.uiManager.toClayEID(context.createChildElementId("action-cell/trash-placeholder"));
		const bool rowHovered = context.uiManager.getPreviousFramesInteraction().isHovered(rootId);

		std::vector<dataInputColumnView> columns{};
		const FlowPlotGui::RunningDataset* dataset = nullptr;
		if (context.params.guiState != nullptr && context.params.datasetIndex < context.params.guiState->datasets.size())
		{
			dataset = &context.params.guiState->datasets[context.params.datasetIndex];
			columns = dataInputBuildColumnViews(*dataset);
		}

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		root.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(context.params.rowHeight)};
		root.layout.childGap = context.params.columnGap;
		root.backgroundColor = rowHovered ? context.params.hoverBackgroundColor : context.params.backgroundColor;
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		auto makeCell = [&](Clay_Sizing sizing) {
			Clay_ElementDeclaration cell{};
			cell.layout.sizing = sizing;
			cell.layout.padding = context.params.cellPadding;
			cell.layout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
			cell.backgroundColor = FlowUi::Flow_Color("#00000000");
			cell.border = {.color = context.params.cellBorderColor, .width = context.params.cellBorderWidth};
			return cell;
		};

		Clay_ElementDeclaration idCell = makeCell(
			Clay_Sizing{.width = CLAY_SIZING_FIXED(context.params.idColumnWidth), .height = CLAY_SIZING_GROW(0)});
		Clay_ElementDeclaration columnsArea{};
		columnsArea.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		columnsArea.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
		columnsArea.layout.childGap = context.params.columnGap;
		columnsArea.backgroundColor = FlowUi::Flow_Color("#00000000");
		columnsArea.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
		Clay_ElementDeclaration actionCell = makeCell(
			Clay_Sizing{.width = CLAY_SIZING_FIXED(context.params.actionColumnWidth), .height = CLAY_SIZING_GROW(0)});
		actionCell.layout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = CLAY_TEXT_WRAP_NONE;
		textConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		textConfig.fontId = context.params.fontId;

			CLAY(rootId, root){
				CLAY(idCellId, idCell){
					CLAY_TEXT(context.uiManager.toClayString(std::to_string(context.params.rowIndex)), CLAY_TEXT_CONFIG(textConfig));
				};

				CLAY(columnsAreaId, columnsArea){
					for (std::size_t columnIndex = 0; columnIndex < columns.size(); ++columnIndex)
					{
						const dataInputColumnView& column = columns[columnIndex];
						const std::string cellPath = context.createChildElementId("columns/column-" + std::to_string(columnIndex));
						Clay_ElementDeclaration columnCell = makeCell(
							Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)});
						columnCell.layout.padding = Clay_Padding{4, 4, 3, 3};
						const Clay_ElementId cellId = context.uiManager.toClayEID(cellPath);

						CLAY(cellId, columnCell){
							switch (column.type)
							{
							case FlowPlotGui::DatasetFieldType::Number: {
								if (column.typedColumnIndex >= dataset->numericColumns.size() ||
									context.params.rowIndex >= dataset->numericColumns[column.typedColumnIndex].data.size())
								{
									break;
								}
								const std::string inputPath = cellPath + "/numeric/input";
								numericInputFieldParams numericParams{};
								numericParams.valueType = numericInputValueType::Double;
								numericParams.value = dataset->numericColumns[column.typedColumnIndex].data[context.params.rowIndex];
								numericParams.minValue = std::numeric_limits<double>::lowest();
								numericParams.maxValue = std::numeric_limits<double>::max();
								numericParams.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
								numericParams.fontId = context.params.fontId;
								numericParams.fontSize = context.params.fontSize;
								numericParams.inputTextColor = context.params.textColor;
								numericParams.inputPadding = Clay_Padding{6, 6, 2, 2};
								numericParams.inputBorderColor = FlowUi::Flow_Color("#00000000");
								numericParams.inputBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
								numericParams.inputBackgroundColor = FlowUi::Flow_Color("#1f1f24ff");
								numericParams.onEditBegin = [
									guiState = context.params.guiState,
									fieldId = inputPath
								]() {
									if (guiState != nullptr)
									{
										FlowPlotGui::beginDeferredDocumentEdit(
											*guiState,
											FlowPlotGui::makeDatasetEditTarget(
												"data-cell/" + fieldId,
												FlowPlotGui::makeDatasetsEditComparator()),
											10.0f);
										guiState->dataInputFocusGrid.setFocusedField(fieldId);
									}
								};
								numericParams.onEditEnd = [
									guiState = context.params.guiState,
									fieldId = inputPath
								]() {
									if (guiState != nullptr)
									{
										guiState->dataInputFocusGrid.clearFocusedField(fieldId);
										FlowPlotGui::endDeferredDocumentEdit(*guiState);
									}
								};
								numericParams.onChange = [
									guiState = context.params.guiState,
									datasetIndex = context.params.datasetIndex,
									typedColumnIndex = column.typedColumnIndex,
									rowIndex = context.params.rowIndex
								](double changed) {
									if (guiState != nullptr)
									{
										FlowPlotGui::setNumericCell(*guiState, datasetIndex, typedColumnIndex, rowIndex, changed);
									}
								};
								if (context.params.guiState != nullptr)
								{
									context.params.guiState->dataInputFocusGrid.registerCell({
										.fieldId = inputPath,
										.datasetIndex = context.params.datasetIndex,
										.row = context.params.rowIndex,
										.column = columnIndex,
										.scrollContainerId = context.params.rowsContainerId,
										.rowHeight = context.params.rowHeight,
										.rowGap = context.params.rowGap,
									});
								}
								context.uiManager.createElement(kNumericInputField, cellPath + "/numeric")
									.setParameters(std::move(numericParams))
									.draw();
								break;
							}
							case FlowPlotGui::DatasetFieldType::String: {
								if (column.typedColumnIndex >= dataset->stringColumns.size() ||
									context.params.rowIndex >= dataset->stringColumns[column.typedColumnIndex].data.size())
								{
									break;
								}
								const std::string inputPath = cellPath + "/string";
								basicInputFieldParams stringParams{};
								stringParams.fieldId = inputPath;
								stringParams.value = dataset->stringColumns[column.typedColumnIndex].data[context.params.rowIndex];
								stringParams.syncValueFromParams = true;
								stringParams.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
								stringParams.padding = Clay_Padding{6, 6, 2, 2};
								stringParams.borderColor = FlowUi::Flow_Color("#00000000");
								stringParams.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
								stringParams.backgroundColor = FlowUi::Flow_Color("#1f1f24ff");
								stringParams.cornerRadius = CLAY_CORNER_RADIUS(5);
								stringParams.fontId = context.params.fontId;
								stringParams.fontSize = context.params.fontSize;
								stringParams.textColor = context.params.textColor;
								stringParams.onEditBegin = [
									guiState = context.params.guiState,
									fieldId = inputPath
								]() {
									if (guiState != nullptr)
									{
										FlowPlotGui::beginDeferredDocumentEdit(
											*guiState,
											FlowPlotGui::makeDatasetEditTarget(
												"data-cell/" + fieldId,
												FlowPlotGui::makeDatasetsEditComparator()),
											10.0f);
										guiState->dataInputFocusGrid.setFocusedField(fieldId);
									}
								};
								stringParams.onEditEnd = [
									guiState = context.params.guiState,
									fieldId = inputPath
								]() {
									if (guiState != nullptr)
									{
										guiState->dataInputFocusGrid.clearFocusedField(fieldId);
										FlowPlotGui::endDeferredDocumentEdit(*guiState);
									}
								};
								stringParams.onTextChangedCallback = [
									guiState = context.params.guiState,
									datasetIndex = context.params.datasetIndex,
									typedColumnIndex = column.typedColumnIndex,
									rowIndex = context.params.rowIndex
								](std::string_view changed) {
									if (guiState != nullptr)
									{
										FlowPlotGui::setStringCell(*guiState, datasetIndex, typedColumnIndex, rowIndex, changed);
									}
								};
								if (context.params.guiState != nullptr)
								{
									context.params.guiState->dataInputFocusGrid.registerCell({
										.fieldId = inputPath,
										.datasetIndex = context.params.datasetIndex,
										.row = context.params.rowIndex,
										.column = columnIndex,
										.scrollContainerId = context.params.rowsContainerId,
										.rowHeight = context.params.rowHeight,
										.rowGap = context.params.rowGap,
									});
								}
								context.uiManager.createElement(kBasicInputField, cellPath + "/string")
									.setParameters(std::move(stringParams))
									.draw();
								break;
							}
						case FlowPlotGui::DatasetFieldType::Boolean: {
							if (column.typedColumnIndex >= dataset->boolColumns.size() ||
								context.params.rowIndex >= dataset->boolColumns[column.typedColumnIndex].data.size())
							{
								break;
							}
							boolToggleParams toggleParams{};
							toggleParams.value = dataset->boolColumns[column.typedColumnIndex].data[context.params.rowIndex];
							toggleParams.fontId = context.params.fontId;
							toggleParams.fontSize = context.params.fontSize;
							toggleParams.onChange = [
								guiState = context.params.guiState,
								datasetIndex = context.params.datasetIndex,
								typedColumnIndex = column.typedColumnIndex,
								rowIndex = context.params.rowIndex
							](bool changed) {
								if (guiState != nullptr)
								{
									FlowPlotGui::setBoolCell(*guiState, datasetIndex, typedColumnIndex, rowIndex, changed);
								}
							};
							context.uiManager.createElement(kBoolToggle, cellPath + "/bool")
								.setParameters(std::move(toggleParams))
								.draw();
							break;
						}
						}
					};
				}
			};

			CLAY(actionCellId, actionCell){
				if (rowHovered)
				{
					basicButtonParams trashButton{};
					trashButton.icon = DataInputHeaderRowDef::resources->trashIcon;
					trashButton.contentMode = basicButtonParams::ContentMode::IconOnly;
					trashButton.padding = CLAY_PADDING_ALL(0);
					trashButton.sizing = context.params.iconButtonSizing;
					trashButton.backgroundColor = FlowUi::Flow_Color("#00000000");
					trashButton.hoverBackgroundColor = FlowUi::Flow_Color("#00000000");
					trashButton.borderColor = FlowUi::Flow_Color("#00000000");
					trashButton.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
					trashButton.iconContainerSizing = context.params.iconButtonSizing;
					trashButton.iconTintColor = context.params.iconColor;
					trashButton.onHoveredCallback = [hoverColor = context.params.trashIconHoverColor](BasicButtonInteractionContext buttonContext) {
						buttonContext.params.iconTintColor = hoverColor;
					};
					trashButton.onPressedCallback = [
						guiState = context.params.guiState,
						datasetIndex = context.params.datasetIndex,
						rowIndex = context.params.rowIndex
					](BasicButtonInteractionContext) {
						if (guiState != nullptr)
						{
							FlowPlotGui::removeRow(*guiState, datasetIndex, rowIndex);
						}
					};
					context.uiManager.createElement(kBasicButton, context.createChildElementId("action-cell/trash"))
						.setParameters(std::move(trashButton))
						.draw();
				}
				else
				{
					Clay_ElementDeclaration placeholder{};
					placeholder.layout.sizing = context.params.iconButtonSizing;
					placeholder.backgroundColor = FlowUi::Flow_Color("#00000000");
					placeholder.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
					CLAY(trashPlaceholderId, placeholder){};
				}
			};
		};
	},
};

struct dataInputTableParams {
	FlowPlotGui::state* guiState = nullptr;
	std::size_t* activeDatasetIndex = nullptr;
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	uint16_t rowGap = 0;

	float headerRowHeight = 36.0f;
	float dataRowHeight = 34.0f;
	float idColumnWidth = 44.0f;
	float actionColumnWidth = 40.0f;
	uint16_t columnGap = 0;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#26262bff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	dataInputTableParams,
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, rowGap),
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, headerRowHeight),
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, dataRowHeight),
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, idColumnWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, actionColumnWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, columnGap),
	FLOWUI_DEV_REFLECT_FIELD(dataInputTableParams, backgroundColor));

using DataInputTableDef = FlowUi::ElementDefinition<dataInputTableParams, void, void, FLOW_DEF_ID("DataInputTable")>;

inline const DataInputTableDef kDataInputTable = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DataInputTableDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const std::string rowsContainerPath = context.createChildElementId("rows-container");
		const Clay_ElementId rowsContainerId = context.uiManager.toClayEID(rowsContainerPath);
		const Clay_Vector2 rowsScrollOffset =
			dataInputTableScrollOffsetForElementId(context.uiManager, rowsContainerPath);
		std::size_t activeDatasetIndex = context.params.activeDatasetIndex != nullptr
			? *context.params.activeDatasetIndex
			: 0;
			if (context.params.guiState != nullptr && !context.params.guiState->datasets.empty() &&
				activeDatasetIndex >= context.params.guiState->datasets.size())
			{
				activeDatasetIndex = context.params.guiState->datasets.size() - 1;
			}
			if (context.params.guiState != nullptr)
			{
				context.params.guiState->dataInputFocusGrid.clear();
			}

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		root.layout.sizing = context.params.sizing;
		root.layout.padding = context.params.padding;
		root.layout.childGap = context.params.rowGap;
		root.backgroundColor = context.params.backgroundColor;
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_ElementDeclaration rowsContainer{};
		rowsContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		rowsContainer.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
		rowsContainer.layout.childGap = context.params.rowGap;
		rowsContainer.backgroundColor = FlowUi::Flow_Color("#00000000");
		rowsContainer.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
		rowsContainer.clip = {
			.horizontal = false,
			.vertical = true,
			.childOffset = rowsScrollOffset,
		};

		CLAY(rootId, root){
			context.uiManager.createElement(kDataInputHeaderRow, context.createChildElementId("header-row"))
				.setParameters({
					.guiState = context.params.guiState,
					.datasetIndex = activeDatasetIndex,
					.rowHeight = context.params.headerRowHeight,
					.idColumnWidth = context.params.idColumnWidth,
					.actionColumnWidth = context.params.actionColumnWidth,
					.columnGap = context.params.columnGap,
				})
				.draw();

			CLAY(rowsContainerId, rowsContainer)
			{
				if (context.params.guiState != nullptr && activeDatasetIndex < context.params.guiState->datasets.size())
				{
					const std::size_t rowCount = FlowPlotGui::datasetRowCount(context.params.guiState->datasets[activeDatasetIndex]);
					for (std::size_t rowIndex = 0; rowIndex < rowCount; ++rowIndex)
					{
						context.uiManager.createElement(kDataInputDataRow, context.createChildElementId("rows/row-" + std::to_string(rowIndex)))
							.setParameters({
									.guiState = context.params.guiState,
									.datasetIndex = activeDatasetIndex,
									.rowIndex = rowIndex,
									.rowsContainerId = rowsContainerPath,
									.rowHeight = context.params.dataRowHeight,
									.rowGap = static_cast<float>(context.params.rowGap),
									.idColumnWidth = context.params.idColumnWidth,
								.actionColumnWidth = context.params.actionColumnWidth,
								.columnGap = context.params.columnGap,
							})
							.draw();
					}
				}
			};
		};
	},
};
