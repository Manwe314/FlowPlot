#pragma once

#include <array>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

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

inline std::span<const std::string> textSpecEditorDefaultOverflowOptions()
{
	static const std::array<std::string, 3> options{"clip", "visible", "ellipsis"};
	return options;
}

inline std::span<const std::string> textSpecEditorDefaultHorizontalAlignOptions()
{
	static const std::array<std::string, 3> options{"left", "center", "right"};
	return options;
}

inline std::span<const std::string> textSpecEditorDefaultVerticalAlignOptions()
{
	static const std::array<std::string, 3> options{"top", "middle", "bottom"};
	return options;
}

struct textSpecEditorParams {
	std::string hintText = "Text";
	FlowPlot::Spec::TextSpec value{};
	std::function<void(FlowPlot::Spec::TextSpec)> onChange = nullptr;

	std::span<const std::string> overflowOptions{};
	std::span<const std::string> horizontalAlignOptions{};
	std::span<const std::string> verticalAlignOptions{};

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

	toggleCardParams visibleToggle{};
	stringInputCardParams textInput{};
	stringInputCardParams fontFamilyInput{};
	numericInputCardParams fontSizeInput{};
	numericInputCardParams fontWeightInput{};
	colorPickerCardParams textColorInput{};
	enumPickerCardParams overflowInput{};
	enumPickerCardParams horizontalAlignInput{};
	enumPickerCardParams verticalAlignInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	textSpecEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, defaultExpanded),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, cardCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, headerSizing),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, headerPadding),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, headerBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, disclosureIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, rowSizing),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, rowPadding),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, rowChildGap),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, rowBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, insetSizing),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, visibleToggle),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, textInput),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, fontFamilyInput),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, fontSizeInput),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, fontWeightInput),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, textColorInput),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, overflowInput),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, horizontalAlignInput),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorParams, verticalAlignInput));

struct textSpecEditorState {
	bool initialized = false;
	bool isExpanded = true;
};

FLOWUI_DEV_REGISTER_STRUCT(
	textSpecEditorState,
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(textSpecEditorState, isExpanded));

struct textSpecEditorResources {
	textSpecEditorResources() = default;
	explicit textSpecEditorResources(FlowUi::App& app) :
		downArrowIcon(app.icons().textureRef("DownArrow")),
		upArrowIcon(app.icons().textureRef("UpArrow"))
	{}

	FlowUi::TextureRef downArrowIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef upArrowIcon = FlowUi::TextureRef{};
};

using TextSpecEditorDef = FlowUi::ElementDefinition<
	textSpecEditorParams,
	textSpecEditorState,
	textSpecEditorResources,
	FLOW_DEF_ID("TextSpecEditor")>;
using TextSpecEditorInteractionContext = TextSpecEditorDef::InteractionContext;

