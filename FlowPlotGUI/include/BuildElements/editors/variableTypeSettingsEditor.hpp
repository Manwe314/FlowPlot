#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "BuildElements/editors/histogramConfigEditor.hpp"
#include "BuildElements/editors/histogramMappingEditor.hpp"
#include "BuildElements/editors/histogramStatsEditor.hpp"
#include "BuildElements/editors/histogramStyleEditor.hpp"
#include "BuildElements/editors/scatterConfigEditor.hpp"
#include "BuildElements/editors/scatterMappingEditor.hpp"
#include "BuildElements/editors/scatterStatsEditor.hpp"
#include "BuildElements/editors/scatterStyleEditor.hpp"
#include "FlowPlot_Defaults.hpp"
#include "FlowPlotGui.hpp"

enum class variableLayerType : std::uint8_t {
	Scatter,
	Histogram,
};

FLOWUI_DEV_REGISTER_ENUM(
	variableLayerType,
	FLOWUI_DEV_ENUM_VALUE(variableLayerType::Scatter),
	FLOWUI_DEV_ENUM_VALUE(variableLayerType::Histogram));

struct variableTypeSettingsEditorParams {
	std::string hintText = "Variable Type Settings";
	variableLayerType layerType = variableLayerType::Scatter;
	FlowPlot::Spec::LayerSpec value{};
	std::function<void(FlowPlot::Spec::LayerSpec)> onChange = nullptr;

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
	Clay_Sizing insetSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(0), .height = CLAY_SIZING_PERCENT(1.0f)};

	scatterMappingEditorParams scatterMappings{};
	histogramMappingEditorParams histogramMappings{};
	scatterStyleEditorParams scatterStyle{};
	histogramStyleEditorParams histogramStyle{};
	scatterStatsEditorParams scatterStats{};
	histogramStatsEditorParams histogramStats{};
	scatterConfigEditorParams scatterConfig{};
	histogramConfigEditorParams histogramConfig{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	variableTypeSettingsEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, layerType),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, defaultExpanded),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, cardCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, headerSizing),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, headerPadding),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, headerBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, fontId),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, fontSize),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, textColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, textWrapMode),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, textAlignment),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureButtonSizing),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureButtonPadding),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureButtonBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureButtonHoverBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureButtonCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureButtonBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureButtonBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureIconContainerSizing),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, disclosureIconTintColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, rowSizing),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, rowPadding),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, rowChildGap),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, rowBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, insetSizing),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, scatterMappings),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, histogramMappings),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, scatterStyle),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, histogramStyle),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, scatterStats),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, histogramStats),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, scatterConfig),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorParams, histogramConfig));

struct variableTypeSettingsEditorState {
	bool initialized = false;
	bool isExpanded = true;
};

FLOWUI_DEV_REGISTER_STRUCT(
	variableTypeSettingsEditorState,
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorState, initialized),
	FLOWUI_DEV_REFLECT_FIELD(variableTypeSettingsEditorState, isExpanded));

struct variableTypeSettingsEditorResources {
	variableTypeSettingsEditorResources() = default;
	explicit variableTypeSettingsEditorResources(FlowUi::App& app) :
		downArrowIcon(app.icons().textureRef("DownArrow")),
		upArrowIcon(app.icons().textureRef("UpArrow"))
	{}

	FlowUi::TextureRef downArrowIcon = FlowUi::TextureRef{};
	FlowUi::TextureRef upArrowIcon = FlowUi::TextureRef{};
};

using VariableTypeSettingsEditorDef = FlowUi::ElementDefinition<
	variableTypeSettingsEditorParams,
	variableTypeSettingsEditorState,
	variableTypeSettingsEditorResources,
	FLOW_DEF_ID("VariableTypeSettingsEditor")>;
using VariableTypeSettingsEditorInteractionContext = VariableTypeSettingsEditorDef::InteractionContext;

