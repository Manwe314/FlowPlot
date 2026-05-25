#define FLOW_PLOT_RENDERER
#define FLOW_PLOT_IMPLEMENTATION
#include "templateExport.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <fstream>
#include <optional>
#include <set>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <nfd_glfw3.h>

namespace FlowPlotGui {
namespace {

using Allocator = rapidjson::Document::AllocatorType;
using JsonValue = rapidjson::Value;

std::string toLowerAscii(std::string value)
{
	for (char& c : value)
	{
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	return value;
}

JsonValue jsonString(std::string_view text, Allocator& allocator)
{
	JsonValue value;
	value.SetString(text.data(), static_cast<rapidjson::SizeType>(text.size()), allocator);
	return value;
}

void addMember(JsonValue& object, std::string_view key, JsonValue&& value, Allocator& allocator)
{
	object.AddMember(jsonString(key, allocator), value.Move(), allocator);
}

void addString(JsonValue& object, std::string_view key, std::string_view value, Allocator& allocator)
{
	addMember(object, key, jsonString(value, allocator), allocator);
}

void addBool(JsonValue& object, std::string_view key, bool value, Allocator& allocator)
{
	JsonValue json;
	json.SetBool(value);
	addMember(object, key, std::move(json), allocator);
}

void addUint(JsonValue& object, std::string_view key, std::uint32_t value, Allocator& allocator)
{
	JsonValue json;
	json.SetUint(value);
	addMember(object, key, std::move(json), allocator);
}

void addInt(JsonValue& object, std::string_view key, std::int32_t value, Allocator& allocator)
{
	JsonValue json;
	json.SetInt(value);
	addMember(object, key, std::move(json), allocator);
}

void addFloat(JsonValue& object, std::string_view key, float value, Allocator& allocator)
{
	JsonValue json;
	json.SetFloat(value);
	addMember(object, key, std::move(json), allocator);
}

template <typename T>
void addIfChanged(JsonValue& object, const char* key, const T& value, const T& defaultValue, Allocator& allocator)
{
	if (value != defaultValue)
	{
		if constexpr (std::is_same_v<T, bool>)
		{
			addBool(object, key, value, allocator);
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			addString(object, key, value, allocator);
		}
		else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>)
		{
			addUint(object, key, static_cast<std::uint32_t>(value), allocator);
		}
		else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>)
		{
			addInt(object, key, static_cast<std::int32_t>(value), allocator);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			addFloat(object, key, static_cast<float>(value), allocator);
		}
	}
}

JsonValue writeStringArray(const std::vector<std::string>& values, Allocator& allocator)
{
	JsonValue array(rapidjson::kArrayType);
	for (const std::string& value : values)
	{
		array.PushBack(jsonString(value, allocator), allocator);
	}
	return array;
}

JsonValue writeFloatArray(const std::vector<float>& values, Allocator& allocator)
{
	JsonValue array(rapidjson::kArrayType);
	for (const float value : values)
	{
		JsonValue json;
		json.SetFloat(value);
		array.PushBack(json, allocator);
	}
	return array;
}

JsonValue writeDoubleArray(const std::vector<double>& values, Allocator& allocator)
{
	JsonValue array(rapidjson::kArrayType);
	for (const double value : values)
	{
		JsonValue json;
		json.SetDouble(value);
		array.PushBack(json, allocator);
	}
	return array;
}

JsonValue writeOptionalFloat(const std::optional<float>& value, Allocator&)
{
	JsonValue json;
	if (value.has_value())
	{
		json.SetFloat(*value);
	}
	else
	{
		json.SetNull();
	}
	return json;
}

JsonValue writeBoxSpec(
	const FlowPlot::Spec::BoxSpec& box,
	const FlowPlot::Spec::BoxSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	if (box.x != defaults.x)
	{
		addMember(object, "x", writeOptionalFloat(box.x, allocator), allocator);
	}
	if (box.y != defaults.y)
	{
		addMember(object, "y", writeOptionalFloat(box.y, allocator), allocator);
	}
	if (box.width != defaults.width)
	{
		addMember(object, "width", writeOptionalFloat(box.width, allocator), allocator);
	}
	if (box.height != defaults.height)
	{
		addMember(object, "height", writeOptionalFloat(box.height, allocator), allocator);
	}
	return object;
}

JsonValue writePaddingSpec(
	const FlowPlot::Spec::PaddingSpec& padding,
	const FlowPlot::Spec::PaddingSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "left", padding.left, defaults.left, allocator);
	addIfChanged(object, "right", padding.right, defaults.right, allocator);
	addIfChanged(object, "top", padding.top, defaults.top, allocator);
	addIfChanged(object, "bottom", padding.bottom, defaults.bottom, allocator);
	return object;
}

