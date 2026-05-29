#pragma once

#include <array>
#include <cmath>
#include <functional>
#include <iomanip>
#include <limits>
#include <span>
#include <sstream>
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
#include "BuildElements/editors/basicInputs/twoColumnInput.hpp"
#include "FlowPlot_Defaults.hpp"
#include "FlowPlotGui.hpp"

inline std::span<const std::string> mappingSettingsTypeOptions()
{
	static const std::array<std::string, 2> options{"bins", "contiguous"};
	return options;
}

inline std::span<const std::string> scatterMarkerShapeOptions()
{
	static const std::array<std::string, 4> options{"circle", "square", "diamond", "triangle"};
	return options;
}

inline std::span<const std::string> histogramDataAxisOptions()
{
	static const std::array<std::string, 2> options{"X", "Y"};
	return options;
}

inline std::string histogramDataAxisToString(FlowPlot::Spec::HistogramDataAxis axis)
{
	return axis == FlowPlot::Spec::HistogramDataAxis::Y ? "Y" : "X";
}

inline FlowPlot::Spec::HistogramDataAxis histogramDataAxisFromString(std::string_view text)
{
	return text == "Y" || text == "y"
		? FlowPlot::Spec::HistogramDataAxis::Y
		: FlowPlot::Spec::HistogramDataAxis::X;
}

inline bool mappingSettingsUsesBins(std::string_view type)
{
	return type.empty() || type == "bins";
}

inline std::string mappingSettingsFloatToText(float value)
{
	if (!std::isfinite(value))
	{
		return "0";
	}
	std::ostringstream stream{};
	stream << std::setprecision(7) << value;
	std::string text = stream.str();
	if (text.find('.') != std::string::npos)
	{
		while (!text.empty() && text.back() == '0')
		{
			text.pop_back();
		}
		if (!text.empty() && text.back() == '.')
		{
			text.pop_back();
		}
	}
	return text.empty() ? "0" : text;
}

inline std::vector<std::string> mappingSettingsFloatVectorToStrings(const std::vector<float>& values)
{
	std::vector<std::string> textValues{};
	textValues.reserve(values.size());
	for (const float value : values)
	{
		textValues.push_back(mappingSettingsFloatToText(value));
	}
	return textValues;
}

inline float mappingSettingsParseFloatOrZero(std::string_view text)
{
	double parsed = 0.0;
	if (!numericInputTryParseDouble(text, parsed))
	{
		return 0.0f;
	}
	const double normalized = numericInputNormalizeValue(
		numericInputValueType::Float,
		parsed,
		-static_cast<double>(std::numeric_limits<float>::max()),
		static_cast<double>(std::numeric_limits<float>::max()));
	return static_cast<float>(normalized);
}

inline void mappingSettingsPrepareStringInputCard(stringInputCardParams& params)
{
	params.cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	params.cardLayout = CLAY_TOP_TO_BOTTOM;
	params.textWrapMode = CLAY_TEXT_WRAP_WORDS;
}

inline void mappingSettingsPrepareEnumPickerCard(enumPickerCardParams& params)
{
	params.cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	params.cardLayout = CLAY_LEFT_TO_RIGHT;
	params.textWrapMode = CLAY_TEXT_WRAP_NONE;
	params.menu.sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(100.0f, 180.0f), .height = CLAY_SIZING_FIT(0)};
	params.menu.dropdownSizing = Clay_Sizing{.width = CLAY_SIZING_FIT(120.0f, 220.0f), .height = CLAY_SIZING_FIT(0)};
}

inline void mappingSettingsPrepareNumericInputCard(numericInputCardParams& params)
{
	params.cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	params.cardLayout = CLAY_LEFT_TO_RIGHT;
	params.textWrapMode = CLAY_TEXT_WRAP_NONE;
	params.field.sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(120.0f, 180.0f), .height = CLAY_SIZING_FIT(0)};
}

inline void mappingSettingsPrepareColorPickerCard(colorPickerCardParams& params)
{
	params.cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	params.cardLayout = CLAY_LEFT_TO_RIGHT;
	params.textWrapMode = CLAY_TEXT_WRAP_NONE;
	params.swatch.sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
}

inline void mappingSettingsPrepareTwoColumnInputCard(twoColumnInputCardParams& params)
{
	params.cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0, 360)};
	params.cardLayout = CLAY_TOP_TO_BOTTOM;
	params.textWrapMode = CLAY_TEXT_WRAP_WORDS;
	params.table.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	params.table.backgroundColor = FlowUi::Flow_Color("#11161dcc");
	params.table.borderColor = FlowUi::Flow_Color("#2b3340ff");
	params.table.borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	params.table.firstColumnSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	params.table.secondColumnSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
}

struct mappingSettingsEditorShellParams {
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
};

FLOWUI_DEV_REGISTER_STRUCT(
	mappingSettingsEditorShellParams,
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, defaultExpanded),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, cardCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, headerSizing),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, headerPadding),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, headerBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, disclosureIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, rowSizing),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, rowPadding),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, rowChildGap),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, rowBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorShellParams, insetSizing));

