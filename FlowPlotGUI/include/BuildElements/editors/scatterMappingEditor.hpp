#pragma once

#include <cstddef>
#include <functional>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/editors/basicInputs/stringInput.hpp"
#include "BuildElements/editors/basicInputs/twoColumnInput.hpp"
#include "BuildElements/editors/mappingSettingsEditorCommon.hpp"
#include "BuildElements/editors/scatterColorMappingSettings.hpp"
#include "BuildElements/editors/scatterSizeMappingSettings.hpp"
#include "FlowPlot_Defaults.hpp"

struct scatterMappingEditorParams {
	std::string hintText = "Scatter Mappings";
	FlowPlot::Spec::ScatterMappingSpec value{};
	std::function<void(FlowPlot::Spec::ScatterMappingSpec)> onChange = nullptr;
	std::span<const std::string> labelValueOptions{};

	mappingSettingsEditorShellParams shell{};
	stringInputCardParams xFieldInput{};
	stringInputCardParams yFieldInput{};
	stringInputCardParams colorFieldInput{};
	scatterColorMappingSettingsParams colorMappingInput{};
	stringInputCardParams sizeFieldInput{};
	scatterSizeMappingSettingsParams sizeMappingInput{};
	stringInputCardParams labelFieldInput{};
	twoColumnInputCardParams labelMappingInput{};
};

FLOWUI_DEV_REGISTER_STRUCT(
	scatterMappingEditorParams,
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, hintText),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, shell),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, xFieldInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, yFieldInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, colorFieldInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, colorMappingInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, sizeFieldInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, sizeMappingInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, labelFieldInput),
	FLOWUI_DEV_REFLECT_FIELD(scatterMappingEditorParams, labelMappingInput));

using ScatterMappingSettingsEditorDef = FlowUi::ElementDefinition<
	scatterMappingEditorParams,
	mappingSettingsEditorState,
	mappingSettingsEditorResources,
	FLOW_DEF_ID("ScatterMappingSettingsEditor")>;

inline void scatterMappingEditorEmit(
	FlowPlot::Spec::ScatterMappingSpec nextSpec,
	const std::function<void(FlowPlot::Spec::ScatterMappingSpec)>& onChange)
{
	if (onChange != nullptr)
	{
		onChange(std::move(nextSpec));
	}
}

