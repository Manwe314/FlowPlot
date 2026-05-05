#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <limits>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/basicInputs/stringInput.hpp"
#include "BuildElements/editors/basicInputs/toggle.hpp"
#include "FlowPlot_Defaults.hpp"
#include "FlowPlotGui.hpp"

inline std::span<const std::string> tickSettingsEditorDefaultFontFamilyOptions()
{
	static const std::array<std::string, 1> options{"Default"};
	return options;
}

inline std::span<const std::string> tickSettingsEditorDefaultFontWeightOptions()
{
	static const std::array<std::string, 1> options{"400"};
	return options;
}

inline std::span<const std::string> tickSettingsEditorDefaultFontStyleOptions()
{
	static const std::array<std::string, 1> options{"normal"};
	return options;
}

struct tickSettingsEditorParams {
	std::string hintText = "Tick Settings";
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
	std::vector<std::string> fontFamilyOptions{};
	std::vector<std::string> fontWeightOptions{};
	std::vector<std::string> fontStyleOptions{};

	colorPickerCardParams lineColorInput{};
	numericInputCardParams lineWidthInput{};
	numericInputCardParams lineLengthInput{};
	numericInputCardParams countInput{};
	numericInputCardParams gapToLabelInput{};
	enumPickerCardParams labelFontFamilyInput{};
	numericInputCardParams labelFontSizeInput{};
	enumPickerCardParams labelFontWeightInput{};
	enumPickerCardParams labelFontStyleInput{};
	colorPickerCardParams labelColorInput{};
	toggleCardParams showMinorTicksInput{};
	numericInputCardParams minorTickCountInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	tickSettingsEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, defaultExpanded),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, cardCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, headerSizing),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, headerPadding),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, headerBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, disclosureIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, rowSizing),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, rowPadding),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, rowChildGap),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, rowBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, insetSizing),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, lineColorInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, lineWidthInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, lineLengthInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, countInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, gapToLabelInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, labelFontFamilyInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, labelFontSizeInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, labelFontWeightInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, labelFontStyleInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, labelColorInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, showMinorTicksInput),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorParams, minorTickCountInput));

struct tickSettingsEditorState {
	bool initialized = false;
	bool isExpanded = true;
};

FLOWUI_DEV_REGISTER_STRUCT(
	tickSettingsEditorState,
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(tickSettingsEditorState, isExpanded));

struct tickSettingsEditorResources {
	tickSettingsEditorResources() = default;
	explicit tickSettingsEditorResources(FlowUi::App& app) :
		downArrowIcon(app.icons().textureRef("DownArrow")),
		upArrowIcon(app.icons().textureRef("UpArrow"))
	{}

	FlowUi::TextureRef downArrowIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef upArrowIcon = FlowUi::TextureRef{};
};

using TickSettingsEditorDef = FlowUi::ElementDefinition<
	tickSettingsEditorParams,
	tickSettingsEditorState,
	tickSettingsEditorResources,
	FLOW_DEF_ID("TickSettingsEditor")>;
using TickSettingsEditorInteractionContext = TickSettingsEditorDef::InteractionContext;

