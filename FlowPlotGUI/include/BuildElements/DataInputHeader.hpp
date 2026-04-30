#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "DatasetsHelpers.hpp"
#include "FlowPlotGui.hpp"
#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicInputField.hpp"

struct dataInputHeaderState {
	std::size_t activeDatasetIndex = 0;
};

FLOWUI_DEV_REGISTER_STRUCT(
	dataInputHeaderState,
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderState, activeDatasetIndex));

struct dataTabParams {
	FlowPlotGui::state* guiState = nullptr;
	std::size_t* activeDatasetIndex = nullptr;
	std::size_t datasetIndex = 0;
	std::string text{};
	bool active = false;

	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Padding padding = Clay_Padding{12, 8, 0, 0};
	uint16_t childGap = 6;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#252529ff");
	Clay_Color hoverBackgroundColor = FlowUi::Flow_Color("#303037ff");
	Clay_Color activeBackgroundColor = FlowUi::Flow_Color("#34343aff");
	Clay_Color borderColor = FlowUi::Flow_Color("#3f3f46ff");
	Clay_Color activeBorderColor = FlowUi::Flow_Color("#68c4c0ff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_CornerRadius cornerRadius = Clay_CornerRadius{12.0f, 12.0f, 0.0f, 0.0f};

	Clay_Sizing nameSizing = Clay_Sizing{.width = CLAY_SIZING_FIT(54, 180), .height = CLAY_SIZING_GROW(0)};
	Clay_Padding namePadding = Clay_Padding{0, 0, 0, 0};
	uint16_t fontId = 0;
	uint16_t fontSize = 13;
	Clay_Color textColor = FlowUi::Flow_Color("#d8d8dcff");
	Clay_Color activeTextColor = FlowUi::Flow_Color("#ffffffff");

	Clay_Sizing closeButtonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(16), .height = CLAY_SIZING_FIXED(16)};
	Clay_Color closeIconColor = FlowUi::Flow_Color("#9ca3afff");
	Clay_Color closeIconHoverColor = FlowUi::Flow_Color("#ef4444ff");
};

struct dataTabState {
	bool editingName = false;
	bool requestedNameEditThisFrame = false;
};

FLOWUI_DEV_REGISTER_STRUCT(
	dataTabParams,
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, text),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, active),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, hoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, activeBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, activeBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, cornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, nameSizing),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, namePadding),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, activeTextColor),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, closeButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, closeIconColor),
	FLOWUI_DEV_REFLECT_FIELD(dataTabParams, closeIconHoverColor));

FLOWUI_DEV_REGISTER_STRUCT(
	dataTabState,
	FLOWUI_DEV_REFLECT_FIELD(dataTabState, editingName),
	FLOWUI_DEV_REFLECT_FIELD(dataTabState, requestedNameEditThisFrame));

struct dataTabResources {
	dataTabResources() = default;
	explicit dataTabResources(FlowUi::App& app) :
		closeIcon(app.icons().textureRef("X"))
	{}

	FlowUi::TextureRef closeIcon = FlowUi::TextureRef{};
};

using DataTabDef = FlowUi::ElementDefinition<
	dataTabParams,
	dataTabState,
	dataTabResources,
	FLOW_DEF_ID("DataTab")>;

inline std::string dataTabNameFieldId(std::string_view elementId)
{
	return std::string(elementId) + "/name-field";
}

