#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/basicInputs/toggle.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct scatterStatsEditorParams {
	std::string hintText = "Scatter Stats";
	FlowPlot::Spec::ScatterStatsSpec value{};
	std::function<void(FlowPlot::Spec::ScatterStatsSpec)> onChange = nullptr;
	std::span<const std::string> meanPointShapeOptions{};

	mappingSettingsEditorShellParams shell{};
	toggleCardParams showMeanPointInput{};
	enumPickerCardParams meanPointShapeInput{};
	colorPickerCardParams meanPointColorInput{};
	colorPickerCardParams meanPointStrokeColorInput{};
	numericInputCardParams meanPointStrokeWidthInput{};
	numericInputCardParams meanPointSizeInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	scatterStatsEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(scatterStatsEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(scatterStatsEditorParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(scatterStatsEditorParams, showMeanPointInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStatsEditorParams, meanPointShapeInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStatsEditorParams, meanPointColorInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStatsEditorParams, meanPointStrokeColorInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStatsEditorParams, meanPointStrokeWidthInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStatsEditorParams, meanPointSizeInput));

using ScatterStatsSettingsEditorDef = FlowUi::ElementDefinition<
	scatterStatsEditorParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("ScatterStatsSettingsEditor")>;

inline void scatterStatsEditorEmit(
	FlowPlot::Spec::ScatterStatsSpec nextSpec,
	const std::function<void(FlowPlot::Spec::ScatterStatsSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const ScatterStatsSettingsEditorDef kScatterStatsSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ScatterStatsSettingsEditorDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			ScatterStatsSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](ScatterStatsSettingsEditorDef::BuildContext& context) {
		const std::span<const std::string> shapeOptions = context.params.meanPointShapeOptions.empty()
			? scatterMarkerShapeOptions()
			: context.params.meanPointShapeOptions;
		mappingSettingsEditorDrawShell<ScatterStatsSettingsEditorDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-show-mean-point", context.params.shell, [&](const std::string& rowPath) {
					toggleCardParams params = context.params.showMeanPointInput;
					params.hintText = "Show Mean Point";
					params.value = context.params.value.showMeanPoint;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](bool changed) mutable {
						value.showMeanPoint = changed;
						scatterStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kToggleCard, rowPath + "/show-mean-point").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-mean-point-shape", context.params.shell, [&](const std::string& rowPath) {
					enumPickerCardParams params = context.params.meanPointShapeInput;
					params.hintText = "Mean Point Shape";
					params.options = shapeOptions;
					params.value = context.params.value.meanPointShape;
					params.defaultValue = "diamond";
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.meanPointShape = std::string(changed);
						scatterStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kEnumPickerCard, rowPath + "/mean-point-shape").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-mean-point-color", context.params.shell, [&](const std::string& rowPath) {
					colorPickerCardParams params = context.params.meanPointColorInput;
					params.hintText = "Mean Point Color";
					params.value = context.params.value.meanPointColor;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.meanPointColor = std::string(changed);
						scatterStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kColorPickerCard, rowPath + "/mean-point-color").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-mean-point-stroke-color", context.params.shell, [&](const std::string& rowPath) {
					colorPickerCardParams params = context.params.meanPointStrokeColorInput;
					params.hintText = "Stroke Color";
					params.value = context.params.value.meanPointStrokeColor;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.meanPointStrokeColor = std::string(changed);
						scatterStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kColorPickerCard, rowPath + "/mean-point-stroke-color").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-mean-point-stroke-width", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.meanPointStrokeWidthInput;
					params.hintText = "Mean Point Stroke Width";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.meanPointStrokeWidth);
					params.minValue = 0.0;
					params.maxValue = 1000000.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.meanPointStrokeWidth = static_cast<float>(changed);
						scatterStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/mean-point-stroke-width").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-mean-point-size", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.meanPointSizeInput;
					params.hintText = "Mean Point Size";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.meanPointSize);
					params.minValue = 0.0;
					params.maxValue = 1000000.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.meanPointSize = static_cast<float>(changed);
						scatterStatsEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/mean-point-size").setParameters(std::move(params)).draw();
				});
			});
	},
};