JsonValue writeTextSpec(
	const FlowPlot::Spec::TextSpec& text,
	const FlowPlot::Spec::TextSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "visible", text.visible, defaults.visible, allocator);
	addIfChanged(object, "text", text.text, defaults.text, allocator);
	addIfChanged(object, "fontFamily", text.fontFamily, defaults.fontFamily, allocator);
	addIfChanged(object, "fontSize", text.fontSize, defaults.fontSize, allocator);
	addIfChanged(object, "fontWeight", text.fontWeight, defaults.fontWeight, allocator);
	addIfChanged(object, "fontStyle", text.fontStyle, defaults.fontStyle, allocator);
	addIfChanged(object, "color", text.color, defaults.color, allocator);
	addIfChanged(object, "overflow", text.overflow, defaults.overflow, allocator);
	addIfChanged(object, "clip", text.clip, defaults.clip, allocator);
	addIfChanged(object, "hAlign", text.hAlign, defaults.hAlign, allocator);
	addIfChanged(object, "vAlign", text.vAlign, defaults.vAlign, allocator);

	JsonValue box = writeBoxSpec(text.box, defaults.box, allocator);
	if (!box.ObjectEmpty())
	{
		addMember(object, "box", std::move(box), allocator);
	}
	return object;
}

std::string axisDataRoleToString(FlowPlot::Spec::AxisDataRole role)
{
	switch (role)
	{
	case FlowPlot::Spec::AxisDataRole::Primary:
		return "primary";
	case FlowPlot::Spec::AxisDataRole::Secondary:
		return "secondary";
	case FlowPlot::Spec::AxisDataRole::Null:
	default:
		return "null";
	}
}

std::string histogramAxisToString(FlowPlot::Spec::HistogramDataAxis axis)
{
	return axis == FlowPlot::Spec::HistogramDataAxis::Y ? "y" : "x";
}

std::string datasetFieldTypeToString(FlowPlot::Spec::DatasetSpec::FieldType type)
{
	switch (type)
	{
	case FlowPlot::Spec::DatasetSpec::FieldType::String:
		return "string";
	case FlowPlot::Spec::DatasetSpec::FieldType::Boolean:
		return "boolean";
	case FlowPlot::Spec::DatasetSpec::FieldType::Number:
	default:
		return "number";
	}
}

JsonValue writeLegendElementSpec(
	const FlowPlot::Spec::LegendElementSpec& element,
	const FlowPlot::Spec::LegendElementSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "id", element.id, defaults.id, allocator);
	addIfChanged(object, "text", element.text, defaults.text, allocator);
	addIfChanged(object, "fontFamily", element.fontFamily, defaults.fontFamily, allocator);
	addIfChanged(object, "fontSize", element.fontSize, defaults.fontSize, allocator);
	addIfChanged(object, "fontWeight", element.fontWeight, defaults.fontWeight, allocator);
	addIfChanged(object, "fontStyle", element.fontStyle, defaults.fontStyle, allocator);
	addIfChanged(object, "color", element.color, defaults.color, allocator);
	addIfChanged(object, "overflow", element.overflow, defaults.overflow, allocator);
	addIfChanged(object, "clip", element.clip, defaults.clip, allocator);
	addIfChanged(object, "iconShape", element.iconShape, defaults.iconShape, allocator);
	addIfChanged(object, "iconColor", element.iconColor, defaults.iconColor, allocator);

	JsonValue box = writeBoxSpec(element.box, defaults.box, allocator);
	if (!box.ObjectEmpty())
	{
		addMember(object, "box", std::move(box), allocator);
	}
	return object;
}

JsonValue writeLegendSpec(
	const FlowPlot::Spec::LegendSpec& legend,
	const FlowPlot::Spec::LegendSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "id", legend.id, defaults.id, allocator);
	addIfChanged(object, "visible", legend.visible, defaults.visible, allocator);
	addIfChanged(object, "background", legend.background, defaults.background, allocator);
	addIfChanged(object, "borderColor", legend.borderColor, defaults.borderColor, allocator);
	addIfChanged(object, "borderWidth", legend.borderWidth, defaults.borderWidth, allocator);
	addIfChanged(object, "gap", legend.gap, defaults.gap, allocator);

	JsonValue padding = writePaddingSpec(legend.padding, defaults.padding, allocator);
	if (!padding.ObjectEmpty())
	{
		addMember(object, "padding", std::move(padding), allocator);
	}
	JsonValue box = writeBoxSpec(legend.box, defaults.box, allocator);
	if (!box.ObjectEmpty())
	{
		addMember(object, "box", std::move(box), allocator);
	}
	if (!legend.legendElements.empty())
	{
		JsonValue elements(rapidjson::kArrayType);
		const FlowPlot::Spec::LegendElementSpec elementDefaults{};
		for (const FlowPlot::Spec::LegendElementSpec& element : legend.legendElements)
		{
			elements.PushBack(writeLegendElementSpec(element, elementDefaults, allocator), allocator);
		}
		addMember(object, "legendElements", std::move(elements), allocator);
	}
	return object;
}