inline const DataTabDef kDataTab = {
		nullptr,
	+[](DataTabDef::InteractionContext& context) {
		dataTabState& tabState = DataTabDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (context.params.activeDatasetIndex != nullptr)
		{
			*context.params.activeDatasetIndex = context.params.datasetIndex;
		}

		if (context.uiManager.getPreviousFrameInput().ctrl)
		{
			tabState.editingName = true;
			tabState.requestedNameEditThisFrame = true;
			context.uiManager.inputFields().requestCaret(
				dataTabNameFieldId(context.elementID),
				FlowUi::InputFieldManager::CaretRequestKind::SetPrimary);
		}
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DataTabDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId nameTextId = context.uiManager.toClayEID(context.createChildElementId("name-text"));
		const Clay_ElementId closePlaceholderId = context.uiManager.toClayEID(context.createChildElementId("close-placeholder"));

		const bool hovered = context.uiManager.getPreviousFramesInteraction().isHovered(rootId);
		const std::string nameInputPath = context.createChildElementId("name-input");
		dataTabState& tabState = DataTabDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (tabState.editingName && !tabState.requestedNameEditThisFrame)
		{
			const basicInputFieldState* inputState = BasicInputFieldDef::tryGetStateConst(FlowUi::toFlowId(nameInputPath));
			if (inputState != nullptr && !inputState->hadPrimaryCaretLastFrame)
			{
				tabState.editingName = false;
			}
		}
		tabState.requestedNameEditThisFrame = false;
		const bool canRemoveDataset =
			context.params.guiState != nullptr && context.params.guiState->datasets.size() > 1;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		rootLayout.childGap = context.params.childGap;

		Clay_Color backgroundColor = context.params.backgroundColor;
		if (context.params.active)
		{
			backgroundColor = context.params.activeBackgroundColor;
		}
		else if (hovered)
		{
			backgroundColor = context.params.hoverBackgroundColor;
		}

		Clay_ElementDeclaration root{};
		root.layout = rootLayout;
		root.backgroundColor = backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.border = {
			.color = context.params.active ? context.params.activeBorderColor : context.params.borderColor,
			.width = context.params.borderWidth,
		};

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.active ? context.params.activeTextColor : context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = CLAY_TEXT_WRAP_NONE;
		textConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;
		textConfig.fontId = context.params.fontId;

		Clay_LayoutConfig nameLayout{};
		nameLayout.sizing = context.params.nameSizing;
		nameLayout.padding = context.params.namePadding;
		nameLayout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

		Clay_ElementDeclaration name{};
		name.layout = nameLayout;
		name.backgroundColor = FlowUi::Flow_Color("#00000000");
		name.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig closePlaceholderLayout{};
		closePlaceholderLayout.sizing = context.params.closeButtonSizing;

		Clay_ElementDeclaration closePlaceholder{};
		closePlaceholder.layout = closePlaceholderLayout;
		closePlaceholder.backgroundColor = FlowUi::Flow_Color("#00000000");
		closePlaceholder.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(rootId, root){
			if (tabState.editingName)
			{
				basicInputFieldParams nameInput{};
				nameInput.fieldId = dataTabNameFieldId(context.elementID);
				nameInput.value = context.params.text;
				nameInput.syncValueFromParams = true;
				nameInput.padding = context.params.namePadding;
				nameInput.sizing = context.params.nameSizing;
				nameInput.borderColor = FlowUi::Flow_Color("#00000000");
				nameInput.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
				nameInput.backgroundColor = FlowUi::Flow_Color("#00000000");
				nameInput.cornerRadius = CLAY_CORNER_RADIUS(0);
				nameInput.fontId = context.params.fontId;
				nameInput.fontSize = context.params.fontSize;
				nameInput.textColor = context.params.active ? context.params.activeTextColor : context.params.textColor;
				nameInput.onTextChangedCallback = [
					guiState = context.params.guiState,
					datasetIndex = context.params.datasetIndex
				](std::string_view changed) {
					if (guiState == nullptr)
					{
						return;
					}
					if (!FlowPlotGui::renameDataset(*guiState, datasetIndex, changed))
					{
						return;
					}
				};
				context.uiManager.createElement(kBasicInputField, nameInputPath)
					.setParameters(std::move(nameInput))
					.draw();
			}
			else
			{
				CLAY(nameTextId, name){
					CLAY_TEXT(context.uiManager.toClayString(context.params.text), CLAY_TEXT_CONFIG(textConfig));
				};
			}

			if (hovered && canRemoveDataset)
			{
				basicButtonParams closeButton{};
				closeButton.icon = DataTabDef::resources->closeIcon;
				closeButton.contentMode = basicButtonParams::ContentMode::IconOnly;
				closeButton.padding = CLAY_PADDING_ALL(0);
				closeButton.sizing = context.params.closeButtonSizing;
				closeButton.backgroundColor = FlowUi::Flow_Color("#00000000");
				closeButton.hoverBackgroundColor = FlowUi::Flow_Color("#00000000");
				closeButton.borderColor = FlowUi::Flow_Color("#00000000");
				closeButton.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
				closeButton.cornerRadius = CLAY_CORNER_RADIUS(0);
				closeButton.iconContainerSizing = context.params.closeButtonSizing;
				closeButton.iconTintColor = context.params.closeIconColor;
				closeButton.onHoveredCallback = [hoverColor = context.params.closeIconHoverColor](BasicButtonInteractionContext buttonContext) {
					buttonContext.params.iconTintColor = hoverColor;
				};
				closeButton.onPressedCallback = [
					guiState = context.params.guiState,
					datasetIndex = context.params.datasetIndex,
					activeDatasetIndex = context.params.activeDatasetIndex
				](BasicButtonInteractionContext) {
					if (guiState == nullptr || datasetIndex >= guiState->datasets.size())
					{
						return;
					}

					std::size_t nextActiveIndex = datasetIndex;
					if (guiState->datasets.size() > 1)
					{
						nextActiveIndex = (datasetIndex + 1 < guiState->datasets.size())
							? datasetIndex + 1
							: datasetIndex - 1;
					}

					if (!FlowPlotGui::removeDataset(*guiState, datasetIndex))
					{
						return;
					}

					if (activeDatasetIndex != nullptr)
					{
						if (nextActiveIndex > datasetIndex && nextActiveIndex > 0)
						{
							--nextActiveIndex;
						}
						*activeDatasetIndex = nextActiveIndex;
					}
				};
				context.uiManager.createElement(kBasicButton, context.createChildElementId("close"))
					.setParameters(std::move(closeButton))
					.draw();
			}
			else if (canRemoveDataset)
			{
				CLAY(closePlaceholderId, closePlaceholder){};
			}
		};
	},
};