struct mappingSettingsEditorState {
	bool initialized = false;
	bool isExpanded = true;
};

FLOWUI_DEV_REGISTER_STRUCT(
	mappingSettingsEditorState,
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(mappingSettingsEditorState, isExpanded));

struct mappingSettingsEditorResources {
	mappingSettingsEditorResources() = default;
	explicit mappingSettingsEditorResources(FlowUi::App& app) :
		downArrowIcon(app.icons().textureRef("DownArrow")),
		upArrowIcon(app.icons().textureRef("UpArrow"))
	{}

	FlowUi::TextureRef downArrowIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef upArrowIcon = FlowUi::TextureRef{};
};

template <typename DefinitionT, typename BuildContextT>
inline bool mappingSettingsEditorBegin(
	BuildContextT& context,
	const mappingSettingsEditorShellParams& params,
	std::string_view hintText)
{
	mappingSettingsEditorState& state = DefinitionT::getOrCreateState(FlowUi::toFlowId(context.elementID));
	if (!state.initialized)
	{
		state.initialized = true;
		state.isExpanded = params.defaultExpanded;
	}

	FlowUi::TextureRef disclosureIcon{};
	if (DefinitionT::resources.has_value())
	{
		disclosureIcon = state.isExpanded
			? DefinitionT::resources->downArrowIcon
			: DefinitionT::resources->upArrowIcon;
	}

	const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
	const Clay_ElementId headerId = context.uiManager.toClayEID(context.createChildElementId("header"));
	const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("header/hint"));
	const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("header/spacer"));
	const std::string disclosurePath = context.createChildElementId("header/disclosure");

	Clay_ElementDeclaration root{};
	root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	root.layout.sizing = params.cardSizing;
	root.layout.padding = params.cardPadding;
	root.layout.childGap = params.cardChildGap;
	root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
	root.backgroundColor = params.cardBackgroundColor;
	root.cornerRadius = params.cardCornerRadius;
	root.border = {.color = params.cardBorderColor, .width = params.cardBorderWidth};

	Clay_ElementDeclaration header{};
	header.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
	header.layout.sizing = params.headerSizing;
	header.layout.padding = params.headerPadding;
	header.layout.childGap = 0;
	header.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	header.backgroundColor = params.headerBackgroundColor;

	Clay_TextElementConfig hintTextConfig{};
	hintTextConfig.textColor = params.textColor;
	hintTextConfig.fontSize = params.fontSize;
	hintTextConfig.wrapMode = params.textWrapMode;
	hintTextConfig.textAlignment = params.textAlignment;
	hintTextConfig.fontId = params.fontId;

	Clay_ElementDeclaration spacer{};
	spacer.layout.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_PERCENT(1.0f)};

	basicButtonParams disclosureParams{};
	disclosureParams.icon = disclosureIcon;
	disclosureParams.contentMode = basicButtonParams::ContentMode::IconOnly;
	disclosureParams.onPressedCallback = [elementFlowId = FlowUi::toFlowId(context.elementID)](BasicButtonInteractionContext) {
		mappingSettingsEditorState* latestState = DefinitionT::tryGetState(elementFlowId);
		if (latestState != nullptr)
		{
			latestState->isExpanded = !latestState->isExpanded;
		}
	};
	disclosureParams.padding = params.disclosureButtonPadding;
	disclosureParams.sizing = params.disclosureButtonSizing;
	disclosureParams.backgroundColor = params.disclosureButtonBackgroundColor;
	disclosureParams.hoverBackgroundColor = params.disclosureButtonHoverBackgroundColor;
	disclosureParams.cornerRadius = params.disclosureButtonCornerRadius;
	disclosureParams.borderColor = params.disclosureButtonBorderColor;
	disclosureParams.borderWidth = params.disclosureButtonBorderWidth;
	disclosureParams.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	disclosureParams.iconContainerSizing = params.disclosureIconContainerSizing;
	disclosureParams.iconTintColor = params.disclosureIconTintColor;

	CLAY(rootId, root)
	{
		CLAY(headerId, header)
		{
			CLAY(hintId, {})
			{
				CLAY_TEXT(
					context.uiManager.toClayString(std::string(hintText)),
					CLAY_TEXT_CONFIG(hintTextConfig)
				);
			};
			CLAY(spacerId, spacer) {};
			context.uiManager.createElement(kBasicButton, disclosurePath)
				.setParameters(std::move(disclosureParams))
				.draw();
		};
	};

	return state.isExpanded;
}