JsonValue writeAxisSpec(
	const FlowPlot::Spec::AxisSpec& axis,
	const FlowPlot::Spec::AxisSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "visible", axis.visible, defaults.visible, allocator);
	addIfChanged(object, "scale", axis.scale, defaults.scale, allocator);
	if (axis.min != defaults.min)
	{
		addMember(object, "min", writeOptionalFloat(axis.min, allocator), allocator);
	}
	if (axis.max != defaults.max)
	{
		addMember(object, "max", writeOptionalFloat(axis.max, allocator), allocator);
	}
	addIfChanged(object, "invert", axis.invert, defaults.invert, allocator);
	addIfChanged(object, "grid", axis.grid, defaults.grid, allocator);
	addIfChanged(object, "gridColor", axis.gridColor, defaults.gridColor, allocator);
	addIfChanged(object, "gridWidth", axis.gridWidth, defaults.gridWidth, allocator);
	addIfChanged(object, "lineColor", axis.lineColor, defaults.lineColor, allocator);
	addIfChanged(object, "lineWidth", axis.lineWidth, defaults.lineWidth, allocator);
	addIfChanged(object, "tickColor", axis.tickColor, defaults.tickColor, allocator);
	addIfChanged(object, "tickWidth", axis.tickWidth, defaults.tickWidth, allocator);
	addIfChanged(object, "tickLength", axis.tickLength, defaults.tickLength, allocator);
	addIfChanged(object, "tickCount", axis.tickCount, defaults.tickCount, allocator);
	if (axis.tickValues != defaults.tickValues)
	{
		addMember(object, "tickValues", writeDoubleArray(axis.tickValues, allocator), allocator);
	}
	addIfChanged(object, "tickValueGap", axis.tickValueGap, defaults.tickValueGap, allocator);
	addIfChanged(object, "tickLabelFormat", axis.tickLabelFormat, defaults.tickLabelFormat, allocator);
	addIfChanged(object, "tickLabelFontFamily", axis.tickLabelFontFamily, defaults.tickLabelFontFamily, allocator);
	addIfChanged(object, "tickLabelFontSize", axis.tickLabelFontSize, defaults.tickLabelFontSize, allocator);
	addIfChanged(object, "tickLabelFontWeight", axis.tickLabelFontWeight, defaults.tickLabelFontWeight, allocator);
	addIfChanged(object, "tickLabelFontStyle", axis.tickLabelFontStyle, defaults.tickLabelFontStyle, allocator);
	addIfChanged(object, "tickLabelColor", axis.tickLabelColor, defaults.tickLabelColor, allocator);
	addIfChanged(object, "showMinorTicks", axis.showMinorTicks, defaults.showMinorTicks, allocator);
	addIfChanged(object, "minorTickCount", axis.minorTickCount, defaults.minorTickCount, allocator);

	JsonValue title = writeTextSpec(axis.title, defaults.title, allocator);
	if (!title.ObjectEmpty())
	{
		addMember(object, "title", std::move(title), allocator);
	}
	return object;
}

JsonValue writeAxisDataSpec(
	const FlowPlot::Spec::LayerAxisDataSpec& axisData,
	const FlowPlot::Spec::LayerAxisDataSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	if (axisData.x != defaults.x)
	{
		addString(object, "x", axisDataRoleToString(axisData.x), allocator);
	}
	if (axisData.y != defaults.y)
	{
		addString(object, "y", axisDataRoleToString(axisData.y), allocator);
	}
	return object;
}

JsonValue writeScatterColorMapping(
	const FlowPlot::Spec::ScatterColorMappingSpec& mapping,
	const FlowPlot::Spec::ScatterColorMappingSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "type", mapping.type, defaults.type, allocator);
	if (mapping.categories != defaults.categories)
	{
		addMember(object, "categories", writeStringArray(mapping.categories, allocator), allocator);
	}
	if (mapping.values != defaults.values)
	{
		addMember(object, "values", writeStringArray(mapping.values, allocator), allocator);
	}
	addIfChanged(object, "minValue", mapping.minValue, defaults.minValue, allocator);
	addIfChanged(object, "maxValue", mapping.maxValue, defaults.maxValue, allocator);
	return object;
}

JsonValue writeScatterSizeMapping(
	const FlowPlot::Spec::ScatterSizeMappingSpec& mapping,
	const FlowPlot::Spec::ScatterSizeMappingSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "type", mapping.type, defaults.type, allocator);
	if (mapping.categories != defaults.categories)
	{
		addMember(object, "categories", writeStringArray(mapping.categories, allocator), allocator);
	}
	if (mapping.values != defaults.values)
	{
		addMember(object, "values", writeFloatArray(mapping.values, allocator), allocator);
	}
	addIfChanged(object, "minValue", mapping.minValue, defaults.minValue, allocator);
	addIfChanged(object, "maxValue", mapping.maxValue, defaults.maxValue, allocator);
	return object;
}

JsonValue writeScatterLabelMapping(
	const FlowPlot::Spec::ScatterLabelMappingSpec& mapping,
	const FlowPlot::Spec::ScatterLabelMappingSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	if (mapping.categories != defaults.categories)
	{
		addMember(object, "categories", writeStringArray(mapping.categories, allocator), allocator);
	}
	if (mapping.values != defaults.values)
	{
		addMember(object, "values", writeStringArray(mapping.values, allocator), allocator);
	}
	return object;
}

