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
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct scatterStyleEditorParams {
	std::string hintText = "Scatter Style";
	FlowPlot::Spec::ScatterStyleSpec value{};
	std::function<void(FlowPlot::Spec::ScatterStyleSpec)> onChange = nullptr;
	std::span<const std::string> markerShapeOptions{};

	mappingSettingsEditorShellParams shell{};
	enumPickerCardParams markerShapeInput{};
	colorPickerCardParams fillColorInput{};
	colorPickerCardParams strokeColorInput{};
	numericInputCardParams strokeWidthInput{};
	numericInputCardParams markerSizeInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	scatterStyleEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(scatterStyleEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(scatterStyleEditorParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(scatterStyleEditorParams, markerShapeInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStyleEditorParams, fillColorInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStyleEditorParams, strokeColorInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStyleEditorParams, strokeWidthInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterStyleEditorParams, markerSizeInput));

using ScatterStyleSettingsEditorDef = FlowUi::ElementDefinition<
	scatterStyleEditorParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("ScatterStyleSettingsEditor")>;

inline void scatterStyleEditorEmit(
	FlowPlot::Spec::ScatterStyleSpec nextSpec,
	const std::function<void(FlowPlot::Spec::ScatterStyleSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const ScatterStyleSettingsEditorDef kScatterStyleSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ScatterStyleSettingsEditorDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			ScatterStyleSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](ScatterStyleSettingsEditorDef::BuildContext& context) {
		const std::span<const std::string> shapeOptions = context.params.markerShapeOptions.empty()
			? scatterMarkerShapeOptions()
			: context.params.markerShapeOptions;
		mappingSettingsEditorDrawShell<ScatterStyleSettingsEditorDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-marker-shape", context.params.shell, [&](const std::string& rowPath) {
					enumPickerCardParams params = context.params.markerShapeInput;
					params.hintText = "Marker Shape";
					params.options = shapeOptions;
					params.value = context.params.value.markerShape;
					params.defaultValue = "circle";
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.markerShape = std::string(changed);
						scatterStyleEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kEnumPickerCard, rowPath + "/marker-shape").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-fill-color", context.params.shell, [&](const std::string& rowPath) {
					colorPickerCardParams params = context.params.fillColorInput;
					params.hintText = "Fill Color";
					params.value = context.params.value.fillColor;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.fillColor = std::string(changed);
						scatterStyleEditorEmit(std::move(value), onChange);
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
						scatterStyleEditorEmit(std::move(value), onChange);
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
						scatterStyleEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/stroke-width").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-marker-size", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.markerSizeInput;
					params.hintText = "Marker Size";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.markerSize);
					params.minValue = 0.0;
					params.maxValue = 1000000.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.markerSize = static_cast<float>(changed);
						scatterStyleEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/marker-size").setParameters(std::move(params)).draw();
				});
			});
	},
};
