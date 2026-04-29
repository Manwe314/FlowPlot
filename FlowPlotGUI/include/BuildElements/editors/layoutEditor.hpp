#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "FlowPlot_Defaults.hpp"
#include "FlowPlotGui.hpp"

struct layoutEditorParams {
	std::string hintText = "Layout";
	FlowPlot::Spec::LayoutSpec value{};
	std::function<void(FlowPlot::Spec::LayoutSpec)> onChange = nullptr;

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

	numericInputCardParams rowsInput{};
	numericInputCardParams columnsInput{};
	numericInputCardParams gapInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	layoutEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, defaultExpanded),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, cardCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, headerSizing),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, headerPadding),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, headerBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, disclosureIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, rowSizing),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, rowPadding),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, rowChildGap),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, rowBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, insetSizing),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, rowsInput),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, columnsInput),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorParams, gapInput));

struct layoutEditorState {
	bool initialized = false;
	bool isExpanded = true;
};

FLOWUI_DEV_REGISTER_STRUCT(
	layoutEditorState,
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(layoutEditorState, isExpanded));

struct layoutEditorResources {
	layoutEditorResources() = default;
	explicit layoutEditorResources(FlowUi::App& app) :
		downArrowIcon(app.icons().textureRef("DownArrow")),
		upArrowIcon(app.icons().textureRef("UpArrow"))
	{}

	FlowUi::TextureRef downArrowIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef upArrowIcon = FlowUi::TextureRef{};
};

using LayoutEditorDef = FlowUi::ElementDefinition<
	layoutEditorParams,
	layoutEditorState,
	layoutEditorResources,
	FLOW_DEF_ID("LayoutEditor")>;
using LayoutEditorInteractionContext = LayoutEditorDef::InteractionContext;

inline void layoutEditorEmit(
	FlowPlot::Spec::LayoutSpec nextSpec,
	const std::function<void(FlowPlot::Spec::LayoutSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

template <typename DrawChildFn>
inline void layoutEditorDrawInputRow(
	LayoutEditorDef::BuildContext& context,
	std::string_view localId,
	const layoutEditorParams& params,
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

inline const LayoutEditorDef kLayoutEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](LayoutEditorInteractionContext& context) {
		layoutEditorState& state = LayoutEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}
	},
	nullptr,
	+[](LayoutEditorDef::BuildContext& context) {
		layoutEditorState& state = LayoutEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}

		FlowUi::TextureRef disclosureIcon{};
		if (LayoutEditorDef::resources.has_value())
		{
			disclosureIcon = state.isExpanded
				? LayoutEditorDef::resources->downArrowIcon
				: LayoutEditorDef::resources->upArrowIcon;
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
			layoutEditorState* latestState = LayoutEditorDef::tryGetState(elementFlowId);
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

			layoutEditorDrawInputRow(context, "row-rows", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.rowsInput;
				params.hintText = "Rows:";
				params.valueType = numericInputValueType::UInt32;
				params.value = static_cast<double>(context.params.value.rows);
				params.minValue = 1.0;
				params.maxValue = static_cast<double>(std::numeric_limits<std::uint32_t>::max());
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.rows = static_cast<std::uint32_t>(numericInputNormalizeValue(
						numericInputValueType::UInt32,
						changed,
						1.0,
						static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
					layoutEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/rows")
					.setParameters(std::move(params))
					.draw();
			});

			layoutEditorDrawInputRow(context, "row-columns", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.columnsInput;
				params.hintText = "Columns:";
				params.valueType = numericInputValueType::UInt32;
				params.value = static_cast<double>(context.params.value.cols);
				params.minValue = 1.0;
				params.maxValue = static_cast<double>(std::numeric_limits<std::uint32_t>::max());
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.cols = static_cast<std::uint32_t>(numericInputNormalizeValue(
						numericInputValueType::UInt32,
						changed,
						1.0,
						static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
					layoutEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/columns")
					.setParameters(std::move(params))
					.draw();
			});

			layoutEditorDrawInputRow(context, "row-gap", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.gapInput;
				params.hintText = "Gap:";
				params.valueType = numericInputValueType::Float;
				params.value = static_cast<double>(context.params.value.gap);
				params.minValue = 0.0;
				params.maxValue = 1000000.0;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.gap = static_cast<float>(changed);
					layoutEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/gap")
					.setParameters(std::move(params))
					.draw();
			});
		};
	},
};