JsonValue writeScatterMappingSpec(
	const FlowPlot::Spec::ScatterMappingSpec& mapping,
	const FlowPlot::Spec::ScatterMappingSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	auto addFieldSection = [&](const char* key, std::string_view field, std::string_view defaultField, JsonValue mappingObject) {
		if (field == defaultField && mappingObject.ObjectEmpty())
		{
			return;
		}
		JsonValue section(rapidjson::kObjectType);
		if (field != defaultField)
		{
			addString(section, "field", field, allocator);
		}
		if (!mappingObject.ObjectEmpty())
		{
			const char* mappingKey = nullptr;
			if (std::string_view(key) == "color")
			{
				mappingKey = "colorMapping-scatter";
			}
			else if (std::string_view(key) == "size")
			{
				mappingKey = "sizeMapping-scatter";
			}
			else
			{
				mappingKey = "labelMapping-scatter";
			}
			addMember(section, mappingKey, std::move(mappingObject), allocator);
		}
		addMember(object, key, std::move(section), allocator);
	};

	JsonValue colorMapping = writeScatterColorMapping(mapping.colorMapping, defaults.colorMapping, allocator);
	JsonValue sizeMapping = writeScatterSizeMapping(mapping.sizeMapping, defaults.sizeMapping, allocator);
	JsonValue labelMapping = writeScatterLabelMapping(mapping.labelMapping, defaults.labelMapping, allocator);

	addFieldSection("x", mapping.xField, defaults.xField, JsonValue(rapidjson::kObjectType));
	addFieldSection("y", mapping.yField, defaults.yField, JsonValue(rapidjson::kObjectType));
	addFieldSection("color", mapping.colorField, defaults.colorField, std::move(colorMapping));
	addFieldSection("size", mapping.sizeField, defaults.sizeField, std::move(sizeMapping));
	addFieldSection("label", mapping.labelField, defaults.labelField, std::move(labelMapping));
	return object;
}

JsonValue writeHistogramColorMapping(
	const FlowPlot::Spec::HistogramColorMappingSpec& mapping,
	const FlowPlot::Spec::HistogramColorMappingSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "type", mapping.type, defaults.type, allocator);
	if (mapping.categories != defaults.categories)
	{
		addMember(object, "categories", writeStringArray(mapping.categories, allocator), allocator);
	}
	if (mapping.values != defaults.values)
	{
		addMember(object, "values", writeStringArray(mapping.values, allocator), allocator);
	}
	addIfChanged(object, "minValue", mapping.minValue, defaults.minValue, allocator);
	addIfChanged(object, "maxValue", mapping.maxValue, defaults.maxValue, allocator);
	return object;
}

JsonValue writeHistogramMappingSpec(
	const FlowPlot::Spec::HistogramMappingSpec& mapping,
	const FlowPlot::Spec::HistogramMappingSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	if (mapping.dataField != defaults.dataField || mapping.axis != defaults.axis)
	{
		JsonValue data(rapidjson::kObjectType);
		if (mapping.dataField != defaults.dataField)
		{
			addString(data, "field", mapping.dataField, allocator);
		}
		if (mapping.axis != defaults.axis)
		{
			addString(data, "axis", histogramAxisToString(mapping.axis), allocator);
		}
		addMember(object, "data", std::move(data), allocator);
	}

	JsonValue colorMapping = writeHistogramColorMapping(mapping.colorMapping, defaults.colorMapping, allocator);
	if (mapping.colorField != defaults.colorField || !colorMapping.ObjectEmpty())
	{
		JsonValue color(rapidjson::kObjectType);
		if (mapping.colorField != defaults.colorField)
		{
			addString(color, "field", mapping.colorField, allocator);
		}
		if (!colorMapping.ObjectEmpty())
		{
			addMember(color, "colorMapping-histogram", std::move(colorMapping), allocator);
		}
		addMember(object, "color", std::move(color), allocator);
	}
	return object;
}

JsonValue writeScatterStyleSpec(
	const FlowPlot::Spec::ScatterStyleSpec& style,
	const FlowPlot::Spec::ScatterStyleSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "markerShape", style.markerShape, defaults.markerShape, allocator);
	addIfChanged(object, "fillColor", style.fillColor, defaults.fillColor, allocator);
	addIfChanged(object, "strokeColor", style.strokeColor, defaults.strokeColor, allocator);
	addIfChanged(object, "strokeWidth", style.strokeWidth, defaults.strokeWidth, allocator);
	addIfChanged(object, "markerSize", style.markerSize, defaults.markerSize, allocator);
	return object;
}

JsonValue writeScatterStatsSpec(
	const FlowPlot::Spec::ScatterStatsSpec& stats,
	const FlowPlot::Spec::ScatterStatsSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "showMeanPoint", stats.showMeanPoint, defaults.showMeanPoint, allocator);
	addIfChanged(object, "meanPointShape", stats.meanPointShape, defaults.meanPointShape, allocator);
	addIfChanged(object, "meanPointColor", stats.meanPointColor, defaults.meanPointColor, allocator);
	addIfChanged(object, "meanPointStrokeColor", stats.meanPointStrokeColor, defaults.meanPointStrokeColor, allocator);
	addIfChanged(object, "meanPointStrokeWidth", stats.meanPointStrokeWidth, defaults.meanPointStrokeWidth, allocator);
	addIfChanged(object, "meanPointSize", stats.meanPointSize, defaults.meanPointSize, allocator);
	return object;
}

JsonValue writeScatterConfigSpec(
	const FlowPlot::Spec::ScatterConfigSpec& config,
	const FlowPlot::Spec::ScatterConfigSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "jitterX", config.jitterX, defaults.jitterX, allocator);
	addIfChanged(object, "jitterY", config.jitterY, defaults.jitterY, allocator);
	addIfChanged(object, "domainPadding", config.domainPadding, defaults.domainPadding, allocator);
	return object;
}