struct dataInputHeaderParams {
	FlowPlotGui::state* guiState = nullptr;
	std::size_t* activeDatasetIndex = nullptr;
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIXED(44)};
	Clay_Padding padding = Clay_Padding{8, 8, 6, 6};
	uint16_t childGap = 6;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#1f1f23ff");
	Clay_Color borderColor = FlowUi::Flow_Color("#34343aff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 1, 0};

	Clay_Padding addRowContainerPadding = Clay_Padding{6, 0, 0, 0};
	Clay_Sizing addRowContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0)};
};

FLOWUI_DEV_REGISTER_STRUCT(
	dataInputHeaderParams,
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderParams, addRowContainerPadding),
	FLOWUI_DEV_REFLECT_FIELD(dataInputHeaderParams, addRowContainerSizing));

struct dataInputHeaderResources {
	dataInputHeaderResources() = default;
	explicit dataInputHeaderResources(FlowUi::App& app) :
		plusIcon(app.icons().textureRef("Plus"))
	{}

	FlowUi::TextureRef plusIcon = FlowUi::TextureRef{};
};

using DataInputHeaderDef = FlowUi::ElementDefinition<
	dataInputHeaderParams,
	dataInputHeaderState,
	dataInputHeaderResources,
	FLOW_DEF_ID("DataInputHeader")>;

