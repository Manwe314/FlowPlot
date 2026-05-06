#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "FlowPlot_Defaults.hpp"
#include "FlowPlot.hpp"

namespace FlowPlotGui {
inline constexpr std::size_t kInitialDatasetCapacity = 3;
inline constexpr std::size_t kInitialColumnsPerTypeCapacity = 4;
inline constexpr std::size_t kInitialRowsCapacity = 8;
inline constexpr std::size_t kMaxDatasetCount = 10;

enum class GlobalState : uint8_t
{
	working,
	exporting,
};

enum class TemplateNodeKind : uint8_t {
	Figure,
	FigureTitle,
	PanelsGroup,
	Panel,
	PanelTitle,
	XAxis,
	YAxis,
	XSecondaryAxis,
	YSecondaryAxis,
	AxisTitle,
	LayersGroup,
	Layer,
	LegendsGroup,
	Legend,
	LegendElement,
};

struct TemplateNodeKey {
	TemplateNodeKind kind = TemplateNodeKind::Figure;
	std::string outer{};
	std::string inner{};
	std::string flowElementId{};
};

struct numericColumn {
	numericColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<double> data;
	std::string name;
};

struct stringColumn {
	stringColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<std::string> data;
	std::string name;
};

struct boolColumn {
	boolColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<bool> data;
	std::string name;
};

struct RunningDataset {
	RunningDataset()
	{
		numericColumns.reserve(kInitialColumnsPerTypeCapacity);
		stringColumns.reserve(kInitialColumnsPerTypeCapacity);
		boolColumns.reserve(kInitialColumnsPerTypeCapacity);
	}

	std::vector<numericColumn> numericColumns;
	std::vector<stringColumn> stringColumns;
	std::vector<boolColumn> boolColumns; 

	std::string name;
};

struct AddedFontVariant {
	std::string family = "Default";
	std::uint16_t weight = 400;
	FlowPlot::FontStyle style = FlowPlot::FontStyle::Normal;
	std::filesystem::path path{};
};

inline void appendUniqueString(std::vector<std::string>& values, std::string value)
{
	if (std::find(values.begin(), values.end(), value) == values.end())
	{
		values.push_back(std::move(value));
	}
}

inline std::vector<std::string> availableFontFamilies(const std::vector<AddedFontVariant>& fontLibrary)
{
	std::vector<std::string> families{};
	families.reserve(fontLibrary.size() + 1U);
	families.push_back("Default");

	for (const AddedFontVariant& variant : fontLibrary)
	{
		if (!variant.family.empty())
		{
			appendUniqueString(families, variant.family);
		}
	}

	if (families.size() > 1U)
	{
		std::sort(families.begin() + 1, families.end());
	}
	return families;
}

inline std::vector<std::string> availableWeightsForFamily(
	const std::vector<AddedFontVariant>& fontLibrary,
	std::string_view family)
{
	std::vector<std::uint16_t> weights{};
	weights.push_back(400);

	for (const AddedFontVariant& variant : fontLibrary)
	{
		if (variant.family == family
			&& std::find(weights.begin(), weights.end(), variant.weight) == weights.end())
		{
			weights.push_back(variant.weight);
		}
	}

	std::sort(weights.begin(), weights.end());

	std::vector<std::string> options{};
	options.reserve(weights.size());
	for (const std::uint16_t weight : weights)
	{
		options.push_back(std::to_string(weight));
	}
	return options;
}

inline std::vector<std::string> availableStylesForFamilyWeight(
	const std::vector<AddedFontVariant>& fontLibrary,
	std::string_view family,
	std::uint16_t weight)
{
	std::vector<FlowPlot::FontStyle> styles{};
	styles.push_back(FlowPlot::FontStyle::Normal);

	for (const AddedFontVariant& variant : fontLibrary)
	{
		if (variant.family == family
			&& variant.weight == weight
			&& std::find(styles.begin(), styles.end(), variant.style) == styles.end())
		{
			styles.push_back(variant.style);
		}
	}

	std::sort(styles.begin(), styles.end(), [](FlowPlot::FontStyle lhs, FlowPlot::FontStyle rhs) {
		return static_cast<std::uint8_t>(lhs) < static_cast<std::uint8_t>(rhs);
	});

	std::vector<std::string> options{};
	options.reserve(styles.size());
	for (const FlowPlot::FontStyle style : styles)
	{
		options.emplace_back(FlowPlot::fontStyleName(style));
	}
	return options;
}

struct state {
	state()
	{
		activeTemplate.datasets.reserve(kInitialDatasetCapacity);
		datasets.reserve(kInitialDatasetCapacity);
	}

	GlobalState globalAppState = GlobalState::working;
	std::uint64_t templateRevision = 1;
	std::uint64_t datasetRevision = 1;
	std::uint64_t viewportRevision = 1;
	FlowPlot::Spec::MasterTemplateSpec activeTemplate{};
	std::optional<TemplateNodeKey> selectedNode{};
	std::vector<RunningDataset> datasets;
	std::vector<AddedFontVariant> fontLibrary{};
	std::shared_ptr<FlowPlot::ITextEngine> textEngine{};
	std::filesystem::path lastFontDialogDirectory{};
};



} // namespace FlowPlotGui
