#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "FlowPlot_Defaults.hpp"
#include "FlowPlotGui.hpp"

struct gridSettingsEditorParams {
	std::string hintText = "Grid Settings";
	FlowPlot::Spec::AxisSpec value{};
	std::function<void(FlowPlot::Spec::AxisSpec)> onChange = nullptr;

	bool defaultExpanded = true;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding cardPadding = Clay_Padding{8, 8, 6, 6};
	uint16_t cardChildGap = 4;
	Clay_Color cardBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color cardBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth cardBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_CornerRadius cardCornerRadius = CLAY_CORNER_RADIUS(0);

	Clay_Sizing headerSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding headerPadding = Clay_Padding{8, 8, 6, 6};
	Clay_Color headerBackgroundColor = FlowUi::Flow_Color("#00000000");

	uint16_t fontId = 0;
	uint16_t fontSize = 14;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;

	Clay_Sizing disclosureButtonSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(24), .height = CLAY_SIZING_FIXED(24)};
	Clay_Padding disclosureButtonPadding = CLAY_PADDING_ALL(4);
	Clay_Color disclosureButtonBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color disclosureButtonHoverBackgroundColor = FlowUi::Flow_Color("#2c333dff");
	Clay_CornerRadius disclosureButtonCornerRadius = CLAY_CORNER_RADIUS(4);
	Clay_Color disclosureButtonBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth disclosureButtonBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_Sizing disclosureIconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(14), .height = CLAY_SIZING_FIXED(14)};
	Clay_Color disclosureIconTintColor = FlowUi::Flow_Color("#ffffffff");

	Clay_Sizing rowSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding rowPadding = CLAY_PADDING_ALL(0);
	uint16_t rowChildGap = 0;
	Clay_Color rowBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Sizing insetSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_PERCENT(1.0f)};

	colorPickerCardParams gridColorInput{};
	numericInputCardParams gridLineWidthInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	gridSettingsEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, defaultExpanded),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, cardCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, headerSizing),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, headerPadding),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, headerBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, disclosureIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, rowSizing),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, rowPadding),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, rowChildGap),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, rowBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, insetSizing),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, gridColorInput),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorParams, gridLineWidthInput));

struct gridSettingsEditorState {
	bool initialized = false;
	bool isExpanded = true;
};

FLOWUI_DEV_REGISTER_STRUCT(
	gridSettingsEditorState,
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(gridSettingsEditorState, isExpanded));

struct gridSettingsEditorResources {
	gridSettingsEditorResources() = default;
	explicit gridSettingsEditorResources(FlowUi::App& app) :
		downArrowIcon(app.icons().textureRef("DownArrow")),
		upArrowIcon(app.icons().textureRef("UpArrow"))
	{}

	FlowUi::TextureRef downArrowIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef upArrowIcon = FlowUi::TextureRef{};
};

using GridSettingsEditorDef = FlowUi::ElementDefinition<
	gridSettingsEditorParams,
	gridSettingsEditorState,
	gridSettingsEditorResources,
	FLOW_DEF_ID("GridSettingsEditor")>;
using GridSettingsEditorInteractionContext = GridSettingsEditorDef::InteractionContext;