JsonValue writeHistogramStyleSpec(
	const FlowPlot::Spec::HistogramStyleSpec& style,
	const FlowPlot::Spec::HistogramStyleSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "fillColor", style.fillColor, defaults.fillColor, allocator);
	addIfChanged(object, "strokeColor", style.strokeColor, defaults.strokeColor, allocator);
	addIfChanged(object, "strokeWidth", style.strokeWidth, defaults.strokeWidth, allocator);
	return object;
}

JsonValue writeHistogramStatsSpec(
	const FlowPlot::Spec::HistogramStatsSpec& stats,
	const FlowPlot::Spec::HistogramStatsSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "showMeanLine", stats.showMeanLine, defaults.showMeanLine, allocator);
	addIfChanged(object, "meanLineColor", stats.meanLineColor, defaults.meanLineColor, allocator);
	addIfChanged(object, "meanLineWidth", stats.meanLineWidth, defaults.meanLineWidth, allocator);
	addIfChanged(object, "showMedianLine", stats.showMedianLine, defaults.showMedianLine, allocator);
	addIfChanged(object, "medianLineColor", stats.medianLineColor, defaults.medianLineColor, allocator);
	addIfChanged(object, "medianLineWidth", stats.medianLineWidth, defaults.medianLineWidth, allocator);
	return object;
}

JsonValue writeHistogramConfigSpec(
	const FlowPlot::Spec::HistogramConfigSpec& config,
	const FlowPlot::Spec::HistogramConfigSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "binCount", config.binCount, defaults.binCount, allocator);
	addIfChanged(object, "normalize", config.normalize, defaults.normalize, allocator);
	addIfChanged(object, "cumulative", config.cumulative, defaults.cumulative, allocator);
	addIfChanged(object, "showEmptyBins", config.showEmptyBins, defaults.showEmptyBins, allocator);
	addIfChanged(object, "domainPadding", config.domainPadding, defaults.domainPadding, allocator);
	return object;
}

JsonValue writeLayerSpec(
	const FlowPlot::Spec::LayerSpec& layer,
	const FlowPlot::Spec::LayerSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "id", layer.id, defaults.id, allocator);
	addIfChanged(object, "type", layer.type, defaults.type, allocator);
	addIfChanged(object, "visible", layer.visible, defaults.visible, allocator);
	addIfChanged(object, "opacity", layer.opacity, defaults.opacity, allocator);
	addIfChanged(object, "zIndex", layer.zIndex, defaults.zIndex, allocator);
	addIfChanged(object, "dataset", layer.dataset, defaults.dataset, allocator);

	JsonValue axisData = writeAxisDataSpec(layer.axisData, defaults.axisData, allocator);
	if (!axisData.ObjectEmpty())
	{
		addMember(object, "axisData", std::move(axisData), allocator);
	}

	if (layer.type == "histogram")
	{
		FlowPlot::Spec::HistogramMappingSpec mappingDefaults{};
		FlowPlot::Spec::HistogramStyleSpec styleDefaults{};
		FlowPlot::Spec::HistogramStatsSpec statsDefaults{};
		FlowPlot::Spec::HistogramConfigSpec configDefaults{};

		JsonValue mapping = writeHistogramMappingSpec(layer.histogramMapping, mappingDefaults, allocator);
		JsonValue style = writeHistogramStyleSpec(layer.histogramStyle, styleDefaults, allocator);
		JsonValue stats = writeHistogramStatsSpec(layer.histogramStats, statsDefaults, allocator);
		JsonValue config = writeHistogramConfigSpec(layer.histogramConfig, configDefaults, allocator);
		if (!mapping.ObjectEmpty()) addMember(object, "mapping", std::move(mapping), allocator);
		if (!style.ObjectEmpty()) addMember(object, "style", std::move(style), allocator);
		if (!stats.ObjectEmpty()) addMember(object, "stats", std::move(stats), allocator);
		if (!config.ObjectEmpty()) addMember(object, "config", std::move(config), allocator);
	}
	else
	{
		FlowPlot::Spec::ScatterMappingSpec mappingDefaults{};
		FlowPlot::Spec::ScatterStyleSpec styleDefaults{};
		FlowPlot::Spec::ScatterStatsSpec statsDefaults{};
		FlowPlot::Spec::ScatterConfigSpec configDefaults{};

		JsonValue mapping = writeScatterMappingSpec(layer.scatterMapping, mappingDefaults, allocator);
		JsonValue style = writeScatterStyleSpec(layer.scatterStyle, styleDefaults, allocator);
		JsonValue stats = writeScatterStatsSpec(layer.scatterStats, statsDefaults, allocator);
		JsonValue config = writeScatterConfigSpec(layer.scatterConfig, configDefaults, allocator);
		if (!mapping.ObjectEmpty()) addMember(object, "mapping", std::move(mapping), allocator);
		if (!style.ObjectEmpty()) addMember(object, "style", std::move(style), allocator);
		if (!stats.ObjectEmpty()) addMember(object, "stats", std::move(stats), allocator);
		if (!config.ObjectEmpty()) addMember(object, "config", std::move(config), allocator);
	}

	return object;
}