inline const ScatterMappingSettingsEditorDef kScatterMappingSettingsEditor = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ScatterMappingSettingsEditorDef::InteractionContext& context) {
		mappingSettingsEditorState& state =
			ScatterMappingSettingsEditorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!state.initialized)
		{
			state.initialized = true;
			state.isExpanded = context.params.shell.defaultExpanded;
		}
	},
	nullptr,
	+[](ScatterMappingSettingsEditorDef::BuildContext& context) {
		mappingSettingsEditorDrawShell<ScatterMappingSettingsEditorDef>(
			context,
			context.params.shell,
			context.params.hintText,
			[&]() {
				auto drawString = [&](std::string_view rowId, stringInputCardParams inputParams, std::string hint, std::string valueText, auto assign) {
					mappingSettingsEditorDrawInputRow(context, rowId, context.params.shell, [&](const std::string& rowPath) {
						stringInputCardParams params = std::move(inputParams);
						mappingSettingsPrepareStringInputCard(params);
						params.hintText = std::move(hint);
						params.value = std::move(valueText);
						params.fontId = context.params.shell.fontId;
						params.onChange = [value = context.params.value, onChange = context.params.onChange, assign](
											  std::string_view changed) mutable {
							assign(value, changed);
							scatterMappingEditorEmit(std::move(value), onChange);
						};
						context.uiManager.createElement(kStringInputCard, rowPath + "/input")
							.setParameters(std::move(params))
							.draw();
					});
				};

				drawString("row-x-field", context.params.xFieldInput, "xField Schema name", context.params.value.xField, [](auto& value, std::string_view changed) {
					value.xField = std::string(changed);
				});
				drawString("row-y-field", context.params.yFieldInput, "yField Schema name", context.params.value.yField, [](auto& value, std::string_view changed) {
					value.yField = std::string(changed);
				});
				drawString("row-color-field", context.params.colorFieldInput, "colorField Schema name", context.params.value.colorField, [](auto& value, std::string_view changed) {
					value.colorField = std::string(changed);
				});

				mappingSettingsEditorDrawInputRow(context, "row-color-mapping", context.params.shell, [&](const std::string& rowPath) {
					scatterColorMappingSettingsParams params = context.params.colorMappingInput;
					params.shell.cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
					params.value = context.params.value.colorMapping;
					params.shell.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::ScatterColorMappingSpec changed) mutable {
						value.colorMapping = std::move(changed);
						scatterMappingEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kScatterColorMappingSettings, rowPath + "/color-mapping")
						.setParameters(std::move(params))
						.draw();
				});

				drawString("row-size-field", context.params.sizeFieldInput, "sizefield Schema name", context.params.value.sizeField, [](auto& value, std::string_view changed) {
					value.sizeField = std::string(changed);
				});

				mappingSettingsEditorDrawInputRow(context, "row-size-mapping", context.params.shell, [&](const std::string& rowPath) {
					scatterSizeMappingSettingsParams params = context.params.sizeMappingInput;
					params.shell.cardSizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
					params.value = context.params.value.sizeMapping;
					params.shell.fontId = context.params.shell.fontId;
					params.onChange = [value = context.params.value, onChange = context.params.onChange](
										   FlowPlot::Spec::ScatterSizeMappingSpec changed) mutable {
						value.sizeMapping = std::move(changed);
						scatterMappingEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kScatterSizeMappingSettings, rowPath + "/size-mapping")
						.setParameters(std::move(params))
						.draw();
				});

				drawString("row-label-field", context.params.labelFieldInput, "labelfield Schema name", context.params.value.labelField, [](auto& value, std::string_view changed) {
					value.labelField = std::string(changed);
				});

				mappingSettingsEditorDrawInputRow(context, "row-label-mapping", context.params.shell, [&](const std::string& rowPath) {
					twoColumnInputCardParams params = context.params.labelMappingInput;
					mappingSettingsPrepareTwoColumnInputCard(params);
					params.hintText = "Label Mapping";
					params.fontId = context.params.shell.fontId;
					params.table.categories = context.params.value.labelMapping.categories;
					params.table.values = context.params.value.labelMapping.values;
					params.table.valueKind = twoColumnInputValueKind::Enum;
					params.table.enumOptions = context.params.labelValueOptions;
					params.table.defaultCategory = "";
					params.table.defaultValue = context.params.labelValueOptions.empty()
						? ""
						: context.params.labelValueOptions.front();
					params.table.onAddRow = [value = context.params.value, onChange = context.params.onChange](
												 std::string_view category,
												 std::string_view mappedValue) mutable {
						value.labelMapping.categories.push_back(std::string(category));
						value.labelMapping.values.push_back(std::string(mappedValue));
						scatterMappingEditorEmit(std::move(value), onChange);
					};
					params.table.onDeleteRow = [value = context.params.value, onChange = context.params.onChange](std::size_t row) mutable {
						if (row < value.labelMapping.categories.size())
						{
							value.labelMapping.categories.erase(value.labelMapping.categories.begin() + static_cast<std::ptrdiff_t>(row));
						}
						if (row < value.labelMapping.values.size())
						{
							value.labelMapping.values.erase(value.labelMapping.values.begin() + static_cast<std::ptrdiff_t>(row));
						}
						scatterMappingEditorEmit(std::move(value), onChange);
					};
					params.table.onCategoryChange = [value = context.params.value, onChange = context.params.onChange](
													   std::size_t row,
													   std::string_view changed) mutable {
						if (row >= value.labelMapping.categories.size())
						{
							value.labelMapping.categories.resize(row + 1u);
						}
						if (row >= value.labelMapping.values.size())
						{
							value.labelMapping.values.resize(row + 1u);
						}
						value.labelMapping.categories[row] = std::string(changed);
						scatterMappingEditorEmit(std::move(value), onChange);
					};
					params.table.onValueChange = [value = context.params.value, onChange = context.params.onChange](
													std::size_t row,
													std::string_view changed) mutable {
						if (row >= value.labelMapping.categories.size())
						{
							value.labelMapping.categories.resize(row + 1u);
						}
						if (row >= value.labelMapping.values.size())
						{
							value.labelMapping.values.resize(row + 1u);
						}
						value.labelMapping.values[row] = std::string(changed);
						scatterMappingEditorEmit(std::move(value), onChange);
					};
					context.uiManager.createElement(kTwoColumnInputCard, rowPath + "/label-mapping")
						.setParameters(std::move(params))
						.draw();
				});
			});
	},
};
