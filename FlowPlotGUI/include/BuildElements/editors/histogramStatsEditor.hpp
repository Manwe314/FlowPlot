#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/basicInputs/toggle.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct histogramStatsEditorParams {
	std::string hintText = "Histogram Stats";
	FlowPlot::Spec::HistogramStatsSpec value{};
	std::function<void(FlowPlot::Spec::HistogramStatsSpec)> onChange = nullptr;

	mappingSettingsEditorShellParams shell{};
	toggleCardParams showMeanLineInput{};
	colorPickerCardParams meanLineColorInput{};
	numericInputCardParams meanLineWidthInput{};
	toggleCardParams showMedianLineInput{};
	colorPickerCardParams medianLineColorInput{};
	numericInputCardParams medianLineWidthInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	histogramStatsEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(histogramStatsEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(histogramStatsEditorParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(histogramStatsEditorParams, showMeanLineInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramStatsEditorParams, meanLineColorInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramStatsEditorParams, meanLineWidthInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramStatsEditorParams, showMedianLineInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramStatsEditorParams, medianLineColorInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramStatsEditorParams, medianLineWidthInput));

using HistogramStatsSettingsEditorDef = FlowUi::ElementDefinition<
	histogramStatsEditorParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("HistogramStatsSettingsEditor")>;

inline void histogramStatsEditorEmit(
	FlowPlot::Spec::HistogramStatsSpec nextSpec,
	const std::function<void(FlowPlot::Spec::HistogramStatsSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const HistogramStatsSettingsEditorDef kHistogramStatsSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](HistogramStatsSettingsEditorDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			HistogramStatsSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](HistogramStatsSettingsEditorDef::BuildContext& context) {
		mappingSettingsEditorDrawShell<HistogramStatsSettingsEditorDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-show-mean-line", context.params.shell, [&](const std::string& rowPath) {
					toggleCardParams params = context.params.showMeanLineInput;
					params.hintText = "Show Mean Line";
					params.value = context.params.value.showMeanLine;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](bool changed) mutable {
						value.showMeanLine = changed;
						histogramStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kToggleCard, rowPath + "/show-mean-line").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-mean-line-color", context.params.shell, [&](const std::string& rowPath) {
					colorPickerCardParams params = context.params.meanLineColorInput;
					params.hintText = "Mean Line Color";
					params.value = context.params.value.meanLineColor;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.meanLineColor = std::string(changed);
						histogramStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kColorPickerCard, rowPath + "/mean-line-color").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-mean-line-width", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.meanLineWidthInput;
					params.hintText = "Mean Line Width";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.meanLineWidth);
					params.minValue = 0.0;
					params.maxValue = 1000000.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.meanLineWidth = static_cast<float>(changed);
						histogramStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/mean-line-width").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-show-median-line", context.params.shell, [&](const std::string& rowPath) {
					toggleCardParams params = context.params.showMedianLineInput;
					params.hintText = "Show Median Line";
					params.value = context.params.value.showMedianLine;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](bool changed) mutable {
						value.showMedianLine = changed;
						histogramStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kToggleCard, rowPath + "/show-median-line").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-median-line-color", context.params.shell, [&](const std::string& rowPath) {
					colorPickerCardParams params = context.params.medianLineColorInput;
					params.hintText = "Median Line Color";
					params.value = context.params.value.medianLineColor;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.medianLineColor = std::string(changed);
						histogramStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kColorPickerCard, rowPath + "/median-line-color").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-median-line-width", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.medianLineWidthInput;
					params.hintText = "Median Line Width";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.medianLineWidth);
					params.minValue = 0.0;
					params.maxValue = 1000000.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.medianLineWidth = static_cast<float>(changed);
						histogramStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/median-line-width").setParameters(std::move(params)).draw();
				});
			});
	},
};