JsonValue writePanelSpec(
	const FlowPlot::Spec::PanelSpec& panel,
	const FlowPlot::Spec::PanelSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "id", panel.id, defaults.id, allocator);
	addIfChanged(object, "visible", panel.visible, defaults.visible, allocator);
	addIfChanged(object, "background", panel.background, defaults.background, allocator);
	addIfChanged(object, "borderColor", panel.borderColor, defaults.borderColor, allocator);
	addIfChanged(object, "borderWidth", panel.borderWidth, defaults.borderWidth, allocator);
	addIfChanged(object, "clipContent", panel.clipContent, defaults.clipContent, allocator);

	JsonValue padding = writePaddingSpec(panel.padding, defaults.padding, allocator);
	if (!padding.ObjectEmpty()) addMember(object, "padding", std::move(padding), allocator);
	JsonValue title = writeTextSpec(panel.title, defaults.title, allocator);
	if (!title.ObjectEmpty()) addMember(object, "title", std::move(title), allocator);
	JsonValue xAxis = writeAxisSpec(panel.xAxis, defaults.xAxis, allocator);
	if (!xAxis.ObjectEmpty()) addMember(object, "xAxis", std::move(xAxis), allocator);
	JsonValue yAxis = writeAxisSpec(panel.yAxis, defaults.yAxis, allocator);
	if (!yAxis.ObjectEmpty()) addMember(object, "yAxis", std::move(yAxis), allocator);
	JsonValue xSecondary = writeAxisSpec(panel.xSecondary, defaults.xSecondary, allocator);
	if (!xSecondary.ObjectEmpty()) addMember(object, "xSecondary", std::move(xSecondary), allocator);
	JsonValue ySecondary = writeAxisSpec(panel.ySecondary, defaults.ySecondary, allocator);
	if (!ySecondary.ObjectEmpty()) addMember(object, "ySecondary", std::move(ySecondary), allocator);

	if (!panel.layers.empty())
	{
		JsonValue layers(rapidjson::kArrayType);
		const FlowPlot::Spec::LayerSpec layerDefaults{};
		for (const FlowPlot::Spec::LayerSpec& layer : panel.layers)
		{
			layers.PushBack(writeLayerSpec(layer, layerDefaults, allocator), allocator);
		}
		addMember(object, "layers", std::move(layers), allocator);
	}
	return object;
}

JsonValue writeFigureSpec(
	const FlowPlot::Spec::FigureSpec& figure,
	const FlowPlot::Spec::FigureSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "width", figure.width, defaults.width, allocator);
	addIfChanged(object, "height", figure.height, defaults.height, allocator);
	addIfChanged(object, "dpi", figure.dpi, defaults.dpi, allocator);
	addIfChanged(object, "background", figure.background, defaults.background, allocator);
	JsonValue padding = writePaddingSpec(figure.padding, defaults.padding, allocator);
	if (!padding.ObjectEmpty()) addMember(object, "padding", std::move(padding), allocator);
	JsonValue title = writeTextSpec(figure.title, defaults.title, allocator);
	if (!title.ObjectEmpty()) addMember(object, "title", std::move(title), allocator);
	if (!figure.legends.empty())
	{
		JsonValue legends(rapidjson::kArrayType);
		const FlowPlot::Spec::LegendSpec legendDefaults{};
		for (const FlowPlot::Spec::LegendSpec& legend : figure.legends)
		{
			legends.PushBack(writeLegendSpec(legend, legendDefaults, allocator), allocator);
		}
		addMember(object, "legends", std::move(legends), allocator);
	}
	return object;
}

JsonValue writeDatasetSpec(const FlowPlot::Spec::DatasetSpec& dataset, Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addString(object, "name", dataset.name, allocator);

	JsonValue schema(rapidjson::kObjectType);
	std::vector<std::string> fields{};
	fields.reserve(dataset.schema.size());
	for (const auto& [fieldName, _] : dataset.schema)
	{
		fields.push_back(fieldName);
	}
	std::sort(fields.begin(), fields.end());
	for (const std::string& fieldName : fields)
	{
		addString(schema, fieldName, datasetFieldTypeToString(dataset.schema.at(fieldName)), allocator);
	}
	addMember(object, "schema", std::move(schema), allocator);
	return object;
}

JsonValue writeLayoutSpec(
	const FlowPlot::Spec::LayoutSpec& layout,
	const FlowPlot::Spec::LayoutSpec& defaults,
	Allocator& allocator)
{
	JsonValue object(rapidjson::kObjectType);
	addIfChanged(object, "rows", layout.rows, defaults.rows, allocator);
	addIfChanged(object, "cols", layout.cols, defaults.cols, allocator);
	addIfChanged(object, "gap", layout.gap, defaults.gap, allocator);
	return object;
}

void collectTextFont(
	std::set<std::tuple<std::string, std::uint16_t, std::string>>& fonts,
	const FlowPlot::Spec::TextSpec& text)
{
	if (text.fontFamily != "Default")
	{
		fonts.emplace(text.fontFamily, text.fontWeight, text.fontStyle);
	}
}

void collectLegendElementFont(
	std::set<std::tuple<std::string, std::uint16_t, std::string>>& fonts,
	const FlowPlot::Spec::LegendElementSpec& element)
{
	if (element.fontFamily != "Default")
	{
		fonts.emplace(element.fontFamily, element.fontWeight, element.fontStyle);
	}
}

std::optional<FlowPlot::FontStyle> tryParseFontStyle(std::string_view rawStyle)
{
	try
	{
		return FlowPlot::parseFontStyle(rawStyle);
	}
	catch (...)
	{
		return std::nullopt;
	}
}

