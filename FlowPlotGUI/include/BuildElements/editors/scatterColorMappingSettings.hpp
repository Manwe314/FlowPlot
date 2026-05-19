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

struct scatterColorMappingSettingsParams {
	std::string hintText = "Scatter Color Mapping";
	FlowPlot::Spec::ScatterColorMappingSpec value{};
	std::function<void(FlowPlot::Spec::ScatterColorMappingSpec)> onChange = nullptr;

	mappingSettingsEditorShellParams shell{};
	enumPickerCardParams typeInput{};
	twoColumnInputCardParams binsInput{};
	colorPickerCardParams minValueInput{};
	colorPickerCardParams maxValueInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	scatterColorMappingSettingsParams,
	FLOWUI_DEV_REFLECT_FIELD(scatterColorMappingSettingsParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(scatterColorMappingSettingsParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(scatterColorMappingSettingsParams, typeInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterColorMappingSettingsParams, binsInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterColorMappingSettingsParams, minValueInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterColorMappingSettingsParams, maxValueInput));

using ScatterColorMappingSettingsDef = FlowUi::ElementDefinition<
	scatterColorMappingSettingsParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("ScatterColorMappingSettings")>;

inline void scatterColorMappingSettingsEmit(
	FlowPlot::Spec::ScatterColorMappingSpec nextSpec,
	const std::function<void(FlowPlot::Spec::ScatterColorMappingSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const ScatterColorMappingSettingsDef kScatterColorMappingSettings = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ScatterColorMappingSettingsDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			ScatterColorMappingSettingsDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](ScatterColorMappingSettingsDef::BuildContext& context) {
		mappingSettingsEditorDrawShell<ScatterColorMappingSettingsDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				mappingSettingsEditorDrawInputRow(context, "row-type", context.params.shell, [&](const std::string& rowPath) {
					enumPickerCardParams params = context.params.typeInput;
					mappingSettingsPrepareEnumPickerCard(params);
					params.hintText = "Type";
					params.options = mappingSettingsTypeOptions();
					params.value = context.params.value.type;
					params.defaultValue = "bins";
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.type = std::string(changed);
						scatterColorMappingSettingsEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kEnumPickerCard, rowPath + "/type")
						.setParameters(std::move(params))
						.draw();
				});

				if (mappingSettingsUsesBins(context.params.value.type))
				{
					mappingSettingsEditorDrawInputRow(context, "row-bins", context.params.shell, [&](const std::string& rowPath) {
						twoColumnInputCardParams params = context.params.binsInput;
						mappingSettingsPrepareTwoColumnInputCard(params);
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
							scatterColorMappingSettingsEmit(std::move(value), onChange);
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
							scatterColorMappingSettingsEmit(std::move(value), onChange);
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
							scatterColorMappingSettingsEmit(std::move(value), onChange);
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
							scatterColorMappingSettingsEmit(std::move(value), onChange);
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
						mappingSettingsPrepareColorPickerCard(params);
						params.hintText = "Min Value";
						params.value = context.params.value.minValue;
						params.fontId = context.params.shell.fontId;
						params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
							value.minValue = std::string(changed);
							scatterColorMappingSettingsEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kColorPickerCard, rowPath + "/min-value")
							.setParameters(std::move(params))
							.draw();
					});

					mappingSettingsEditorDrawInputRow(context, "row-max-value", context.params.shell, [&](const std::string& rowPath) {
						colorPickerCardParams params = context.params.maxValueInput;
						mappingSettingsPrepareColorPickerCard(params);
						params.hintText = "Max Value";
						params.value = context.params.value.maxValue;
						params.fontId = context.params.shell.fontId;
						params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
							value.maxValue = std::string(changed);
							scatterColorMappingSettingsEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kColorPickerCard, rowPath + "/max-value")
							.setParameters(std::move(params))
							.draw();
					});
				}
			});
	},
};
