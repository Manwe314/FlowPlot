#pragma once

#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "BuildElements/editors/basicInputs/stringInput.hpp"
#include "BuildElements/editors/histogramColorMappingSettings.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct histogramMappingEditorParams {
	std::string hintText = "Histogram Mappings";
	FlowPlot::Spec::HistogramMappingSpec value{};
	std::function<void(FlowPlot::Spec::HistogramMappingSpec)> onChange = nullptr;

	mappingSettingsEditorShellParams shell{};
	stringInputCardParams dataFieldInput{};
	enumPickerCardParams dataAxisInput{};
	stringInputCardParams colorFieldInput{};
	histogramColorMappingSettingsParams colorMappingInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	histogramMappingEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(histogramMappingEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(histogramMappingEditorParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(histogramMappingEditorParams, dataFieldInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramMappingEditorParams, dataAxisInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramMappingEditorParams, colorFieldInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramMappingEditorParams, colorMappingInput));

using HistogramMappingSettingsEditorDef = FlowUi::ElementDefinition<
	histogramMappingEditorParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("HistogramMappingSettingsEditor")>;

inline void histogramMappingEditorEmit(
	FlowPlot::Spec::HistogramMappingSpec nextSpec,
	const std::function<void(FlowPlot::Spec::HistogramMappingSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const HistogramMappingSettingsEditorDef kHistogramMappingSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](HistogramMappingSettingsEditorDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			HistogramMappingSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](HistogramMappingSettingsEditorDef::BuildContext& context) {
		mappingSettingsEditorDrawShell<HistogramMappingSettingsEditorDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-data-field", context.params.shell, [&](const std::string& rowPath) {
					stringInputCardParams params = context.params.dataFieldInput;
					params.hintText = "datafield schema name";
					params.value = context.params.value.dataField;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.dataField = std::string(changed);
						histogramMappingEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kStringInputCard, rowPath + "/data-field").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-data-axis", context.params.shell, [&](const std::string& rowPath) {
					enumPickerCardParams params = context.params.dataAxisInput;
					params.hintText = "Data Axis";
					params.options = histogramDataAxisOptions();
					params.value = histogramDataAxisToString(context.params.value.axis);
					params.defaultValue = "X";
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.axis = histogramDataAxisFromString(changed);
						histogramMappingEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kEnumPickerCard, rowPath + "/data-axis").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-color-field", context.params.shell, [&](const std::string& rowPath) {
					stringInputCardParams params = context.params.colorFieldInput;
					params.hintText = "colorfield schema name";
					params.value = context.params.value.colorField;
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.colorField = std::string(changed);
						histogramMappingEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kStringInputCard, rowPath + "/color-field").setParameters(std::move(params)).draw();
				});

				mappingSettingsEditorDrawInputRow(context, "row-color-mapping", context.params.shell, [&](const std::string& rowPath) {
					histogramColorMappingSettingsParams params = context.params.colorMappingInput;
					params.value = context.params.value.colorMapping;
					params.shell.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::HistogramColorMappingSpec changed) mutable {
						value.colorMapping = std::move(changed);
						histogramMappingEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kHistogramColorMappingSettings, rowPath + "/color-mapping")
						.setParameters(std::move(params))
						.draw();
				});
			});
	},
};