JsonValue writeFonts(
	const FlowPlot::Spec::MasterTemplateSpec& spec,
	const std::vector<AddedFontVariant>& fontLibrary,
	Allocator& allocator)
{
	std::set<std::tuple<std::string, std::uint16_t, std::string>> usedFonts{};
	collectTextFont(usedFonts, spec.figure.title);
	for (const FlowPlot::Spec::LegendSpec& legend : spec.figure.legends)
	{
		for (const FlowPlot::Spec::LegendElementSpec& element : legend.legendElements)
		{
			collectLegendElementFont(usedFonts, element);
		}
	}
	for (const FlowPlot::Spec::PanelSpec& panel : spec.panels)
	{
		collectTextFont(usedFonts, panel.title);
		collectTextFont(usedFonts, panel.xAxis.title);
		collectTextFont(usedFonts, panel.yAxis.title);
		collectTextFont(usedFonts, panel.xSecondary.title);
		collectTextFont(usedFonts, panel.ySecondary.title);
		if (panel.xAxis.tickLabelFontFamily != "Default")
		{
			usedFonts.emplace(panel.xAxis.tickLabelFontFamily, panel.xAxis.tickLabelFontWeight, panel.xAxis.tickLabelFontStyle);
		}
		if (panel.yAxis.tickLabelFontFamily != "Default")
		{
			usedFonts.emplace(panel.yAxis.tickLabelFontFamily, panel.yAxis.tickLabelFontWeight, panel.yAxis.tickLabelFontStyle);
		}
		if (panel.xSecondary.tickLabelFontFamily != "Default")
		{
			usedFonts.emplace(panel.xSecondary.tickLabelFontFamily, panel.xSecondary.tickLabelFontWeight, panel.xSecondary.tickLabelFontStyle);
		}
		if (panel.ySecondary.tickLabelFontFamily != "Default")
		{
			usedFonts.emplace(panel.ySecondary.tickLabelFontFamily, panel.ySecondary.tickLabelFontWeight, panel.ySecondary.tickLabelFontStyle);
		}
	}

	JsonValue fonts(rapidjson::kArrayType);
	for (const auto& fontKey : usedFonts)
	{
		const std::string& family = std::get<0>(fontKey);
		const std::uint16_t weight = std::get<1>(fontKey);
		const std::string& styleText = std::get<2>(fontKey);
		const std::optional<FlowPlot::FontStyle> style = tryParseFontStyle(styleText);
		if (!style.has_value())
		{
			continue;
		}

		const auto match = std::find_if(fontLibrary.begin(), fontLibrary.end(), [&](const AddedFontVariant& variant) {
			return variant.family == family && variant.weight == weight && variant.style == *style;
		});
		if (match == fontLibrary.end())
		{
			continue;
		}

		JsonValue font(rapidjson::kObjectType);
		addString(font, "family", match->family, allocator);
		addUint(font, "weight", match->weight, allocator);
		addString(font, "style", FlowPlot::fontStyleName(match->style), allocator);
		addString(font, "path", std::filesystem::absolute(match->path).string(), allocator);
		fonts.PushBack(std::move(font), allocator);
	}
	return fonts;
}

void logExportResult(const TemplateExportResult& result)
{
	for (const std::string& error : result.errors)
	{
		std::fprintf(stderr, "[FlowPlotGUI] Template export: %s\n", error.c_str());
	}
	if (!result.path.empty() && result.errors.empty() && !result.cancelled)
	{
		std::fprintf(stderr, "[FlowPlotGUI] Exported template to %s\n", result.path.string().c_str());
	}
}

} // namespace

std::filesystem::path normalizeTemplateExportPath(std::filesystem::path path)
{
	const std::string filename = path.filename().string();
	const std::string lower = toLowerAscii(filename);
	if (lower.find(".json") != std::string::npos)
	{
		return path;
	}
	path.replace_extension(".json");
	return path;
}

std::string exportTemplateJson(
	const FlowPlot::Spec::MasterTemplateSpec& activeTemplate,
	const std::vector<AddedFontVariant>& fontLibrary)
{
	rapidjson::Document document;
	document.SetObject();
	Allocator& allocator = document.GetAllocator();

	const FlowPlot::Spec::MasterTemplateSpec defaults{};

	addString(document, "version", kTemplateExportVersion, allocator);

	JsonValue fonts = writeFonts(activeTemplate, fontLibrary, allocator);
	if (!fonts.Empty())
	{
		addMember(document, "fonts", std::move(fonts), allocator);
	}

	JsonValue figure = writeFigureSpec(activeTemplate.figure, defaults.figure, allocator);
	if (!figure.ObjectEmpty())
	{
		addMember(document, "figure", std::move(figure), allocator);
	}

	if (!activeTemplate.datasets.empty())
	{
		JsonValue datasets(rapidjson::kArrayType);
		for (const FlowPlot::Spec::DatasetSpec& dataset : activeTemplate.datasets)
		{
			datasets.PushBack(writeDatasetSpec(dataset, allocator), allocator);
		}
		addMember(document, "datasets", std::move(datasets), allocator);
	}

	JsonValue layout = writeLayoutSpec(activeTemplate.layout, defaults.layout, allocator);
	if (!layout.ObjectEmpty())
	{
		addMember(document, "layout", std::move(layout), allocator);
	}

	if (!activeTemplate.panels.empty())
	{
		JsonValue panels(rapidjson::kArrayType);
		const FlowPlot::Spec::PanelSpec panelDefaults{};
		for (const FlowPlot::Spec::PanelSpec& panel : activeTemplate.panels)
		{
			panels.PushBack(writePanelSpec(panel, panelDefaults, allocator), allocator);
		}
		addMember(document, "panels", std::move(panels), allocator);
	}

	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
	writer.SetIndent(' ', 2);
	document.Accept(writer);
	return std::string(buffer.GetString(), buffer.GetSize());
}