inline void textSpecEditorEmit(
	FlowPlot::Spec::TextSpec nextSpec,
	const std::function<void(FlowPlot::Spec::TextSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

template <typename DrawChildFn>
inline void textSpecEditorDrawInputRow(
	TextSpecEditorDef::BuildContext& context,
	std::string_view localId,
	const textSpecEditorParams& params,
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

inline const TextSpecEditorDef kTextSpecEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](TextSpecEditorInteractionContext& context) {
		textSpecEditorState& state = TextSpecEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}
	},
	nullptr,
	+[](TextSpecEditorDef::BuildContext& context) {
		textSpecEditorState& state = TextSpecEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}

		FlowUi::TextureRef disclosureIcon{};
		if (TextSpecEditorDef::resources.has_value())
		{
			disclosureIcon = state.isExpanded
				? TextSpecEditorDef::resources->downArrowIcon
				: TextSpecEditorDef::resources->upArrowIcon;
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
			textSpecEditorState* latestState = TextSpecEditorDef::tryGetState(elementFlowId);
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

		const std::span<const std::string> overflowOptions = context.params.overflowOptions.empty()
			? textSpecEditorDefaultOverflowOptions()
			: context.params.overflowOptions;
		const std::span<const std::string> horizontalAlignOptions = context.params.horizontalAlignOptions.empty()
			? textSpecEditorDefaultHorizontalAlignOptions()
			: context.params.horizontalAlignOptions;
		const std::span<const std::string> verticalAlignOptions = context.params.verticalAlignOptions.empty()
			? textSpecEditorDefaultVerticalAlignOptions()
			: context.params.verticalAlignOptions;

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

			textSpecEditorDrawInputRow(context, "row-visible", context.params, [&](const std::string& rowPath) {
				toggleCardParams params = context.params.visibleToggle;
				params.hintText = "Visible";
				params.value = context.params.value.visible;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](bool changed) mutable {
					value.visible = changed;
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kToggleCard, rowPath + "/visible")
					.setParameters(std::move(params))
					.draw();
			});

			textSpecEditorDrawInputRow(context, "row-text", context.params, [&](const std::string& rowPath) {
				stringInputCardParams params = context.params.textInput;
				params.hintText = "Text";
				params.value = context.params.value.text;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.text = std::string(changed);
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kStringInputCard, rowPath + "/text")
					.setParameters(std::move(params))
					.draw();
			});

			textSpecEditorDrawInputRow(context, "row-font-family", context.params, [&](const std::string& rowPath) {
				stringInputCardParams params = context.params.fontFamilyInput;
				params.hintText = "Font Family";
				params.value = context.params.value.fontFamily;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.fontFamily = std::string(changed);
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kStringInputCard, rowPath + "/font-family")
					.setParameters(std::move(params))
					.draw();
			});

			textSpecEditorDrawInputRow(context, "row-font-size", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.fontSizeInput;
				params.hintText = "Font Size";
				params.valueType = numericInputValueType::Float;
				params.value = static_cast<double>(context.params.value.fontSize);
				params.minValue = 0.0;
				params.maxValue = 10000.0;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.fontSize = static_cast<float>(changed);
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/font-size")
					.setParameters(std::move(params))
					.draw();
			});

			textSpecEditorDrawInputRow(context, "row-font-weight", context.params, [&](const std::string& rowPath) {
				numericInputCardParams params = context.params.fontWeightInput;
				params.hintText = "Font Weight";
				params.valueType = numericInputValueType::UInt16;
				params.value = static_cast<double>(context.params.value.fontWeight);
				params.minValue = 0.0;
				params.maxValue = 1000.0;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
					value.fontWeight = static_cast<std::uint16_t>(numericInputNormalizeValue(
						numericInputValueType::UInt16,
						changed,
						0.0,
						1000.0));
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kNumericInputCard, rowPath + "/font-weight")
					.setParameters(std::move(params))
					.draw();
			});

			textSpecEditorDrawInputRow(context, "row-text-color", context.params, [&](const std::string& rowPath) {
				colorPickerCardParams params = context.params.textColorInput;
				params.hintText = "Text Color";
				params.value = context.params.value.color;
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.color = std::string(changed);
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kColorPickerCard, rowPath + "/text-color")
					.setParameters(std::move(params))
					.draw();
			});

			textSpecEditorDrawInputRow(context, "row-overflow", context.params, [&](const std::string& rowPath) {
				enumPickerCardParams params = context.params.overflowInput;
				params.hintText = "Overflow";
				params.options = overflowOptions;
				params.value = context.params.value.overflow;
				params.defaultValue = "clip";
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.overflow = std::string(changed);
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kEnumPickerCard, rowPath + "/overflow")
					.setParameters(std::move(params))
					.draw();
			});

			textSpecEditorDrawInputRow(context, "row-horizontal-align", context.params, [&](const std::string& rowPath) {
				enumPickerCardParams params = context.params.horizontalAlignInput;
				params.hintText = "Horizontal Align";
				params.options = horizontalAlignOptions;
				params.value = context.params.value.hAlign;
				params.defaultValue = "left";
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.hAlign = std::string(changed);
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kEnumPickerCard, rowPath + "/horizontal-align")
					.setParameters(std::move(params))
					.draw();
			});

			textSpecEditorDrawInputRow(context, "row-vertical-align", context.params, [&](const std::string& rowPath) {
				enumPickerCardParams params = context.params.verticalAlignInput;
				params.hintText = "Vertical Align";
				params.options = verticalAlignOptions;
				params.value = context.params.value.vAlign;
				params.defaultValue = "top";
				params.fontId = context.params.fontId;
				params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
					value.vAlign = std::string(changed);
					textSpecEditorEmit(std::move(value), onChange);
				};
				context.uiManager.createElement(kEnumPickerCard, rowPath + "/vertical-align")
					.setParameters(std::move(params))
					.draw();
			});
		};
	},
};