inline void tickSettingsEditorEmit(
	FlowPlot::Spec::AxisSpec nextSpec,
	const std::function<void(FlowPlot::Spec::AxisSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

template <typename DrawChildFn>
inline void tickSettingsEditorDrawInputRow(
	TickSettingsEditorDef::BuildContext& context,
	std::string_view localId,
	const tickSettingsEditorParams& params,
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

inline const TickSettingsEditorDef kTickSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](TickSettingsEditorInteractionContext& context) {
		tickSettingsEditorState& state = TickSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}
	},
	nullptr,
	+[](TickSettingsEditorDef::BuildContext& context) {
		tickSettingsEditorState& state = TickSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}

		FlowUi::TextureRef disclosureIcon{};
		if (TickSettingsEditorDef::resources.has_value())
		{
			disclosureIcon = state.isExpanded
				? TickSettingsEditorDef::resources->downArrowIcon
				: TickSettingsEditorDef::resources->upArrowIcon;
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
			tickSettingsEditorState* latestState = TickSettingsEditorDef::tryGetState(elementFlowId);
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

		const std::span<const std::string> fontFamilyOptions = context.params.fontFamilyOptions.empty()
			? tickSettingsEditorDefaultFontFamilyOptions()
			: std::span<const std::string>(context.params.fontFamilyOptions);
		const std::span<const std::string> fontWeightOptions = context.params.fontWeightOptions.empty()
			? tickSettingsEditorDefaultFontWeightOptions()
			: std::span<const std::string>(context.params.fontWeightOptions);
		const std::span<const std::string> fontStyleOptions = context.params.fontStyleOptions.empty()
			? tickSettingsEditorDefaultFontStyleOptions()
			: std::span<const std::string>(context.params.fontStyleOptions);

		tickSettingsEditorDrawInputRow(context, "row-line-color", context.params, [&](const std::string& rowPath) {
				colorPickerCardParams params = context.params.lineColorInput;
				params.hintText = "Line Color";
				params.value = context.params.value.tickColor;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.tickColor = std::string(changed);
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kColorPickerCard, rowPath + "/line-color").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-line-width", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.lineWidthInput;
				params.hintText = "Line Width";
				params.valueType = numericInputValueType::Float;
				params.value = static_cast<double>(context.params.value.tickWidth);
				params.minValue = 0.0;
				params.maxValue = 1000000.0;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.tickWidth = static_cast<float>(changed);
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/line-width").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-line-length", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.lineLengthInput;
				params.hintText = "Line Length";
				params.valueType = numericInputValueType::Float;
				params.value = static_cast<double>(context.params.value.tickLength);
				params.minValue = 0.0;
				params.maxValue = 1000000.0;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.tickLength = static_cast<float>(changed);
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/line-length").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-count", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.countInput;
				params.hintText = "Count";
				params.valueType = numericInputValueType::UInt32;
				params.value = static_cast<double>(context.params.value.tickCount);
				params.minValue = 0.0;
				params.maxValue = static_cast<double>(std::numeric_limits<std::uint32_t>::max());
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.tickCount = static_cast<std::uint32_t>(numericInputNormalizeValue(
						numericInputValueType::UInt32,
						changed,
						0.0,
						static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/count").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-gap-to-label", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.gapToLabelInput;
				params.hintText = "Gap to Label";
				params.valueType = numericInputValueType::Float;
				params.value = static_cast<double>(context.params.value.tickValueGap);
				params.minValue = 0.0;
				params.maxValue = 1000000.0;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.tickValueGap = static_cast<float>(changed);
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/gap-to-label").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-label-font-family", context.params, [&](const std::string& rowPath) {
				enumPickerCardParams params = context.params.labelFontFamilyInput;
				params.hintText = "Label Font Family";
				params.options = fontFamilyOptions;
				params.value = context.params.value.tickLabelFontFamily;
				params.defaultValue = "Default";
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.tickLabelFontFamily = std::string(changed);
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kEnumPickerCard, rowPath + "/label-font-family").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-label-font-size", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.labelFontSizeInput;
				params.hintText = "Label Font Size";
				params.valueType = numericInputValueType::Float;
				params.value = static_cast<double>(context.params.value.tickLabelFontSize);
				params.minValue = 0.0;
				params.maxValue = 10000.0;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.tickLabelFontSize = static_cast<float>(changed);
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/label-font-size").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-label-font-weight", context.params, [&](const std::string& rowPath) {
				enumPickerCardParams params = context.params.labelFontWeightInput;
				params.hintText = "Label Font Weight";
				params.options = fontWeightOptions;
				params.value = std::to_string(context.params.value.tickLabelFontWeight);
				params.defaultValue = "400";
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					try
					{
						const unsigned long parsed = std::stoul(std::string(changed));
						value.tickLabelFontWeight = static_cast<std::uint16_t>(std::min<unsigned long>(
							parsed,
							static_cast<unsigned long>(std::numeric_limits<std::uint16_t>::max())));
					}
					catch (...)
					{
						value.tickLabelFontWeight = 400;
					}
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kEnumPickerCard, rowPath + "/label-font-weight").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-label-font-style", context.params, [&](const std::string& rowPath) {
				enumPickerCardParams params = context.params.labelFontStyleInput;
				params.hintText = "Label Font Style";
				params.options = fontStyleOptions;
				params.value = context.params.value.tickLabelFontStyle;
				params.defaultValue = "normal";
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.tickLabelFontStyle = std::string(changed);
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kEnumPickerCard, rowPath + "/label-font-style").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-label-color", context.params, [&](const std::string& rowPath) {
				colorPickerCardParams params = context.params.labelColorInput;
				params.hintText = "Label Color";
				params.value = context.params.value.tickLabelColor;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.tickLabelColor = std::string(changed);
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kColorPickerCard, rowPath + "/label-color").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-show-minor-ticks", context.params, [&](const std::string& rowPath) {
				toggleCardParams params = context.params.showMinorTicksInput;
				params.hintText = "Show Minor Ticks";
				params.value = context.params.value.showMinorTicks;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](bool changed) mutable {
					value.showMinorTicks = changed;
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kToggleCard, rowPath + "/show-minor-ticks").setParameters(std::move(params)).draw();
			});

			tickSettingsEditorDrawInputRow(context, "row-minor-tick-count", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.minorTickCountInput;
				params.hintText = "Minor Tick Count";
				params.valueType = numericInputValueType::UInt32;
				params.value = static_cast<double>(context.params.value.minorTickCount);
				params.minValue = 0.0;
				params.maxValue = static_cast<double>(std::numeric_limits<std::uint32_t>::max());
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.minorTickCount = static_cast<std::uint32_t>(numericInputNormalizeValue(
						numericInputValueType::UInt32,
						changed,
						0.0,
						static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
					tickSettingsEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/minor-tick-count").setParameters(std::move(params)).draw();
			});
		};
	},
};
