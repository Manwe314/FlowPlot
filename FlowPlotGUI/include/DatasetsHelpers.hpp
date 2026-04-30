#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "FlowPlot_Defaults.hpp"
#include "FlowPlotGui.hpp"


namespace FlowPlotGui {

using DatasetFieldType = FlowPlot::Spec::DatasetSpec::FieldType;

inline std::string makeDatasetName(std::size_t datasetNumber)
{
	return "dataset_" + std::to_string(datasetNumber);
}

inline bool hasDatasetName(const state& guiState, std::string_view name)
{
	for (const RunningDataset& dataset : guiState.datasets)
	{
		if (dataset.name == name)
		{
			return true;
		}
	}

	return false;
}

inline std::string makeNextDatasetName(const state& guiState)
{
	std::size_t datasetNumber = guiState.datasets.size() + 1;
	std::string name = makeDatasetName(datasetNumber);
	while (hasDatasetName(guiState, name))
	{
		++datasetNumber;
		name = makeDatasetName(datasetNumber);
	}

	return name;
}

inline bool addDataset(state& guiState)
{
	if (guiState.datasets.size() >= kMaxDatasetCount)
	{
		return false;
	}

	const std::string datasetName = makeNextDatasetName(guiState);

	RunningDataset runningDataset{};
	runningDataset.name = datasetName;
	guiState.datasets.push_back(std::move(runningDataset));

	FlowPlot::Spec::DatasetSpec datasetSpec{};
	datasetSpec.name = datasetName;
	guiState.activeTemplate.datasets.push_back(std::move(datasetSpec));

	return true;
}

inline bool removeDataset(state& guiState, std::size_t datasetIndex)
{
	if (guiState.datasets.size() <= 1 || datasetIndex >= guiState.datasets.size())
	{
		return false;
	}
	if (datasetIndex >= guiState.activeTemplate.datasets.size())
	{
		return false;
	}

	guiState.datasets.erase(guiState.datasets.begin() + static_cast<std::ptrdiff_t>(datasetIndex));
	guiState.activeTemplate.datasets.erase(
		guiState.activeTemplate.datasets.begin() + static_cast<std::ptrdiff_t>(datasetIndex));

	return true;
}

inline bool renameDataset(state& guiState, std::size_t datasetIndex, std::string_view name)
{
	if (name.empty() || datasetIndex >= guiState.datasets.size() || datasetIndex >= guiState.activeTemplate.datasets.size())
	{
		return false;
	}

	guiState.datasets[datasetIndex].name = std::string(name);
	guiState.activeTemplate.datasets[datasetIndex].name = std::string(name);

	return true;
}

inline std::size_t datasetRowCount(const RunningDataset& dataset)
{
	if (!dataset.numericColumns.empty())
	{
		return dataset.numericColumns.front().data.size();
	}
	if (!dataset.stringColumns.empty())
	{
		return dataset.stringColumns.front().data.size();
	}
	if (!dataset.boolColumns.empty())
	{
		return dataset.boolColumns.front().data.size();
	}

	return 0;
}

inline std::size_t datasetColumnCount(const RunningDataset& dataset)
{
	return dataset.numericColumns.size() + dataset.stringColumns.size() + dataset.boolColumns.size();
}

inline std::string makeColumnName(std::size_t columnNumber)
{
	return "column_" + std::to_string(columnNumber);
}

inline bool hasColumnName(const RunningDataset& dataset, std::string_view name)
{
	for (const numericColumn& column : dataset.numericColumns)
	{
		if (column.name == name)
		{
			return true;
		}
	}
	for (const stringColumn& column : dataset.stringColumns)
	{
		if (column.name == name)
		{
			return true;
		}
	}
	for (const boolColumn& column : dataset.boolColumns)
	{
		if (column.name == name)
		{
			return true;
		}
	}

	return false;
}

inline std::string makeNextColumnName(const RunningDataset& dataset)
{
	std::size_t columnNumber = datasetColumnCount(dataset) + 1;
	std::string name = makeColumnName(columnNumber);
	while (hasColumnName(dataset, name))
	{
		++columnNumber;
		name = makeColumnName(columnNumber);
	}

	return name;
}

inline bool addColumn(state& guiState, std::size_t datasetIndex, DatasetFieldType type)
{
	if (datasetIndex >= guiState.datasets.size() || datasetIndex >= guiState.activeTemplate.datasets.size())
	{
		return false;
	}

	RunningDataset& dataset = guiState.datasets[datasetIndex];
	const std::string columnName = makeNextColumnName(dataset);
	const std::size_t rowCount = datasetRowCount(dataset);
	guiState.activeTemplate.datasets[datasetIndex].schema[columnName] = type;

	switch (type)
	{
	case DatasetFieldType::Number: {
		numericColumn column{};
		column.name = columnName;
		column.data.resize(rowCount, 0.0);
		dataset.numericColumns.push_back(std::move(column));
		break;
	}
	case DatasetFieldType::String: {
		stringColumn column{};
		column.name = columnName;
		column.data.resize(rowCount);
		dataset.stringColumns.push_back(std::move(column));
		break;
	}
	case DatasetFieldType::Boolean: {
		boolColumn column{};
		column.name = columnName;
		column.data.resize(rowCount, false);
		dataset.boolColumns.push_back(std::move(column));
		break;
	}
	}

	return true;
}

inline bool removeColumn(state& guiState, std::size_t datasetIndex, DatasetFieldType type, std::size_t typedColumnIndex)
{
	if (datasetIndex >= guiState.datasets.size() || datasetIndex >= guiState.activeTemplate.datasets.size())
	{
		return false;
	}

	RunningDataset& dataset = guiState.datasets[datasetIndex];
	std::string columnName{};
	switch (type)
	{
	case DatasetFieldType::Number:
		if (typedColumnIndex >= dataset.numericColumns.size())
		{
			return false;
		}
		columnName = dataset.numericColumns[typedColumnIndex].name;
		dataset.numericColumns.erase(dataset.numericColumns.begin() + static_cast<std::ptrdiff_t>(typedColumnIndex));
		break;
	case DatasetFieldType::String:
		if (typedColumnIndex >= dataset.stringColumns.size())
		{
			return false;
		}
		columnName = dataset.stringColumns[typedColumnIndex].name;
		dataset.stringColumns.erase(dataset.stringColumns.begin() + static_cast<std::ptrdiff_t>(typedColumnIndex));
		break;
	case DatasetFieldType::Boolean:
		if (typedColumnIndex >= dataset.boolColumns.size())
		{
			return false;
		}
		columnName = dataset.boolColumns[typedColumnIndex].name;
		dataset.boolColumns.erase(dataset.boolColumns.begin() + static_cast<std::ptrdiff_t>(typedColumnIndex));
		break;
	}

	guiState.activeTemplate.datasets[datasetIndex].schema.erase(columnName);
	return true;
}

inline bool renameColumn(
	state& guiState,
	std::size_t datasetIndex,
	DatasetFieldType type,
	std::size_t typedColumnIndex,
	std::string_view name)
{
	if (name.empty() || datasetIndex >= guiState.datasets.size() || datasetIndex >= guiState.activeTemplate.datasets.size())
	{
		return false;
	}

	RunningDataset& dataset = guiState.datasets[datasetIndex];
	std::string oldName{};
	switch (type)
	{
	case DatasetFieldType::Number:
		if (typedColumnIndex >= dataset.numericColumns.size())
		{
			return false;
		}
		oldName = dataset.numericColumns[typedColumnIndex].name;
		dataset.numericColumns[typedColumnIndex].name = std::string(name);
		break;
	case DatasetFieldType::String:
		if (typedColumnIndex >= dataset.stringColumns.size())
		{
			return false;
		}
		oldName = dataset.stringColumns[typedColumnIndex].name;
		dataset.stringColumns[typedColumnIndex].name = std::string(name);
		break;
	case DatasetFieldType::Boolean:
		if (typedColumnIndex >= dataset.boolColumns.size())
		{
			return false;
		}
		oldName = dataset.boolColumns[typedColumnIndex].name;
		dataset.boolColumns[typedColumnIndex].name = std::string(name);
		break;
	}

	FlowPlot::Spec::DatasetSpec& datasetSpec = guiState.activeTemplate.datasets[datasetIndex];
	datasetSpec.schema.erase(oldName);
	datasetSpec.schema[std::string(name)] = type;
	return true;
}

inline bool addRow(state& guiState, std::size_t datasetIndex)
{
	if (datasetIndex >= guiState.datasets.size())
	{
		return false;
	}

	RunningDataset& dataset = guiState.datasets[datasetIndex];
	for (numericColumn& column : dataset.numericColumns)
	{
		column.data.push_back(0.0);
	}
	for (stringColumn& column : dataset.stringColumns)
	{
		column.data.emplace_back();
	}
	for (boolColumn& column : dataset.boolColumns)
	{
		column.data.push_back(false);
	}

	return true;
}

inline bool removeRow(state& guiState, std::size_t datasetIndex, std::size_t rowIndex)
{
	if (datasetIndex >= guiState.datasets.size())
	{
		return false;
	}

	RunningDataset& dataset = guiState.datasets[datasetIndex];
	const std::size_t rowCount = datasetRowCount(dataset);
	if (rowIndex >= rowCount)
	{
		return false;
	}

	for (numericColumn& column : dataset.numericColumns)
	{
		if (rowIndex < column.data.size())
		{
			column.data.erase(column.data.begin() + static_cast<std::ptrdiff_t>(rowIndex));
		}
	}
	for (stringColumn& column : dataset.stringColumns)
	{
		if (rowIndex < column.data.size())
		{
			column.data.erase(column.data.begin() + static_cast<std::ptrdiff_t>(rowIndex));
		}
	}
	for (boolColumn& column : dataset.boolColumns)
	{
		if (rowIndex < column.data.size())
		{
			column.data.erase(column.data.begin() + static_cast<std::ptrdiff_t>(rowIndex));
		}
	}

	return true;
}

inline bool setNumericCell(
	state& guiState,
	std::size_t datasetIndex,
	std::size_t typedColumnIndex,
	std::size_t rowIndex,
	double value)
{
	if (datasetIndex >= guiState.datasets.size())
	{
		return false;
	}
	RunningDataset& dataset = guiState.datasets[datasetIndex];
	if (typedColumnIndex >= dataset.numericColumns.size() || rowIndex >= dataset.numericColumns[typedColumnIndex].data.size())
	{
		return false;
	}

	dataset.numericColumns[typedColumnIndex].data[rowIndex] = value;
	return true;
}

inline bool setStringCell(
	state& guiState,
	std::size_t datasetIndex,
	std::size_t typedColumnIndex,
	std::size_t rowIndex,
	std::string_view value)
{
	if (datasetIndex >= guiState.datasets.size())
	{
		return false;
	}
	RunningDataset& dataset = guiState.datasets[datasetIndex];
	if (typedColumnIndex >= dataset.stringColumns.size() || rowIndex >= dataset.stringColumns[typedColumnIndex].data.size())
	{
		return false;
	}

	dataset.stringColumns[typedColumnIndex].data[rowIndex] = std::string(value);
	return true;
}

inline bool setBoolCell(
	state& guiState,
	std::size_t datasetIndex,
	std::size_t typedColumnIndex,
	std::size_t rowIndex,
	bool value)
{
	if (datasetIndex >= guiState.datasets.size())
	{
		return false;
	}
	RunningDataset& dataset = guiState.datasets[datasetIndex];
	if (typedColumnIndex >= dataset.boolColumns.size() || rowIndex >= dataset.boolColumns[typedColumnIndex].data.size())
	{
		return false;
	}

	dataset.boolColumns[typedColumnIndex].data[rowIndex] = value;
	return true;
}

} // namespace FlowPlotGui
