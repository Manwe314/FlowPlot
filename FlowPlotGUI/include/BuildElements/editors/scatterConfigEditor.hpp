#pragma once

#include <functional>
#include <string>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct scatterConfigEditorParams {
	std::string hintText = "Scatter Config";
	FlowPlot::Spec::ScatterConfigSpec value{};
	std::function<void(FlowPlot::Spec::ScatterConfigSpec)> onChange = nullptr;

	mappingSettingsEditorShellParams shell{};
	numericInputCardParams jitterXInput{};
	numericInputCardParams jitterYInput{};
	numericInputCardParams domainPaddingInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	scatterConfigEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(scatterConfigEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(scatterConfigEditorParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(scatterConfigEditorParams, jitterXInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterConfigEditorParams, jitterYInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterConfigEditorParams, domainPaddingInput));

using ScatterConfigSettingsEditorDef = FlowUi::ElementDefinition<
	scatterConfigEditorParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("ScatterConfigSettingsEditor")>;

inline void scatterConfigEditorEmit(
	FlowPlot::Spec::ScatterConfigSpec nextSpec,
	const std::function<void(FlowPlot::Spec::ScatterConfigSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const ScatterConfigSettingsEditorDef kScatterConfigSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ScatterConfigSettingsEditorDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			ScatterConfigSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](ScatterConfigSettingsEditorDef::BuildContext& context) {
		mappingSettingsEditorDrawShell<ScatterConfigSettingsEditorDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-jitter-x", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.jitterXInput;
					params.hintText = "jitterX";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.jitterX);
					params.minValue = -1000000.0;
					params.maxValue = 1000000.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.jitterX = static_cast<float>(changed);
						scatterConfigEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/jitter-x").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-jitter-y", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.jitterYInput;
					params.hintText = "jitterY";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.jitterY);
					params.minValue = -1000000.0;
					params.maxValue = 1000000.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.jitterY = static_cast<float>(changed);
						scatterConfigEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/jitter-y").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-domain-padding", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.domainPaddingInput;
					params.hintText = "domainPadding";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.domainPadding);
					params.minValue = 0.0;
					params.maxValue = 1.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.domainPadding = static_cast<float>(changed);
						scatterConfigEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/domain-padding").setParameters(std::move(params)).draw();
				});
			});
	},
};