inline const DataInputHeaderDef kDataInputHeader = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DataInputHeaderDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("spacer"));
		const Clay_ElementId addRowContainerId = context.uiManager.toClayEID(context.createChildElementId("add-row-container"));

		dataInputHeaderState& headerState = DataInputHeaderDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		std::size_t& activeDatasetIndex = context.params.activeDatasetIndex != nullptr
			? *context.params.activeDatasetIndex
			: headerState.activeDatasetIndex;
		FlowPlotGui::state* guiState = context.params.guiState;
		if (guiState != nullptr && !guiState->datasets.empty() && activeDatasetIndex >= guiState->datasets.size())
		{
			activeDatasetIndex = guiState->datasets.size() - 1;
		}

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		rootLayout.childGap = context.params.childGap;

		Clay_ElementDeclaration root{};
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_LayoutConfig spacerLayout{};
		spacerLayout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};

		Clay_ElementDeclaration spacer{};
		spacer.layout = spacerLayout;
		spacer.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig addRowContainerLayout{};
		addRowContainerLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		addRowContainerLayout.sizing = context.params.addRowContainerSizing;
		addRowContainerLayout.padding = context.params.addRowContainerPadding;
		addRowContainerLayout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_RIGHT, .y = CLAY_ALIGN_Y_CENTER};

		Clay_ElementDeclaration addRowContainer{};
		addRowContainer.layout = addRowContainerLayout;
		addRowContainer.backgroundColor = FlowUi::Flow_Color("#00000000");
		addRowContainer.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(rootId, root){
			if (guiState != nullptr)
			{
				for (std::size_t datasetIndex = 0; datasetIndex < guiState->datasets.size(); ++datasetIndex)
				{
					const FlowPlotGui::RunningDataset& dataset = guiState->datasets[datasetIndex];
					dataTabParams tabParams{};
					tabParams.guiState = guiState;
					tabParams.activeDatasetIndex = &activeDatasetIndex;
					tabParams.datasetIndex = datasetIndex;
					tabParams.text = dataset.name;
					tabParams.active = datasetIndex == activeDatasetIndex;
					context.uiManager.createElement(kDataTab, context.createChildElementId("tabs/" + std::to_string(datasetIndex)))
						.setParameters(std::move(tabParams))
						.draw();
				}
			}

			basicButtonParams addDatasetButton{};
			addDatasetButton.text = "+";
			addDatasetButton.contentMode = basicButtonParams::ContentMode::TextOnly;
			addDatasetButton.padding = Clay_Padding{10, 10, 0, 0};
			addDatasetButton.sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(34), .height = CLAY_SIZING_GROW(0)};
			addDatasetButton.backgroundColor = FlowUi::Flow_Color("#2b2b31ff");
			addDatasetButton.hoverBackgroundColor = FlowUi::Flow_Color("#3a3a42ff");
			addDatasetButton.borderColor = FlowUi::Flow_Color("#4a4a54ff");
			addDatasetButton.borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
			addDatasetButton.cornerRadius = CLAY_CORNER_RADIUS(4);
			addDatasetButton.fontSize = 16;
			addDatasetButton.textColor = FlowUi::Flow_Color("#f4f4f5ff");
			addDatasetButton.onPressedCallback = [
				guiState,
				headerFlowId = FlowUi::toFlowId(context.elementID)
			](BasicButtonInteractionContext) {
				if (guiState == nullptr)
				{
					return;
				}

				const std::size_t nextIndex = guiState->datasets.size();
				if (!FlowPlotGui::addDataset(*guiState))
				{
					return;
				}

				if (dataInputHeaderState* latestState = DataInputHeaderDef::tryGetState(headerFlowId))
				{
					latestState->activeDatasetIndex = nextIndex;
				}
			};
			context.uiManager.createElement(kBasicButton, context.createChildElementId("add-dataset"))
				.setParameters(std::move(addDatasetButton))
				.draw();

			CLAY(spacerId, spacer){};

			CLAY(addRowContainerId, addRowContainer){
				basicButtonParams addRowButton{};
				addRowButton.text = "add Row";
				addRowButton.icon = DataInputHeaderDef::resources->plusIcon;
				addRowButton.contentMode = basicButtonParams::ContentMode::IconThenText;
				addRowButton.padding = Clay_Padding{10, 12, 0, 0};
				addRowButton.sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_GROW(0)};
				addRowButton.backgroundColor = FlowUi::Flow_Color("#2a3938ff");
				addRowButton.hoverBackgroundColor = FlowUi::Flow_Color("#33504eff");
				addRowButton.borderColor = FlowUi::Flow_Color("#4d7774ff");
				addRowButton.borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
				addRowButton.cornerRadius = CLAY_CORNER_RADIUS(4);
				addRowButton.fontSize = 13;
				addRowButton.textColor = FlowUi::Flow_Color("#edfdfcff");
				addRowButton.iconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(14), .height = CLAY_SIZING_FIXED(14)};
				addRowButton.iconTintColor = FlowUi::Flow_Color("#edfdfcff");
				addRowButton.onPressedCallback = [
					guiState,
					activeDatasetIndex = &activeDatasetIndex
				](BasicButtonInteractionContext) {
					if (guiState != nullptr && activeDatasetIndex != nullptr)
					{
						FlowPlotGui::addRow(*guiState, *activeDatasetIndex);
					}
				};
				context.uiManager.createElement(kBasicButton, context.createChildElementId("add-row-container/add-row"))
					.setParameters(std::move(addRowButton))
					.draw();
			};
		};
	},
};
