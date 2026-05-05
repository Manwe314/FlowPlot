#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <limits>
#include <cmath>
#include <optional>
#include <string>
#include <string_view>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicTitle.hpp"
#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/doubleEnumPicker.hpp"
#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "BuildElements/editors/basicInputs/nineSplitInput.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/basicInputs/stringInput.hpp"
#include "BuildElements/editors/basicInputs/toggle.hpp"
#include "BuildElements/editors/gridSettingsEditor.hpp"
#include "BuildElements/editors/layoutEditor.hpp"
#include "BuildElements/editors/lineSettingsEditor.hpp"
#include "BuildElements/editors/textSpecEditor.hpp"
#include "BuildElements/editors/tickSettingsEditor.hpp"
#include "BuildElements/editors/variableTypeSettingsEditor.hpp"
#include "FlowPlotGui.hpp"

struct propertiesContentParams {
	Clay_Padding padding = Clay_Padding{12, 12, 12, 12};
	uint16_t childGap = 8;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");

	FlowPlotGui::state* guiState = nullptr;
};

FLOWUI_DEV_REGISTER_STRUCT(
	propertiesContentParams,
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, borderColor));

using PropertiesContentDef = FlowUi::ElementDefinition<
	propertiesContentParams,
	void,
	void,
	FLOW_DEF_ID("PropertiesContent")>;

