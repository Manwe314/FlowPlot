#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct histogramStyleEditorParams {
	std::string hintText = "Histogram Style";
	FlowPlot::Spec::HistogramStyleSpec value{};
	std::function<void(FlowPlot::Spec::HistogramStyleSpec)> onChange = nullptr;

	mappingSettingsEditorShellParams shell{};
	colorPickerCardParams fillColorInput{};
	colorPickerCardParams strokeColorInput{};
	numericInputCardParams strokeWidthInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	histogramStyleEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(histogramStyleEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(histogramStyleEditorParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(histogramStyleEditorParams, fillColorInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramStyleEditorParams, strokeColorInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramStyleEditorParams, strokeWidthInput));

using HistogramStyleSettingsEditorDef = FlowUi::ElementDefinition<
	histogramStyleEditorParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("HistogramStyleSettingsEditor")>;

inline void histogramStyleEditorEmit(
	FlowPlot::Spec::HistogramStyleSpec nextSpec,
	const std::function<void(FlowPlot::Spec::HistogramStyleSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const HistogramStyleSettingsEditorDef kHistogramStyleSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](HistogramStyleSettingsEditorDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			HistogramStyleSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](HistogramStyleSettingsEditorDef::BuildContext& context) {
		mappingSettingsEditorDrawShell<HistogramStyleSettingsEditorDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-fill-color", context.params.shell, [&](const std::string& rowPath) {
					colorPickerCardParams params = context.params.fillColorInput;
					params.hintText = "Fill Color";
					params.value = context.params.value.fillColor;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.fillColor = std::string(changed);
						histogramStyleEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kColorPickerCard, rowPath + "/fill-color").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-stroke-color", context.params.shell, [&](const std::string& rowPath) {
					colorPickerCardParams params = context.params.strokeColorInput;
					params.hintText = "Stroke Color";
					params.value = context.params.value.strokeColor;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.strokeColor = std::string(changed);
						histogramStyleEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kColorPickerCard, rowPath + "/stroke-color").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-stroke-width", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.strokeWidthInput;
					params.hintText = "Stroke Width";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.strokeWidth);
					params.minValue = 0.0;
					params.maxValue = 1000000.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.strokeWidth = static_cast<float>(changed);
						histogramStyleEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/stroke-width").setParameters(std::move(params)).draw();
				});
			});
	},
};
