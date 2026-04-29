#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/colorPicker.hpp"
#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "BuildElements/editors/basicInputs/twoColumnInput.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct histogramColorMappingSettingsParams {
	std::string hintText = "Histogram Color Mapping";
	FlowPlot::Spec::HistogramColorMappingSpec value{};
	std::function<void(FlowPlot::Spec::HistogramColorMappingSpec)> onChange = nullptr;

	mappingSettingsEditorShellParams shell{};
	enumPickerCardParams typeInput{};
	twoColumnInputCardParams binsInput{};
	colorPickerCardParams minValueInput{};
	colorPickerCardParams maxValueInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	histogramColorMappingSettingsParams,
	FLOWUI_DEV_REFLECT_FIELD(histogramColorMappingSettingsParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(histogramColorMappingSettingsParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(histogramColorMappingSettingsParams, typeInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramColorMappingSettingsParams, binsInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramColorMappingSettingsParams, minValueInput),
	FLOWUI_DEV_REFLECT_FIELD(histogramColorMappingSettingsParams, maxValueInput));

using HistogramColorMappingSettingsDef = FlowUi::ElementDefinition<
	histogramColorMappingSettingsParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("HistogramColorMappingSettings")>;

inline void histogramColorMappingSettingsEmit(
	FlowPlot::Spec::HistogramColorMappingSpec nextSpec,
	const std::function<void(FlowPlot::Spec::HistogramColorMappingSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const HistogramColorMappingSettingsDef kHistogramColorMappingSettings = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](HistogramColorMappingSettingsDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			HistogramColorMappingSettingsDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](HistogramColorMappingSettingsDef::BuildContext& context) {
		mappingSettingsEditorDrawShell<HistogramColorMappingSettingsDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-type", context.params.shell, [&](const std::string& rowPath) {
					enumPickerCardParams params = context.params.typeInput;
					params.hintText = "Type";
					params.options = mappingSettingsTypeOptions();
					params.value = context.params.value.type;
					params.defaultValue = "bins";
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.type = std::string(changed);
						histogramColorMappingSettingsEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kEnumPickerCard, rowPath + "/type")
						.setParameters(std::move(params))
						.draw();
				});

				if (mappingSettingsUsesBins(context.params.value.type))
				{
					mappingSettingsEditorDrawInputRow(context, "row-bins", context.params.shell, [&](const std::string& rowPath) {
						twoColumnInputCardParams params = context.params.binsInput;
						params.hintText = "Bins";
						params.fontId = context.params.shell.fontId;
						params.table.categories = context.params.value.categories;
						params.table.values = context.params.value.values;
						params.table.valueKind = twoColumnInputValueKind::Color;
						params.table.defaultCategory = "";
						params.table.defaultValue = "#000000";
						params.table.onAddRow = [value = context.params.value, onChange = context.params.onChange](
													 std::string_view category,
													 std::string_view mappedValue) mutable {
							value.categories.push_back(std::string(category));
							value.values.push_back(std::string(mappedValue));
							histogramColorMappingSettingsEmit(std::move(value), onChange);
						};
						params.table.onDeleteRow = [value = context.params.value, onChange = context.params.onChange](std::size_t row) mutable {
							if (row < value.categories.size())
							{
								value.categories.erase(value.categories.begin() + static_cast<std::ptrdiff_t>(row));
							}
							if (row < value.values.size())
							{
								value.values.erase(value.values.begin() + static_cast<std::ptrdiff_t>(row));
							}
							histogramColorMappingSettingsEmit(std::move(value), onChange);
						};
						params.table.onCategoryChange = [value = context.params.value, onChange = context.params.onChange](
														   std::size_t row,
														   std::string_view changed) mutable {
							if (row >= value.categories.size())
							{
								value.categories.resize(row + 1u);
							}
							if (row >= value.values.size())
							{
								value.values.resize(row + 1u, "#000000");
							}
							value.categories[row] = std::string(changed);
							histogramColorMappingSettingsEmit(std::move(value), onChange);
						};
						params.table.onValueChange = [value = context.params.value, onChange = context.params.onChange](
														std::size_t row,
														std::string_view changed) mutable {
							if (row >= value.categories.size())
							{
								value.categories.resize(row + 1u);
							}
							if (row >= value.values.size())
							{
								value.values.resize(row + 1u, "#000000");
							}
							value.values[row] = std::string(changed);
							histogramColorMappingSettingsEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kTwoColumnInputCard, rowPath + "/bins")
							.setParameters(std::move(params))
							.draw();
					});
				}
				else
				{
					mappingSettingsEditorDrawInputRow(context, "row-min-value", context.params.shell, [&](const std::string& rowPath) {
						colorPickerCardParams params = context.params.minValueInput;
						params.hintText = "Min Value";
						params.value = context.params.value.minValue;
						params.fontId = context.params.shell.fontId;
						params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
							value.minValue = std::string(changed);
							histogramColorMappingSettingsEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kColorPickerCard, rowPath + "/min-value")
							.setParameters(std::move(params))
							.draw();
					});

					mappingSettingsEditorDrawInputRow(context, "row-max-value", context.params.shell, [&](const std::string& rowPath) {
						colorPickerCardParams params = context.params.maxValueInput;
						params.hintText = "Max Value";
						params.value = context.params.value.maxValue;
						params.fontId = context.params.shell.fontId;
						params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
							value.maxValue = std::string(changed);
							histogramColorMappingSettingsEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kColorPickerCard, rowPath + "/max-value")
							.setParameters(std::move(params))
							.draw();
					});
				}
			});
	},
};