inline void variableTypeSettingsEditorEmit(
	FlowPlot::Spec::LayerSpec nextSpec,
	const std::function<void(FlowPlot::Spec::LayerSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

template <typename DrawChildFn>
inline void variableTypeSettingsEditorDrawInputRow(
	VariableTypeSettingsEditorDef::BuildContext& context,
	std::string_view localId,
	const variableTypeSettingsEditorParams& params,
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

inline const VariableTypeSettingsEditorDef kVariableTypeSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](VariableTypeSettingsEditorInteractionContext& context) {
		variableTypeSettingsEditorState& state =
			VariableTypeSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}
	},
	nullptr,
	+[](VariableTypeSettingsEditorDef::BuildContext& context) {
		variableTypeSettingsEditorState& state =
			VariableTypeSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.defaultExpanded;
		}

		FlowUi::TextureRef disclosureIcon{};
		if (VariableTypeSettingsEditorDef::resources.has_value())
		{
			disclosureIcon = state.isExpanded
				? VariableTypeSettingsEditorDef::resources->downArrowIcon
				: VariableTypeSettingsEditorDef::resources->upArrowIcon;
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
		spacer.layout.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_PERCENT(1.0f)};

		basicButtonParams disclosureParams{};
		disclosureParams.icon = disclosureIcon;
		disclosureParams.contentMode = basicButtonParams::ContentMode::IconOnly;
		disclosureParams.onPressedCallback = [elementFlowId = FlowUi::toFlowId(context.elementID)](BasicButtonInteractionContext) {
			variableTypeSettingsEditorState* latestState = VariableTypeSettingsEditorDef::tryGetState(elementFlowId);
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

			if (state.isExpanded)
			{
			variableTypeSettingsEditorDrawInputRow(context, "row-mappings", context.params, [&](const std::string& rowPath) {
				if (context.params.layerType == variableLayerType::Scatter)
				{
					scatterMappingEditorParams params = context.params.scatterMappings;
					params.value = context.params.value.scatterMapping;
					params.shell.fontId = context.params.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::ScatterMappingSpec changed) mutable {
						value.scatterMapping = std::move(changed);
						variableTypeSettingsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kScatterMappingSettingsEditor, rowPath + "/scatter-mappings")
						.setParameters(std::move(params))
						.draw();
				}
				else
				{
					histogramMappingEditorParams params = context.params.histogramMappings;
					params.value = context.params.value.histogramMapping;
					params.shell.fontId = context.params.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::HistogramMappingSpec changed) mutable {
						value.histogramMapping = std::move(changed);
						variableTypeSettingsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kHistogramMappingSettingsEditor, rowPath + "/histogram-mappings")
						.setParameters(std::move(params))
						.draw();
				}
			});

			variableTypeSettingsEditorDrawInputRow(context, "row-style", context.params, [&](const std::string& rowPath) {
				if (context.params.layerType == variableLayerType::Scatter)
				{
					scatterStyleEditorParams params = context.params.scatterStyle;
					params.value = context.params.value.scatterStyle;
					params.shell.fontId = context.params.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::ScatterStyleSpec changed) mutable {
						value.scatterStyle = std::move(changed);
						variableTypeSettingsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kScatterStyleSettingsEditor, rowPath + "/scatter-style")
						.setParameters(std::move(params))
						.draw();
				}
				else
				{
					histogramStyleEditorParams params = context.params.histogramStyle;
					params.value = context.params.value.histogramStyle;
					params.shell.fontId = context.params.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::HistogramStyleSpec changed) mutable {
						value.histogramStyle = std::move(changed);
						variableTypeSettingsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kHistogramStyleSettingsEditor, rowPath + "/histogram-style")
						.setParameters(std::move(params))
						.draw();
				}
			});

			variableTypeSettingsEditorDrawInputRow(context, "row-stats", context.params, [&](const std::string& rowPath) {
				if (context.params.layerType == variableLayerType::Scatter)
				{
					scatterStatsEditorParams params = context.params.scatterStats;
					params.value = context.params.value.scatterStats;
					params.shell.fontId = context.params.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::ScatterStatsSpec changed) mutable {
						value.scatterStats = std::move(changed);
						variableTypeSettingsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kScatterStatsSettingsEditor, rowPath + "/scatter-stats")
						.setParameters(std::move(params))
						.draw();
				}
				else
				{
					histogramStatsEditorParams params = context.params.histogramStats;
					params.value = context.params.value.histogramStats;
					params.shell.fontId = context.params.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::HistogramStatsSpec changed) mutable {
						value.histogramStats = std::move(changed);
						variableTypeSettingsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kHistogramStatsSettingsEditor, rowPath + "/histogram-stats")
						.setParameters(std::move(params))
						.draw();
				}
			});

			variableTypeSettingsEditorDrawInputRow(context, "row-config", context.params, [&](const std::string& rowPath) {
				if (context.params.layerType == variableLayerType::Scatter)
				{
					scatterConfigEditorParams params = context.params.scatterConfig;
					params.value = context.params.value.scatterConfig;
					params.shell.fontId = context.params.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::ScatterConfigSpec changed) mutable {
						value.scatterConfig = std::move(changed);
						variableTypeSettingsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kScatterConfigSettingsEditor, rowPath + "/scatter-config")
						.setParameters(std::move(params))
						.draw();
				}
				else
				{
					histogramConfigEditorParams params = context.params.histogramConfig;
					params.value = context.params.value.histogramConfig;
					params.shell.fontId = context.params.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::HistogramConfigSpec changed) mutable {
						value.histogramConfig = std::move(changed);
						variableTypeSettingsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kHistogramConfigSettingsEditor, rowPath + "/histogram-config")
						.setParameters(std::move(params))
						.draw();
				}
			});
			}
		};
	},
};