TemplateExportResult exportTemplateToPath(
	const FlowPlot::Spec::MasterTemplateSpec& activeTemplate,
	const std::vector<AddedFontVariant>& fontLibrary,
	const std::filesystem::path& path)
{
	TemplateExportResult result{};
	result.path = normalizeTemplateExportPath(path);

	try
	{
		const std::string json = exportTemplateJson(activeTemplate, fontLibrary);
		std::ofstream file(result.path, std::ios::binary | std::ios::trunc);
		if (!file)
		{
			result.errors.push_back("could not open " + result.path.string() + " for writing");
			logExportResult(result);
			return result;
		}
		file.write(json.data(), static_cast<std::streamsize>(json.size()));
		file.put('\n');
		if (!file)
		{
			result.errors.push_back("failed while writing " + result.path.string());
		}
	}
	catch (const std::exception& e)
	{
		result.errors.emplace_back(e.what());
	}

	logExportResult(result);
	return result;
}

bool masterTemplateSpecsEqual(
	const FlowPlot::Spec::MasterTemplateSpec& lhs,
	const FlowPlot::Spec::MasterTemplateSpec& rhs)
{
	return exportTemplateJson(lhs, {}) == exportTemplateJson(rhs, {});
}

void ensureActiveTemplateExportComparisonChecked(state& guiState)
{
	if (guiState.activeTemplateExportComparisonChecked)
	{
		return;
	}

	try
	{
		guiState.activeTemplateDiffersFromLastExport =
			!masterTemplateSpecsEqual(guiState.activeTemplate, guiState.lastExportedTemplate);
		guiState.activeTemplateExportComparisonChecked = true;
		clearDiagnosticsBySource(guiState, "export-comparison");
	}
	catch (const std::exception& e)
	{
		guiState.activeTemplateDiffersFromLastExport = true;
		guiState.activeTemplateExportComparisonChecked = true;
		recordDiagnostic(guiState, Diagnostic{
			.severity = DiagnosticSeverity::Error,
			.source = "export-comparison",
			.message = e.what(),
		});
	}
}

TemplateExportResult ExportTemplateWithExportDialog(
	state& guiState,
	void* nativeWindowHandle)
{
	TemplateExportResult result{};
	if (NFD_Init() != NFD_OKAY)
	{
		const char* error = NFD_GetError();
		result.errors.emplace_back(error != nullptr ? error : "NFD initialization failed");
		recordDiagnostic(guiState, Diagnostic{
			.severity = DiagnosticSeverity::Error,
			.source = "export",
			.message = result.errors.back(),
		});
		logExportResult(result);
		return result;
	}

	NFD_SetDisplayPropertiesFromGLFW();

	nfdu8filteritem_t filters[] = {
		{"FlowPlot template", "json"},
	};

	nfdsavedialogu8args_t args{};
	args.filterList = filters;
	args.filterCount = 1;
	args.defaultName = "template.json";
	if (nativeWindowHandle != nullptr)
	{
		NFD_GetNativeWindowFromGLFWWindow(static_cast<GLFWwindow*>(nativeWindowHandle), &args.parentWindow);
	}

	nfdu8char_t* rawPath = nullptr;
	const nfdresult_t dialogResult = NFD_SaveDialogU8_With(&rawPath, &args);
	if (dialogResult == NFD_CANCEL)
	{
		result.cancelled = true;
		NFD_Quit();
		return result;
	}
	if (dialogResult != NFD_OKAY)
	{
		const char* error = NFD_GetError();
		result.errors.emplace_back(error != nullptr ? error : "template export dialog failed");
		recordDiagnostic(guiState, Diagnostic{
			.severity = DiagnosticSeverity::Error,
			.source = "export",
			.message = result.errors.back(),
		});
		logExportResult(result);
		NFD_Quit();
		return result;
	}

	if (rawPath == nullptr)
	{
		result.errors.emplace_back("template export dialog returned an empty path");
		recordDiagnostic(guiState, Diagnostic{
			.severity = DiagnosticSeverity::Error,
			.source = "export",
			.message = result.errors.back(),
		});
		logExportResult(result);
		NFD_Quit();
		return result;
	}

	const std::filesystem::path selectedPath(reinterpret_cast<const char*>(rawPath));
	NFD_FreePathU8(rawPath);
	NFD_Quit();

	result = exportTemplateToPath(guiState.activeTemplate, guiState.fontLibrary, selectedPath);
	if (result.errors.empty())
	{
		acceptCurrentTemplateAsExported(guiState);
		clearDiagnosticsBySource(guiState, "export");
	}
	else if (!result.errors.empty())
	{
		recordDiagnostic(guiState, Diagnostic{
			.severity = DiagnosticSeverity::Error,
			.source = "export",
			.message = result.errors.back(),
		});
	}
	return result;
}

} // namespace FlowPlotGui