namespace FlowPlotGui
{
	inline Clay_Vector2 propertiesContentScrollOffsetForElementId(
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

	inline void drawPropertiesHint(
		FlowUi::UiManager& ui,
		std::string_view elementId,
		std::string text)
	{
		ui.createElement(kBasicTitle, elementId)
			.setParameters({
				.text = std::move(text),
				.padding = Clay_Padding{0, 0, 0, 0},
				.sizing = Clay_Sizing{
					.width = CLAY_SIZING_GROW(0),
					.height = CLAY_SIZING_FIT(0),
				},
				.textWrapMode = CLAY_TEXT_WRAP_WORDS,
				.fontSize = 13,
				.textColor = FlowUi::Flow_Color("#aeb2b8ff"),
			})
			.draw();
	}

	inline std::optional<std::size_t> propertiesContentFindPanelIndex(
		const FlowPlot::Spec::MasterTemplateSpec& spec,
		std::string_view panelId)
	{
		for (std::size_t i = 0; i < spec.panels.size(); ++i)
		{
			if (spec.panels[i].id == panelId)
			{
				return i;
			}
		}
		return std::nullopt;
	}

	inline void propertiesContentApplyTextFontOptions(
		textSpecEditorParams& params,
		const state& guiState,
		const FlowPlot::Spec::TextSpec& text)
	{
		params.fontFamilyOptions = availableFontFamilies(guiState.fontLibrary);
		params.fontWeightOptions = availableWeightsForFamily(guiState.fontLibrary, text.fontFamily);
		params.fontStyleOptions = availableStylesForFamilyWeight(guiState.fontLibrary, text.fontFamily, text.fontWeight);
	}

	inline void propertiesContentApplyTickFontOptions(
		tickSettingsEditorParams& params,
		const state& guiState,
		const FlowPlot::Spec::AxisSpec& axis)
	{
		params.fontFamilyOptions = availableFontFamilies(guiState.fontLibrary);
		params.fontWeightOptions = availableWeightsForFamily(guiState.fontLibrary, axis.tickLabelFontFamily);
		params.fontStyleOptions = availableStylesForFamilyWeight(
			guiState.fontLibrary,
			axis.tickLabelFontFamily,
			axis.tickLabelFontWeight);
	}

	inline std::optional<std::size_t> propertiesContentFindLayerIndex(
		const FlowPlot::Spec::PanelSpec& panel,
		std::string_view layerId)
	{
		for (std::size_t i = 0; i < panel.layers.size(); ++i)
		{
			if (panel.layers[i].id == layerId)
			{
				return i;
			}
		}
		return std::nullopt;
	}

	inline std::optional<std::size_t> propertiesContentFindLegendIndex(
		const FlowPlot::Spec::FigureSpec& figure,
		std::string_view legendId)
	{
		for (std::size_t i = 0; i < figure.legends.size(); ++i)
		{
			if (figure.legends[i].id == legendId)
			{
				return i;
			}
		}
		return std::nullopt;
	}

	inline std::optional<std::size_t> propertiesContentFindLegendElementIndex(
		const FlowPlot::Spec::LegendSpec& legend,
		std::string_view elementId)
	{
		for (std::size_t i = 0; i < legend.legendElements.size(); ++i)
		{
			if (legend.legendElements[i].id == elementId)
			{
				return i;
			}
		}
		return std::nullopt;
	}

	inline Clay_Padding propertiesContentPaddingToClay(const FlowPlot::Spec::PaddingSpec& padding)
	{
		return Clay_Padding{
			.left = static_cast<std::uint16_t>(std::clamp(
				std::lround(padding.left),
				0L,
				static_cast<long>(std::numeric_limits<std::uint16_t>::max()))),
			.right = static_cast<std::uint16_t>(std::clamp(
				std::lround(padding.right),
				0L,
				static_cast<long>(std::numeric_limits<std::uint16_t>::max()))),
			.top = static_cast<std::uint16_t>(std::clamp(
				std::lround(padding.top),
				0L,
				static_cast<long>(std::numeric_limits<std::uint16_t>::max()))),
			.bottom = static_cast<std::uint16_t>(std::clamp(
				std::lround(padding.bottom),
				0L,
				static_cast<long>(std::numeric_limits<std::uint16_t>::max()))),
		};
	}

	inline FlowPlot::Spec::AxisSpec* propertiesContentAxisForKind(
		FlowPlot::Spec::PanelSpec& panel,
		TemplateNodeKind axisKind)
	{
		switch (axisKind)
		{
		case TemplateNodeKind::XAxis:
			return &panel.xAxis;
		case TemplateNodeKind::YAxis:
			return &panel.yAxis;
		case TemplateNodeKind::XSecondaryAxis:
			return &panel.xSecondary;
		case TemplateNodeKind::YSecondaryAxis:
			return &panel.ySecondary;
		default:
			return nullptr;
		}
	}

	inline std::optional<TemplateNodeKind> propertiesContentAxisKindFromSlug(std::string_view slug)
	{
		if (slug == "x-axis")
		{
			return TemplateNodeKind::XAxis;
		}
		if (slug == "y-axis")
		{
			return TemplateNodeKind::YAxis;
		}
		if (slug == "x-secondary-axis")
		{
			return TemplateNodeKind::XSecondaryAxis;
		}
		if (slug == "y-secondary-axis")
		{
			return TemplateNodeKind::YSecondaryAxis;
		}
		return std::nullopt;
	}

	inline const FlowPlot::Spec::AxisSpec* propertiesContentAxisForKind(
		const FlowPlot::Spec::PanelSpec& panel,
		TemplateNodeKind axisKind)
	{
		switch (axisKind)
		{
		case TemplateNodeKind::XAxis:
			return &panel.xAxis;
		case TemplateNodeKind::YAxis:
			return &panel.yAxis;
		case TemplateNodeKind::XSecondaryAxis:
			return &panel.xSecondary;
		case TemplateNodeKind::YSecondaryAxis:
			return &panel.ySecondary;
		default:
			return nullptr;
		}
	}

	inline void propertiesContentSetPanelAxis(
		state& guiState,
		std::string_view panelId,
		TemplateNodeKind axisKind,
		FlowPlot::Spec::AxisSpec changed)
	{
		if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
		{
			if (FlowPlot::Spec::AxisSpec* axis =
					propertiesContentAxisForKind(guiState.activeTemplate.panels[*index], axisKind))
			{
				*axis = std::move(changed);
			}
		}
	}

	inline FlowPlot::Spec::LayerSpec* propertiesContentLayerForIds(
		state& guiState,
		std::string_view panelId,
		std::string_view layerId)
	{
		const std::optional<std::size_t> panelIndex =
			propertiesContentFindPanelIndex(guiState.activeTemplate, panelId);
		if (!panelIndex)
		{
			return nullptr;
		}

		FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
		const std::optional<std::size_t> layerIndex = propertiesContentFindLayerIndex(panel, layerId);
		if (!layerIndex)
		{
			return nullptr;
		}
		return &panel.layers[*layerIndex];
	}

	inline FlowPlot::Spec::LegendSpec* propertiesContentLegendForId(
		state& guiState,
		std::string_view legendId)
	{
		const std::optional<std::size_t> legendIndex =
			propertiesContentFindLegendIndex(guiState.activeTemplate.figure, legendId);
		if (!legendIndex)
		{
			return nullptr;
		}
		return &guiState.activeTemplate.figure.legends[*legendIndex];
	}

	inline FlowPlot::Spec::LegendElementSpec* propertiesContentLegendElementForIds(
		state& guiState,
		std::string_view legendId,
		std::string_view elementId)
	{
		FlowPlot::Spec::LegendSpec* legend = propertiesContentLegendForId(guiState, legendId);
		if (legend == nullptr)
		{
			return nullptr;
		}

		const std::optional<std::size_t> elementIndex =
			propertiesContentFindLegendElementIndex(*legend, elementId);
		if (!elementIndex)
		{
			return nullptr;
		}
		return &legend->legendElements[*elementIndex];
	}

	inline variableLayerType propertiesContentVariableLayerTypeFromString(std::string_view type)
	{
		return type == "histogram" ? variableLayerType::Histogram : variableLayerType::Scatter;
	}

	inline std::string propertiesContentAxisRoleToString(FlowPlot::Spec::AxisDataRole role)
	{
		switch (role)
		{
		case FlowPlot::Spec::AxisDataRole::Null:
			return "null";
		case FlowPlot::Spec::AxisDataRole::Secondary:
			return "secondary";
		case FlowPlot::Spec::AxisDataRole::Primary:
		default:
			return "primary";
		}
	}

	inline FlowPlot::Spec::AxisDataRole propertiesContentAxisRoleFromString(std::string_view role)
	{
		if (role == "null")
		{
			return FlowPlot::Spec::AxisDataRole::Null;
		}
		if (role == "secondary")
		{
			return FlowPlot::Spec::AxisDataRole::Secondary;
		}
		return FlowPlot::Spec::AxisDataRole::Primary;
	}

	inline void drawFigureEditor(FlowUi::UiManager& ui, state& guiState, std::string_view rootId)
	{
		auto& figure = guiState.activeTemplate.figure;

		ui.createElement(kNumericInputCard, std::string(rootId) + "/figure-width")
			.setParameters(numericInputCardParams{
				.hintText = "Width",
				.valueType = numericInputValueType::UInt32,
				.value = static_cast<double>(figure.width),
				.minValue = 1.0,
				.maxValue = static_cast<double>(std::numeric_limits<std::uint32_t>::max()),
				.onChange = [&guiState](double changed) {
					guiState.activeTemplate.figure.width = static_cast<std::uint32_t>(numericInputNormalizeValue(
						numericInputValueType::UInt32,
						changed,
						1.0,
						static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/figure-height")
			.setParameters(numericInputCardParams{
				.hintText = "Height",
				.valueType = numericInputValueType::UInt32,
				.value = static_cast<double>(figure.height),
				.minValue = 1.0,
				.maxValue = static_cast<double>(std::numeric_limits<std::uint32_t>::max()),
				.onChange = [&guiState](double changed) {
					guiState.activeTemplate.figure.height = static_cast<std::uint32_t>(numericInputNormalizeValue(
						numericInputValueType::UInt32,
						changed,
						1.0,
						static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
				},
			})
			.draw();

		ui.createElement(kColorPickerCard, std::string(rootId) + "/figure-background")
			.setParameters(colorPickerCardParams{
				.hintText = "Background",
				.value = figure.background,
				.onChange = [&guiState](std::string_view changed) {
					guiState.activeTemplate.figure.background = std::string(changed);
				},
			})
			.draw();

		const Clay_Padding currentPadding{
			.left = static_cast<std::uint16_t>(std::clamp(
				std::lround(figure.padding.left),
				0L,
				static_cast<long>(std::numeric_limits<std::uint16_t>::max()))),
			.right = static_cast<std::uint16_t>(std::clamp(
				std::lround(figure.padding.right),
				0L,
				static_cast<long>(std::numeric_limits<std::uint16_t>::max()))),
			.top = static_cast<std::uint16_t>(std::clamp(
				std::lround(figure.padding.top),
				0L,
				static_cast<long>(std::numeric_limits<std::uint16_t>::max()))),
			.bottom = static_cast<std::uint16_t>(std::clamp(
				std::lround(figure.padding.bottom),
				0L,
				static_cast<long>(std::numeric_limits<std::uint16_t>::max()))),
		};

		ui.createElement(kNineSplitCard, std::string(rootId) + "/figure-padding")
			.setParameters(nineSplitCardParams{
				.hintText = "Padding",
				.value = currentPadding,
				.onChange = [&guiState](Clay_Padding changed) {
					guiState.activeTemplate.figure.padding.left = static_cast<float>(changed.left);
					guiState.activeTemplate.figure.padding.right = static_cast<float>(changed.right);
					guiState.activeTemplate.figure.padding.top = static_cast<float>(changed.top);
					guiState.activeTemplate.figure.padding.bottom = static_cast<float>(changed.bottom);
				},
			})
			.draw();
	}

	inline void drawFigureTitleEditor(FlowUi::UiManager& ui, state& guiState, std::string_view rootId)
	{
		textSpecEditorParams params{};
		params.hintText = "Figure Title";
		params.value = guiState.activeTemplate.figure.title;
		params.onChange = [&guiState](FlowPlot::Spec::TextSpec changed) {
			guiState.activeTemplate.figure.title = std::move(changed);
		};
		propertiesContentApplyTextFontOptions(params, guiState, params.value);

		ui.createElement(kTextSpecEditor, std::string(rootId) + "/figure-title-editor")
			.setParameters(std::move(params))
			.draw();
	}

	inline void drawLayoutEditor(FlowUi::UiManager& ui, state& guiState, std::string_view rootId)
	{
		ui.createElement(kLayoutEditor, std::string(rootId) + "/layout-editor")
			.setParameters(layoutEditorParams{
				.hintText = "Layout",
				.value = guiState.activeTemplate.layout,
				.onChange = [&guiState](FlowPlot::Spec::LayoutSpec changed) {
					guiState.activeTemplate.layout = std::move(changed);
				},
			})
			.draw();
	}

	inline void drawPanelEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		const std::optional<std::size_t> panelIndex =
			propertiesContentFindPanelIndex(guiState.activeTemplate, key.outer);
		if (!panelIndex)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/panel-missing", "Panel is unavailable");
			return;
		}

		const FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
		const std::string panelId = panel.id;

		ui.createElement(kToggleCard, std::string(rootId) + "/panel-visible")
			.setParameters(toggleCardParams{
				.hintText = "Visible",
				.value = panel.visible,
				.onChange = [&guiState, panelId](bool changed) {
					if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
					{
						guiState.activeTemplate.panels[*index].visible = changed;
					}
				},
			})
			.draw();

		ui.createElement(kColorPickerCard, std::string(rootId) + "/panel-background")
			.setParameters(colorPickerCardParams{
				.hintText = "Background",
				.value = panel.background,
				.onChange = [&guiState, panelId](std::string_view changed) {
					if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
					{
						guiState.activeTemplate.panels[*index].background = std::string(changed);
					}
				},
			})
			.draw();

		ui.createElement(kColorPickerCard, std::string(rootId) + "/panel-border-color")
			.setParameters(colorPickerCardParams{
				.hintText = "Border Color",
				.value = panel.borderColor,
				.onChange = [&guiState, panelId](std::string_view changed) {
					if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
					{
						guiState.activeTemplate.panels[*index].borderColor = std::string(changed);
					}
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/panel-border-width")
			.setParameters(numericInputCardParams{
				.hintText = "Border Width",
				.valueType = numericInputValueType::Float,
				.value = static_cast<double>(panel.borderWidth),
				.minValue = 0.0,
				.maxValue = 1000000.0,
				.onChange = [&guiState, panelId](double changed) {
					if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
					{
						guiState.activeTemplate.panels[*index].borderWidth = static_cast<float>(changed);
					}
				},
			})
			.draw();

		ui.createElement(kNineSplitCard, std::string(rootId) + "/panel-padding")
			.setParameters(nineSplitCardParams{
				.hintText = "Padding",
				.value = propertiesContentPaddingToClay(panel.padding),
				.onChange = [&guiState, panelId](Clay_Padding changed) {
					if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
					{
						FlowPlot::Spec::PaddingSpec& padding = guiState.activeTemplate.panels[*index].padding;
						padding.left = static_cast<float>(changed.left);
						padding.right = static_cast<float>(changed.right);
						padding.top = static_cast<float>(changed.top);
						padding.bottom = static_cast<float>(changed.bottom);
					}
				},
			})
			.draw();

		ui.createElement(kToggleCard, std::string(rootId) + "/panel-clip-content")
			.setParameters(toggleCardParams{
				.hintText = "Clip Content",
				.value = panel.clipContent,
				.onChange = [&guiState, panelId](bool changed) {
					if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
					{
						guiState.activeTemplate.panels[*index].clipContent = changed;
					}
				},
			})
			.draw();
	}

	inline void drawPanelTitleEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		const std::optional<std::size_t> panelIndex =
			propertiesContentFindPanelIndex(guiState.activeTemplate, key.outer);
		if (!panelIndex)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/panel-title-missing", "Panel title is unavailable");
			return;
		}

		const FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
		const std::string panelId = panel.id;
		textSpecEditorParams params{};
		params.hintText = "Panel Title";
		params.value = panel.title;
		params.onChange = [&guiState, panelId](FlowPlot::Spec::TextSpec changed) {
			if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
			{
				guiState.activeTemplate.panels[*index].title = std::move(changed);
			}
		};
		propertiesContentApplyTextFontOptions(params, guiState, params.value);

		ui.createElement(kTextSpecEditor, std::string(rootId) + "/panel-title-editor")
			.setParameters(std::move(params))
			.draw();
	}

	inline void drawAxisEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		static const std::array<std::string, 1> kAxisScaleOptions = {"linear"};

		const std::optional<std::size_t> panelIndex =
			propertiesContentFindPanelIndex(guiState.activeTemplate, key.outer);
		if (!panelIndex)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/axis-missing-panel", "Axis panel is unavailable");
			return;
		}

		const FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
		const FlowPlot::Spec::AxisSpec* axis = propertiesContentAxisForKind(panel, key.kind);
		if (axis == nullptr)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/axis-missing", "Axis is unavailable");
			return;
		}

		const std::string panelId = panel.id;
		const TemplateNodeKind axisKind = key.kind;
		const auto emitAxisChange = [&guiState, panelId, axisKind](FlowPlot::Spec::AxisSpec changed) {
			propertiesContentSetPanelAxis(guiState, panelId, axisKind, std::move(changed));
		};

		ui.createElement(kToggleCard, std::string(rootId) + "/axis-visible")
			.setParameters(toggleCardParams{
				.hintText = "Visible",
				.value = axis->visible,
				.onChange = [axisValue = *axis, emitAxisChange](bool changed) mutable {
					axisValue.visible = changed;
					emitAxisChange(std::move(axisValue));
				},
			})
			.draw();

		ui.createElement(kEnumPickerCard, std::string(rootId) + "/axis-scale")
			.setParameters(enumPickerCardParams{
				.hintText = "Scale",
				.options = kAxisScaleOptions,
				.value = axis->scale,
				.defaultValue = "linear",
				.onChange = [axisValue = *axis, emitAxisChange](std::string_view changed) mutable {
					axisValue.scale = std::string(changed);
					emitAxisChange(std::move(axisValue));
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/axis-min")
			.setParameters(numericInputCardParams{
				.hintText = "Min",
				.valueType = numericInputValueType::Float,
				.value = axis->min.has_value() ? static_cast<double>(*axis->min) : 0.0,
				.minValue = -1000000.0,
				.maxValue = 1000000.0,
				.onChange = [axisValue = *axis, emitAxisChange](double changed) mutable {
					axisValue.min = static_cast<float>(changed);
					emitAxisChange(std::move(axisValue));
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/axis-max")
			.setParameters(numericInputCardParams{
				.hintText = "Max",
				.valueType = numericInputValueType::Float,
				.value = axis->max.has_value() ? static_cast<double>(*axis->max) : 0.0,
				.minValue = -1000000.0,
				.maxValue = 1000000.0,
				.onChange = [axisValue = *axis, emitAxisChange](double changed) mutable {
					axisValue.max = static_cast<float>(changed);
					emitAxisChange(std::move(axisValue));
				},
			})
			.draw();

		ui.createElement(kToggleCard, std::string(rootId) + "/axis-invert")
			.setParameters(toggleCardParams{
				.hintText = "Invert",
				.value = axis->invert,
				.onChange = [axisValue = *axis, emitAxisChange](bool changed) mutable {
					axisValue.invert = changed;
					emitAxisChange(std::move(axisValue));
				},
			})
			.draw();

		ui.createElement(kToggleCard, std::string(rootId) + "/axis-grid")
			.setParameters(toggleCardParams{
				.hintText = "Grid",
				.value = axis->grid,
				.onChange = [axisValue = *axis, emitAxisChange](bool changed) mutable {
					axisValue.grid = changed;
					emitAxisChange(std::move(axisValue));
				},
			})
			.draw();

		ui.createElement(kGridSettingsEditor, std::string(rootId) + "/axis-grid-settings")
			.setParameters(gridSettingsEditorParams{
				.hintText = "Grid Settings",
				.value = *axis,
				.onChange = emitAxisChange,
			})
			.draw();

		ui.createElement(kLineSettingsEditor, std::string(rootId) + "/axis-line-settings")
			.setParameters(lineSettingsEditorParams{
				.hintText = "Line Settings",
				.value = *axis,
				.onChange = emitAxisChange,
			})
			.draw();

		ui.createElement(kTickSettingsEditor, std::string(rootId) + "/axis-tick-settings")
			.setParameters([&]() {
				tickSettingsEditorParams params{};
				params.hintText = "Tick Settings";
				params.value = *axis;
				params.onChange = emitAxisChange;
				propertiesContentApplyTickFontOptions(params, guiState, params.value);
				return params;
			}())
			.draw();
	}

	inline void drawAxisTitleEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		const std::optional<std::size_t> panelIndex =
			propertiesContentFindPanelIndex(guiState.activeTemplate, key.outer);
		if (!panelIndex)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/axis-title-missing-panel", "Axis title panel is unavailable");
			return;
		}

		const std::optional<TemplateNodeKind> axisKind = propertiesContentAxisKindFromSlug(key.inner);
		if (!axisKind)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/axis-title-missing-kind", "Axis title is unavailable");
			return;
		}

		const FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
		const FlowPlot::Spec::AxisSpec* axis = propertiesContentAxisForKind(panel, *axisKind);
		if (axis == nullptr)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/axis-title-missing-axis", "Axis title is unavailable");
			return;
		}

		const std::string panelId = panel.id;
		textSpecEditorParams params{};
		params.hintText = "Axis Title";
		params.value = axis->title;
		params.onChange = [&guiState, panelId, axisKind = *axisKind](FlowPlot::Spec::TextSpec changed) {
			if (const auto index = propertiesContentFindPanelIndex(guiState.activeTemplate, panelId))
			{
				if (FlowPlot::Spec::AxisSpec* axis =
						propertiesContentAxisForKind(guiState.activeTemplate.panels[*index], axisKind))
				{
					axis->title = std::move(changed);
				}
			}
		};
		propertiesContentApplyTextFontOptions(params, guiState, params.value);

		ui.createElement(kTextSpecEditor, std::string(rootId) + "/axis-title-editor")
			.setParameters(std::move(params))
			.draw();
	}

	inline void drawLayersEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/layers-editor", "Select a layer to edit Its Properties");
	}

	inline void drawLayerEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		static const std::array<std::string, 2> kLayerTypeOptions = {"scatter", "histogram"};
		static const std::array<std::string, 3> kAxisDataRoleOptions = {"primary", "secondary", "null"};

		const std::optional<std::size_t> panelIndex =
			propertiesContentFindPanelIndex(guiState.activeTemplate, key.outer);
		if (!panelIndex)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/layer-missing-panel", "Layer panel is unavailable");
			return;
		}

		const FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
		const std::optional<std::size_t> layerIndex = propertiesContentFindLayerIndex(panel, key.inner);
		if (!layerIndex)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/layer-missing", "Layer is unavailable");
			return;
		}

		const FlowPlot::Spec::LayerSpec& layer = panel.layers[*layerIndex];
		const std::string panelId = panel.id;
		const std::string layerId = layer.id;
		const auto emitLayerChange = [&guiState, panelId, layerId](FlowPlot::Spec::LayerSpec changed) {
			if (FlowPlot::Spec::LayerSpec* target = propertiesContentLayerForIds(guiState, panelId, layerId))
			{
				*target = std::move(changed);
			}
		};

		ui.createElement(kEnumPickerCard, std::string(rootId) + "/layer-type")
			.setParameters(enumPickerCardParams{
				.hintText = "Type",
				.options = kLayerTypeOptions,
				.value = layer.type,
				.defaultValue = "scatter",
				.onChange = [layerValue = layer, emitLayerChange](std::string_view changed) mutable {
					layerValue.type = std::string(changed);
					emitLayerChange(std::move(layerValue));
				},
			})
			.draw();

		ui.createElement(kToggleCard, std::string(rootId) + "/layer-visible")
			.setParameters(toggleCardParams{
				.hintText = "Visible",
				.value = layer.visible,
				.onChange = [layerValue = layer, emitLayerChange](bool changed) mutable {
					layerValue.visible = changed;
					emitLayerChange(std::move(layerValue));
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/layer-opacity")
			.setParameters(numericInputCardParams{
				.hintText = "Opacity",
				.valueType = numericInputValueType::Float,
				.value = static_cast<double>(layer.opacity),
				.minValue = 0.0,
				.maxValue = 1.0,
				.onChange = [layerValue = layer, emitLayerChange](double changed) mutable {
					layerValue.opacity = static_cast<float>(changed);
					emitLayerChange(std::move(layerValue));
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/layer-z-index")
			.setParameters(numericInputCardParams{
				.hintText = "Z Index",
				.valueType = numericInputValueType::Int32,
				.value = static_cast<double>(layer.zIndex),
				.minValue = static_cast<double>(std::numeric_limits<std::int32_t>::min()),
				.maxValue = static_cast<double>(std::numeric_limits<std::int32_t>::max()),
				.onChange = [layerValue = layer, emitLayerChange](double changed) mutable {
					layerValue.zIndex = static_cast<std::int32_t>(numericInputNormalizeValue(
						numericInputValueType::Int32,
						changed,
						static_cast<double>(std::numeric_limits<std::int32_t>::min()),
						static_cast<double>(std::numeric_limits<std::int32_t>::max())));
					emitLayerChange(std::move(layerValue));
				},
			})
			.draw();

		ui.createElement(kStringInputCard, std::string(rootId) + "/layer-dataset")
			.setParameters(stringInputCardParams{
				.hintText = "Dataset",
				.value = layer.dataset,
				.onChange = [layerValue = layer, emitLayerChange](std::string_view changed) mutable {
					layerValue.dataset = std::string(changed);
					emitLayerChange(std::move(layerValue));
				},
			})
			.draw();

		ui.createElement(kDoubleEnumPickerCard, std::string(rootId) + "/layer-axis-data")
			.setParameters(doubleEnumPickerCardParams{
				.hintText = "Axis Data",
				.firstHintText = "X",
				.firstOptions = kAxisDataRoleOptions,
				.firstValue = propertiesContentAxisRoleToString(layer.axisData.x),
				.firstDefaultValue = "primary",
				.secondHintText = "Y",
				.secondOptions = kAxisDataRoleOptions,
				.secondValue = propertiesContentAxisRoleToString(layer.axisData.y),
				.secondDefaultValue = "primary",
				.onChange = [layerValue = layer, emitLayerChange](std::string_view xRole, std::string_view yRole) mutable {
					layerValue.axisData.x = propertiesContentAxisRoleFromString(xRole);
					layerValue.axisData.y = propertiesContentAxisRoleFromString(yRole);
					emitLayerChange(std::move(layerValue));
				},
			})
			.draw();

		ui.createElement(kVariableTypeSettingsEditor, std::string(rootId) + "/layer-variable-type-settings")
			.setParameters(variableTypeSettingsEditorParams{
				.hintText = "Variable Type Settings",
				.layerType = propertiesContentVariableLayerTypeFromString(layer.type),
				.value = layer,
				.onChange = emitLayerChange,
			})
			.draw();
	}

	inline void drawLegendsEditor(FlowUi::UiManager& ui, state& guiState, std::string_view rootId)
	{
		(void)guiState;
		drawPropertiesHint(ui, std::string(rootId) + "/legends-editor", "Add or Select a Legend to edit its properties");
	}

	inline void drawLegendEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		const std::optional<std::size_t> legendIndex =
			propertiesContentFindLegendIndex(guiState.activeTemplate.figure, key.outer);
		if (!legendIndex)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/legend-missing", "Legend is unavailable");
			return;
		}

		const FlowPlot::Spec::LegendSpec& legend = guiState.activeTemplate.figure.legends[*legendIndex];
		const std::string legendId = legend.id;
		const auto emitLegendChange = [&guiState, legendId](FlowPlot::Spec::LegendSpec changed) {
			if (FlowPlot::Spec::LegendSpec* target = propertiesContentLegendForId(guiState, legendId))
			{
				*target = std::move(changed);
			}
		};

		ui.createElement(kToggleCard, std::string(rootId) + "/legend-visible")
			.setParameters(toggleCardParams{
				.hintText = "Visible",
				.value = legend.visible,
				.onChange = [legendValue = legend, emitLegendChange](bool changed) mutable {
					legendValue.visible = changed;
					emitLegendChange(std::move(legendValue));
				},
			})
			.draw();

		ui.createElement(kColorPickerCard, std::string(rootId) + "/legend-background")
			.setParameters(colorPickerCardParams{
				.hintText = "Background Color",
				.value = legend.background,
				.onChange = [legendValue = legend, emitLegendChange](std::string_view changed) mutable {
					legendValue.background = std::string(changed);
					emitLegendChange(std::move(legendValue));
				},
			})
			.draw();

		ui.createElement(kColorPickerCard, std::string(rootId) + "/legend-border-color")
			.setParameters(colorPickerCardParams{
				.hintText = "Border Color",
				.value = legend.borderColor,
				.onChange = [legendValue = legend, emitLegendChange](std::string_view changed) mutable {
					legendValue.borderColor = std::string(changed);
					emitLegendChange(std::move(legendValue));
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/legend-border-width")
			.setParameters(numericInputCardParams{
				.hintText = "Border Width",
				.valueType = numericInputValueType::Float,
				.value = static_cast<double>(legend.borderWidth),
				.minValue = 0.0,
				.maxValue = 1000000.0,
				.onChange = [legendValue = legend, emitLegendChange](double changed) mutable {
					legendValue.borderWidth = static_cast<float>(changed);
					emitLegendChange(std::move(legendValue));
				},
			})
			.draw();

		ui.createElement(kNineSplitCard, std::string(rootId) + "/legend-padding")
			.setParameters(nineSplitCardParams{
				.hintText = "Padding",
				.value = propertiesContentPaddingToClay(legend.padding),
				.onChange = [legendValue = legend, emitLegendChange](Clay_Padding changed) mutable {
					legendValue.padding.left = static_cast<float>(changed.left);
					legendValue.padding.right = static_cast<float>(changed.right);
					legendValue.padding.top = static_cast<float>(changed.top);
					legendValue.padding.bottom = static_cast<float>(changed.bottom);
					emitLegendChange(std::move(legendValue));
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/legend-gap")
			.setParameters(numericInputCardParams{
				.hintText = "Gap",
				.valueType = numericInputValueType::Float,
				.value = static_cast<double>(legend.gap),
				.minValue = 0.0,
				.maxValue = 1000000.0,
				.onChange = [legendValue = legend, emitLegendChange](double changed) mutable {
					legendValue.gap = static_cast<float>(changed);
					emitLegendChange(std::move(legendValue));
				},
			})
			.draw();
	}

	inline void drawLegendElementEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		static const std::array<std::string, 3> kOverflowOptions = {"clip", "visible", "ellipsis"};
		static const std::array<std::string, 5> kIconShapeOptions = {"circle", "square", "diamond", "triangle", "cross"};

		const FlowPlot::Spec::LegendSpec* legend = nullptr;
		if (const FlowPlot::Spec::LegendSpec* foundLegend =
				[&guiState, &key]() -> const FlowPlot::Spec::LegendSpec* {
					const auto legendIndex = propertiesContentFindLegendIndex(guiState.activeTemplate.figure, key.outer);
					if (!legendIndex)
					{
						return nullptr;
					}
					return &guiState.activeTemplate.figure.legends[*legendIndex];
				}())
		{
			legend = foundLegend;
		}
		if (legend == nullptr)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/legend-element-missing-legend", "Legend element is unavailable");
			return;
		}

		const std::optional<std::size_t> elementIndex = propertiesContentFindLegendElementIndex(*legend, key.inner);
		if (!elementIndex)
		{
			drawPropertiesHint(ui, std::string(rootId) + "/legend-element-missing", "Legend element is unavailable");
			return;
		}

		const FlowPlot::Spec::LegendElementSpec& element = legend->legendElements[*elementIndex];
		const std::string legendId = legend->id;
		const std::string elementId = element.id;
		const std::vector<std::string> fontFamilyOptions = availableFontFamilies(guiState.fontLibrary);
		const std::vector<std::string> fontWeightOptions = availableWeightsForFamily(
			guiState.fontLibrary,
			element.fontFamily);
		const std::vector<std::string> fontStyleOptions = availableStylesForFamilyWeight(
			guiState.fontLibrary,
			element.fontFamily,
			element.fontWeight);
		const auto emitElementChange = [&guiState, legendId, elementId](FlowPlot::Spec::LegendElementSpec changed) {
			if (FlowPlot::Spec::LegendElementSpec* target =
					propertiesContentLegendElementForIds(guiState, legendId, elementId))
			{
				*target = std::move(changed);
			}
		};

		ui.createElement(kStringInputCard, std::string(rootId) + "/legend-element-text")
			.setParameters(stringInputCardParams{
				.hintText = "Text",
				.value = element.text,
				.onChange = [elementValue = element, emitElementChange](std::string_view changed) mutable {
					elementValue.text = std::string(changed);
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();

		ui.createElement(kEnumPickerCard, std::string(rootId) + "/legend-element-font-family")
			.setParameters(enumPickerCardParams{
				.hintText = "Font Family",
				.options = fontFamilyOptions,
				.value = element.fontFamily,
				.defaultValue = "Default",
				.onChange = [elementValue = element, emitElementChange](std::string_view changed) mutable {
					elementValue.fontFamily = std::string(changed);
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();

		ui.createElement(kNumericInputCard, std::string(rootId) + "/legend-element-font-size")
			.setParameters(numericInputCardParams{
				.hintText = "Font Size",
				.valueType = numericInputValueType::Float,
				.value = static_cast<double>(element.fontSize),
				.minValue = 0.0,
				.maxValue = 1000000.0,
				.onChange = [elementValue = element, emitElementChange](double changed) mutable {
					elementValue.fontSize = static_cast<float>(changed);
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();

		ui.createElement(kEnumPickerCard, std::string(rootId) + "/legend-element-font-weight")
			.setParameters(enumPickerCardParams{
				.hintText = "Font Weight",
				.options = fontWeightOptions,
				.value = std::to_string(element.fontWeight),
				.defaultValue = "400",
				.onChange = [elementValue = element, emitElementChange](std::string_view changed) mutable {
					try
					{
						const unsigned long parsed = std::stoul(std::string(changed));
						elementValue.fontWeight = static_cast<std::uint16_t>(std::min<unsigned long>(
							parsed,
							static_cast<unsigned long>(std::numeric_limits<std::uint16_t>::max())));
					}
					catch (...)
					{
						elementValue.fontWeight = 400;
					}
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();

		ui.createElement(kEnumPickerCard, std::string(rootId) + "/legend-element-font-style")
			.setParameters(enumPickerCardParams{
				.hintText = "Font Style",
				.options = fontStyleOptions,
				.value = element.fontStyle,
				.defaultValue = "normal",
				.onChange = [elementValue = element, emitElementChange](std::string_view changed) mutable {
					elementValue.fontStyle = std::string(changed);
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();

		ui.createElement(kColorPickerCard, std::string(rootId) + "/legend-element-color")
			.setParameters(colorPickerCardParams{
				.hintText = "Color",
				.value = element.color,
				.onChange = [elementValue = element, emitElementChange](std::string_view changed) mutable {
					elementValue.color = std::string(changed);
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();

		ui.createElement(kEnumPickerCard, std::string(rootId) + "/legend-element-overflow")
			.setParameters(enumPickerCardParams{
				.hintText = "Overflow",
				.options = kOverflowOptions,
				.value = element.overflow,
				.defaultValue = "clip",
				.onChange = [elementValue = element, emitElementChange](std::string_view changed) mutable {
					elementValue.overflow = std::string(changed);
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();

		ui.createElement(kEnumPickerCard, std::string(rootId) + "/legend-element-icon-shape")
			.setParameters(enumPickerCardParams{
				.hintText = "Icon Shape",
				.options = kIconShapeOptions,
				.value = element.iconShape,
				.defaultValue = "square",
				.onChange = [elementValue = element, emitElementChange](std::string_view changed) mutable {
					elementValue.iconShape = std::string(changed);
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();

		ui.createElement(kColorPickerCard, std::string(rootId) + "/legend-element-icon-color")
			.setParameters(colorPickerCardParams{
				.hintText = "Icon Color",
				.value = element.iconColor,
				.onChange = [elementValue = element, emitElementChange](std::string_view changed) mutable {
					elementValue.iconColor = std::string(changed);
					emitElementChange(std::move(elementValue));
				},
			})
			.draw();
	}

	inline void drawPropertiesForSelection(
		FlowUi::UiManager& ui,
		state& guiState,
		std::string_view rootId)
	{
		if (!guiState.selectedNode.has_value())
		{
			drawPropertiesHint(ui, std::string(rootId) + "/no-selection", "Select an item to edit its properties");
			return;
		}

		const TemplateNodeKey& key = *guiState.selectedNode;
		switch (key.kind)
		{
		case TemplateNodeKind::Figure:
			drawFigureEditor(ui, guiState, rootId);
			break;
		case TemplateNodeKind::FigureTitle:
			drawFigureTitleEditor(ui, guiState, rootId);
			break;
		case TemplateNodeKind::PanelsGroup:
			drawLayoutEditor(ui, guiState, rootId);
			break;
		case TemplateNodeKind::Panel:
			drawPanelEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::PanelTitle:
			drawPanelTitleEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::XAxis:
		case TemplateNodeKind::YAxis:
		case TemplateNodeKind::XSecondaryAxis:
		case TemplateNodeKind::YSecondaryAxis:
			drawAxisEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::AxisTitle:
			drawAxisTitleEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::LayersGroup:
			drawLayersEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::Layer:
			drawLayerEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::LegendsGroup:
			drawLegendsEditor(ui, guiState, rootId);
			break;
		case TemplateNodeKind::Legend:
			drawLegendEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::LegendElement:
			drawLegendElementEditor(ui, guiState, key, rootId);
			break;
		}
	}
} // namespace FlowPlotGui

inline const PropertiesContentDef kPropertiesContent = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](PropertiesContentDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_Vector2 scrollOffset =
			FlowPlotGui::propertiesContentScrollOffsetForElementId(context.uiManager, context.elementID);

		Clay_ElementDeclaration root{};
		root.layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_GROW(0),
		};
		root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		root.layout.padding = context.params.padding;
		root.layout.childGap = context.params.childGap;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		root.backgroundColor = context.params.backgroundColor;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};
		root.clip = {
			.horizontal = false,
			.vertical = true,
			.childOffset = scrollOffset,
		};

		CLAY(rootId, root)
		{
			if (context.params.guiState == nullptr)
			{
				FlowPlotGui::drawPropertiesHint(
					context.uiManager,
					context.createChildElementId("missing-state"),
					"Properties state is unavailable");
			}
			else
			{
				FlowPlotGui::drawPropertiesForSelection(
					context.uiManager,
					*context.params.guiState,
					context.createChildElementId("selection"));
			}
		}
	},
};
