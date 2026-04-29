#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <string>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/basicInputs/toggle.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct histogramConfigEditorParams {
	std::string hintText = "Histogram Config";
	FlowPlot::Spec::HistogramConfigSpec value{};
	std::function<void(FlowPlot::Spec::HistogramConfigSpec)> onChange = nullptr;

	mappingSettingsEditorShellParams shell{};
	numericInputCardParams binCountInput{};
	toggleCardParams normalizeInput{};
	toggleCardParams cumulativeInput{};
	toggleCardParams showEmptyBinsInput{};
	numericInputCardParams domainPaddingInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	histogramConfigEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(histogramConfigEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(histogramConfigEditorParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(histogramConfigEditorParams, binCountInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramConfigEditorParams, normalizeInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramConfigEditorParams, cumulativeInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramConfigEditorParams, showEmptyBinsInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramConfigEditorParams, domainPaddingInput));

using HistogramConfigSettingsEditorDef = FlowUi::ElementDefinition<
	histogramConfigEditorParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("HistogramConfigSettingsEditor")>;

inline void histogramConfigEditorEmit(
	FlowPlot::Spec::HistogramConfigSpec nextSpec,
	const std::function<void(FlowPlot::Spec::HistogramConfigSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const HistogramConfigSettingsEditorDef kHistogramConfigSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](HistogramConfigSettingsEditorDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			HistogramConfigSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](HistogramConfigSettingsEditorDef::BuildContext& context) {
		mappingSettingsEditorDrawShell<HistogramConfigSettingsEditorDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-bin-count", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.binCountInput;
					params.hintText = "binCount";
					params.valueType = numericInputValueType::UInt32;
					params.value = static_cast<double>(context.params.value.binCount);
					params.minValue = 1.0;
					params.maxValue = static_cast<double>(std::numeric_limits<std::uint32_t>::max());
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.binCount = static_cast<std::uint32_t>(numericInputNormalizeValue(
							numericInputValueType::UInt32,
							changed,
							1.0,
							static_cast<double>(std::numeric_limits<std::uint32_t>::max())));
						histogramConfigEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/bin-count").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-normalize", context.params.shell, [&](const std::string& rowPath) {
					toggleCardParams params = context.params.normalizeInput;
					params.hintText = "Normalize";
					params.value = context.params.value.normalize;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](bool changed) mutable {
						value.normalize = changed;
						histogramConfigEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kToggleCard, rowPath + "/normalize").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-cumulative", context.params.shell, [&](const std::string& rowPath) {
					toggleCardParams params = context.params.cumulativeInput;
					params.hintText = "Cumulative";
					params.value = context.params.value.cumulative;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](bool changed) mutable {
						value.cumulative = changed;
						histogramConfigEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kToggleCard, rowPath + "/cumulative").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-show-empty-bins", context.params.shell, [&](const std::string& rowPath) {
					toggleCardParams params = context.params.showEmptyBinsInput;
					params.hintText = "Show Empty Bins";
					params.value = context.params.value.showEmptyBins;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](bool changed) mutable {
						value.showEmptyBins = changed;
						histogramConfigEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kToggleCard, rowPath + "/show-empty-bins").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-domain-padding", context.params.shell, [&](const std::string& rowPath) {
					numericInputCardParams params = context.params.domainPaddingInput;
					params.hintText = "domain padding";
					params.valueType = numericInputValueType::Float;
					params.value = static_cast<double>(context.params.value.domainPadding);
					params.minValue = 0.0;
					params.maxValue = 1.0;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
						value.domainPadding = static_cast<float>(changed);
						histogramConfigEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kNumericInputCard, rowPath + "/domain-padding").setParameters(std::move(params)).draw();
				});
			});
	},
};