inline void gridSettingsEditorEmit(
	FlowPlot::Spec::AxisSpec nextSpec,
	const std::function<void(FlowPlot::Spec::AxisSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

template <typename DrawChildFn>
inline void gridSettingsEditorDrawInputRow(
	GridSettingsEditorDef::BuildContext& context,
	std::string_view localId,
	const gridSettingsEditorParams& params,
	DrawChildFn&& drawChild)
{
	const std::string rowPath = context.createChildElementId(localId);
	const Clay_ElementId rowId = context.uiManager.toClayEID(rowPath);
	const Clay_ElementId insetId = context.uiManager.toClayEID(rowPath + "/inset");

	Clay_ElementDeclaration row{};
	row.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
	row.layout.sizing = params.rowSizing;
	row.layout.padding = params.rowPadding;
	row.layout.childGap = params.rowChildGap;
	row.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	row.backgroundColor = params.rowBackgroundColor;
	row.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

	Clay_ElementDeclaration inset{};
	inset.layout.sizing = params.insetSizing;
	inset.backgroundColor = FlowUi::Flow_Color("#00000000");

	CLAY(rowId, row)
	{
		CLAY(insetId, inset) {};
		drawChild(rowPath);
	};
}

inline const GridSettingsEditorDef kGridSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](GridSettingsEditorInteractionContext& context) {
		gridSettingsEditorState& state = GridSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}
	},
	nullptr,
	+[](GridSettingsEditorDef::BuildContext& context) {
		gridSettingsEditorState& state = GridSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}

		FlowUi::TextureRef disclosureIcon{};
		if (GridSettingsEditorDef::resources.has_value())
		{
			disclosureIcon = state.isExpanded
				? GridSettingsEditorDef::resources->downArrowIcon
				: GridSettingsEditorDef::resources->upArrowIcon;
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId headerId = context.uiManager.toClayEID(context.createChildElementId("header"));
		const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("header/hint"));
		const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("header/spacer"));
		const std::string disclosurePath = context.createChildElementId("header/disclosure");

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		root.layout.sizing = context.params.cardSizing;
		root.layout.padding = context.params.cardPadding;
		root.layout.childGap = context.params.cardChildGap;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		root.backgroundColor = context.params.cardBackgroundColor;
		root.cornerRadius = context.params.cardCornerRadius;
		root.border = {.color = context.params.cardBorderColor, .width = context.params.cardBorderWidth};

		Clay_ElementDeclaration header{};
		header.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		header.layout.sizing = context.params.headerSizing;
		header.layout.padding = context.params.headerPadding;
		header.layout.childGap = 0;
		header.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		header.backgroundColor = context.params.headerBackgroundColor;

		Clay_TextElementConfig hintTextConfig{};
		hintTextConfig.textColor = context.params.textColor;
		hintTextConfig.fontSize = context.params.fontSize;
		hintTextConfig.wrapMode = context.params.textWrapMode;
		hintTextConfig.textAlignment = context.params.textAlignment;
		hintTextConfig.fontId = context.params.fontId;

		Clay_ElementDeclaration spacer{};
		spacer.layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		basicButtonParams disclosureParams{};
		disclosureParams.icon = disclosureIcon;
		disclosureParams.contentMode = basicButtonParams::ContentMode::IconOnly;
		disclosureParams.onPressedCallback = [elementFlowId = FlowUi::toFlowId(context.elementID)](BasicButtonInteractionContext) {
			gridSettingsEditorState* latestState = GridSettingsEditorDef::tryGetState(elementFlowId);
			if (latestState != nullptr)
			{
				latestState->isExpanded = !latestState->isExpanded;
			}
		};
		disclosureParams.padding = context.params.disclosureButtonPadding;
		disclosureParams.sizing = context.params.disclosureButtonSizing;
		disclosureParams.backgroundColor = context.params.disclosureButtonBackgroundColor;
		disclosureParams.hoverBackgroundColor = context.params.disclosureButtonHoverBackgroundColor;
		disclosureParams.cornerRadius = context.params.disclosureButtonCornerRadius;
		disclosureParams.borderColor = context.params.disclosureButtonBorderColor;
		disclosureParams.borderWidth = context.params.disclosureButtonBorderWidth;
		disclosureParams.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		disclosureParams.iconContainerSizing = context.params.disclosureIconContainerSizing;
		disclosureParams.iconTintColor = context.params.disclosureIconTintColor;

		CLAY(rootId, root)
		{
			CLAY(headerId, header)
			{
				CLAY(hintId, {})
				{
					CLAY_TEXT(
						context.uiManager.toClayString(context.params.hintText),
						CLAY_TEXT_CONFIG(hintTextConfig)
					);
				};
				CLAY(spacerId, spacer) {};
				context.uiManager.createElement(kBasicButton, disclosurePath)
					.setParameters(std::move(disclosureParams))
					.draw();
			};

			if (!state.isExpanded)
			{
				continue;
			}

			gridSettingsEditorDrawInputRow(context, "row-grid-color", context.params, [&](const std::string& rowPath) {
				colorPickerCardParams params = context.params.gridColorInput;
				params.hintText = "Grid Color";
				params.value = context.params.value.gridColor;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.gridColor = std::string(changed);
					gridSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kColorPickerCard, rowPath + "/grid-color")
					.setParameters(std::move(params))
					.draw();
			});

			gridSettingsEditorDrawInputRow(context, "row-grid-line-width", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.gridLineWidthInput;
				params.hintText = "Grid Line Width";
				params.valueType = numericInputValueType::Float;
				params.value = static_cast<double>(context.params.value.gridWidth);
				params.minValue = 0.0;
				params.maxValue = 1000000.0;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.gridWidth = static_cast<float>(changed);
					gridSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/grid-line-width")
					.setParameters(std::move(params))
					.draw();
			});
		};
	},
};