template <typename DefinitionT, typename BuildContextT, typename DrawExpandedFn>
inline void mappingSettingsEditorDrawShell(
	BuildContextT& context,
	const mappingSettingsEditorShellParams& params,
	std::string_view hintText,
	DrawExpandedFn&& drawExpanded)
{
	mappingSettingsEditorState& state = DefinitionT::getOrCreateState(FlowUi::toFlowId(context.elementID));
	if (!state.initialized)
	{
		state.initialized = true;
		state.isExpanded = params.defaultExpanded;
	}

	FlowUi::TextureRef disclosureIcon{};
	if (DefinitionT::resources.has_value())
	{
		disclosureIcon = state.isExpanded
			? DefinitionT::resources->downArrowIcon
			: DefinitionT::resources->upArrowIcon;
	}

	const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
	const Clay_ElementId headerId = context.uiManager.toClayEID(context.createChildElementId("header"));
	const Clay_ElementId hintId = context.uiManager.toClayEID(context.createChildElementId("header/hint"));
	const Clay_ElementId spacerId = context.uiManager.toClayEID(context.createChildElementId("header/spacer"));
	const std::string disclosurePath = context.createChildElementId("header/disclosure");

	Clay_ElementDeclaration root{};
	root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	root.layout.sizing = params.cardSizing;
	root.layout.padding = params.cardPadding;
	root.layout.childGap = params.cardChildGap;
	root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
	root.backgroundColor = params.cardBackgroundColor;
	root.cornerRadius = params.cardCornerRadius;
	root.border = {.color = params.cardBorderColor, .width = params.cardBorderWidth};

	Clay_ElementDeclaration header{};
	header.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
	header.layout.sizing = params.headerSizing;
	header.layout.padding = params.headerPadding;
	header.layout.childGap = 0;
	header.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	header.backgroundColor = params.headerBackgroundColor;

	Clay_TextElementConfig hintTextConfig{};
	hintTextConfig.textColor = params.textColor;
	hintTextConfig.fontSize = params.fontSize;
	hintTextConfig.wrapMode = params.textWrapMode;
	hintTextConfig.textAlignment = params.textAlignment;
	hintTextConfig.fontId = params.fontId;

	Clay_ElementDeclaration spacer{};
	spacer.layout.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_PERCENT(1.0f)};

	basicButtonParams disclosureParams{};
	disclosureParams.icon = disclosureIcon;
	disclosureParams.contentMode = basicButtonParams::ContentMode::IconOnly;
	disclosureParams.onPressedCallback = [elementFlowId = FlowUi::toFlowId(context.elementID)](BasicButtonInteractionContext) {
		mappingSettingsEditorState* latestState = DefinitionT::tryGetState(elementFlowId);
		if (latestState != nullptr)
		{
			latestState->isExpanded = !latestState->isExpanded;
		}
	};
	disclosureParams.padding = params.disclosureButtonPadding;
	disclosureParams.sizing = params.disclosureButtonSizing;
	disclosureParams.backgroundColor = params.disclosureButtonBackgroundColor;
	disclosureParams.hoverBackgroundColor = params.disclosureButtonHoverBackgroundColor;
	disclosureParams.cornerRadius = params.disclosureButtonCornerRadius;
	disclosureParams.borderColor = params.disclosureButtonBorderColor;
	disclosureParams.borderWidth = params.disclosureButtonBorderWidth;
	disclosureParams.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	disclosureParams.iconContainerSizing = params.disclosureIconContainerSizing;
	disclosureParams.iconTintColor = params.disclosureIconTintColor;

	CLAY(rootId, root)
	{
		CLAY(headerId, header)
		{
			CLAY(hintId, {})
			{
				CLAY_TEXT(
					context.uiManager.toClayString(std::string(hintText)),
					CLAY_TEXT_CONFIG(hintTextConfig)
				);
			};
			CLAY(spacerId, spacer) {};
			context.uiManager.createElement(kBasicButton, disclosurePath)
				.setParameters(std::move(disclosureParams))
				.draw();
		};

		if (state.isExpanded)
		{
			drawExpanded();
		}
	};
}

template <typename BuildContextT, typename DrawChildFn>
inline void mappingSettingsEditorDrawInputRow(
	BuildContextT& context,
	std::string_view localId,
	const mappingSettingsEditorShellParams& params,
	DrawChildFn&& drawChild)
{
	const std::string rowPath = context.createChildElementId(localId);
	const Clay_ElementId rowId = context.uiManager.toClayEID(rowPath);
	const Clay_ElementId insetId = context.uiManager.toClayEID(rowPath + "/inset");
	const Clay_ElementId contentId = context.uiManager.toClayEID(rowPath + "/content");

	Clay_ElementDeclaration row{};
	row.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
	row.layout.sizing = params.rowSizing;
	row.layout.padding = params.rowPadding;
	row.layout.childGap = params.rowChildGap;
	row.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
	row.backgroundColor = params.rowBackgroundColor;
	row.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

	Clay_ElementDeclaration inset{};
	inset.layout.sizing = params.insetSizing;
	inset.backgroundColor = FlowUi::Flow_Color("#00000000");

	Clay_ElementDeclaration content{};
	content.layout.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	content.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	content.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
	content.backgroundColor = FlowUi::Flow_Color("#00000000");

	CLAY(rowId, row)
	{
		CLAY(insetId, inset) {};
		CLAY(contentId, content)
		{
			drawChild(rowPath);
		};
	};
}
