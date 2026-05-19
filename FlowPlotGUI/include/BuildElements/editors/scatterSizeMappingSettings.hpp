#pragma once

#include <cstddef>
#include <functional>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/enumPicker.hpp"
#include "BuildElements/editors/basicInputs/numericInput.hpp"
#include "BuildElements/editors/basicInputs/twoColumnInput.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "FlowPlot_Defaults.hpp"

struct scatterSizeMappingSettingsParams {
	std::string hintText = "Scatter Size Mapping";
	FlowPlot::Spec::ScatterSizeMappingSpec value{};
	std::function<void(FlowPlot::Spec::ScatterSizeMappingSpec)> onChange = nullptr;

	mappingSettingsEditorShellParams shell{};
	enumPickerCardParams typeInput{};
	twoColumnInputCardParams binsInput{};
	numericInputCardParams minValueInput{};
	numericInputCardParams maxValueInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	scatterSizeMappingSettingsParams,
	FLOWUI_DEV_REFLECT_FIELD(scatterSizeMappingSettingsParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(scatterSizeMappingSettingsParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(scatterSizeMappingSettingsParams, typeInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterSizeMappingSettingsParams, binsInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterSizeMappingSettingsParams, minValueInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterSizeMappingSettingsParams, maxValueInput));

using ScatterSizeMappingSettingsDef = FlowUi::ElementDefinition<
	scatterSizeMappingSettingsParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("ScatterSizeMappingSettings")>;

inline void scatterSizeMappingSettingsEmit(
	FlowPlot::Spec::ScatterSizeMappingSpec nextSpec,
	const std::function<void(FlowPlot::Spec::ScatterSizeMappingSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const ScatterSizeMappingSettingsDef kScatterSizeMappingSettings = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ScatterSizeMappingSettingsDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			ScatterSizeMappingSettingsDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](ScatterSizeMappingSettingsDef::BuildContext& context) {
		std::vector<std::string> valueTexts = mappingSettingsFloatVectorToStrings(context.params.value.values);

		mappingSettingsEditorDrawShell<ScatterSizeMappingSettingsDef>(
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
					params.defaultValue = "contiguous";
					params.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](std::string_view changed) mutable {
						value.type = std::string(changed);
						scatterSizeMappingSettingsEmit(std::move(value), onChange);
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
						params.table.values = valueTexts;
						params.table.valueKind = twoColumnInputValueKind::Numeric;
						params.table.defaultCategory = "";
						params.table.defaultValue = "0";
						params.table.numericValue.valueType = numericInputValueType::Float;
						params.table.onAddRow = [value = context.params.value, onChange = context.params.onChange](
													 std::string_view category,
													 std::string_view mappedValue) mutable {
							value.categories.push_back(std::string(category));
							value.values.push_back(mappingSettingsParseFloatOrZero(mappedValue));
							scatterSizeMappingSettingsEmit(std::move(value), onChange);
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
							scatterSizeMappingSettingsEmit(std::move(value), onChange);
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
								value.values.resize(row + 1u, 0.0f);
							}
							value.categories[row] = std::string(changed);
							scatterSizeMappingSettingsEmit(std::move(value), onChange);
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
								value.values.resize(row + 1u, 0.0f);
							}
							value.values[row] = mappingSettingsParseFloatOrZero(changed);
							scatterSizeMappingSettingsEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kTwoColumnInputCard, rowPath + "/bins")
							.setParameters(std::move(params))
							.draw();
					});
				}
				else
				{
					mappingSettingsEditorDrawInputRow(context, "row-min-value", context.params.shell, [&](const std::string& rowPath) {
						numericInputCardParams params = context.params.minValueInput;
						mappingSettingsPrepareNumericInputCard(params);
						params.hintText = "Min Value";
						params.valueType = numericInputValueType::Float;
						params.value = static_cast<double>(context.params.value.minValue);
						params.minValue = -static_cast<double>(std::numeric_limits<float>::max());
						params.maxValue = static_cast<double>(std::numeric_limits<float>::max());
						params.fontId = context.params.shell.fontId;
						params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
							value.minValue = static_cast<float>(changed);
							scatterSizeMappingSettingsEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kNumericInputCard, rowPath + "/min-value")
							.setParameters(std::move(params))
							.draw();
					});

					mappingSettingsEditorDrawInputRow(context, "row-max-value", context.params.shell, [&](const std::string& rowPath) {
						numericInputCardParams params = context.params.maxValueInput;
						mappingSettingsPrepareNumericInputCard(params);
						params.hintText = "Max Value";
						params.valueType = numericInputValueType::Float;
						params.value = static_cast<double>(context.params.value.maxValue);
						params.minValue = -static_cast<double>(std::numeric_limits<float>::max());
						params.maxValue = static_cast<double>(std::numeric_limits<float>::max());
						params.fontId = context.params.shell.fontId;
						params.onChange = [value = context.params.value, onChange = context.params.onChange](double changed) mutable {
							value.maxValue = static_cast<float>(changed);
							scatterSizeMappingSettingsEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kNumericInputCard, rowPath + "/max-value")
							.setParameters(std::move(params))
							.draw();
					});
				}
			});
	},
};
