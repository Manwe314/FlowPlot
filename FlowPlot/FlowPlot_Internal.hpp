#pragma once

#include "FlowPlot_Defaults.hpp"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <limits>
#include <optional>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <algorithm>
#include <array>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace FlowInternal
{
	inline std::string jsonStringToStdString(const rapidjson::Value& value)
	{
		return std::string(value.GetString(), value.GetStringLength());
	}

	inline const rapidjson::Value* findJsonMember(const rapidjson::Value& objectJson, const char* key)
	{
		if (!objectJson.IsObject())
			return nullptr;

		const auto memberIt = objectJson.FindMember(key);
		if (memberIt == objectJson.MemberEnd())
			return nullptr;
		return &memberIt->value;
	}

	inline rapidjson::Value* findJsonMember(rapidjson::Value& objectJson, const char* key)
	{
		if (!objectJson.IsObject())
			return nullptr;

		auto memberIt = objectJson.FindMember(key);
		if (memberIt == objectJson.MemberEnd())
			return nullptr;
		return &memberIt->value;
	}

	inline rapidjson::Document parseJsonDocument(std::string_view jsonText, const char* context)
	{
		rapidjson::Document document;
		document.Parse(jsonText.data(), jsonText.size());
		if (document.HasParseError())
		{
			throw std::runtime_error(
				std::string(context)
				+ ": failed to parse JSON at offset "
				+ std::to_string(document.GetErrorOffset())
				+ ": "
				+ rapidjson::GetParseError_En(document.GetParseError()));
		}
		return document;
	}

	inline std::string serializeJson(const rapidjson::Value& jsonValue, bool pretty)
	{
		rapidjson::StringBuffer buffer;
		if (pretty)
		{
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			jsonValue.Accept(writer);
		}
		else
		{
			rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			jsonValue.Accept(writer);
		}
		return std::string(buffer.GetString(), buffer.GetSize());
	}

	struct DataView
	{
		enum class ValueType : std::uint8_t
		{
			Number,
			String,
			Boolean,
			Unknown
		};

		const void* ptr = nullptr;
		std::size_t count = 0;
		std::size_t elemSize = 0;
		std::type_index type = std::type_index(typeid(void));
		ValueType valueType = ValueType::Unknown;
	};

	template<typename T>
	constexpr DataView::ValueType inferValueType() noexcept
	{
		using Decayed = std::remove_cv_t<std::remove_reference_t<T>>;
		if constexpr (std::is_same_v<Decayed, bool>)
			return DataView::ValueType::Boolean;
		else if constexpr (std::is_arithmetic_v<Decayed>)
			return DataView::ValueType::Number;
		else if constexpr (
			std::is_same_v<Decayed, std::string>
			|| std::is_same_v<Decayed, std::string_view>
			|| std::is_same_v<Decayed, const char*>
			|| std::is_same_v<Decayed, char*>)
		{
			return DataView::ValueType::String;
		}
		else
		{
			return DataView::ValueType::Unknown;
		}
	}

	struct DataKey
	{
		std::string dataset;
		std::string field;

		bool operator==(const DataKey& other) const
		{
			return dataset == other.dataset && field == other.field;
		}
	};

	struct DataKeyHash
	{
		std::size_t operator()(const DataKey& key) const noexcept
		{
			const std::size_t datasetHash = std::hash<std::string>{}(key.dataset);
			const std::size_t fieldHash = std::hash<std::string>{}(key.field);
			return datasetHash ^ (fieldHash + 0x9e3779b97f4a7c15ULL + (datasetHash << 6U) + (datasetHash >> 2U));
		}
	};

	template<typename T>
	std::span<const T> asSpan(const DataView& view)
	{
		if (view.type != std::type_index(typeid(T)))
			throw std::runtime_error("DataView type mismatch");

		return std::span<const T>(static_cast<const T*>(view.ptr), view.count);
	}

	namespace BoundIR
	{
		struct BoundDataset
		{
			std::uint32_t rowCount = 0;
			std::vector<DataView> views{};
		};

		struct BoundScatterInputs
		{
			std::uint32_t dataset = 0;
			std::uint32_t x = 0;
			std::uint32_t y = 0;
			std::optional<std::uint32_t> color = std::nullopt;
			std::optional<std::uint32_t> size = std::nullopt;
			std::optional<std::uint32_t> label = std::nullopt;
		};

		struct BoundHistogramInputs
		{
			std::uint32_t dataset = 0;
			std::uint32_t data = 0;
			FlowPlot::Spec::HistogramDataAxis dataAxis = FlowPlot::Spec::HistogramDataAxis::X;
			std::optional<std::uint32_t> color = std::nullopt;
		};

		using BoundLayerInputs = std::variant<BoundScatterInputs, BoundHistogramInputs>;

		struct BoundLayer
		{
			std::uint32_t panelIndex = 0;
			std::uint32_t layerIndex = 0;
			std::int32_t zIndex = 0;
			float opacity = 1.0f;
			FlowPlot::Spec::LayerAxisDataSpec axisData{};
			BoundLayerInputs inputs{};
		};

		struct PlotBoundIR
		{
			std::vector<BoundDataset> datasets{};
			std::vector<BoundLayer> layersInPainterOrder{};
		};
	} // namespace BoundIR

	namespace ResolvedIR
	{
		using ResolvedText = FlowPlot::TextCommand;
		using ResolvedBox = FlowPlot::BoxCommand;
		using ResolvedLine = FlowPlot::PolylineCommand;
		using ResolvedMarkers = FlowPlot::MarkersCommand;

		struct AxisResolved
		{
			std::vector<ResolvedLine> gridLines{};
			std::vector<ResolvedLine> tickLines{};
			std::vector<ResolvedText> tickLabels{};
			std::optional<ResolvedText> title = std::nullopt;
			std::optional<ResolvedLine> axisLine = std::nullopt;
		};

		struct ScatterResolved
		{
			std::vector<ResolvedMarkers> markerBatches{};
		};

		struct HistogramResolved
		{
			std::vector<ResolvedBox> bars{};
			std::vector<ResolvedLine> statLines{};
		};

		using LayerPayload = std::variant<ScatterResolved, HistogramResolved>;

		struct LayerResolved
		{
			std::int32_t zIndex = 0;
			float opacity = 1.0f;
			LayerPayload payload{};
		};

		struct PanelResolved
		{
			bool visible = true;
			ResolvedBox frame{};
			FlowPlot::RectF clipRectPanel{};
			std::optional<ResolvedText> title = std::nullopt;
			AxisResolved xAxis{};
			AxisResolved yAxis{};
			AxisResolved xSecondary{};
			AxisResolved ySecondary{};
			FlowPlot::RectF clipRectLayer{};
			std::vector<LayerResolved> layers{};
		};

		struct LegendResolved
		{
			ResolvedBox frame{};
			std::vector<ResolvedBox> iconBoxes{};
			std::vector<ResolvedText> labels{};
		};

		struct FigureResolved
		{
			std::uint32_t width = 0;
			std::uint32_t height = 0;
			FlowPlot::Color background{};
			std::optional<ResolvedText> title = std::nullopt;
			std::vector<LegendResolved> legends{};
		};

		struct PlotResolvedIR
		{
			FigureResolved figure{};
			std::vector<PanelResolved> panels{};
		};
	} // namespace ResolvedIR

	namespace DataBinding
	{
		inline bool schemaTypeMatchesViewType(
			FlowPlot::Spec::DatasetSpec::FieldType schemaType,
			DataView::ValueType valueType) noexcept
		{
			switch (schemaType)
			{
			case FlowPlot::Spec::DatasetSpec::FieldType::Number:
				return valueType == DataView::ValueType::Number;
			case FlowPlot::Spec::DatasetSpec::FieldType::String:
				return valueType == DataView::ValueType::String;
			case FlowPlot::Spec::DatasetSpec::FieldType::Boolean:
				return valueType == DataView::ValueType::Boolean;
			default:
				return false;
			}
		}

		inline const char* schemaTypeName(FlowPlot::Spec::DatasetSpec::FieldType schemaType) noexcept
		{
			switch (schemaType)
			{
			case FlowPlot::Spec::DatasetSpec::FieldType::Number:
				return "number";
			case FlowPlot::Spec::DatasetSpec::FieldType::String:
				return "string";
			case FlowPlot::Spec::DatasetSpec::FieldType::Boolean:
				return "boolean";
			default:
				return "unknown";
			}
		}

		inline const char* valueTypeName(DataView::ValueType valueType) noexcept
		{
			switch (valueType)
			{
			case DataView::ValueType::Number:
				return "number";
			case DataView::ValueType::String:
				return "string";
			case DataView::ValueType::Boolean:
				return "boolean";
			case DataView::ValueType::Unknown:
				return "unknown";
			default:
				return "unknown";
			}
		}

		inline bool tryResolveRequiredFieldIndex(
			const FlowPlot::Spec::DatasetSpec& datasetSpec,
			const std::unordered_map<std::string, std::uint32_t>& boundFieldIndexByName,
			const std::string& fieldName,
			const std::string& fieldPath,
			std::uint32_t& outIndex)
		{
			if (fieldName.empty())
				return false;

			auto schemaFieldIt = datasetSpec.schema.find(fieldName);
			if (schemaFieldIt == datasetSpec.schema.end())
			{
				throw std::runtime_error(
					"buildBoundIR: field '" + fieldName + "' at '" + fieldPath
					+ "' is not declared in dataset schema '" + datasetSpec.name + "'");
			}

			auto boundFieldIt = boundFieldIndexByName.find(fieldName);
			if (boundFieldIt == boundFieldIndexByName.end())
				return false;

			outIndex = boundFieldIt->second;
			return true;
		}

		inline std::optional<std::uint32_t> tryResolveOptionalFieldIndex(
			const FlowPlot::Spec::DatasetSpec& datasetSpec,
			const std::unordered_map<std::string, std::uint32_t>& boundFieldIndexByName,
			const std::string& fieldName,
			const std::string& fieldPath)
		{
			if (fieldName.empty())
				return std::nullopt;

			auto schemaFieldIt = datasetSpec.schema.find(fieldName);
			if (schemaFieldIt == datasetSpec.schema.end())
			{
				throw std::runtime_error(
					"buildBoundIR: field '" + fieldName + "' at '" + fieldPath
					+ "' is not declared in dataset schema '" + datasetSpec.name + "'");
			}

			auto boundFieldIt = boundFieldIndexByName.find(fieldName);
			if (boundFieldIt == boundFieldIndexByName.end())
				return std::nullopt;

			return boundFieldIt->second;
		}
	} // namespace DataBinding

	namespace SpecCompiler
	{
		using json = rapidjson::Value;

		inline std::string childPath(const std::string& path, const std::string& key)
		{
			return path.empty() ? key : (path + "." + key);
		}

		inline const json* findKey(const json& objectJson, const char* key)
		{
			return findJsonMember(objectJson, key);
		}

		inline const json* findObject(const json& objectJson, const char* key, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return nullptr;
			if (!value->IsObject())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be an object");
			return value;
		}

		inline const json* findArray(const json& objectJson, const char* key, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return nullptr;
			if (!value->IsArray())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be an array");
			return value;
		}

		inline void readBool(const json& objectJson, const char* key, bool& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsBool())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be a boolean");
			out = value->GetBool();
		}

		inline void readString(const json& objectJson, const char* key, std::string& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsString())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be a string");
			out = jsonStringToStdString(*value);
		}

		inline std::string normalizeDatasetFieldTypeToken(std::string_view rawType)
		{
			std::size_t begin = 0;
			while (begin < rawType.size() && std::isspace(static_cast<unsigned char>(rawType[begin])) != 0)
				++begin;

			std::size_t end = rawType.size();
			while (end > begin && std::isspace(static_cast<unsigned char>(rawType[end - 1])) != 0)
				--end;

			std::string normalized;
			normalized.reserve(end - begin);
			for (std::size_t i = begin; i < end; ++i)
				normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(rawType[i]))));
			return normalized;
		}

		inline FlowPlot::Spec::DatasetSpec::FieldType parseDatasetFieldType(std::string_view rawType, const std::string& path)
		{
			const std::string normalized = normalizeDatasetFieldTypeToken(rawType);
			if (normalized == "number" || normalized == "numeric" || normalized == "int" || normalized == "integer" || normalized == "float" || normalized == "double")
				return FlowPlot::Spec::DatasetSpec::FieldType::Number;
			if (normalized == "string" || normalized == "text")
				return FlowPlot::Spec::DatasetSpec::FieldType::String;
			if (normalized == "bool" || normalized == "boolean")
				return FlowPlot::Spec::DatasetSpec::FieldType::Boolean;

			throw std::runtime_error("compileTemplateToSpec: unsupported dataset field type '" + std::string(rawType) + "' at '" + path + "'");
		}

		inline void readFloat(const json& objectJson, const char* key, float& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsNumber())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be a number");
			out = static_cast<float>(value->GetDouble());
		}

		inline void readInt32(const json& objectJson, const char* key, std::int32_t& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsInt64())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be an integer");

			const long long parsed = value->GetInt64();
			if (parsed < static_cast<long long>(std::numeric_limits<std::int32_t>::min())
				|| parsed > static_cast<long long>(std::numeric_limits<std::int32_t>::max()))
			{
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' is out of int32 range");
			}
			out = static_cast<std::int32_t>(parsed);
		}

		inline void readUint16(const json& objectJson, const char* key, std::uint16_t& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsInt64() && !value->IsUint64())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be an integer");

			unsigned long long parsed = 0ULL;
			if (value->IsUint64())
			{
				parsed = value->GetUint64();
			}
			else
			{
				const long long signedValue = value->GetInt64();
				if (signedValue < 0)
					throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be non-negative");
				parsed = static_cast<unsigned long long>(signedValue);
			}

			if (parsed > static_cast<unsigned long long>(std::numeric_limits<std::uint16_t>::max()))
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' is out of uint16 range");
			out = static_cast<std::uint16_t>(parsed);
		}

		inline void readUint32(const json& objectJson, const char* key, std::uint32_t& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsInt64() && !value->IsUint64())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be an integer");

			unsigned long long parsed = 0ULL;
			if (value->IsUint64())
			{
				parsed = value->GetUint64();
			}
			else
			{
				const long long signedValue = value->GetInt64();
				if (signedValue < 0)
					throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be non-negative");
				parsed = static_cast<unsigned long long>(signedValue);
			}

			if (parsed > static_cast<unsigned long long>(std::numeric_limits<std::uint32_t>::max()))
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' is out of uint32 range");
			out = static_cast<std::uint32_t>(parsed);
		}

		inline void readOptionalFloat(const json& objectJson, const char* key, std::optional<float>& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (value->IsNull())
			{
				out = std::nullopt;
				return;
			}
			if (!value->IsNumber())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be null or a number");
			out = static_cast<float>(value->GetDouble());
		}

		inline void readStringArray(const json& objectJson, const char* key, std::vector<std::string>& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsArray())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be an array");

			std::vector<std::string> parsed;
			parsed.reserve(value->Size());
			for (rapidjson::SizeType i = 0; i < value->Size(); ++i)
			{
				const json& element = (*value)[i];
				if (!element.IsString())
					throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "[" + std::to_string(i) + "]' must be a string");
				parsed.push_back(jsonStringToStdString(element));
			}
			out = std::move(parsed);
		}

		inline void readFloatArray(const json& objectJson, const char* key, std::vector<float>& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsArray())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be an array");

			std::vector<float> parsed;
			parsed.reserve(value->Size());
			for (rapidjson::SizeType i = 0; i < value->Size(); ++i)
			{
				const json& element = (*value)[i];
				if (!element.IsNumber())
					throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "[" + std::to_string(i) + "]' must be a number");
				parsed.push_back(static_cast<float>(element.GetDouble()));
			}
			out = std::move(parsed);
		}

		inline void readDoubleArray(const json& objectJson, const char* key, std::vector<double>& out, const std::string& path)
		{
			const json* value = findKey(objectJson, key);
			if (value == nullptr)
				return;
			if (!value->IsArray())
				throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "' must be an array");

			std::vector<double> parsed;
			parsed.reserve(value->Size());
			for (rapidjson::SizeType i = 0; i < value->Size(); ++i)
			{
				const json& element = (*value)[i];
				if (!element.IsNumber())
					throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, key) + "[" + std::to_string(i) + "]' must be a number");
				parsed.push_back(element.GetDouble());
			}
			out = std::move(parsed);
		}

		inline void applyBoxSpec(FlowPlot::Spec::BoxSpec& box, const json& objectJson, const std::string& path)
		{
			readOptionalFloat(objectJson, "x", box.x, path);
			readOptionalFloat(objectJson, "y", box.y, path);
			readOptionalFloat(objectJson, "width", box.width, path);
			readOptionalFloat(objectJson, "height", box.height, path);
		}

		inline void applyPaddingSpec(FlowPlot::Spec::PaddingSpec& padding, const json& objectJson, const std::string& path)
		{
			readFloat(objectJson, "left", padding.left, path);
			readFloat(objectJson, "right", padding.right, path);
			readFloat(objectJson, "top", padding.top, path);
			readFloat(objectJson, "bottom", padding.bottom, path);
		}

		inline void applyTextSpec(FlowPlot::Spec::TextSpec& text, const json& objectJson, const std::string& path)
		{
			readBool(objectJson, "visible", text.visible, path);
			readString(objectJson, "text", text.text, path);
			readString(objectJson, "fontFamily", text.fontFamily, path);
			readFloat(objectJson, "fontSize", text.fontSize, path);
			readUint16(objectJson, "fontWeight", text.fontWeight, path);
			readString(objectJson, "color", text.color, path);
			readString(objectJson, "overflow", text.overflow, path);
			readBool(objectJson, "clip", text.clip, path);
			readString(objectJson, "hAlign", text.hAlign, path);
			readString(objectJson, "vAlign", text.vAlign, path);

			if (const json* boxObject = findObject(objectJson, "box", path))
				applyBoxSpec(text.box, *boxObject, childPath(path, "box"));
		}

		inline void applyLegendElementSpec(FlowPlot::Spec::LegendElementSpec& element, const json& objectJson, const std::string& path)
		{
			readString(objectJson, "text", element.text, path);
			readString(objectJson, "fontFamily", element.fontFamily, path);
			readFloat(objectJson, "fontSize", element.fontSize, path);
			readUint16(objectJson, "fontWeight", element.fontWeight, path);
			readString(objectJson, "color", element.color, path);
			readString(objectJson, "overflow", element.overflow, path);
			readBool(objectJson, "clip", element.clip, path);
			readString(objectJson, "iconShape", element.iconShape, path);
			readString(objectJson, "iconColor", element.iconColor, path);

			if (const json* boxObject = findObject(objectJson, "box", path))
				applyBoxSpec(element.box, *boxObject, childPath(path, "box"));
		}

		inline void applyLegendSpec(FlowPlot::Spec::LegendSpec& legend, const json& objectJson, const std::string& path)
		{
			readBool(objectJson, "visible", legend.visible, path);
			readString(objectJson, "background", legend.background, path);
			readString(objectJson, "borderColor", legend.borderColor, path);
			readFloat(objectJson, "borderWidth", legend.borderWidth, path);
			readFloat(objectJson, "gap", legend.gap, path);

			if (const json* boxObject = findObject(objectJson, "box", path))
				applyBoxSpec(legend.box, *boxObject, childPath(path, "box"));
			if (const json* paddingObject = findObject(objectJson, "padding", path))
				applyPaddingSpec(legend.padding, *paddingObject, childPath(path, "padding"));

			if (const json* elementsArray = findArray(objectJson, "legendElements", path))
			{
				legend.legendElements.clear();
				if (elementsArray->Empty())
				{
					legend.legendElements.emplace_back(FlowPlot::Spec::LegendElementSpec{});
				}
				else
				{
					legend.legendElements.reserve(elementsArray->Size());
					for (rapidjson::SizeType i = 0; i < elementsArray->Size(); ++i)
					{
						const json& elementObject = (*elementsArray)[i];
						if (!elementObject.IsObject())
							throw std::runtime_error("compileTemplateToSpec: '" + childPath(path, "legendElements") + "[" + std::to_string(i) + "]' must be an object");

						FlowPlot::Spec::LegendElementSpec elementSpec{};
						applyLegendElementSpec(elementSpec, elementObject, childPath(path, "legendElements") + "[" + std::to_string(i) + "]");
						legend.legendElements.push_back(std::move(elementSpec));
					}
				}
			}
		}

			inline void applyAxisSpec(FlowPlot::Spec::AxisSpec& axis, const json& objectJson, const std::string& path)
			{
			readBool(objectJson, "visible", axis.visible, path);
			readString(objectJson, "scale", axis.scale, path);
			readOptionalFloat(objectJson, "min", axis.min, path);
			readOptionalFloat(objectJson, "max", axis.max, path);
			readBool(objectJson, "invert", axis.invert, path);
			readBool(objectJson, "grid", axis.grid, path);
			readString(objectJson, "gridColor", axis.gridColor, path);
			readFloat(objectJson, "gridWidth", axis.gridWidth, path);
			readString(objectJson, "lineColor", axis.lineColor, path);
			readFloat(objectJson, "lineWidth", axis.lineWidth, path);
			readString(objectJson, "tickColor", axis.tickColor, path);
			readFloat(objectJson, "tickWidth", axis.tickWidth, path);
			readFloat(objectJson, "tickLength", axis.tickLength, path);
			readUint32(objectJson, "tickCount", axis.tickCount, path);
			readFloat(objectJson, "tickValueGap", axis.tickValueGap, path);
			readString(objectJson, "tickLabelFormat", axis.tickLabelFormat, path);
			readString(objectJson, "tickLabelFontFamily", axis.tickLabelFontFamily, path);
			readFloat(objectJson, "tickLabelFontSize", axis.tickLabelFontSize, path);
			readUint16(objectJson, "tickLabelFontWeight", axis.tickLabelFontWeight, path);
			readString(objectJson, "tickLabelColor", axis.tickLabelColor, path);
			readBool(objectJson, "showMinorTicks", axis.showMinorTicks, path);
			readUint32(objectJson, "minorTickCount", axis.minorTickCount, path);
			readDoubleArray(objectJson, "tickValues", axis.tickValues, path);

				if (const json* titleObject = findObject(objectJson, "title", path))
					applyTextSpec(axis.title, *titleObject, childPath(path, "title"));
			}

			inline FlowPlot::Spec::AxisDataRole parseAxisDataRoleToken(std::string_view token, const std::string& path)
			{
				std::size_t begin = 0;
				while (begin < token.size() && std::isspace(static_cast<unsigned char>(token[begin])) != 0)
					++begin;
				std::size_t end = token.size();
				while (end > begin && std::isspace(static_cast<unsigned char>(token[end - 1])) != 0)
					--end;

				std::string mode;
				mode.reserve(end - begin);
				for (std::size_t i = begin; i < end; ++i)
					mode.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(token[i]))));

				if (mode == "null")
					return FlowPlot::Spec::AxisDataRole::Null;
				if (mode == "primary")
					return FlowPlot::Spec::AxisDataRole::Primary;
				if (mode == "secondary")
					return FlowPlot::Spec::AxisDataRole::Secondary;

				throw std::runtime_error("compileTemplateToSpec: unsupported axis role '" + std::string(token) + "' at '" + path + "'");
			}

			inline FlowPlot::Spec::HistogramDataAxis parseHistogramDataAxisToken(std::string_view token, const std::string& path)
			{
				std::size_t begin = 0;
				while (begin < token.size() && std::isspace(static_cast<unsigned char>(token[begin])) != 0)
					++begin;
				std::size_t end = token.size();
				while (end > begin && std::isspace(static_cast<unsigned char>(token[end - 1])) != 0)
					--end;

				std::string axis;
				axis.reserve(end - begin);
				for (std::size_t i = begin; i < end; ++i)
					axis.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(token[i]))));

				if (axis == "x")
					return FlowPlot::Spec::HistogramDataAxis::X;
				if (axis == "y")
					return FlowPlot::Spec::HistogramDataAxis::Y;
				throw std::runtime_error(
					"compileTemplateToSpec: unsupported histogram data axis '" + std::string(token)
					+ "' at '" + path + "' (expected 'x' or 'y')");
			}

			inline void applyLayerAxisDataSpec(
				FlowPlot::Spec::LayerAxisDataSpec& axisData,
				const json& objectJson,
				const std::string& path)
			{
				std::string xRole = "primary";
				std::string yRole = "primary";
				readString(objectJson, "x", xRole, path);
				readString(objectJson, "y", yRole, path);
				axisData.x = parseAxisDataRoleToken(xRole, childPath(path, "x"));
				axisData.y = parseAxisDataRoleToken(yRole, childPath(path, "y"));
			}

		inline void applyScatterMappingSpec(FlowPlot::Spec::ScatterMappingSpec& mapping, const json& objectJson, const std::string& path)
		{
			if (const json* xObject = findObject(objectJson, "x", path))
				readString(*xObject, "field", mapping.xField, childPath(path, "x"));

			if (const json* yObject = findObject(objectJson, "y", path))
				readString(*yObject, "field", mapping.yField, childPath(path, "y"));

			if (const json* colorObject = findObject(objectJson, "color", path))
			{
				readString(*colorObject, "field", mapping.colorField, childPath(path, "color"));
				if (const json* colorMapObject = findObject(*colorObject, "colorMapping-scatter", childPath(path, "color")))
				{
					readString(*colorMapObject, "type", mapping.colorMapping.type, childPath(path, "color.colorMapping-scatter"));
					readStringArray(*colorMapObject, "categories", mapping.colorMapping.categories, childPath(path, "color.colorMapping-scatter"));
					readStringArray(*colorMapObject, "values", mapping.colorMapping.values, childPath(path, "color.colorMapping-scatter"));
					readString(*colorMapObject, "minValue", mapping.colorMapping.minValue, childPath(path, "color.colorMapping-scatter"));
					readString(*colorMapObject, "maxValue", mapping.colorMapping.maxValue, childPath(path, "color.colorMapping-scatter"));
				}
			}

			if (const json* sizeObject = findObject(objectJson, "size", path))
			{
				readString(*sizeObject, "field", mapping.sizeField, childPath(path, "size"));
				if (const json* sizeMapObject = findObject(*sizeObject, "sizeMapping-scatter", childPath(path, "size")))
				{
					readString(*sizeMapObject, "type", mapping.sizeMapping.type, childPath(path, "size.sizeMapping-scatter"));
					readStringArray(*sizeMapObject, "categories", mapping.sizeMapping.categories, childPath(path, "size.sizeMapping-scatter"));
					readFloatArray(*sizeMapObject, "values", mapping.sizeMapping.values, childPath(path, "size.sizeMapping-scatter"));
					readFloat(*sizeMapObject, "minValue", mapping.sizeMapping.minValue, childPath(path, "size.sizeMapping-scatter"));
					readFloat(*sizeMapObject, "maxValue", mapping.sizeMapping.maxValue, childPath(path, "size.sizeMapping-scatter"));
				}
			}

			if (const json* labelObject = findObject(objectJson, "label", path))
			{
				readString(*labelObject, "field", mapping.labelField, childPath(path, "label"));
				if (const json* labelMapObject = findObject(*labelObject, "labelMapping-scatter", childPath(path, "label")))
				{
					readStringArray(*labelMapObject, "categories", mapping.labelMapping.categories, childPath(path, "label.labelMapping-scatter"));
					readStringArray(*labelMapObject, "values", mapping.labelMapping.values, childPath(path, "label.labelMapping-scatter"));
				}
			}
		}

		inline void applyScatterStyleSpec(FlowPlot::Spec::ScatterStyleSpec& style, const json& objectJson, const std::string& path)
		{
			readString(objectJson, "markerShape", style.markerShape, path);
			readString(objectJson, "fillColor", style.fillColor, path);
			readString(objectJson, "strokeColor", style.strokeColor, path);
			readFloat(objectJson, "strokeWidth", style.strokeWidth, path);
			readFloat(objectJson, "markerSize", style.markerSize, path);
		}

		inline void applyScatterStatsSpec(FlowPlot::Spec::ScatterStatsSpec& stats, const json& objectJson, const std::string& path)
		{
			readBool(objectJson, "showMeanPoint", stats.showMeanPoint, path);
			readString(objectJson, "meanPointShape", stats.meanPointShape, path);
			readString(objectJson, "meanPointColor", stats.meanPointColor, path);
			readString(objectJson, "meanPointStrokeColor", stats.meanPointStrokeColor, path);
			readFloat(objectJson, "meanPointStrokeWidth", stats.meanPointStrokeWidth, path);
			readFloat(objectJson, "meanPointSize", stats.meanPointSize, path);
		}

		inline void applyScatterConfigSpec(FlowPlot::Spec::ScatterConfigSpec& config, const json& objectJson, const std::string& path)
		{
			readFloat(objectJson, "jitterX", config.jitterX, path);
			readFloat(objectJson, "jitterY", config.jitterY, path);
			readFloat(objectJson, "domainPadding", config.domainPadding, path);
		}

			inline void applyHistogramMappingSpec(FlowPlot::Spec::HistogramMappingSpec& mapping, const json& objectJson, const std::string& path)
			{
				if (const json* dataObject = findObject(objectJson, "data", path))
				{
					readString(*dataObject, "field", mapping.dataField, childPath(path, "data"));
					std::string axisToken = "x";
					readString(*dataObject, "axis", axisToken, childPath(path, "data"));
					mapping.axis = parseHistogramDataAxisToken(axisToken, childPath(path, "data.axis"));
				}

				if (const json* colorObject = findObject(objectJson, "color", path))
				{
				readString(*colorObject, "field", mapping.colorField, childPath(path, "color"));
				if (const json* colorMapObject = findObject(*colorObject, "colorMapping-histogram", childPath(path, "color")))
				{
					readString(*colorMapObject, "type", mapping.colorMapping.type, childPath(path, "color.colorMapping-histogram"));
					readStringArray(*colorMapObject, "categories", mapping.colorMapping.categories, childPath(path, "color.colorMapping-histogram"));
					readStringArray(*colorMapObject, "values", mapping.colorMapping.values, childPath(path, "color.colorMapping-histogram"));
					readString(*colorMapObject, "minValue", mapping.colorMapping.minValue, childPath(path, "color.colorMapping-histogram"));
					readString(*colorMapObject, "maxValue", mapping.colorMapping.maxValue, childPath(path, "color.colorMapping-histogram"));
				}
			}
		}

		inline void applyHistogramStyleSpec(FlowPlot::Spec::HistogramStyleSpec& style, const json& objectJson, const std::string& path)
		{
			readString(objectJson, "fillColor", style.fillColor, path);
			readString(objectJson, "strokeColor", style.strokeColor, path);
			readFloat(objectJson, "strokeWidth", style.strokeWidth, path);
		}

		inline void applyHistogramStatsSpec(FlowPlot::Spec::HistogramStatsSpec& stats, const json& objectJson, const std::string& path)
		{
			readBool(objectJson, "showMeanLine", stats.showMeanLine, path);
			readString(objectJson, "meanLineColor", stats.meanLineColor, path);
			readFloat(objectJson, "meanLineWidth", stats.meanLineWidth, path);
			readBool(objectJson, "showMedianLine", stats.showMedianLine, path);
			readString(objectJson, "medianLineColor", stats.medianLineColor, path);
			readFloat(objectJson, "medianLineWidth", stats.medianLineWidth, path);
		}

			inline void applyHistogramConfigSpec(FlowPlot::Spec::HistogramConfigSpec& config, const json& objectJson, const std::string& path)
			{
				readUint32(objectJson, "binCount", config.binCount, path);
				readBool(objectJson, "normalize", config.normalize, path);
				readBool(objectJson, "cumulative", config.cumulative, path);
				readBool(objectJson, "showEmptyBins", config.showEmptyBins, path);
				readFloat(objectJson, "domainPadding", config.domainPadding, path);
			}

		inline FlowPlot::Spec::LayerSpec compileLayerSpec(const json& layerJson, const std::string& path)
		{
			if (!layerJson.IsObject())
				throw std::runtime_error("compileTemplateToSpec: '" + path + "' must be an object");

			FlowPlot::Spec::LayerSpec layer{};
			readString(layerJson, "id", layer.id, path);
			readString(layerJson, "type", layer.type, path);
			readBool(layerJson, "visible", layer.visible, path);
				readFloat(layerJson, "opacity", layer.opacity, path);
				readInt32(layerJson, "zIndex", layer.zIndex, path);
				readString(layerJson, "dataset", layer.dataset, path);
				if (const json* axisDataObject = findObject(layerJson, "axisData", path))
					applyLayerAxisDataSpec(layer.axisData, *axisDataObject, childPath(path, "axisData"));

			if (layer.type == "scatter")
			{
				if (const json* mappingObject = findObject(layerJson, "mapping", path))
					applyScatterMappingSpec(layer.scatterMapping, *mappingObject, childPath(path, "mapping"));
				if (const json* styleObject = findObject(layerJson, "style", path))
					applyScatterStyleSpec(layer.scatterStyle, *styleObject, childPath(path, "style"));
				if (const json* statsObject = findObject(layerJson, "stats", path))
					applyScatterStatsSpec(layer.scatterStats, *statsObject, childPath(path, "stats"));
				if (const json* configObject = findObject(layerJson, "config", path))
					applyScatterConfigSpec(layer.scatterConfig, *configObject, childPath(path, "config"));
			}
			else if (layer.type == "histogram")
			{
				if (const json* mappingObject = findObject(layerJson, "mapping", path))
					applyHistogramMappingSpec(layer.histogramMapping, *mappingObject, childPath(path, "mapping"));
				if (const json* styleObject = findObject(layerJson, "style", path))
					applyHistogramStyleSpec(layer.histogramStyle, *styleObject, childPath(path, "style"));
				if (const json* statsObject = findObject(layerJson, "stats", path))
					applyHistogramStatsSpec(layer.histogramStats, *statsObject, childPath(path, "stats"));
				if (const json* configObject = findObject(layerJson, "config", path))
					applyHistogramConfigSpec(layer.histogramConfig, *configObject, childPath(path, "config"));
			}
			else
			{
				throw std::runtime_error("compileTemplateToSpec: unsupported layer type '" + layer.type + "' at '" + childPath(path, "type") + "'");
			}

			return layer;
		}

		inline FlowPlot::Spec::PanelSpec compilePanelSpec(const json& panelJson, const std::string& path)
		{
			if (!panelJson.IsObject())
				throw std::runtime_error("compileTemplateToSpec: '" + path + "' must be an object");

			FlowPlot::Spec::PanelSpec panel{};
			readString(panelJson, "id", panel.id, path);
			readBool(panelJson, "visible", panel.visible, path);
			readString(panelJson, "background", panel.background, path);
			readString(panelJson, "borderColor", panel.borderColor, path);
			readFloat(panelJson, "borderWidth", panel.borderWidth, path);
			readBool(panelJson, "clipContent", panel.clipContent, path);

			if (const json* paddingObject = findObject(panelJson, "padding", path))
				applyPaddingSpec(panel.padding, *paddingObject, childPath(path, "padding"));
			if (const json* titleObject = findObject(panelJson, "title", path))
				applyTextSpec(panel.title, *titleObject, childPath(path, "title"));
				if (const json* xAxisObject = findObject(panelJson, "xAxis", path))
					applyAxisSpec(panel.xAxis, *xAxisObject, childPath(path, "xAxis"));
				if (const json* yAxisObject = findObject(panelJson, "yAxis", path))
					applyAxisSpec(panel.yAxis, *yAxisObject, childPath(path, "yAxis"));
				if (const json* xSecondaryObject = findObject(panelJson, "xSecondary", path))
					applyAxisSpec(panel.xSecondary, *xSecondaryObject, childPath(path, "xSecondary"));
				if (const json* ySecondaryObject = findObject(panelJson, "ySecondary", path))
					applyAxisSpec(panel.ySecondary, *ySecondaryObject, childPath(path, "ySecondary"));

			if (const json* layersArray = findArray(panelJson, "layers", path))
			{
				panel.layers.clear();
				if (layersArray->Empty())
				{
					panel.layers.emplace_back(FlowPlot::Spec::LayerSpec{});
				}
				else
				{
					panel.layers.reserve(layersArray->Size());
					for (rapidjson::SizeType i = 0; i < layersArray->Size(); ++i)
					{
						const json& layerObject = (*layersArray)[i];
						panel.layers.push_back(compileLayerSpec(layerObject, childPath(path, "layers") + "[" + std::to_string(i) + "]"));
					}
				}
			}
			else
			{
				panel.layers.clear();
				panel.layers.emplace_back(FlowPlot::Spec::LayerSpec{});
			}

			return panel;
		}

		inline void applyFigureSpec(FlowPlot::Spec::FigureSpec& figure, const json& figureJson)
		{
			readUint32(figureJson, "width", figure.width, "figure");
			readUint32(figureJson, "height", figure.height, "figure");
			readUint32(figureJson, "dpi", figure.dpi, "figure");
			readString(figureJson, "background", figure.background, "figure");

			if (const json* paddingObject = findObject(figureJson, "padding", "figure"))
				applyPaddingSpec(figure.padding, *paddingObject, "figure.padding");
			if (const json* titleObject = findObject(figureJson, "title", "figure"))
				applyTextSpec(figure.title, *titleObject, "figure.title");

			if (const json* legendsArray = findArray(figureJson, "legends", "figure"))
			{
				figure.legends.clear();
				if (legendsArray->Empty())
				{
					figure.legends.emplace_back(FlowPlot::Spec::LegendSpec{});
				}
				else
				{
					figure.legends.reserve(legendsArray->Size());
					for (rapidjson::SizeType i = 0; i < legendsArray->Size(); ++i)
					{
						const json& legendObject = (*legendsArray)[i];
						if (!legendObject.IsObject())
							throw std::runtime_error("compileTemplateToSpec: 'figure.legends[" + std::to_string(i) + "]' must be an object");

						FlowPlot::Spec::LegendSpec legendSpec{};
						applyLegendSpec(legendSpec, legendObject, "figure.legends[" + std::to_string(i) + "]");
						if (legendSpec.legendElements.empty())
							legendSpec.legendElements.emplace_back(FlowPlot::Spec::LegendElementSpec{});
						figure.legends.push_back(std::move(legendSpec));
					}
				}
			}
			else
			{
				figure.legends.clear();
				figure.legends.emplace_back(FlowPlot::Spec::LegendSpec{});
			}
		}
	} // namespace SpecCompiler

	inline FlowPlot::Spec::MasterTemplateSpec compileTemplateToSpec(const rapidjson::Value& templateJson)
	{
		if (!templateJson.IsObject())
			throw std::runtime_error("compileTemplateToSpec: template root must be a JSON object");

		FlowPlot::Spec::MasterTemplateSpec spec{};

		if (const rapidjson::Value* versionValue = SpecCompiler::findKey(templateJson, "version"))
		{
			if (!versionValue->IsString())
				throw std::runtime_error("compileTemplateToSpec: 'version' must be a string");

			spec.version = jsonStringToStdString(*versionValue);
			if (spec.version != "1.0")
				throw std::runtime_error("compileTemplateToSpec: unsupported version '" + spec.version + "'");
		}

		if (const rapidjson::Value* figureObject = SpecCompiler::findObject(templateJson, "figure", ""))
			SpecCompiler::applyFigureSpec(spec.figure, *figureObject);

		if (const rapidjson::Value* layoutObject = SpecCompiler::findObject(templateJson, "layout", ""))
		{
			SpecCompiler::readUint32(*layoutObject, "rows", spec.layout.rows, "layout");
			SpecCompiler::readUint32(*layoutObject, "cols", spec.layout.cols, "layout");
			SpecCompiler::readFloat(*layoutObject, "gap", spec.layout.gap, "layout");
		}

		if (const rapidjson::Value* datasetsArray = SpecCompiler::findArray(templateJson, "datasets", ""))
		{
			spec.datasets.clear();
			spec.datasets.reserve(datasetsArray->Size());
			std::unordered_set<std::string> seenDatasetNames;
			seenDatasetNames.reserve(datasetsArray->Size());
			for (rapidjson::SizeType i = 0; i < datasetsArray->Size(); ++i)
			{
				const rapidjson::Value& datasetObject = (*datasetsArray)[i];
				const std::string datasetPath = "datasets[" + std::to_string(i) + "]";
				if (!datasetObject.IsObject())
					throw std::runtime_error("compileTemplateToSpec: '" + datasetPath + "' must be an object");

				FlowPlot::Spec::DatasetSpec datasetSpec{};
				const rapidjson::Value* datasetNameValue = SpecCompiler::findKey(datasetObject, "name");
				if (datasetNameValue == nullptr)
					throw std::runtime_error("compileTemplateToSpec: '" + SpecCompiler::childPath(datasetPath, "name") + "' is required");
				if (!datasetNameValue->IsString())
					throw std::runtime_error("compileTemplateToSpec: '" + SpecCompiler::childPath(datasetPath, "name") + "' must be a string");

				datasetSpec.name = jsonStringToStdString(*datasetNameValue);
				if (datasetSpec.name.empty())
					throw std::runtime_error("compileTemplateToSpec: '" + SpecCompiler::childPath(datasetPath, "name") + "' must not be empty");
				if (!seenDatasetNames.insert(datasetSpec.name).second)
					throw std::runtime_error("compileTemplateToSpec: duplicate dataset name '" + datasetSpec.name + "'");

				const rapidjson::Value* schemaObject = SpecCompiler::findObject(datasetObject, "schema", datasetPath);
				if (schemaObject == nullptr)
					throw std::runtime_error("compileTemplateToSpec: '" + SpecCompiler::childPath(datasetPath, "schema") + "' is required");

				for (auto schemaIt = schemaObject->MemberBegin(); schemaIt != schemaObject->MemberEnd(); ++schemaIt)
				{
					const std::string fieldName = std::string(schemaIt->name.GetString(), schemaIt->name.GetStringLength());
					const std::string fieldPath = SpecCompiler::childPath(
						SpecCompiler::childPath(datasetPath, "schema"),
						fieldName);
					if (fieldName.empty())
						throw std::runtime_error("compileTemplateToSpec: '" + fieldPath + "' must not be empty");

					if (!schemaIt->value.IsString())
					{
						throw std::runtime_error(
							"compileTemplateToSpec: '"
							+ fieldPath
							+ "' must be a string");
					}

					const std::string fieldTypeToken = jsonStringToStdString(schemaIt->value);
					datasetSpec.schema[fieldName] = SpecCompiler::parseDatasetFieldType(
						fieldTypeToken,
						fieldPath);
				}
				spec.datasets.push_back(std::move(datasetSpec));
			}
		}

		if (const rapidjson::Value* panelsArray = SpecCompiler::findArray(templateJson, "panels", ""))
		{
			spec.panels.clear();
			if (panelsArray->Empty())
			{
				spec.panels.emplace_back(FlowPlot::Spec::PanelSpec{});
			}
			else
			{
				spec.panels.reserve(panelsArray->Size());
				for (rapidjson::SizeType i = 0; i < panelsArray->Size(); ++i)
				{
					const rapidjson::Value& panelObject = (*panelsArray)[i];
					spec.panels.push_back(SpecCompiler::compilePanelSpec(panelObject, "panels[" + std::to_string(i) + "]"));
				}
			}
		}
		else
		{
			spec.panels.clear();
			spec.panels.emplace_back(FlowPlot::Spec::PanelSpec{});
		}

		return spec;
	}

	inline BoundIR::PlotBoundIR buildBoundIR(
		const FlowPlot::Spec::MasterTemplateSpec& spec,
		const std::unordered_map<DataKey, DataView, DataKeyHash>& dataViews)
	{
		BoundIR::PlotBoundIR bound{};
		bound.datasets.reserve(spec.datasets.size());

		std::unordered_map<std::string, std::uint32_t> datasetIndexByName;
		datasetIndexByName.reserve(spec.datasets.size());
		std::vector<std::unordered_map<std::string, std::uint32_t>> fieldIndexByNamePerDataset;
		fieldIndexByNamePerDataset.reserve(spec.datasets.size());

		for (std::size_t datasetIdx = 0; datasetIdx < spec.datasets.size(); ++datasetIdx)
		{
			const FlowPlot::Spec::DatasetSpec& datasetSpec = spec.datasets[datasetIdx];
			if (datasetSpec.name.empty())
				throw std::runtime_error("buildBoundIR: datasets[" + std::to_string(datasetIdx) + "].name must not be empty");

			if (!datasetIndexByName.emplace(datasetSpec.name, static_cast<std::uint32_t>(datasetIdx)).second)
				throw std::runtime_error("buildBoundIR: duplicate dataset name '" + datasetSpec.name + "'");

			BoundIR::BoundDataset boundDataset{};
			boundDataset.views.reserve(datasetSpec.schema.size());

			std::unordered_map<std::string, std::uint32_t> fieldIndexByName;
			fieldIndexByName.reserve(datasetSpec.schema.size());

			std::size_t sharedCount = 0;
			bool hasAnyBoundField = false;
			for (const auto& [fieldName, fieldType] : datasetSpec.schema)
			{
				DataKey key{datasetSpec.name, fieldName};
				auto dataIt = dataViews.find(key);
				if (dataIt == dataViews.end())
					continue;

				const DataView& view = dataIt->second;
				if (view.ptr == nullptr && view.count > 0)
					throw std::runtime_error("buildBoundIR: data buffer for '" + datasetSpec.name + "." + fieldName + "' is null but count is non-zero");

				if (!DataBinding::schemaTypeMatchesViewType(fieldType, view.valueType))
				{
					throw std::runtime_error(
						"buildBoundIR: type mismatch for '" + datasetSpec.name + "." + fieldName + "' (schema='"
						+ DataBinding::schemaTypeName(fieldType)
						+ "', provided='"
						+ DataBinding::valueTypeName(view.valueType)
						+ "')");
				}

				if (!hasAnyBoundField)
				{
					sharedCount = view.count;
					hasAnyBoundField = true;
				}
				else if (view.count != sharedCount)
					throw std::runtime_error("buildBoundIR: inconsistent vector lengths in dataset '" + datasetSpec.name + "'");

				if (boundDataset.views.size() >= static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()))
					throw std::runtime_error("buildBoundIR: too many bound fields in dataset '" + datasetSpec.name + "'");

				const std::uint32_t viewIndex = static_cast<std::uint32_t>(boundDataset.views.size());
				boundDataset.views.push_back(view);
				fieldIndexByName.emplace(fieldName, viewIndex);
			}

			if (hasAnyBoundField)
			{
				if (sharedCount > static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()))
					throw std::runtime_error("buildBoundIR: dataset '" + datasetSpec.name + "' row count exceeds uint32");
				boundDataset.rowCount = static_cast<std::uint32_t>(sharedCount);
			}

			bound.datasets.push_back(std::move(boundDataset));
			fieldIndexByNamePerDataset.push_back(std::move(fieldIndexByName));
		}

		for (std::size_t panelIdx = 0; panelIdx < spec.panels.size(); ++panelIdx)
		{
			const FlowPlot::Spec::PanelSpec& panel = spec.panels[panelIdx];
			if (!panel.visible)
				continue;

			std::vector<BoundIR::BoundLayer> panelLayers;
			panelLayers.reserve(panel.layers.size());

			for (std::size_t layerIdx = 0; layerIdx < panel.layers.size(); ++layerIdx)
			{
				const FlowPlot::Spec::LayerSpec& layer = panel.layers[layerIdx];
				if (!layer.visible)
					continue;

				if (layer.dataset.empty())
					continue;

				auto datasetIdxIt = datasetIndexByName.find(layer.dataset);
				if (datasetIdxIt == datasetIndexByName.end())
				{
					throw std::runtime_error(
						"buildBoundIR: unknown dataset '" + layer.dataset + "' used at panels["
						+ std::to_string(panelIdx) + "].layers[" + std::to_string(layerIdx) + "]");
				}

				const std::uint32_t datasetIndex = datasetIdxIt->second;
				const FlowPlot::Spec::DatasetSpec& datasetSpec = spec.datasets[datasetIndex];
				const std::unordered_map<std::string, std::uint32_t>& fieldIndexByName = fieldIndexByNamePerDataset[datasetIndex];
				const BoundIR::BoundDataset& boundDataset = bound.datasets[datasetIndex];
				if (boundDataset.rowCount == 0)
					continue;

				BoundIR::BoundLayer boundLayer{};
				boundLayer.panelIndex = static_cast<std::uint32_t>(panelIdx);
				boundLayer.layerIndex = static_cast<std::uint32_t>(layerIdx);
				boundLayer.zIndex = layer.zIndex;
				boundLayer.opacity = layer.opacity;
				boundLayer.axisData = layer.axisData;

				const std::string layerPath = "panels[" + std::to_string(panelIdx) + "].layers[" + std::to_string(layerIdx) + "]";

				if (layer.type == "scatter")
				{
					BoundIR::BoundScatterInputs inputs{};
					inputs.dataset = datasetIndex;

					if (!DataBinding::tryResolveRequiredFieldIndex(
							datasetSpec,
							fieldIndexByName,
							layer.scatterMapping.xField,
							layerPath + ".mapping.x.field",
							inputs.x))
					{
						continue;
					}

					if (!DataBinding::tryResolveRequiredFieldIndex(
							datasetSpec,
							fieldIndexByName,
							layer.scatterMapping.yField,
							layerPath + ".mapping.y.field",
							inputs.y))
					{
						continue;
					}

					inputs.color = DataBinding::tryResolveOptionalFieldIndex(
						datasetSpec,
						fieldIndexByName,
						layer.scatterMapping.colorField,
						layerPath + ".mapping.color.field");
					inputs.size = DataBinding::tryResolveOptionalFieldIndex(
						datasetSpec,
						fieldIndexByName,
						layer.scatterMapping.sizeField,
						layerPath + ".mapping.size.field");
					inputs.label = DataBinding::tryResolveOptionalFieldIndex(
						datasetSpec,
						fieldIndexByName,
						layer.scatterMapping.labelField,
						layerPath + ".mapping.label.field");

					boundLayer.inputs = inputs;
				}
				else if (layer.type == "histogram")
				{
					BoundIR::BoundHistogramInputs inputs{};
					inputs.dataset = datasetIndex;
					inputs.dataAxis = layer.histogramMapping.axis;

					if (!DataBinding::tryResolveRequiredFieldIndex(
							datasetSpec,
							fieldIndexByName,
							layer.histogramMapping.dataField,
							layerPath + ".mapping.data.field",
							inputs.data))
					{
						continue;
					}

					inputs.color = DataBinding::tryResolveOptionalFieldIndex(
						datasetSpec,
						fieldIndexByName,
						layer.histogramMapping.colorField,
						layerPath + ".mapping.color.field");

					boundLayer.inputs = inputs;
				}
				else
				{
					throw std::runtime_error("buildBoundIR: unsupported layer type '" + layer.type + "' at '" + layerPath + ".type'");
				}

				panelLayers.push_back(std::move(boundLayer));
			}

			std::stable_sort(
				panelLayers.begin(),
				panelLayers.end(),
				[](const BoundIR::BoundLayer& lhs, const BoundIR::BoundLayer& rhs)
				{
					return lhs.zIndex < rhs.zIndex;
				});

			bound.layersInPainterOrder.insert(
				bound.layersInPainterOrder.end(),
				std::make_move_iterator(panelLayers.begin()),
				std::make_move_iterator(panelLayers.end()));
		}

		return bound;
	}

	namespace ResolveCompiler
	{
		inline std::uint64_t makePanelLayerKey(std::uint32_t panelIndex, std::uint32_t layerIndex) noexcept
		{
			return (static_cast<std::uint64_t>(panelIndex) << 32U) | static_cast<std::uint64_t>(layerIndex);
		}

		inline const BoundIR::BoundLayer* findBoundLayer(
			const std::unordered_map<std::uint64_t, const BoundIR::BoundLayer*>& boundLayerByKey,
			std::size_t panelIndex,
			std::size_t layerIndex)
		{
			if (panelIndex > static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max())
				|| layerIndex > static_cast<std::size_t>(std::numeric_limits<std::uint32_t>::max()))
			{
				throw std::runtime_error("resolvePlotIR: panel/layer index exceeds uint32 range");
			}

			const std::uint64_t key = makePanelLayerKey(
				static_cast<std::uint32_t>(panelIndex),
				static_cast<std::uint32_t>(layerIndex));
			auto it = boundLayerByKey.find(key);
			if (it == boundLayerByKey.end())
				return nullptr;
			return it->second;
		}

		inline std::string canonicalizeToken(std::string_view token)
		{
			std::size_t begin = 0;
			while (begin < token.size() && std::isspace(static_cast<unsigned char>(token[begin])) != 0)
				++begin;

			std::size_t end = token.size();
			while (end > begin && std::isspace(static_cast<unsigned char>(token[end - 1])) != 0)
				--end;

			std::string canonical;
			canonical.reserve(end - begin);
			for (std::size_t i = begin; i < end; ++i)
				canonical.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(token[i]))));
			return canonical;
		}

		inline std::uint8_t parseHexNibble(char c, const std::string& path, const std::string& rawColor)
		{
			if (c >= '0' && c <= '9')
				return static_cast<std::uint8_t>(c - '0');

			const char lower = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
			if (lower >= 'a' && lower <= 'f')
				return static_cast<std::uint8_t>(10 + (lower - 'a'));

			throw std::runtime_error(
				"resolvePlotIR: '" + path + "' has invalid color '" + rawColor + "'");
		}

		inline std::uint8_t parseHexByte(char hi, char lo, const std::string& path, const std::string& rawColor)
		{
			const std::uint8_t highNibble = parseHexNibble(hi, path, rawColor);
			const std::uint8_t lowNibble = parseHexNibble(lo, path, rawColor);
			return static_cast<std::uint8_t>((highNibble << 4U) | lowNibble);
		}

		inline FlowPlot::Color parseColor(std::string_view rawColor, const std::string& path)
		{
			if (rawColor.empty())
				throw std::runtime_error("resolvePlotIR: '" + path + "' color must not be empty");
			if (rawColor.front() != '#')
				throw std::runtime_error("resolvePlotIR: '" + path + "' color must start with '#'");

			const std::string colorString(rawColor);
			if (colorString.size() != 7 && colorString.size() != 9)
			{
				throw std::runtime_error(
					"resolvePlotIR: '" + path + "' color must be '#RRGGBB' or '#RRGGBBAA' (got '" + colorString + "')");
			}

			FlowPlot::Color color{};
			color.r = parseHexByte(colorString[1], colorString[2], path, colorString);
			color.g = parseHexByte(colorString[3], colorString[4], path, colorString);
			color.b = parseHexByte(colorString[5], colorString[6], path, colorString);
			color.a = (colorString.size() == 9)
				? parseHexByte(colorString[7], colorString[8], path, colorString)
				: static_cast<std::uint8_t>(255);
			return color;
		}

		inline FlowPlot::HorizontalAlign parseHorizontalAlign(std::string_view rawAlign, const std::string& path)
		{
			const std::string align = canonicalizeToken(rawAlign);
			if (align == "left")
				return FlowPlot::HorizontalAlign::Left;
			if (align == "center" || align == "middle")
				return FlowPlot::HorizontalAlign::Center;
			if (align == "right")
				return FlowPlot::HorizontalAlign::Right;

			throw std::runtime_error("resolvePlotIR: unsupported horizontal align '" + std::string(rawAlign) + "' at '" + path + "'");
		}

		inline FlowPlot::VerticalAlign parseVerticalAlign(std::string_view rawAlign, const std::string& path)
		{
			const std::string align = canonicalizeToken(rawAlign);
			if (align == "top")
				return FlowPlot::VerticalAlign::Top;
			if (align == "middle" || align == "center")
				return FlowPlot::VerticalAlign::Middle;
			if (align == "bottom")
				return FlowPlot::VerticalAlign::Bottom;

			throw std::runtime_error("resolvePlotIR: unsupported vertical align '" + std::string(rawAlign) + "' at '" + path + "'");
		}

		inline FlowPlot::TextMeasurement measureTextForAutoSizing(
			const FlowPlot::ITextEngine* textEngine,
			std::string_view fontFamily,
			std::uint16_t fontWeight,
			float fontSize,
			std::string_view text,
			const std::string& path)
		{
			if (textEngine == nullptr)
			{
				throw std::runtime_error(
					"resolvePlotIR: '" + path + "' requires text measurement but no text engine is set");
			}

			return textEngine->measureText(
				fontFamily,
				fontWeight,
				fontSize,
				text);
		}

		inline FlowPlot::TextMeasurement measureTextForAutoSizing(
			const FlowPlot::Spec::TextSpec& textSpec,
			const FlowPlot::ITextEngine* textEngine,
			const std::string& path)
		{
			return measureTextForAutoSizing(
				textEngine,
				textSpec.fontFamily,
				textSpec.fontWeight,
				textSpec.fontSize,
				textSpec.text,
				path);
		}

		inline FlowPlot::RectF resolveFigureTitleBox(
			const FlowPlot::Spec::FigureSpec& figureSpec,
			const FlowPlot::Spec::TextSpec& textSpec,
			const FlowPlot::ITextEngine* textEngine)
		{
			const bool missingWidth = !textSpec.box.width.has_value();
			const bool missingHeight = !textSpec.box.height.has_value();

			FlowPlot::TextMeasurement measured{};
			if (missingWidth || missingHeight)
				measured = measureTextForAutoSizing(textSpec, textEngine, "figure.title.box");

			const float width = textSpec.box.width.value_or(measured.width);
			const float height = textSpec.box.height.value_or(measured.height);
			if (width < 0.0f || height < 0.0f)
				throw std::runtime_error("resolvePlotIR: 'figure.title.box' width/height must be non-negative");

			const float figureWidth = static_cast<float>(figureSpec.width);
			const float x = textSpec.box.x.value_or((figureWidth - width) * 0.5f);
			const float y = textSpec.box.y.value_or(figureSpec.padding.top);
			return FlowPlot::RectF{x, y, width, height};
		}

		inline ResolvedIR::ResolvedText resolveFigureTitle(
			const FlowPlot::Spec::FigureSpec& figureSpec,
			const FlowPlot::ITextEngine* textEngine)
		{
			const FlowPlot::Spec::TextSpec& titleSpec = figureSpec.title;

			ResolvedIR::ResolvedText resolved{};
			resolved.text = titleSpec.text;
			resolved.fontFamily = titleSpec.fontFamily;
			resolved.fontSize = titleSpec.fontSize;
			resolved.fontWeight = titleSpec.fontWeight;
			resolved.color = parseColor(titleSpec.color, "figure.title.color");
			resolved.hAlign = parseHorizontalAlign(titleSpec.hAlign, "figure.title.hAlign");
			resolved.vAlign = parseVerticalAlign(titleSpec.vAlign, "figure.title.vAlign");
			resolved.clipToBox = titleSpec.clip;
			resolved.box = resolveFigureTitleBox(figureSpec, titleSpec, textEngine);
			return resolved;
		}

		struct LegendElementLayoutCache
		{
			ResolvedIR::ResolvedBox icon{};
			ResolvedIR::ResolvedText label{};
			float width = 0.0f;
			float height = 0.0f;
		};

		inline LegendElementLayoutCache resolveLegendElementLayout(
			const FlowPlot::Spec::LegendElementSpec& elementSpec,
			const FlowPlot::ITextEngine* textEngine,
			const std::string& path)
		{
			if (elementSpec.fontSize < 0.0f)
				throw std::runtime_error("resolvePlotIR: '" + path + ".fontSize' must be non-negative");

			const bool missingTextWidth = !elementSpec.box.width.has_value();
			const bool missingTextHeight = !elementSpec.box.height.has_value();

			FlowPlot::TextMeasurement measured{};
			if (missingTextWidth || missingTextHeight)
			{
				measured = measureTextForAutoSizing(
					textEngine,
					elementSpec.fontFamily,
					elementSpec.fontWeight,
					elementSpec.fontSize,
					elementSpec.text,
					path + ".box");
			}

			const float textWidth = elementSpec.box.width.value_or(measured.width);
			const float textHeight = elementSpec.box.height.value_or(measured.height);
			if (textWidth < 0.0f || textHeight < 0.0f)
				throw std::runtime_error("resolvePlotIR: '" + path + ".box' width/height must be non-negative");

			LegendElementLayoutCache cache{};
			cache.icon.fill = parseColor(elementSpec.iconColor, path + ".iconColor");
			cache.icon.stroke = cache.icon.fill;
			cache.icon.strokeWidth = 0.0f;
			cache.icon.rect.w = elementSpec.fontSize;
			cache.icon.rect.h = elementSpec.fontSize;

			cache.label.text = elementSpec.text;
			cache.label.fontFamily = elementSpec.fontFamily;
			cache.label.fontSize = elementSpec.fontSize;
			cache.label.fontWeight = elementSpec.fontWeight;
			cache.label.color = parseColor(elementSpec.color, path + ".color");
			cache.label.clipToBox = elementSpec.clip;
			cache.label.box.w = textWidth;
			cache.label.box.h = textHeight;

			cache.width = elementSpec.fontSize + (elementSpec.fontSize * 0.5f) + textWidth;
			cache.height = std::max(elementSpec.fontSize, textHeight);
			return cache;
		}

		inline FlowPlot::RectF resolveLegendFrameRect(
			const FlowPlot::Spec::LegendSpec& legendSpec,
			const std::vector<LegendElementLayoutCache>& elementCaches,
			const FlowPlot::Spec::FigureSpec& figureSpec,
			const FlowPlot::Spec::LayoutSpec& layoutSpec)
		{
			float widestElement = 0.0f;
			float allElementHeights = 0.0f;
			for (const LegendElementLayoutCache& cache : elementCaches)
			{
				widestElement = std::max(widestElement, cache.width);
				allElementHeights += cache.height;
			}

			const float gapTotal =
				(elementCaches.size() > 1)
				? (legendSpec.gap * static_cast<float>(elementCaches.size() - 1U))
				: 0.0f;

			const float autoWidth = widestElement + legendSpec.padding.left + legendSpec.padding.right;
			const float autoHeight = legendSpec.padding.top + allElementHeights + gapTotal + legendSpec.padding.bottom;

			const float width = legendSpec.box.width.value_or(autoWidth);
			const float height = legendSpec.box.height.value_or(autoHeight);
			if (width < 0.0f || height < 0.0f)
				throw std::runtime_error("resolvePlotIR: legend box width/height must be non-negative");

			const float figureWidth = static_cast<float>(figureSpec.width);
			const float autoX = figureWidth - figureSpec.padding.right - layoutSpec.gap - width;
			const float autoY = figureSpec.padding.top + layoutSpec.gap;

			const float x = legendSpec.box.x.value_or(autoX);
			const float y = legendSpec.box.y.value_or(autoY);
			return FlowPlot::RectF{x, y, width, height};
		}

		inline ResolvedIR::LegendResolved resolveLegend(
			const FlowPlot::Spec::LegendSpec& legendSpec,
			std::size_t legendIndex,
			const FlowPlot::Spec::FigureSpec& figureSpec,
			const FlowPlot::Spec::LayoutSpec& layoutSpec,
			const FlowPlot::ITextEngine* textEngine)
		{
			ResolvedIR::LegendResolved resolved{};
			resolved.frame.fill = parseColor(legendSpec.background, "figure.legends[" + std::to_string(legendIndex) + "].background");
			resolved.frame.stroke = parseColor(legendSpec.borderColor, "figure.legends[" + std::to_string(legendIndex) + "].borderColor");
			resolved.frame.strokeWidth = legendSpec.borderWidth;

			std::vector<LegendElementLayoutCache> elementCaches;
			elementCaches.reserve(legendSpec.legendElements.size());
			for (std::size_t elementIdx = 0; elementIdx < legendSpec.legendElements.size(); ++elementIdx)
			{
				const FlowPlot::Spec::LegendElementSpec& elementSpec = legendSpec.legendElements[elementIdx];
				elementCaches.emplace_back(resolveLegendElementLayout(
					elementSpec,
					textEngine,
					"figure.legends[" + std::to_string(legendIndex) + "].legendElements[" + std::to_string(elementIdx) + "]"));
			}

			resolved.frame.rect = resolveLegendFrameRect(legendSpec, elementCaches, figureSpec, layoutSpec);

			resolved.iconBoxes.reserve(elementCaches.size());
			resolved.labels.reserve(elementCaches.size());

			float rowY = resolved.frame.rect.y + legendSpec.padding.top;
			for (std::size_t elementIdx = 0; elementIdx < legendSpec.legendElements.size(); ++elementIdx)
			{
				const FlowPlot::Spec::LegendElementSpec& elementSpec = legendSpec.legendElements[elementIdx];
				LegendElementLayoutCache cache = std::move(elementCaches[elementIdx]);

				const float defaultIconX = resolved.frame.rect.x + legendSpec.padding.left;
				const float defaultIconY = rowY;
				cache.icon.rect.x = defaultIconX;
				cache.icon.rect.y = defaultIconY;

				const float defaultTextX = defaultIconX + cache.icon.rect.w + (cache.icon.rect.w * 0.5f);
				const float defaultTextY = rowY;
				cache.label.box.x = elementSpec.box.x.value_or(defaultTextX);
				cache.label.box.y = elementSpec.box.y.value_or(defaultTextY);

				resolved.iconBoxes.push_back(std::move(cache.icon));
				resolved.labels.push_back(std::move(cache.label));

				rowY += cache.height + legendSpec.gap;
			}

			return resolved;
		}

		inline FlowPlot::RectF resolvePanelFrameRect(
			const FlowPlot::Spec::FigureSpec& figureSpec,
			const FlowPlot::Spec::LayoutSpec& layoutSpec,
			std::uint32_t panelRow,
			std::uint32_t panelCol)
		{
			if (layoutSpec.rows == 0 || layoutSpec.cols == 0)
				throw std::runtime_error("resolvePlotIR: layout rows/cols must be greater than zero");

			const float figureWidth = static_cast<float>(figureSpec.width);
			const float figureHeight = static_cast<float>(figureSpec.height);
			const float cols = static_cast<float>(layoutSpec.cols);
			const float rows = static_cast<float>(layoutSpec.rows);

			const float totalWidth =
				figureWidth
				- figureSpec.padding.left
				- figureSpec.padding.right
				- (layoutSpec.gap * (cols + 1.0f));
			const float totalHeight =
				figureHeight
				- figureSpec.padding.top
				- figureSpec.padding.bottom
				- (layoutSpec.gap * (rows + 1.0f));

			if (totalWidth < 0.0f || totalHeight < 0.0f)
				throw std::runtime_error("resolvePlotIR: figure size/padding/gap leaves negative panel layout space");

			const float panelWidth = totalWidth / cols;
			const float panelHeight = totalHeight / rows;
			const float x = figureSpec.padding.left + layoutSpec.gap + static_cast<float>(panelCol) * (panelWidth + layoutSpec.gap);
			const float y = figureSpec.padding.top + layoutSpec.gap + static_cast<float>(panelRow) * (panelHeight + layoutSpec.gap);
			return FlowPlot::RectF{x, y, panelWidth, panelHeight};
		}

		inline FlowPlot::RectF resolvePanelTitleBox(
			const FlowPlot::Spec::PanelSpec& panelSpec,
			const FlowPlot::RectF& panelFrame,
			const FlowPlot::Spec::TextSpec& titleSpec,
			const FlowPlot::ITextEngine* textEngine,
			const std::string& path)
		{
			const bool missingWidth = !titleSpec.box.width.has_value();
			const bool missingHeight = !titleSpec.box.height.has_value();

			FlowPlot::TextMeasurement measured{};
			if (missingWidth || missingHeight)
				measured = measureTextForAutoSizing(titleSpec, textEngine, path + ".box");

			const float width = titleSpec.box.width.value_or(measured.width);
			const float height = titleSpec.box.height.value_or(measured.height);
			if (width < 0.0f || height < 0.0f)
				throw std::runtime_error("resolvePlotIR: '" + path + ".box' width/height must be non-negative");

			const float x = titleSpec.box.x.value_or(panelFrame.x + (panelFrame.w - width) * 0.5f);
			const float y = titleSpec.box.y.value_or(panelFrame.y + panelSpec.padding.top);
			return FlowPlot::RectF{x, y, width, height};
		}

		inline ResolvedIR::ResolvedText resolvePanelTitle(
			const FlowPlot::Spec::PanelSpec& panelSpec,
			const FlowPlot::RectF& panelFrame,
			std::size_t panelIndex,
			const FlowPlot::ITextEngine* textEngine)
		{
			const FlowPlot::Spec::TextSpec& titleSpec = panelSpec.title;
			const std::string path = "panels[" + std::to_string(panelIndex) + "].title";

			ResolvedIR::ResolvedText resolved{};
			resolved.text = titleSpec.text;
			resolved.fontFamily = titleSpec.fontFamily;
			resolved.fontSize = titleSpec.fontSize;
			resolved.fontWeight = titleSpec.fontWeight;
			resolved.color = parseColor(titleSpec.color, path + ".color");
			resolved.hAlign = parseHorizontalAlign(titleSpec.hAlign, path + ".hAlign");
			resolved.vAlign = parseVerticalAlign(titleSpec.vAlign, path + ".vAlign");
			resolved.clipToBox = titleSpec.clip;
			resolved.box = resolvePanelTitleBox(panelSpec, panelFrame, titleSpec, textEngine, path);
			return resolved;
		}

		inline ResolvedIR::ResolvedLine makeResolvedLine(
			const FlowPlot::PointF& start,
			const FlowPlot::PointF& end,
			const FlowPlot::Color& color,
			float width)
		{
			ResolvedIR::ResolvedLine line{};
			line.color = color;
			line.width = width;
			line.points = {start, end};
			return line;
		}

		inline std::vector<float> computeMajorTickOffsets(
			float axisLength,
			float tickWidth,
			std::uint32_t tickCount)
		{
			std::vector<float> offsets;
			if (tickCount == 0U)
				return offsets;

			offsets.reserve(tickCount);
			const float halfTick = (tickWidth > 0.0f) ? (tickWidth * 0.5f) : 0.0f;
			float minOffset = halfTick;
			float maxOffset = axisLength - halfTick;
			if (maxOffset < minOffset)
			{
				const float center = axisLength * 0.5f;
				minOffset = center;
				maxOffset = center;
			}

			if (tickCount == 1U)
			{
				offsets.push_back((minOffset + maxOffset) * 0.5f);
				return offsets;
			}

			const float span = maxOffset - minOffset;
			const float step = span / static_cast<float>(tickCount - 1U);
			for (std::uint32_t i = 0; i < tickCount; ++i)
				offsets.push_back(minOffset + (step * static_cast<float>(i)));
			return offsets;
		}

		inline std::vector<float> computeMinorTickOffsets(
			const std::vector<float>& majorOffsets,
			std::uint32_t minorTickCount)
		{
			std::vector<float> offsets;
			if (minorTickCount == 0U || majorOffsets.size() < 2U)
				return offsets;

			offsets.reserve((majorOffsets.size() - 1U) * minorTickCount);
			for (std::size_t majorIdx = 0; majorIdx + 1U < majorOffsets.size(); ++majorIdx)
			{
				const float start = majorOffsets[majorIdx];
				const float end = majorOffsets[majorIdx + 1U];
				const float step = (end - start) / static_cast<float>(minorTickCount + 1U);
				for (std::uint32_t minorIdx = 1U; minorIdx <= minorTickCount; ++minorIdx)
					offsets.push_back(start + step * static_cast<float>(minorIdx));
			}
			return offsets;
		}

		template<typename T>
		inline bool computeNumericMinMaxForTypedView(
			const DataView& view,
			double& outMin,
			double& outMax,
			const std::string& path)
		{
			const std::span<const T> values = asSpan<T>(view);
			if (values.empty())
				return false;

			bool found = false;
			double minValue = 0.0;
			double maxValue = 0.0;
			for (const T raw : values)
			{
				const double value = static_cast<double>(raw);
				if constexpr (std::is_floating_point_v<T>)
				{
					if (!std::isfinite(value))
						throw std::runtime_error("resolvePlotIR: non-finite numeric value at '" + path + "'");
				}

				if (!found)
				{
					minValue = value;
					maxValue = value;
					found = true;
				}
				else
				{
					if (value < minValue)
						minValue = value;
					if (value > maxValue)
						maxValue = value;
				}
			}

			outMin = minValue;
			outMax = maxValue;
			return true;
		}

		inline bool computeNumericMinMax(
			const DataView& view,
			double& outMin,
			double& outMax,
			const std::string& path)
		{
			if (view.valueType != DataView::ValueType::Number)
			{
				throw std::runtime_error(
					"resolvePlotIR: expected numeric data for '" + path + "', got non-numeric type");
			}

			if (view.count > 0 && view.ptr == nullptr)
				throw std::runtime_error("resolvePlotIR: null numeric data buffer at '" + path + "'");
			if (view.count == 0)
				return false;

			const std::type_index type = view.type;
			if (type == std::type_index(typeid(float)))
				return computeNumericMinMaxForTypedView<float>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(double)))
				return computeNumericMinMaxForTypedView<double>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(long double)))
				return computeNumericMinMaxForTypedView<long double>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(std::int8_t)))
				return computeNumericMinMaxForTypedView<std::int8_t>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(std::uint8_t)))
				return computeNumericMinMaxForTypedView<std::uint8_t>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(std::int16_t)))
				return computeNumericMinMaxForTypedView<std::int16_t>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(std::uint16_t)))
				return computeNumericMinMaxForTypedView<std::uint16_t>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(std::int32_t)))
				return computeNumericMinMaxForTypedView<std::int32_t>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(std::uint32_t)))
				return computeNumericMinMaxForTypedView<std::uint32_t>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(std::int64_t)))
				return computeNumericMinMaxForTypedView<std::int64_t>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(std::uint64_t)))
				return computeNumericMinMaxForTypedView<std::uint64_t>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(signed char)))
				return computeNumericMinMaxForTypedView<signed char>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(unsigned char)))
				return computeNumericMinMaxForTypedView<unsigned char>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(char)))
				return computeNumericMinMaxForTypedView<char>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(short)))
				return computeNumericMinMaxForTypedView<short>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(unsigned short)))
				return computeNumericMinMaxForTypedView<unsigned short>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(int)))
				return computeNumericMinMaxForTypedView<int>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(unsigned int)))
				return computeNumericMinMaxForTypedView<unsigned int>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(long)))
				return computeNumericMinMaxForTypedView<long>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(unsigned long)))
				return computeNumericMinMaxForTypedView<unsigned long>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(long long)))
				return computeNumericMinMaxForTypedView<long long>(view, outMin, outMax, path);
			if (type == std::type_index(typeid(unsigned long long)))
				return computeNumericMinMaxForTypedView<unsigned long long>(view, outMin, outMax, path);

			throw std::runtime_error(
				"resolvePlotIR: unsupported numeric element type at '" + path + "'");
		}

		inline std::string formatTickValue(double value, const FlowPlot::Spec::AxisSpec& axisSpec, const std::string& path)
		{
			const std::string format = canonicalizeToken(axisSpec.tickLabelFormat);
			if (format.empty() || format == "auto")
			{
				std::ostringstream out;
				out << std::setprecision(6) << std::defaultfloat << value;
				std::string formatted = out.str();
				if (formatted == "-0")
					formatted = "0";
				return formatted;
			}

			throw std::runtime_error(
				"resolvePlotIR: unsupported tick label format '" + axisSpec.tickLabelFormat + "' at '" + path + ".tickLabelFormat'");
		}

			struct AxisFieldBinding
			{
				const DataView* view = nullptr;
				float domainPadding = 0.0f;
				std::string layerPath{};
			};

			struct AxisDomain
			{
				double min = 0.0;
				double max = 1.0;
				DataView::ValueType valueType = DataView::ValueType::Unknown;
				bool hasMappedData = false;
			};

			enum class AxisValueKind : std::uint8_t
			{
				None = 0,
				Data,
				Derived
			};

			struct HistogramPreparedData
			{
				struct Series
				{
					std::int32_t colorBin = -1;
					std::vector<double> binCounts{};
				};

				std::vector<double> binEdges{};
				std::vector<Series> series{};
				double mean = 0.0;
				double median = 0.0;
				bool hasSummaryStats = false;
				double maxCount = 0.0;
			};

			struct LayerAxisMarks
			{
				AxisValueKind xPrimary = AxisValueKind::None;
				AxisValueKind xSecondary = AxisValueKind::None;
				AxisValueKind yPrimary = AxisValueKind::None;
				AxisValueKind ySecondary = AxisValueKind::None;
			};

			struct AxisSlotPlan
			{
				const FlowPlot::Spec::AxisSpec* spec = nullptr;
				std::string path{};
				AxisValueKind kind = AxisValueKind::None;
				std::vector<AxisFieldBinding> dataBindings{};
				bool hasDerived = false;
				double derivedMax = 0.0;
				float derivedPadding = 0.0f;
			};

			struct PanelAxisComputation
			{
				AxisDomain xPrimary{};
				AxisDomain xSecondary{};
				AxisDomain yPrimary{};
				AxisDomain ySecondary{};
				std::unordered_map<std::size_t, HistogramPreparedData> histogramByLayerIndex{};
			};

			inline std::vector<double> extractNumericValues(
				const DataView& view,
				const std::string& path);

			inline std::vector<std::int32_t> assignBinsForView(
				const DataView& view,
				const std::vector<std::string>& categories,
				const std::string& path);

			inline std::size_t axisSlotIndex(bool isXAxis, FlowPlot::Spec::AxisDataRole role, const std::string& contextPath)
			{
				if (role == FlowPlot::Spec::AxisDataRole::Null)
				{
					throw std::runtime_error(
						"resolvePlotIR: axis role cannot be 'null' at '" + contextPath + "'");
				}

				if (isXAxis)
					return (role == FlowPlot::Spec::AxisDataRole::Primary) ? 0U : 1U;
				return (role == FlowPlot::Spec::AxisDataRole::Primary) ? 2U : 3U;
			}

			inline std::string axisSlotPath(std::size_t panelIndex, bool isXAxis, FlowPlot::Spec::AxisDataRole role)
			{
				const std::string panelPath = "panels[" + std::to_string(panelIndex) + "]";
				if (isXAxis)
				{
					return panelPath + ((role == FlowPlot::Spec::AxisDataRole::Primary) ? ".xAxis" : ".xSecondary");
				}
				return panelPath + ((role == FlowPlot::Spec::AxisDataRole::Primary) ? ".yAxis" : ".ySecondary");
			}

			inline LayerAxisMarks classifyLayerAxisMarks(
				const FlowPlot::Spec::LayerSpec& layerSpec,
				const BoundIR::BoundLayer& boundLayer,
				std::size_t panelIndex,
				std::size_t layerIndex)
			{
				LayerAxisMarks marks{};
				const std::string layerPath =
					"panels[" + std::to_string(panelIndex) + "].layers[" + std::to_string(layerIndex) + "]";

				auto assignMark = [&](bool isXAxis, FlowPlot::Spec::AxisDataRole role, AxisValueKind kind, const std::string& fieldPath)
				{
					const std::size_t slot = axisSlotIndex(isXAxis, role, fieldPath);
					AxisValueKind* target = nullptr;
					switch (slot)
					{
					case 0U:
						target = &marks.xPrimary;
						break;
					case 1U:
						target = &marks.xSecondary;
						break;
					case 2U:
						target = &marks.yPrimary;
						break;
					case 3U:
						target = &marks.ySecondary;
						break;
					default:
						throw std::runtime_error("resolvePlotIR: internal axis slot error");
					}

					if (*target == AxisValueKind::None)
					{
						*target = kind;
						return;
					}

					if (*target != kind)
					{
						throw std::runtime_error(
							"resolvePlotIR: layer '" + layerPath
							+ "' marks axis '" + axisSlotPath(panelIndex, isXAxis, role)
							+ "' as both data and derived");
					}
				};

				if (std::get_if<BoundIR::BoundScatterInputs>(&boundLayer.inputs) != nullptr)
				{
					assignMark(true, boundLayer.axisData.x, AxisValueKind::Data, layerPath + ".axisData.x");
					assignMark(false, boundLayer.axisData.y, AxisValueKind::Data, layerPath + ".axisData.y");
					return marks;
				}

				if (const auto* histogramInputs = std::get_if<BoundIR::BoundHistogramInputs>(&boundLayer.inputs))
				{
					if (histogramInputs->dataAxis == FlowPlot::Spec::HistogramDataAxis::X)
					{
						assignMark(true, boundLayer.axisData.x, AxisValueKind::Data, layerPath + ".axisData.x");
						assignMark(false, boundLayer.axisData.y, AxisValueKind::Derived, layerPath + ".axisData.y");
					}
					else
					{
						assignMark(false, boundLayer.axisData.y, AxisValueKind::Data, layerPath + ".axisData.y");
						assignMark(true, boundLayer.axisData.x, AxisValueKind::Derived, layerPath + ".axisData.x");
					}
					return marks;
				}

				(void)layerSpec;
				return marks;
			}

			inline HistogramPreparedData precomputeHistogramData(
				const FlowPlot::Spec::LayerSpec& layerSpec,
				const BoundIR::BoundHistogramInputs& inputs,
				const std::vector<BoundIR::BoundDataset>& datasets,
				std::size_t panelIndex,
				std::size_t layerIndex)
			{
				const std::string layerPath =
					"panels[" + std::to_string(panelIndex) + "].layers[" + std::to_string(layerIndex) + "]";
				if (inputs.dataset >= datasets.size())
					throw std::runtime_error("resolvePlotIR: histogram dataset index out of range at '" + layerPath + "'");

				const BoundIR::BoundDataset& dataset = datasets[inputs.dataset];
				if (inputs.data >= dataset.views.size())
					throw std::runtime_error("resolvePlotIR: histogram data field index out of range at '" + layerPath + "'");

				const DataView& dataView = dataset.views[inputs.data];
				if (dataView.valueType != DataView::ValueType::Number)
					throw std::runtime_error("resolvePlotIR: histogram data field must be numeric at '" + layerPath + ".mapping.data.field'");

				const std::uint32_t binCountRaw = layerSpec.histogramConfig.binCount;
				if (binCountRaw == 0U)
					throw std::runtime_error("resolvePlotIR: histogram binCount must be > 0 at '" + layerPath + ".config.binCount'");
				const std::size_t binCount = static_cast<std::size_t>(binCountRaw);

				const std::vector<double> values = extractNumericValues(dataView, layerPath + ".mapping.data.field");
				HistogramPreparedData prepared{};
				prepared.binEdges.assign(binCount + 1U, 0.0);
				if (values.empty())
					return prepared;

				{
					double sum = 0.0;
					for (const double value : values)
						sum += value;
					prepared.mean = sum / static_cast<double>(values.size());

					std::vector<double> sorted = values;
					std::sort(sorted.begin(), sorted.end());
					const std::size_t n = sorted.size();
					if ((n % 2U) == 1U)
					{
						prepared.median = sorted[n / 2U];
					}
					else
					{
						const double a = sorted[(n / 2U) - 1U];
						const double b = sorted[n / 2U];
						prepared.median = (a + b) * 0.5;
					}
					prepared.hasSummaryStats = true;
				}

				double minValue = 0.0;
				double maxValue = 0.0;
				if (!computeNumericMinMax(dataView, minValue, maxValue, layerPath + ".mapping.data.field"))
					return prepared;

				double lo = minValue;
				double hi = maxValue;
				if (hi == lo)
				{
					const double reference = std::max(std::abs(lo), 1.0);
					lo -= reference * 0.5;
					hi += reference * 0.5;
				}

				const double span = hi - lo;
				for (std::size_t i = 0; i <= binCount; ++i)
				{
					const double t = static_cast<double>(i) / static_cast<double>(binCount);
					prepared.binEdges[i] = lo + span * t;
				}

				std::vector<std::int32_t> rowColorBins{};
				std::size_t seriesCount = 1U;
				if (inputs.color.has_value())
				{
					if (*inputs.color >= dataset.views.size())
					{
						throw std::runtime_error(
							"resolvePlotIR: histogram color field index out of range at '" + layerPath + "'");
					}

					const std::string mode = canonicalizeToken(layerSpec.histogramMapping.colorMapping.type);
					if (!mode.empty() && mode != "bins")
					{
						throw std::runtime_error(
							"resolvePlotIR: histogram color mapping type must be 'bins' at '"
							+ layerPath + ".mapping.color.colorMapping-histogram.type'");
					}

					const auto& categories = layerSpec.histogramMapping.colorMapping.categories;
					const auto& mappedColors = layerSpec.histogramMapping.colorMapping.values;
					if (categories.size() > mappedColors.size())
					{
						throw std::runtime_error(
							"resolvePlotIR: histogram color bins categories exceed values at '"
							+ layerPath + ".mapping.color.colorMapping-histogram'");
					}

					rowColorBins = assignBinsForView(
						dataset.views[*inputs.color],
						categories,
						layerPath + ".mapping.color.colorMapping-histogram");
					if (rowColorBins.size() != values.size())
					{
						throw std::runtime_error(
							"resolvePlotIR: histogram color field size mismatch at '" + layerPath + "'");
					}
					seriesCount = categories.size();
				}

				prepared.series.reserve(seriesCount);
				for (std::size_t i = 0; i < seriesCount; ++i)
				{
					HistogramPreparedData::Series series{};
					series.colorBin = inputs.color.has_value()
						? static_cast<std::int32_t>(i)
						: -1;
					series.binCounts.assign(binCount, 0.0);
					prepared.series.push_back(std::move(series));
				}

				for (std::size_t row = 0; row < values.size(); ++row)
				{
					const double value = values[row];
					std::size_t bin = 0U;
					if (value <= lo)
					{
						bin = 0U;
					}
					else if (value >= hi)
					{
						bin = binCount - 1U;
					}
					else
					{
						const double ratio = (value - lo) / span;
						const double raw = std::floor(ratio * static_cast<double>(binCount));
						bin = static_cast<std::size_t>(raw);
						if (bin >= binCount)
							bin = binCount - 1U;
					}

					const std::size_t seriesIndex = inputs.color.has_value()
						? static_cast<std::size_t>(rowColorBins[row])
						: 0U;
					prepared.series[seriesIndex].binCounts[bin] += 1.0;
				}

				if (layerSpec.histogramConfig.normalize)
				{
					const double invTotal = 1.0 / static_cast<double>(values.size());
					for (HistogramPreparedData::Series& series : prepared.series)
					{
						for (double& count : series.binCounts)
							count *= invTotal;
					}
				}

				if (layerSpec.histogramConfig.cumulative)
				{
					for (HistogramPreparedData::Series& series : prepared.series)
					{
						double running = 0.0;
						for (double& count : series.binCounts)
						{
							running += count;
							count = running;
						}
					}
				}

				for (const HistogramPreparedData::Series& series : prepared.series)
				{
					for (const double count : series.binCounts)
						prepared.maxCount = std::max(prepared.maxCount, count);
				}
				return prepared;
			}

			inline AxisDomain resolveDataAxisDomain(
				const FlowPlot::Spec::AxisSpec& axisSpec,
				const std::vector<AxisFieldBinding>& bindings,
				const std::string& path)
			{
				const std::string scale = canonicalizeToken(axisSpec.scale);
				if (scale != "linear")
					throw std::runtime_error("resolvePlotIR: unsupported axis scale '" + axisSpec.scale + "' at '" + path + ".scale'");

				const bool hasExplicitMin = axisSpec.min.has_value();
				const bool hasExplicitMax = axisSpec.max.has_value();

				AxisDomain resolved{};
				if (hasExplicitMin)
					resolved.min = static_cast<double>(*axisSpec.min);
				if (hasExplicitMax)
					resolved.max = static_cast<double>(*axisSpec.max);

				std::optional<DataView::ValueType> axisType{};
				bool hasDataDomain = false;
				double dataMin = 0.0;
				double dataMax = 0.0;
				float minPadding = 0.0f;
				float maxPadding = 0.0f;

				for (const AxisFieldBinding& binding : bindings)
				{
					const DataView& view = *binding.view;
					if (!axisType.has_value())
					{
						axisType = view.valueType;
					}
					else if (*axisType != view.valueType)
					{
						throw std::runtime_error(
							"resolvePlotIR: mixed data categories on axis '" + path + "'");
					}

					if (view.valueType != DataView::ValueType::Number)
					{
						throw std::runtime_error(
							"resolvePlotIR: linear axis requires numeric data (at '" + binding.layerPath + "')");
					}

					double localMin = 0.0;
					double localMax = 0.0;
					if (!computeNumericMinMax(view, localMin, localMax, binding.layerPath + ".data"))
						continue;

					if (!hasDataDomain)
					{
						hasDataDomain = true;
						dataMin = localMin;
						dataMax = localMax;
						minPadding = binding.domainPadding;
						maxPadding = binding.domainPadding;
						continue;
					}

					if (localMin < dataMin)
					{
						dataMin = localMin;
						minPadding = binding.domainPadding;
					}
					else if (localMin == dataMin)
					{
						minPadding = std::max(minPadding, binding.domainPadding);
					}

					if (localMax > dataMax)
					{
						dataMax = localMax;
						maxPadding = binding.domainPadding;
					}
					else if (localMax == dataMax)
					{
						maxPadding = std::max(maxPadding, binding.domainPadding);
					}
				}

				resolved.valueType = axisType.value_or(DataView::ValueType::Unknown);
				resolved.hasMappedData = axisType.has_value();

				if (hasDataDomain)
				{
					double span = dataMax - dataMin;
					if (span == 0.0)
					{
						const double reference = std::max(std::abs(dataMin), 1.0);
						span = reference;
					}

					const double paddedMin = dataMin - (span * static_cast<double>(minPadding));
					const double paddedMax = dataMax + (span * static_cast<double>(maxPadding));
					if (!hasExplicitMin)
						resolved.min = paddedMin;
					if (!hasExplicitMax)
						resolved.max = paddedMax;
				}
				else
				{
					if (!hasExplicitMin && !hasExplicitMax)
					{
						resolved.min = 0.0;
						resolved.max = 1.0;
					}
					else if (hasExplicitMin && !hasExplicitMax)
					{
						resolved.max = resolved.min + 1.0;
					}
					else if (!hasExplicitMin && hasExplicitMax)
					{
						resolved.min = resolved.max - 1.0;
					}
				}

				if (resolved.max < resolved.min)
					throw std::runtime_error("resolvePlotIR: axis min must be <= max at '" + path + "'");
				return resolved;
			}

			inline AxisDomain resolveDerivedAxisDomain(
				const FlowPlot::Spec::AxisSpec& axisSpec,
				bool hasDerivedData,
				double derivedMax,
				float derivedPadding,
				const std::string& path)
			{
				const std::string scale = canonicalizeToken(axisSpec.scale);
				if (scale != "linear")
					throw std::runtime_error("resolvePlotIR: unsupported axis scale '" + axisSpec.scale + "' at '" + path + ".scale'");

				const bool hasExplicitMin = axisSpec.min.has_value();
				const bool hasExplicitMax = axisSpec.max.has_value();

				AxisDomain resolved{};
				resolved.valueType = DataView::ValueType::Number;
				resolved.hasMappedData = hasDerivedData;
				resolved.min = hasExplicitMin ? static_cast<double>(*axisSpec.min) : 0.0;

				if (hasExplicitMax)
				{
					resolved.max = static_cast<double>(*axisSpec.max);
				}
				else if (hasDerivedData)
				{
					double span = std::abs(derivedMax);
					if (span == 0.0)
						span = 1.0;
					resolved.max = derivedMax + (span * static_cast<double>(derivedPadding));
				}
				else if (hasExplicitMin)
				{
					resolved.max = resolved.min + 1.0;
				}
				else
				{
					resolved.max = 1.0;
				}

				if (resolved.max < resolved.min)
					throw std::runtime_error("resolvePlotIR: axis min must be <= max at '" + path + "'");
				return resolved;
			}

			inline PanelAxisComputation computePanelAxisComputation(
				const FlowPlot::Spec::PanelSpec& panelSpec,
				std::size_t panelIndex,
				const std::vector<const BoundIR::BoundLayer*>& orderedBoundLayers,
				const std::vector<BoundIR::BoundDataset>& datasets)
			{
				PanelAxisComputation computed{};

				std::array<AxisSlotPlan, 4> plans{};
				plans[0].spec = &panelSpec.xAxis;
				plans[0].path = "panels[" + std::to_string(panelIndex) + "].xAxis";
				plans[1].spec = &panelSpec.xSecondary;
				plans[1].path = "panels[" + std::to_string(panelIndex) + "].xSecondary";
				plans[2].spec = &panelSpec.yAxis;
				plans[2].path = "panels[" + std::to_string(panelIndex) + "].yAxis";
				plans[3].spec = &panelSpec.ySecondary;
				plans[3].path = "panels[" + std::to_string(panelIndex) + "].ySecondary";

				auto mergeAxisKind = [&](AxisSlotPlan& plan, AxisValueKind kind)
				{
					if (kind == AxisValueKind::None)
						return;
					if (plan.kind == AxisValueKind::None)
					{
						plan.kind = kind;
						return;
					}
					if (plan.kind != kind)
					{
						throw std::runtime_error(
							"resolvePlotIR: axis '" + plan.path + "' mixes data and derived layer mappings");
					}
				};

				for (const BoundIR::BoundLayer* boundLayer : orderedBoundLayers)
				{
					const std::size_t layerIdx = static_cast<std::size_t>(boundLayer->layerIndex);
					if (layerIdx >= panelSpec.layers.size())
						throw std::runtime_error("resolvePlotIR: bound layer index out of range at 'panels[" + std::to_string(panelIndex) + "]'");

					const FlowPlot::Spec::LayerSpec& layerSpec = panelSpec.layers[layerIdx];
					const std::string layerPath = "panels[" + std::to_string(panelIndex) + "].layers[" + std::to_string(layerIdx) + "]";

					const LayerAxisMarks marks = classifyLayerAxisMarks(layerSpec, *boundLayer, panelIndex, layerIdx);
					mergeAxisKind(plans[0], marks.xPrimary);
					mergeAxisKind(plans[1], marks.xSecondary);
					mergeAxisKind(plans[2], marks.yPrimary);
					mergeAxisKind(plans[3], marks.ySecondary);

					if (const auto* scatterInputs = std::get_if<BoundIR::BoundScatterInputs>(&boundLayer->inputs))
					{
						if (scatterInputs->dataset >= datasets.size())
							throw std::runtime_error("resolvePlotIR: scatter dataset index out of range at '" + layerPath + "'");

						const BoundIR::BoundDataset& dataset = datasets[scatterInputs->dataset];
						if (scatterInputs->x >= dataset.views.size() || scatterInputs->y >= dataset.views.size())
							throw std::runtime_error("resolvePlotIR: scatter field index out of range at '" + layerPath + "'");

						const std::size_t xSlot = axisSlotIndex(true, boundLayer->axisData.x, layerPath + ".axisData.x");
						const std::size_t ySlot = axisSlotIndex(false, boundLayer->axisData.y, layerPath + ".axisData.y");

						plans[xSlot].dataBindings.push_back(AxisFieldBinding{
							&dataset.views[scatterInputs->x],
							layerSpec.scatterConfig.domainPadding,
							layerPath});
						plans[ySlot].dataBindings.push_back(AxisFieldBinding{
							&dataset.views[scatterInputs->y],
							layerSpec.scatterConfig.domainPadding,
							layerPath});
						continue;
					}

					if (const auto* histogramInputs = std::get_if<BoundIR::BoundHistogramInputs>(&boundLayer->inputs))
					{
						if (histogramInputs->dataset >= datasets.size())
							throw std::runtime_error("resolvePlotIR: histogram dataset index out of range at '" + layerPath + "'");
						const BoundIR::BoundDataset& dataset = datasets[histogramInputs->dataset];
						if (histogramInputs->data >= dataset.views.size())
							throw std::runtime_error("resolvePlotIR: histogram data field index out of range at '" + layerPath + "'");

						const HistogramPreparedData prepared = precomputeHistogramData(
							layerSpec,
							*histogramInputs,
							datasets,
							panelIndex,
							layerIdx);
						computed.histogramByLayerIndex[layerIdx] = prepared;

						const bool dataOnX = (histogramInputs->dataAxis == FlowPlot::Spec::HistogramDataAxis::X);
						const std::size_t dataSlot = axisSlotIndex(
							dataOnX,
							dataOnX ? boundLayer->axisData.x : boundLayer->axisData.y,
							layerPath + (dataOnX ? ".axisData.x" : ".axisData.y"));
						const std::size_t derivedSlot = axisSlotIndex(
							!dataOnX,
							dataOnX ? boundLayer->axisData.y : boundLayer->axisData.x,
							layerPath + (dataOnX ? ".axisData.y" : ".axisData.x"));

						plans[dataSlot].dataBindings.push_back(AxisFieldBinding{
							&dataset.views[histogramInputs->data],
							layerSpec.histogramConfig.domainPadding,
							layerPath});

						plans[derivedSlot].hasDerived = true;
						if (prepared.maxCount > plans[derivedSlot].derivedMax)
						{
							plans[derivedSlot].derivedMax = prepared.maxCount;
							plans[derivedSlot].derivedPadding = layerSpec.histogramConfig.domainPadding;
						}
						else if (prepared.maxCount == plans[derivedSlot].derivedMax)
						{
							plans[derivedSlot].derivedPadding = std::max(
								plans[derivedSlot].derivedPadding,
								layerSpec.histogramConfig.domainPadding);
						}
					}
				}

				auto resolveSlot = [&](const AxisSlotPlan& plan) -> AxisDomain
				{
					if (plan.kind == AxisValueKind::Data)
						return resolveDataAxisDomain(*plan.spec, plan.dataBindings, plan.path);
					if (plan.kind == AxisValueKind::Derived)
					{
						return resolveDerivedAxisDomain(
							*plan.spec,
							plan.hasDerived,
							plan.derivedMax,
							plan.derivedPadding,
							plan.path);
					}
					return resolveDataAxisDomain(*plan.spec, {}, plan.path);
				};

				computed.xPrimary = resolveSlot(plans[0]);
				computed.xSecondary = resolveSlot(plans[1]);
				computed.yPrimary = resolveSlot(plans[2]);
				computed.ySecondary = resolveSlot(plans[3]);
				return computed;
			}

			inline ResolvedIR::AxisResolved resolveAxis(
				const FlowPlot::Spec::AxisSpec& axisSpec,
				bool isXAxis,
				bool isSecondaryAxis,
				const FlowPlot::RectF& layerRect,
				const AxisDomain& axisDomain,
				const FlowPlot::ITextEngine* textEngine,
				const std::string& path)
		{
			ResolvedIR::AxisResolved resolved{};
			if (!axisSpec.visible)
				return resolved;

			const std::string scale = canonicalizeToken(axisSpec.scale);
			if (scale != "linear")
				throw std::runtime_error("resolvePlotIR: unsupported axis scale '" + axisSpec.scale + "' at '" + path + ".scale'");

			if (axisSpec.lineWidth < 0.0f || axisSpec.tickWidth < 0.0f || axisSpec.tickLength < 0.0f || axisSpec.gridWidth < 0.0f)
				throw std::runtime_error("resolvePlotIR: negative axis widths/lengths are not allowed at '" + path + "'");

				const FlowPlot::PointF axisStart = isXAxis
					? FlowPlot::PointF{
						layerRect.x,
						isSecondaryAxis ? layerRect.y : (layerRect.y + layerRect.h)}
					: FlowPlot::PointF{
						isSecondaryAxis ? (layerRect.x + layerRect.w) : layerRect.x,
						layerRect.y + layerRect.h};
				const FlowPlot::PointF axisEnd = isXAxis
					? FlowPlot::PointF{layerRect.x + layerRect.w, axisStart.y}
					: FlowPlot::PointF{axisStart.x, layerRect.y};
				const float axisLength = isXAxis ? layerRect.w : layerRect.h;
				const float crossLength = isXAxis ? layerRect.h : layerRect.w;
				const float tickDirection = isXAxis
					? (isSecondaryAxis ? -1.0f : 1.0f)
					: (isSecondaryAxis ? 1.0f : -1.0f);
				const float gridDirection = -tickDirection;

			resolved.axisLine = makeResolvedLine(
				axisStart,
				axisEnd,
				parseColor(axisSpec.lineColor, path + ".lineColor"),
				axisSpec.lineWidth);

			const std::vector<float> majorOffsets = computeMajorTickOffsets(axisLength, axisSpec.tickWidth, axisSpec.tickCount);
			const std::vector<float> minorOffsets =
				(axisSpec.showMinorTicks && axisSpec.minorTickCount > 0U)
				? computeMinorTickOffsets(majorOffsets, axisSpec.minorTickCount)
				: std::vector<float>{};
			const double resolvedMin = axisDomain.min;
			const double resolvedMax = axisDomain.max;

			const FlowPlot::Color tickColor = parseColor(axisSpec.tickColor, path + ".tickColor");
			const FlowPlot::Color gridColor = parseColor(axisSpec.gridColor, path + ".gridColor");
			const FlowPlot::Color tickLabelColor = parseColor(axisSpec.tickLabelColor, path + ".tickLabelColor");

			resolved.tickLines.reserve(majorOffsets.size() + minorOffsets.size());
			resolved.gridLines.reserve(axisSpec.grid ? majorOffsets.size() : 0U);
			resolved.tickLabels.reserve(majorOffsets.size());

				auto appendTickLine = [&](float offset, float length)
				{
					FlowPlot::PointF tickStart{};
					FlowPlot::PointF tickEnd{};
					if (isXAxis)
					{
						tickStart = FlowPlot::PointF{axisStart.x + offset, axisStart.y};
						tickEnd = FlowPlot::PointF{tickStart.x, tickStart.y + length * tickDirection};
					}
					else
					{
						tickStart = FlowPlot::PointF{axisStart.x, axisStart.y - offset};
						tickEnd = FlowPlot::PointF{tickStart.x + length * tickDirection, tickStart.y};
					}
					resolved.tickLines.push_back(makeResolvedLine(tickStart, tickEnd, tickColor, axisSpec.tickWidth));
				};

			for (float offset : majorOffsets)
			{
				appendTickLine(offset, axisSpec.tickLength);

				if (!axisSpec.grid)
					continue;

				FlowPlot::PointF gridStart{};
				FlowPlot::PointF gridEnd{};
					if (isXAxis)
					{
						gridStart = FlowPlot::PointF{axisStart.x + offset, axisStart.y};
						gridEnd = FlowPlot::PointF{gridStart.x, gridStart.y + crossLength * gridDirection};
					}
					else
					{
						gridStart = FlowPlot::PointF{axisStart.x, axisStart.y - offset};
						gridEnd = FlowPlot::PointF{gridStart.x + crossLength * gridDirection, gridStart.y};
					}

				resolved.gridLines.push_back(makeResolvedLine(gridStart, gridEnd, gridColor, axisSpec.gridWidth));
			}

			const float minorLength = axisSpec.tickLength * 0.5f;
			for (float offset : minorOffsets)
				appendTickLine(offset, minorLength);

				float largestTickLabelHeight = 0.0f;
				float largestTickLabelWidth = 0.0f;
			for (std::size_t majorIdx = 0; majorIdx < majorOffsets.size(); ++majorIdx)
			{
				double t = 0.0;
				if (majorOffsets.size() == 1U)
					t = 0.5;
				else if (majorOffsets.size() > 1U)
					t = static_cast<double>(majorIdx) / static_cast<double>(majorOffsets.size() - 1U);

				const double tickValue = resolvedMin + (resolvedMax - resolvedMin) * t;
				const std::string tickText = formatTickValue(tickValue, axisSpec, path);

				const FlowPlot::TextMeasurement measured = measureTextForAutoSizing(
					textEngine,
					axisSpec.tickLabelFontFamily,
					axisSpec.tickLabelFontWeight,
					axisSpec.tickLabelFontSize,
					tickText,
					path + ".tickLabels[" + std::to_string(majorIdx) + "]");
				if (measured.width < 0.0f || measured.height < 0.0f)
				{
					throw std::runtime_error("resolvePlotIR: measured tick label size is negative at '" + path + "'");
					}
					largestTickLabelHeight = std::max(largestTickLabelHeight, measured.height);
					largestTickLabelWidth = std::max(largestTickLabelWidth, measured.width);

				ResolvedIR::ResolvedText label{};
				label.text = tickText;
				label.fontFamily = axisSpec.tickLabelFontFamily;
				label.fontSize = axisSpec.tickLabelFontSize;
				label.fontWeight = axisSpec.tickLabelFontWeight;
				label.color = tickLabelColor;
				label.hAlign = FlowPlot::HorizontalAlign::Left;
				label.vAlign = FlowPlot::VerticalAlign::Top;
				label.clipToBox = true;
				label.box.w = measured.width;
				label.box.h = measured.height;

				const float offset = majorOffsets[majorIdx];
					if (isXAxis)
					{
						const float tickCenterX = axisStart.x + offset;
						label.box.x = tickCenterX - (measured.width * 0.5f);
						const float anchorY = axisStart.y + (axisSpec.tickLength + axisSpec.tickValueGap) * tickDirection;
						label.box.y = (tickDirection > 0.0f) ? anchorY : (anchorY - measured.height);
					}
					else
					{
						const float tickCenterY = axisStart.y - offset;
						const float anchorX = axisStart.x + (axisSpec.tickLength + axisSpec.tickValueGap) * tickDirection;
						label.box.x = (tickDirection > 0.0f) ? anchorX : (anchorX - measured.width);
						label.box.y = tickCenterY - (measured.height * 0.5f);
					}

				resolved.tickLabels.push_back(std::move(label));
			}

			if (axisSpec.title.visible)
			{
				const FlowPlot::Spec::TextSpec& titleSpec = axisSpec.title;

				const bool missingWidth = !titleSpec.box.width.has_value();
				const bool missingHeight = !titleSpec.box.height.has_value();
				FlowPlot::TextMeasurement measuredTitle{};
				if (missingWidth || missingHeight)
				{
					measuredTitle = measureTextForAutoSizing(
						textEngine,
						titleSpec.fontFamily,
						titleSpec.fontWeight,
						titleSpec.fontSize,
						titleSpec.text,
						path + ".title.box");
				}

				const float titleWidth = titleSpec.box.width.value_or(measuredTitle.width);
				const float titleHeight = titleSpec.box.height.value_or(measuredTitle.height);
				if (titleWidth < 0.0f || titleHeight < 0.0f)
					throw std::runtime_error("resolvePlotIR: axis title box width/height must be non-negative at '" + path + ".title.box'");

					const float defaultX = isXAxis
						? ((axisStart.x + (axisLength * 0.5f)) - (titleWidth * 0.5f))
						: ([&]()
						{
							const float anchorX = axisStart.x + (axisSpec.lineWidth * 0.5f + axisSpec.tickLength + axisSpec.tickValueGap + largestTickLabelWidth) * tickDirection;
							return (tickDirection > 0.0f) ? anchorX : (anchorX - titleWidth);
						})();
					const float defaultY = isXAxis
						? ([&]()
						{
							const float anchorY = axisStart.y + (axisSpec.lineWidth * 0.5f + axisSpec.tickLength + axisSpec.tickValueGap + largestTickLabelHeight) * tickDirection;
							return (tickDirection > 0.0f) ? anchorY : (anchorY - titleHeight);
						})()
						: (axisEnd.y - titleHeight);

				ResolvedIR::ResolvedText title{};
				title.text = titleSpec.text;
				title.fontFamily = titleSpec.fontFamily;
				title.fontSize = titleSpec.fontSize;
				title.fontWeight = titleSpec.fontWeight;
				title.color = parseColor(titleSpec.color, path + ".title.color");
				title.hAlign = parseHorizontalAlign(titleSpec.hAlign, path + ".title.hAlign");
				title.vAlign = parseVerticalAlign(titleSpec.vAlign, path + ".title.vAlign");
				title.clipToBox = titleSpec.clip;
				title.box.w = titleWidth;
				title.box.h = titleHeight;
				title.box.x = titleSpec.box.x.value_or(defaultX);
				title.box.y = titleSpec.box.y.value_or(defaultY);
				resolved.title = std::move(title);
			}

			return resolved;
		}

		template<typename T>
		inline std::vector<double> extractNumericValuesForTyped(
			const DataView& view,
			const std::string& path)
		{
			const std::span<const T> values = asSpan<T>(view);
			std::vector<double> out;
			out.reserve(values.size());
			for (const T raw : values)
			{
				const double value = static_cast<double>(raw);
				if constexpr (std::is_floating_point_v<T>)
				{
					if (!std::isfinite(value))
						throw std::runtime_error("resolvePlotIR: non-finite numeric value at '" + path + "'");
				}
				out.push_back(value);
			}
			return out;
		}

		inline std::vector<double> extractNumericValues(
			const DataView& view,
			const std::string& path)
		{
			if (view.valueType != DataView::ValueType::Number)
				throw std::runtime_error("resolvePlotIR: expected numeric field at '" + path + "'");

			const std::type_index type = view.type;
			if (type == std::type_index(typeid(float)))
				return extractNumericValuesForTyped<float>(view, path);
			if (type == std::type_index(typeid(double)))
				return extractNumericValuesForTyped<double>(view, path);
			if (type == std::type_index(typeid(long double)))
				return extractNumericValuesForTyped<long double>(view, path);
			if (type == std::type_index(typeid(std::int8_t)))
				return extractNumericValuesForTyped<std::int8_t>(view, path);
			if (type == std::type_index(typeid(std::uint8_t)))
				return extractNumericValuesForTyped<std::uint8_t>(view, path);
			if (type == std::type_index(typeid(std::int16_t)))
				return extractNumericValuesForTyped<std::int16_t>(view, path);
			if (type == std::type_index(typeid(std::uint16_t)))
				return extractNumericValuesForTyped<std::uint16_t>(view, path);
			if (type == std::type_index(typeid(std::int32_t)))
				return extractNumericValuesForTyped<std::int32_t>(view, path);
			if (type == std::type_index(typeid(std::uint32_t)))
				return extractNumericValuesForTyped<std::uint32_t>(view, path);
			if (type == std::type_index(typeid(std::int64_t)))
				return extractNumericValuesForTyped<std::int64_t>(view, path);
			if (type == std::type_index(typeid(std::uint64_t)))
				return extractNumericValuesForTyped<std::uint64_t>(view, path);
			if (type == std::type_index(typeid(signed char)))
				return extractNumericValuesForTyped<signed char>(view, path);
			if (type == std::type_index(typeid(unsigned char)))
				return extractNumericValuesForTyped<unsigned char>(view, path);
			if (type == std::type_index(typeid(char)))
				return extractNumericValuesForTyped<char>(view, path);
			if (type == std::type_index(typeid(short)))
				return extractNumericValuesForTyped<short>(view, path);
			if (type == std::type_index(typeid(unsigned short)))
				return extractNumericValuesForTyped<unsigned short>(view, path);
			if (type == std::type_index(typeid(int)))
				return extractNumericValuesForTyped<int>(view, path);
			if (type == std::type_index(typeid(unsigned int)))
				return extractNumericValuesForTyped<unsigned int>(view, path);
			if (type == std::type_index(typeid(long)))
				return extractNumericValuesForTyped<long>(view, path);
			if (type == std::type_index(typeid(unsigned long)))
				return extractNumericValuesForTyped<unsigned long>(view, path);
			if (type == std::type_index(typeid(long long)))
				return extractNumericValuesForTyped<long long>(view, path);
			if (type == std::type_index(typeid(unsigned long long)))
				return extractNumericValuesForTyped<unsigned long long>(view, path);

			throw std::runtime_error("resolvePlotIR: unsupported numeric field type at '" + path + "'");
		}

		template<typename TString>
		inline std::vector<std::string> extractStringValuesForTyped(
			const DataView& view,
			const std::string& path)
		{
			const std::span<const TString> values = asSpan<TString>(view);
			std::vector<std::string> out;
			out.reserve(values.size());
			for (const auto& raw : values)
			{
				if constexpr (std::is_same_v<TString, std::string>)
				{
					out.push_back(raw);
				}
				else if constexpr (std::is_same_v<TString, std::string_view>)
				{
					out.push_back(std::string(raw));
				}
				else if constexpr (std::is_same_v<TString, const char*> || std::is_same_v<TString, char*>)
				{
					if (raw == nullptr)
						throw std::runtime_error("resolvePlotIR: null C-string encountered at '" + path + "'");
					out.push_back(std::string(raw));
				}
				else
				{
					static_assert(std::is_same_v<TString, void>, "Unsupported string storage type");
				}
			}
			return out;
		}

		inline std::vector<std::string> extractStringValues(
			const DataView& view,
			const std::string& path)
		{
			if (view.valueType != DataView::ValueType::String)
				throw std::runtime_error("resolvePlotIR: expected string field at '" + path + "'");

			const std::type_index type = view.type;
			if (type == std::type_index(typeid(std::string)))
				return extractStringValuesForTyped<std::string>(view, path);
			if (type == std::type_index(typeid(std::string_view)))
				return extractStringValuesForTyped<std::string_view>(view, path);
			if (type == std::type_index(typeid(const char*)))
				return extractStringValuesForTyped<const char*>(view, path);
			if (type == std::type_index(typeid(char*)))
				return extractStringValuesForTyped<char*>(view, path);

			throw std::runtime_error("resolvePlotIR: unsupported string field type at '" + path + "'");
		}

		inline std::vector<std::uint8_t> extractBooleanValues(
			const DataView& view,
			const std::string& path)
		{
			if (view.valueType != DataView::ValueType::Boolean)
				throw std::runtime_error("resolvePlotIR: expected boolean field at '" + path + "'");
			if (view.type != std::type_index(typeid(bool)))
				throw std::runtime_error("resolvePlotIR: unsupported boolean field type at '" + path + "'");

			const std::span<const bool> values = asSpan<bool>(view);
			std::vector<std::uint8_t> out;
			out.reserve(values.size());
			for (const bool v : values)
				out.push_back(v ? 1U : 0U);
			return out;
		}

		inline bool tryParseFiniteDoubleToken(std::string_view token, double& out)
		{
			std::string trimmed = canonicalizeToken(token);
			if (trimmed.empty())
				return false;

			char* endPtr = nullptr;
			const double parsed = std::strtod(trimmed.c_str(), &endPtr);
			if (endPtr == nullptr || *endPtr != '\0')
				return false;
			if (!std::isfinite(parsed))
				return false;

			out = parsed;
			return true;
		}

		struct NumericBinRule
		{
			bool isRange = false;
			double lo = 0.0;
			double hi = 0.0;
		};

		inline NumericBinRule parseNumericBinRule(
			std::string_view categoryToken,
			const std::string& path)
		{
			double exact = 0.0;
			if (tryParseFiniteDoubleToken(categoryToken, exact))
				return NumericBinRule{false, exact, exact};

			const std::string token = canonicalizeToken(categoryToken);
			for (std::size_t i = 1; i + 1U < token.size(); ++i)
			{
				if (token[i] != '-')
					continue;

				double lo = 0.0;
				double hi = 0.0;
				if (!tryParseFiniteDoubleToken(std::string_view(token).substr(0, i), lo))
					continue;
				if (!tryParseFiniteDoubleToken(std::string_view(token).substr(i + 1U), hi))
					continue;

				if (hi < lo)
					std::swap(lo, hi);
				return NumericBinRule{true, lo, hi};
			}

			throw std::runtime_error("resolvePlotIR: invalid numeric bin category '" + std::string(categoryToken) + "' at '" + path + "'");
		}

		inline bool parseBooleanCategory(std::string_view token, const std::string& path)
		{
			const std::string canonical = canonicalizeToken(token);
			if (canonical == "true" || canonical == "1")
				return true;
			if (canonical == "false" || canonical == "0")
				return false;
			throw std::runtime_error("resolvePlotIR: invalid boolean bin category '" + std::string(token) + "' at '" + path + "'");
		}

		inline std::vector<std::int32_t> assignBinsForView(
			const DataView& view,
			const std::vector<std::string>& categories,
			const std::string& path)
		{
			if (categories.empty())
				throw std::runtime_error("resolvePlotIR: bin mapping categories cannot be empty at '" + path + "'");

			std::vector<std::int32_t> rowBins;
			if (view.valueType == DataView::ValueType::Number)
			{
				const std::vector<double> numericValues = extractNumericValues(view, path);
				std::vector<NumericBinRule> rules;
				rules.reserve(categories.size());
				for (std::size_t i = 0; i < categories.size(); ++i)
				{
					rules.push_back(parseNumericBinRule(
						categories[i],
						path + ".categories[" + std::to_string(i) + "]"));
				}

				rowBins.reserve(numericValues.size());
				for (std::size_t row = 0; row < numericValues.size(); ++row)
				{
					const double value = numericValues[row];
					bool matched = false;
					for (std::size_t catIdx = 0; catIdx < rules.size(); ++catIdx)
					{
						const NumericBinRule& rule = rules[catIdx];
						if (rule.isRange)
						{
							if (value >= rule.lo && value <= rule.hi)
							{
								rowBins.push_back(static_cast<std::int32_t>(catIdx));
								matched = true;
								break;
							}
						}
						else
						{
							const double epsilon = 1e-9 * std::max(1.0, std::max(std::abs(value), std::abs(rule.lo)));
							if (std::abs(value - rule.lo) <= epsilon)
							{
								rowBins.push_back(static_cast<std::int32_t>(catIdx));
								matched = true;
								break;
							}
						}
					}
					if (!matched)
					{
						throw std::runtime_error(
							"resolvePlotIR: value at row " + std::to_string(row)
							+ " does not match any numeric bin category at '" + path + "'");
					}
				}
				return rowBins;
			}

			if (view.valueType == DataView::ValueType::String)
			{
				const std::vector<std::string> stringValues = extractStringValues(view, path);
				rowBins.reserve(stringValues.size());
				for (std::size_t row = 0; row < stringValues.size(); ++row)
				{
					const std::string& value = stringValues[row];
					bool matched = false;
					for (std::size_t catIdx = 0; catIdx < categories.size(); ++catIdx)
					{
						if (value == categories[catIdx])
						{
							rowBins.push_back(static_cast<std::int32_t>(catIdx));
							matched = true;
							break;
						}
					}
					if (!matched)
					{
						throw std::runtime_error(
							"resolvePlotIR: value '" + value
							+ "' at row " + std::to_string(row)
							+ " does not match any string bin category at '" + path + "'");
					}
				}
				return rowBins;
			}

			if (view.valueType == DataView::ValueType::Boolean)
			{
				std::vector<std::uint8_t> boolValues = extractBooleanValues(view, path);
				std::vector<std::uint8_t> categoryValues;
				categoryValues.reserve(categories.size());
				for (std::size_t i = 0; i < categories.size(); ++i)
				{
					categoryValues.push_back(parseBooleanCategory(
						categories[i],
						path + ".categories[" + std::to_string(i) + "]")
						? 1U
						: 0U);
				}

				rowBins.reserve(boolValues.size());
				for (std::size_t row = 0; row < boolValues.size(); ++row)
				{
					const std::uint8_t value = boolValues[row];
					bool matched = false;
					for (std::size_t catIdx = 0; catIdx < categoryValues.size(); ++catIdx)
					{
						if (value == categoryValues[catIdx])
						{
							rowBins.push_back(static_cast<std::int32_t>(catIdx));
							matched = true;
							break;
						}
					}
					if (!matched)
					{
						throw std::runtime_error(
							"resolvePlotIR: boolean value at row " + std::to_string(row)
							+ " does not match any boolean bin category at '" + path + "'");
					}
				}
				return rowBins;
			}

			throw std::runtime_error("resolvePlotIR: unsupported data category for bin mapping at '" + path + "'");
		}

		inline FlowPlot::Color lerpColor(
			const FlowPlot::Color& lo,
			const FlowPlot::Color& hi,
			double t) noexcept
		{
			if (t < 0.0)
				t = 0.0;
			else if (t > 1.0)
				t = 1.0;

			auto lerpChannel = [t](std::uint8_t a, std::uint8_t b) -> std::uint8_t
			{
				const double value = static_cast<double>(a) + (static_cast<double>(b) - static_cast<double>(a)) * t;
				const int rounded = static_cast<int>(std::lround(value));
				if (rounded < 0)
					return 0;
				if (rounded > 255)
					return 255;
				return static_cast<std::uint8_t>(rounded);
			};

			FlowPlot::Color color{};
			color.r = lerpChannel(lo.r, hi.r);
			color.g = lerpChannel(lo.g, hi.g);
			color.b = lerpChannel(lo.b, hi.b);
			color.a = lerpChannel(lo.a, hi.a);
			return color;
		}

		inline FlowPlot::MarkerShape parseMarkerShape(std::string_view rawShape, const std::string& path)
		{
			const std::string shape = canonicalizeToken(rawShape);
			if (shape == "circle")
				return FlowPlot::MarkerShape::Circle;
			if (shape == "square")
				return FlowPlot::MarkerShape::Square;
			if (shape == "diamond")
				return FlowPlot::MarkerShape::Diamond;
			if (shape == "triangle")
				return FlowPlot::MarkerShape::Triangle;
			throw std::runtime_error("resolvePlotIR: unsupported marker shape '" + std::string(rawShape) + "' at '" + path + "'");
		}

		inline float deterministicJitterOffset(std::size_t rowIndex, std::uint32_t seed, float magnitude) noexcept
		{
			if (magnitude == 0.0f)
				return 0.0f;

			std::uint32_t x = static_cast<std::uint32_t>(rowIndex);
			x ^= seed + 0x9e3779b9U + (x << 6U) + (x >> 2U);
			x ^= x >> 16U;
			x *= 0x7feb352dU;
			x ^= x >> 15U;
			x *= 0x846ca68bU;
			x ^= x >> 16U;

			const float u = static_cast<float>(x & 0x00FFFFFFU) / 16777215.0f;
			return (u - 0.5f) * magnitude;
		}

		inline float mapDataValueToAxisPixel(
			double value,
			const AxisDomain& domain,
			float axisStart,
			float axisLength,
			bool invert)
		{
			const double span = domain.max - domain.min;
			double normalized = 0.5;
			if (span != 0.0)
				normalized = (value - domain.min) / span;
			if (invert)
				normalized = 1.0 - normalized;

			return axisStart + static_cast<float>(normalized * static_cast<double>(axisLength));
		}

		enum class ScatterFieldMode : std::uint8_t
		{
			None,
			Bins,
			Contiguous
		};

		struct ScatterBatchKey
		{
			std::int32_t colorBin = -1;
			std::int32_t sizeBin = -1;
			std::int32_t labelBin = -1;

			bool operator==(const ScatterBatchKey& other) const noexcept
			{
				return colorBin == other.colorBin
					&& sizeBin == other.sizeBin
					&& labelBin == other.labelBin;
			}
		};

		struct ScatterBatchKeyHash
		{
			std::size_t operator()(const ScatterBatchKey& key) const noexcept
			{
				const std::size_t a = static_cast<std::size_t>(std::hash<std::int32_t>{}(key.colorBin));
				const std::size_t b = static_cast<std::size_t>(std::hash<std::int32_t>{}(key.sizeBin));
				const std::size_t c = static_cast<std::size_t>(std::hash<std::int32_t>{}(key.labelBin));
				return a ^ (b + 0x9e3779b97f4a7c15ULL + (a << 6U) + (a >> 2U))
					^ (c + 0x9e3779b97f4a7c15ULL + (b << 6U) + (b >> 2U));
			}
		};

		inline ResolvedIR::LayerResolved resolveScatterLayer(
			const FlowPlot::Spec::LayerSpec& layerSpec,
			std::size_t panelIndex,
			std::size_t layerIndex,
			const BoundIR::BoundLayer& boundLayer,
			const BoundIR::BoundScatterInputs& inputs,
			const std::vector<BoundIR::BoundDataset>& datasets,
			const AxisDomain& xDomain,
			const AxisDomain& yDomain,
			const FlowPlot::RectF& layerRect,
			const FlowPlot::Spec::AxisSpec& xAxisSpec,
			const FlowPlot::Spec::AxisSpec& yAxisSpec)
		{
			if (inputs.dataset >= datasets.size())
				throw std::runtime_error("resolvePlotIR: scatter dataset index out of range at panels["+ std::to_string(panelIndex) + "].layers[" + std::to_string(layerIndex) + "]");

			const BoundIR::BoundDataset& dataset = datasets[inputs.dataset];
			const std::size_t rowCount = static_cast<std::size_t>(dataset.rowCount);
			if (rowCount == 0U)
			{
				ResolvedIR::LayerResolved empty{};
				empty.zIndex = boundLayer.zIndex;
				empty.opacity = boundLayer.opacity;
				empty.payload = ResolvedIR::ScatterResolved{};
				return empty;
			}

			if (inputs.x >= dataset.views.size() || inputs.y >= dataset.views.size())
				throw std::runtime_error("resolvePlotIR: scatter x/y field index out of range at panels["+ std::to_string(panelIndex) + "].layers[" + std::to_string(layerIndex) + "]");

			const std::string layerPath = "panels[" + std::to_string(panelIndex) + "].layers[" + std::to_string(layerIndex) + "]";
			const DataView& xView = dataset.views[inputs.x];
			const DataView& yView = dataset.views[inputs.y];
			if (xView.valueType != DataView::ValueType::Number || yView.valueType != DataView::ValueType::Number)
				throw std::runtime_error("resolvePlotIR: scatter x/y fields must be numeric at '" + layerPath + "'");

			const std::vector<double> xValues = extractNumericValues(xView, layerPath + ".mapping.x.field");
			const std::vector<double> yValues = extractNumericValues(yView, layerPath + ".mapping.y.field");
			if (xValues.size() != rowCount || yValues.size() != rowCount)
				throw std::runtime_error("resolvePlotIR: scatter x/y field size mismatch at '" + layerPath + "'");

			const FlowPlot::MarkerShape baseShape = parseMarkerShape(layerSpec.scatterStyle.markerShape, layerPath + ".style.markerShape");
			const FlowPlot::Color baseFill = parseColor(layerSpec.scatterStyle.fillColor, layerPath + ".style.fillColor");
			const FlowPlot::Color baseStroke = parseColor(layerSpec.scatterStyle.strokeColor, layerPath + ".style.strokeColor");
			const float baseSize = layerSpec.scatterStyle.markerSize;
			if (baseSize < 0.0f)
				throw std::runtime_error("resolvePlotIR: scatter marker size must be non-negative at '" + layerPath + ".style.markerSize'");

			ScatterFieldMode colorMode = ScatterFieldMode::None;
			ScatterFieldMode sizeMode = ScatterFieldMode::None;
			bool labelBinned = false;

			std::vector<FlowPlot::Color> rowColors{};
			std::vector<float> rowSizes{};
			std::vector<std::int32_t> rowColorBins{};
			std::vector<std::int32_t> rowSizeBins{};
			std::vector<std::int32_t> rowLabelBins{};
			std::vector<FlowPlot::Color> colorBinValues{};
			std::vector<float> sizeBinValues{};
			std::vector<FlowPlot::MarkerShape> labelBinShapes{};

			if (inputs.color.has_value())
			{
				if (*inputs.color >= dataset.views.size())
					throw std::runtime_error("resolvePlotIR: scatter color field index out of range at '" + layerPath + "'");

				const DataView& colorView = dataset.views[*inputs.color];
				const std::string mode = canonicalizeToken(layerSpec.scatterMapping.colorMapping.type);
				if (mode == "bins" || mode.empty())
				{
					colorMode = ScatterFieldMode::Bins;
					const auto& categories = layerSpec.scatterMapping.colorMapping.categories;
					const auto& values = layerSpec.scatterMapping.colorMapping.values;
					if (categories.size() > values.size())
						throw std::runtime_error("resolvePlotIR: color bins categories exceed values at '" + layerPath + ".mapping.color.colorMapping-scatter'");

					rowColorBins = assignBinsForView(
						colorView,
						categories,
						layerPath + ".mapping.color.colorMapping-scatter");
					colorBinValues.reserve(categories.size());
					for (std::size_t i = 0; i < categories.size(); ++i)
					{
						colorBinValues.push_back(parseColor(
							values[i],
							layerPath + ".mapping.color.colorMapping-scatter.values[" + std::to_string(i) + "]"));
					}
				}
				else if (mode == "contiguous")
				{
					if (colorView.valueType != DataView::ValueType::Number)
					{
						throw std::runtime_error(
							"resolvePlotIR: contiguous color mapping requires numeric data at '" + layerPath + ".mapping.color.field'");
					}

					colorMode = ScatterFieldMode::Contiguous;
					const std::vector<double> values = extractNumericValues(
						colorView,
						layerPath + ".mapping.color.field");
					if (values.size() != rowCount)
						throw std::runtime_error("resolvePlotIR: contiguous color field size mismatch at '" + layerPath + "'");

					double minValue = 0.0;
					double maxValue = 0.0;
					if (!computeNumericMinMax(colorView, minValue, maxValue, layerPath + ".mapping.color.field"))
						throw std::runtime_error("resolvePlotIR: empty color field for contiguous mapping at '" + layerPath + "'");
					const double span = maxValue - minValue;

					const FlowPlot::Color lo = parseColor(
						layerSpec.scatterMapping.colorMapping.minValue,
						layerPath + ".mapping.color.colorMapping-scatter.minValue");
					const FlowPlot::Color hi = parseColor(
						layerSpec.scatterMapping.colorMapping.maxValue,
						layerPath + ".mapping.color.colorMapping-scatter.maxValue");

					rowColors.reserve(values.size());
					for (const double value : values)
					{
						double t = 0.0;
						if (span != 0.0)
							t = (value - minValue) / span;
						rowColors.push_back(lerpColor(lo, hi, t));
					}
				}
				else
				{
					throw std::runtime_error(
						"resolvePlotIR: unsupported scatter color mapping type '" + layerSpec.scatterMapping.colorMapping.type
						+ "' at '" + layerPath + ".mapping.color.colorMapping-scatter.type'");
				}
			}

			if (inputs.size.has_value())
			{
				if (*inputs.size >= dataset.views.size())
					throw std::runtime_error("resolvePlotIR: scatter size field index out of range at '" + layerPath + "'");

				const DataView& sizeView = dataset.views[*inputs.size];
				const std::string mode = canonicalizeToken(layerSpec.scatterMapping.sizeMapping.type);
				if (mode == "bins" || mode.empty())
				{
					sizeMode = ScatterFieldMode::Bins;
					const auto& categories = layerSpec.scatterMapping.sizeMapping.categories;
					const auto& values = layerSpec.scatterMapping.sizeMapping.values;
					if (categories.size() > values.size())
					{
						throw std::runtime_error(
							"resolvePlotIR: size bins categories exceed values at '" + layerPath + ".mapping.size.sizeMapping-scatter'");
					}

					rowSizeBins = assignBinsForView(
						sizeView,
						categories,
						layerPath + ".mapping.size.sizeMapping-scatter");
					sizeBinValues.reserve(categories.size());
					for (std::size_t i = 0; i < categories.size(); ++i)
						sizeBinValues.push_back(values[i]);
				}
				else if (mode == "contiguous")
				{
					if (sizeView.valueType != DataView::ValueType::Number)
					{
						throw std::runtime_error(
							"resolvePlotIR: contiguous size mapping requires numeric data at '" + layerPath + ".mapping.size.field'");
					}

					sizeMode = ScatterFieldMode::Contiguous;
					const std::vector<double> values = extractNumericValues(
						sizeView,
						layerPath + ".mapping.size.field");
					if (values.size() != rowCount)
						throw std::runtime_error("resolvePlotIR: contiguous size field size mismatch at '" + layerPath + "'");

					double minValue = 0.0;
					double maxValue = 0.0;
					if (!computeNumericMinMax(sizeView, minValue, maxValue, layerPath + ".mapping.size.field"))
						throw std::runtime_error("resolvePlotIR: empty size field for contiguous mapping at '" + layerPath + "'");
					const double span = maxValue - minValue;
					const float lo = layerSpec.scatterMapping.sizeMapping.minValue;
					const float hi = layerSpec.scatterMapping.sizeMapping.maxValue;

					rowSizes.reserve(values.size());
					for (const double value : values)
					{
						double t = 0.0;
						if (span != 0.0)
							t = (value - minValue) / span;
						const double size = static_cast<double>(lo) + (static_cast<double>(hi) - static_cast<double>(lo)) * t;
						rowSizes.push_back(static_cast<float>(size));
					}
				}
				else
				{
					throw std::runtime_error(
						"resolvePlotIR: unsupported scatter size mapping type '" + layerSpec.scatterMapping.sizeMapping.type
						+ "' at '" + layerPath + ".mapping.size.sizeMapping-scatter.type'");
				}
			}

			if (inputs.label.has_value())
			{
				if (*inputs.label >= dataset.views.size())
					throw std::runtime_error("resolvePlotIR: scatter label field index out of range at '" + layerPath + "'");

				const auto& categories = layerSpec.scatterMapping.labelMapping.categories;
				const auto& values = layerSpec.scatterMapping.labelMapping.values;
				if (categories.size() > values.size())
				{
					throw std::runtime_error(
						"resolvePlotIR: label bins categories exceed values at '" + layerPath + ".mapping.label.labelMapping-scatter'");
				}

				rowLabelBins = assignBinsForView(
					dataset.views[*inputs.label],
					categories,
					layerPath + ".mapping.label.labelMapping-scatter");
				labelBinShapes.reserve(categories.size());
				for (std::size_t i = 0; i < categories.size(); ++i)
				{
					labelBinShapes.push_back(parseMarkerShape(
						values[i],
						layerPath + ".mapping.label.labelMapping-scatter.values[" + std::to_string(i) + "]"));
				}
				labelBinned = true;
			}

			std::vector<FlowPlot::PointF> rowPoints;
			rowPoints.reserve(rowCount);
			double sumX = 0.0;
			double sumY = 0.0;
			for (std::size_t row = 0; row < rowCount; ++row)
			{
				const double x = xValues[row];
				const double y = yValues[row];
				sumX += x;
				sumY += y;

				float px = mapDataValueToAxisPixel(
					x,
					xDomain,
					layerRect.x,
					layerRect.w,
					xAxisSpec.invert);
				float py = mapDataValueToAxisPixel(
					y,
					yDomain,
					layerRect.y,
					layerRect.h,
					!yAxisSpec.invert);

				px += deterministicJitterOffset(row, 0x517cc1b7U, layerSpec.scatterConfig.jitterX);
				py += deterministicJitterOffset(row, 0xa4f3929dU, layerSpec.scatterConfig.jitterY);
				rowPoints.push_back(FlowPlot::PointF{px, py});
			}

			ResolvedIR::ScatterResolved scatter{};

			const bool hasAnyBins =
				(colorMode == ScatterFieldMode::Bins)
				|| (sizeMode == ScatterFieldMode::Bins)
				|| labelBinned;

			if (!hasAnyBins)
			{
				ResolvedIR::ResolvedMarkers markers{};
				markers.shape = baseShape;
				markers.stroke = baseStroke;
				markers.strokeWidth = layerSpec.scatterStyle.strokeWidth;
				markers.positions = rowPoints;
				if (colorMode == ScatterFieldMode::Contiguous)
					markers.fills = std::move(rowColors);
				else
					markers.fills = {baseFill};

				if (sizeMode == ScatterFieldMode::Contiguous)
					markers.sizes = std::move(rowSizes);
				else
					markers.sizes = {baseSize};

				scatter.markerBatches.push_back(std::move(markers));
			}
			else
			{
				std::unordered_map<ScatterBatchKey, std::size_t, ScatterBatchKeyHash> batchIndexByKey;
				std::vector<ResolvedIR::ResolvedMarkers> batches{};
				batchIndexByKey.reserve(rowCount);

				for (std::size_t row = 0; row < rowCount; ++row)
				{
					ScatterBatchKey key{};
					key.colorBin = (colorMode == ScatterFieldMode::Bins) ? rowColorBins[row] : -1;
					key.sizeBin = (sizeMode == ScatterFieldMode::Bins) ? rowSizeBins[row] : -1;
					key.labelBin = labelBinned ? rowLabelBins[row] : -1;

					auto found = batchIndexByKey.find(key);
					if (found == batchIndexByKey.end())
					{
						ResolvedIR::ResolvedMarkers markers{};
						if (labelBinned)
							markers.shape = labelBinShapes[static_cast<std::size_t>(key.labelBin)];
						else
							markers.shape = baseShape;
						markers.stroke = baseStroke;
						markers.strokeWidth = layerSpec.scatterStyle.strokeWidth;

						if (colorMode == ScatterFieldMode::Bins)
							markers.fills = {colorBinValues[static_cast<std::size_t>(key.colorBin)]};
						else if (colorMode == ScatterFieldMode::Contiguous)
							markers.fills.clear();
						else
							markers.fills = {baseFill};

						if (sizeMode == ScatterFieldMode::Bins)
							markers.sizes = {sizeBinValues[static_cast<std::size_t>(key.sizeBin)]};
						else if (sizeMode == ScatterFieldMode::Contiguous)
							markers.sizes.clear();
						else
							markers.sizes = {baseSize};

						const std::size_t newIndex = batches.size();
						batches.push_back(std::move(markers));
						batchIndexByKey.emplace(key, newIndex);
						found = batchIndexByKey.find(key);
					}

					ResolvedIR::ResolvedMarkers& batch = batches[found->second];
					batch.positions.push_back(rowPoints[row]);
					if (colorMode == ScatterFieldMode::Contiguous)
						batch.fills.push_back(rowColors[row]);
					if (sizeMode == ScatterFieldMode::Contiguous)
						batch.sizes.push_back(rowSizes[row]);
				}

				scatter.markerBatches = std::move(batches);
			}

			if (layerSpec.scatterStats.showMeanPoint && rowCount > 0U)
			{
				const double meanX = sumX / static_cast<double>(rowCount);
				const double meanY = sumY / static_cast<double>(rowCount);

				ResolvedIR::ResolvedMarkers meanMarker{};
				meanMarker.shape = parseMarkerShape(
					layerSpec.scatterStats.meanPointShape,
					layerPath + ".stats.meanPointShape");
				meanMarker.stroke = parseColor(
					layerSpec.scatterStats.meanPointStrokeColor,
					layerPath + ".stats.meanPointStrokeColor");
				meanMarker.strokeWidth = layerSpec.scatterStats.meanPointStrokeWidth;
				meanMarker.fills = {parseColor(
					layerSpec.scatterStats.meanPointColor,
					layerPath + ".stats.meanPointColor")};
				meanMarker.sizes = {layerSpec.scatterStats.meanPointSize};
				meanMarker.positions = {FlowPlot::PointF{
					mapDataValueToAxisPixel(meanX, xDomain, layerRect.x, layerRect.w, xAxisSpec.invert),
					mapDataValueToAxisPixel(meanY, yDomain, layerRect.y, layerRect.h, !yAxisSpec.invert)}};

				scatter.markerBatches.push_back(std::move(meanMarker));
			}

			ResolvedIR::LayerResolved resolvedLayer{};
			resolvedLayer.zIndex = boundLayer.zIndex;
			resolvedLayer.opacity = boundLayer.opacity;
				resolvedLayer.payload = std::move(scatter);
				return resolvedLayer;
			}

			inline ResolvedIR::LayerResolved resolveHistogramLayer(
				const FlowPlot::Spec::LayerSpec& layerSpec,
				std::size_t panelIndex,
				std::size_t layerIndex,
				const BoundIR::BoundLayer& boundLayer,
				const BoundIR::BoundHistogramInputs& inputs,
				const AxisDomain& xDomain,
				const AxisDomain& yDomain,
				const FlowPlot::RectF& layerRect,
				const FlowPlot::Spec::AxisSpec& xAxisSpec,
				const FlowPlot::Spec::AxisSpec& yAxisSpec,
				const HistogramPreparedData& prepared)
			{
				const std::string layerPath = "panels[" + std::to_string(panelIndex) + "].layers[" + std::to_string(layerIndex) + "]";

				const FlowPlot::Color baseFill = parseColor(layerSpec.histogramStyle.fillColor, layerPath + ".style.fillColor");
				const FlowPlot::Color stroke = parseColor(layerSpec.histogramStyle.strokeColor, layerPath + ".style.strokeColor");

				std::vector<FlowPlot::Color> mappedSeriesColors{};
				if (inputs.color.has_value())
				{
					const std::string mode = canonicalizeToken(layerSpec.histogramMapping.colorMapping.type);
					if (!mode.empty() && mode != "bins")
					{
						throw std::runtime_error(
							"resolvePlotIR: histogram color mapping type must be 'bins' at '"
							+ layerPath + ".mapping.color.colorMapping-histogram.type'");
					}

					const auto& categories = layerSpec.histogramMapping.colorMapping.categories;
					const auto& values = layerSpec.histogramMapping.colorMapping.values;
					if (categories.size() > values.size())
					{
						throw std::runtime_error(
							"resolvePlotIR: histogram color bins categories exceed values at '"
							+ layerPath + ".mapping.color.colorMapping-histogram'");
					}

					mappedSeriesColors.reserve(categories.size());
					for (std::size_t idx = 0; idx < categories.size(); ++idx)
					{
						mappedSeriesColors.push_back(parseColor(
							values[idx],
							layerPath + ".mapping.color.colorMapping-histogram.values[" + std::to_string(idx) + "]"));
					}
				}

				ResolvedIR::HistogramResolved histogram{};
				if (prepared.binEdges.size() < 2U || prepared.series.empty())
				{
					ResolvedIR::LayerResolved resolvedLayer{};
					resolvedLayer.zIndex = boundLayer.zIndex;
					resolvedLayer.opacity = boundLayer.opacity;
					resolvedLayer.payload = std::move(histogram);
					return resolvedLayer;
				}

				const bool dataOnX = (inputs.dataAxis == FlowPlot::Spec::HistogramDataAxis::X);
				const std::size_t binCount = prepared.binEdges.size() - 1U;
				const std::size_t seriesCount = prepared.series.size();
				histogram.bars.reserve(binCount * seriesCount);

				for (std::size_t bin = 0; bin < binCount; ++bin)
				{
					const double edgeLo = prepared.binEdges[bin];
					const double edgeHi = prepared.binEdges[bin + 1U];
					const double step = (seriesCount > 0U) ? ((edgeHi - edgeLo) / static_cast<double>(seriesCount)) : 0.0;

					for (std::size_t seriesIdx = 0; seriesIdx < seriesCount; ++seriesIdx)
					{
						const HistogramPreparedData::Series& series = prepared.series[seriesIdx];
						if (bin >= series.binCounts.size())
							throw std::runtime_error("resolvePlotIR: internal histogram series bin mismatch at '" + layerPath + "'");

						const double count = series.binCounts[bin];
						if (!layerSpec.histogramConfig.showEmptyBins && count == 0.0)
							continue;

						const double dataLo = edgeLo + step * static_cast<double>(seriesIdx);
						const double dataHi = edgeLo + step * static_cast<double>(seriesIdx + 1U);

						float x0 = 0.0f;
						float x1 = 0.0f;
						float y0 = 0.0f;
						float y1 = 0.0f;
						if (dataOnX)
						{
							x0 = mapDataValueToAxisPixel(dataLo, xDomain, layerRect.x, layerRect.w, xAxisSpec.invert);
							x1 = mapDataValueToAxisPixel(dataHi, xDomain, layerRect.x, layerRect.w, xAxisSpec.invert);
							y0 = mapDataValueToAxisPixel(0.0, yDomain, layerRect.y, layerRect.h, !yAxisSpec.invert);
							y1 = mapDataValueToAxisPixel(count, yDomain, layerRect.y, layerRect.h, !yAxisSpec.invert);
						}
						else
						{
							x0 = mapDataValueToAxisPixel(0.0, xDomain, layerRect.x, layerRect.w, xAxisSpec.invert);
							x1 = mapDataValueToAxisPixel(count, xDomain, layerRect.x, layerRect.w, xAxisSpec.invert);
							y0 = mapDataValueToAxisPixel(dataLo, yDomain, layerRect.y, layerRect.h, !yAxisSpec.invert);
							y1 = mapDataValueToAxisPixel(dataHi, yDomain, layerRect.y, layerRect.h, !yAxisSpec.invert);
						}

						FlowPlot::Color fill = baseFill;
						if (series.colorBin >= 0)
						{
							const std::size_t colorIndex = static_cast<std::size_t>(series.colorBin);
							if (colorIndex >= mappedSeriesColors.size())
								throw std::runtime_error("resolvePlotIR: histogram color bin index out of range at '" + layerPath + "'");
							fill = mappedSeriesColors[colorIndex];
						}

						ResolvedIR::ResolvedBox bar{};
						bar.fill = fill;
						bar.stroke = stroke;
						bar.strokeWidth = layerSpec.histogramStyle.strokeWidth;
						bar.rect.x = std::min(x0, x1);
						bar.rect.w = std::abs(x1 - x0);
						bar.rect.y = std::min(y0, y1);
						bar.rect.h = std::abs(y1 - y0);
						histogram.bars.push_back(std::move(bar));
					}
				}

				if (prepared.hasSummaryStats)
				{
					auto appendStatLine = [&](double value, const FlowPlot::Color& color, float width)
					{
						FlowPlot::PointF start{};
						FlowPlot::PointF end{};
						if (dataOnX)
						{
							const float x = mapDataValueToAxisPixel(value, xDomain, layerRect.x, layerRect.w, xAxisSpec.invert);
							start = FlowPlot::PointF{
								x,
								mapDataValueToAxisPixel(yDomain.min, yDomain, layerRect.y, layerRect.h, !yAxisSpec.invert)};
							end = FlowPlot::PointF{
								x,
								mapDataValueToAxisPixel(yDomain.max, yDomain, layerRect.y, layerRect.h, !yAxisSpec.invert)};
						}
						else
						{
							const float y = mapDataValueToAxisPixel(value, yDomain, layerRect.y, layerRect.h, !yAxisSpec.invert);
							start = FlowPlot::PointF{
								mapDataValueToAxisPixel(xDomain.min, xDomain, layerRect.x, layerRect.w, xAxisSpec.invert),
								y};
							end = FlowPlot::PointF{
								mapDataValueToAxisPixel(xDomain.max, xDomain, layerRect.x, layerRect.w, xAxisSpec.invert),
								y};
						}

						histogram.statLines.push_back(makeResolvedLine(start, end, color, width));
					};

					if (layerSpec.histogramStats.showMeanLine)
					{
						appendStatLine(
							prepared.mean,
							parseColor(layerSpec.histogramStats.meanLineColor, layerPath + ".stats.meanLineColor"),
							layerSpec.histogramStats.meanLineWidth);
					}
					if (layerSpec.histogramStats.showMedianLine)
					{
						appendStatLine(
							prepared.median,
							parseColor(layerSpec.histogramStats.medianLineColor, layerPath + ".stats.medianLineColor"),
							layerSpec.histogramStats.medianLineWidth);
					}
				}

				ResolvedIR::LayerResolved resolvedLayer{};
				resolvedLayer.zIndex = boundLayer.zIndex;
				resolvedLayer.opacity = boundLayer.opacity;
				resolvedLayer.payload = std::move(histogram);
				return resolvedLayer;
			}

		inline ResolvedIR::PanelResolved resolvePanel(
			const FlowPlot::Spec::PanelSpec& panelSpec,
			std::size_t panelIndex,
			const FlowPlot::Spec::FigureSpec& figureSpec,
			const FlowPlot::Spec::LayoutSpec& layoutSpec,
			const std::unordered_map<std::uint64_t, const BoundIR::BoundLayer*>& boundLayerByKey,
			const std::vector<BoundIR::BoundDataset>& datasets,
			const FlowPlot::ITextEngine* textEngine)
		{
			const std::uint32_t panelSlot = static_cast<std::uint32_t>(panelIndex);
			const std::uint32_t panelRow = panelSlot / layoutSpec.cols;
			const std::uint32_t panelCol = panelSlot % layoutSpec.cols;

			ResolvedIR::PanelResolved resolved{};
			resolved.visible = true;
			resolved.frame.fill = parseColor(panelSpec.background, "panels[" + std::to_string(panelIndex) + "].background");
			resolved.frame.stroke = parseColor(panelSpec.borderColor, "panels[" + std::to_string(panelIndex) + "].borderColor");
			resolved.frame.strokeWidth = panelSpec.borderWidth;
				resolved.frame.rect = resolvePanelFrameRect(figureSpec, layoutSpec, panelRow, panelCol);
				resolved.clipRectPanel = resolved.frame.rect;

			const FlowPlot::RectF axisRect = FlowPlot::RectF{
				resolved.frame.rect.x + panelSpec.padding.left,
				resolved.frame.rect.y + panelSpec.padding.top,
				resolved.frame.rect.w - panelSpec.padding.left - panelSpec.padding.right,
				resolved.frame.rect.h - panelSpec.padding.top - panelSpec.padding.bottom
			};
			if (axisRect.w < 0.0f || axisRect.h < 0.0f)
				throw std::runtime_error("resolvePlotIR: panel padding leaves negative layer clip size at 'panels[" + std::to_string(panelIndex) + "]'");

				if (panelSpec.title.visible)
					resolved.title = resolvePanelTitle(panelSpec, resolved.frame.rect, panelIndex, textEngine);

				std::vector<const BoundIR::BoundLayer*> orderedBoundLayers{};
				orderedBoundLayers.reserve(panelSpec.layers.size());
				for (std::size_t layerIdx = 0; layerIdx < panelSpec.layers.size(); ++layerIdx)
			{
				const BoundIR::BoundLayer* boundLayer = findBoundLayer(boundLayerByKey, panelIndex, layerIdx);
				if (boundLayer == nullptr)
					continue;
				orderedBoundLayers.push_back(boundLayer);
			}

			std::stable_sort(
				orderedBoundLayers.begin(),
				orderedBoundLayers.end(),
				[](const BoundIR::BoundLayer* lhs, const BoundIR::BoundLayer* rhs)
				{
						if (lhs->zIndex != rhs->zIndex)
							return lhs->zIndex < rhs->zIndex;
						return lhs->layerIndex < rhs->layerIndex;
					});

				const PanelAxisComputation axisComputation = computePanelAxisComputation(
					panelSpec,
					panelIndex,
					orderedBoundLayers,
					datasets);

				resolved.xAxis = resolveAxis(
					panelSpec.xAxis,
					true,
					false,
					axisRect,
					axisComputation.xPrimary,
					textEngine,
					"panels[" + std::to_string(panelIndex) + "].xAxis");
				resolved.yAxis = resolveAxis(
					panelSpec.yAxis,
					false,
					false,
					axisRect,
					axisComputation.yPrimary,
					textEngine,
					"panels[" + std::to_string(panelIndex) + "].yAxis");
				resolved.xSecondary = resolveAxis(
					panelSpec.xSecondary,
					true,
					true,
					axisRect,
					axisComputation.xSecondary,
					textEngine,
					"panels[" + std::to_string(panelIndex) + "].xSecondary");
				resolved.ySecondary = resolveAxis(
					panelSpec.ySecondary,
					false,
					true,
					axisRect,
					axisComputation.ySecondary,
					textEngine,
					"panels[" + std::to_string(panelIndex) + "].ySecondary");

				const float leftInset = panelSpec.yAxis.visible ? (panelSpec.yAxis.lineWidth * 0.5f) : 0.0f;
				const float rightInset = panelSpec.ySecondary.visible ? (panelSpec.ySecondary.lineWidth * 0.5f) : 0.0f;
				const float topInset = panelSpec.xSecondary.visible ? (panelSpec.xSecondary.lineWidth * 0.5f) : 0.0f;
				const float bottomInset = panelSpec.xAxis.visible ? (panelSpec.xAxis.lineWidth * 0.5f) : 0.0f;

				resolved.clipRectLayer = axisRect;
				resolved.clipRectLayer.x += leftInset;
				resolved.clipRectLayer.y += topInset;
				resolved.clipRectLayer.w -= (leftInset + rightInset);
				resolved.clipRectLayer.h -= (topInset + bottomInset);
				if (resolved.clipRectLayer.w < 0.0f || resolved.clipRectLayer.h < 0.0f)
				{
					throw std::runtime_error(
						"resolvePlotIR: axis line widths leave negative data clip size at 'panels[" + std::to_string(panelIndex) + "]'");
				}

				auto selectAxisContext = [&](bool isXAxis, FlowPlot::Spec::AxisDataRole role, const std::string& rolePath)
					-> std::pair<const AxisDomain*, const FlowPlot::Spec::AxisSpec*>
				{
					if (role == FlowPlot::Spec::AxisDataRole::Null)
						throw std::runtime_error("resolvePlotIR: axis role cannot be 'null' at '" + rolePath + "'");

					if (isXAxis)
					{
						if (role == FlowPlot::Spec::AxisDataRole::Primary)
							return {&axisComputation.xPrimary, &panelSpec.xAxis};
						return {&axisComputation.xSecondary, &panelSpec.xSecondary};
					}

					if (role == FlowPlot::Spec::AxisDataRole::Primary)
						return {&axisComputation.yPrimary, &panelSpec.yAxis};
					return {&axisComputation.ySecondary, &panelSpec.ySecondary};
				};

				resolved.layers.reserve(orderedBoundLayers.size());
				for (const BoundIR::BoundLayer* boundLayer : orderedBoundLayers)
				{
				const std::size_t layerIdx = static_cast<std::size_t>(boundLayer->layerIndex);
				if (layerIdx >= panelSpec.layers.size())
					throw std::runtime_error("resolvePlotIR: bound layer index out of range at 'panels[" + std::to_string(panelIndex) + "]'");

				const FlowPlot::Spec::LayerSpec& layerSpec = panelSpec.layers[layerIdx];
					if (!layerSpec.visible)
						continue;

					const std::string layerType = canonicalizeToken(layerSpec.type);
					const std::string layerPath = "panels[" + std::to_string(panelIndex) + "].layers[" + std::to_string(layerIdx) + "]";
					const auto [xDomainPtr, xAxisSpecPtr] = selectAxisContext(
						true,
						boundLayer->axisData.x,
						layerPath + ".axisData.x");
					const auto [yDomainPtr, yAxisSpecPtr] = selectAxisContext(
						false,
						boundLayer->axisData.y,
						layerPath + ".axisData.y");

					if (layerType == "scatter")
					{
						const auto* scatterInputs = std::get_if<BoundIR::BoundScatterInputs>(&boundLayer->inputs);
						if (scatterInputs == nullptr)
							continue;

						resolved.layers.push_back(resolveScatterLayer(
							layerSpec,
							panelIndex,
							layerIdx,
							*boundLayer,
							*scatterInputs,
							datasets,
							*xDomainPtr,
							*yDomainPtr,
							axisRect,
							*xAxisSpecPtr,
							*yAxisSpecPtr));
						continue;
					}

					if (layerType == "histogram")
					{
						const auto* histogramInputs = std::get_if<BoundIR::BoundHistogramInputs>(&boundLayer->inputs);
						if (histogramInputs == nullptr)
							continue;

						auto preparedIt = axisComputation.histogramByLayerIndex.find(layerIdx);
						if (preparedIt == axisComputation.histogramByLayerIndex.end())
							throw std::runtime_error("resolvePlotIR: internal histogram precompute missing at '" + layerPath + "'");

						resolved.layers.push_back(resolveHistogramLayer(
							layerSpec,
							panelIndex,
							layerIdx,
							*boundLayer,
							*histogramInputs,
							*xDomainPtr,
							*yDomainPtr,
							axisRect,
							*xAxisSpecPtr,
							*yAxisSpecPtr,
							preparedIt->second));
						continue;
					}

					throw std::runtime_error("resolvePlotIR: unsupported layer type '" + layerSpec.type + "' at '" + layerPath + ".type'");
				}

				return resolved;
			}
	} // namespace ResolveCompiler

	inline ResolvedIR::PlotResolvedIR resolvePlotIR(
		const FlowPlot::Spec::MasterTemplateSpec& spec,
		const BoundIR::PlotBoundIR& bound,
		const FlowPlot::ITextEngine* textEngine)
	{
		ResolvedIR::PlotResolvedIR resolved{};
		resolved.figure.width = spec.figure.width;
		resolved.figure.height = spec.figure.height;
		resolved.figure.background = ResolveCompiler::parseColor(spec.figure.background, "figure.background");

		if (spec.figure.title.visible)
			resolved.figure.title = ResolveCompiler::resolveFigureTitle(spec.figure, textEngine);

		resolved.figure.legends.reserve(spec.figure.legends.size());
		for (std::size_t legendIdx = 0; legendIdx < spec.figure.legends.size(); ++legendIdx)
		{
			const FlowPlot::Spec::LegendSpec& legendSpec = spec.figure.legends[legendIdx];
			if (!legendSpec.visible)
				continue;

			resolved.figure.legends.emplace_back(
				ResolveCompiler::resolveLegend(
					legendSpec,
					legendIdx,
					spec.figure,
					spec.layout,
					textEngine));
		}

		if (spec.layout.rows == 0 || spec.layout.cols == 0)
			throw std::runtime_error("resolvePlotIR: layout rows/cols must be greater than zero");

		const std::size_t rows = static_cast<std::size_t>(spec.layout.rows);
		const std::size_t cols = static_cast<std::size_t>(spec.layout.cols);
		if (rows > 0 && cols > (std::numeric_limits<std::size_t>::max() / rows))
			throw std::runtime_error("resolvePlotIR: layout rows*cols overflows size_t");

		const std::size_t maxPanelSlots = rows * cols;
		const std::size_t panelCount = std::min(spec.panels.size(), maxPanelSlots);
		resolved.panels.reserve(panelCount);

		std::unordered_map<std::uint64_t, const BoundIR::BoundLayer*> boundLayerByKey;
		boundLayerByKey.reserve(bound.layersInPainterOrder.size());
		for (const BoundIR::BoundLayer& layer : bound.layersInPainterOrder)
		{
			const std::uint64_t key = ResolveCompiler::makePanelLayerKey(layer.panelIndex, layer.layerIndex);
			boundLayerByKey[key] = &layer;
		}

		for (std::size_t panelIdx = 0; panelIdx < panelCount; ++panelIdx)
		{
			const FlowPlot::Spec::PanelSpec& panelSpec = spec.panels[panelIdx];
			if (!panelSpec.visible)
				continue;

			resolved.panels.emplace_back(
				ResolveCompiler::resolvePanel(
					panelSpec,
					panelIdx,
					spec.figure,
					spec.layout,
					boundLayerByKey,
					bound.datasets,
					textEngine));
		}

		return resolved;
	}

	inline FlowPlot::RenderPlot buildRenderPlot(const ResolvedIR::PlotResolvedIR& resolved)
	{
		FlowPlot::RenderPlot render{};
		render.width = resolved.figure.width;
		render.height = resolved.figure.height;
		render.background = resolved.figure.background;

		auto clampOpacity = [](float opacity) noexcept -> float
		{
			if (opacity < 0.0f)
				return 0.0f;
			if (opacity > 1.0f)
				return 1.0f;
			return opacity;
		};

		auto applyOpacityToColor = [&](FlowPlot::Color color, float opacity) noexcept -> FlowPlot::Color
		{
			const float alpha = static_cast<float>(color.a) * clampOpacity(opacity);
			const int rounded = static_cast<int>(std::lround(alpha));
			if (rounded <= 0)
			{
				color.a = 0U;
				return color;
			}
			if (rounded >= 255)
			{
				color.a = 255U;
				return color;
			}
			color.a = static_cast<std::uint8_t>(rounded);
			return color;
		};

		for (const ResolvedIR::PanelResolved& panel : resolved.panels)
		{
			if (!panel.visible)
				continue;

			render.commands.push_back(FlowPlot::PushClipRectCommand{panel.clipRectPanel});
			const std::array<const ResolvedIR::AxisResolved*, 4U> axes{
				&panel.xAxis,
				&panel.yAxis,
				&panel.xSecondary,
				&panel.ySecondary};

			// Emit axis primitives by phase so axis lines always render over grid lines.
			for (const ResolvedIR::AxisResolved* axis : axes)
			{
				for (const ResolvedIR::ResolvedLine& grid : axis->gridLines)
					render.commands.push_back(grid);
			}
			for (const ResolvedIR::AxisResolved* axis : axes)
			{
				if (axis->axisLine.has_value())
					render.commands.push_back(*axis->axisLine);
			}
			for (const ResolvedIR::AxisResolved* axis : axes)
			{
				for (const ResolvedIR::ResolvedLine& tick : axis->tickLines)
					render.commands.push_back(tick);
			}
			for (const ResolvedIR::AxisResolved* axis : axes)
			{
				for (const ResolvedIR::ResolvedText& label : axis->tickLabels)
					render.commands.push_back(label);
			}
			for (const ResolvedIR::AxisResolved* axis : axes)
			{
				if (axis->title.has_value())
					render.commands.push_back(*axis->title);
			}

			render.commands.push_back(FlowPlot::PushClipRectCommand{panel.clipRectLayer});
			for (const ResolvedIR::LayerResolved& layer : panel.layers)
			{
				const float opacity = clampOpacity(layer.opacity);
				if (const auto* scatter = std::get_if<ResolvedIR::ScatterResolved>(&layer.payload))
				{
					for (const ResolvedIR::ResolvedMarkers& batch : scatter->markerBatches)
					{
						FlowPlot::MarkersCommand cmd = batch;
						for (FlowPlot::Color& fill : cmd.fills)
							fill = applyOpacityToColor(fill, opacity);
						cmd.stroke = applyOpacityToColor(cmd.stroke, opacity);
						render.commands.push_back(std::move(cmd));
					}
					continue;
				}

				if (const auto* histogram = std::get_if<ResolvedIR::HistogramResolved>(&layer.payload))
				{
					for (const ResolvedIR::ResolvedBox& bar : histogram->bars)
					{
						FlowPlot::BoxCommand cmd = bar;
						cmd.fill = applyOpacityToColor(cmd.fill, opacity);
						cmd.stroke = applyOpacityToColor(cmd.stroke, opacity);
						render.commands.push_back(std::move(cmd));
					}
					for (const ResolvedIR::ResolvedLine& line : histogram->statLines)
					{
						FlowPlot::PolylineCommand cmd = line;
						cmd.color = applyOpacityToColor(cmd.color, opacity);
						render.commands.push_back(std::move(cmd));
					}
				}
			}
			render.commands.push_back(FlowPlot::PopClipRectCommand{});

			if (panel.title.has_value())
				render.commands.push_back(*panel.title);

			render.commands.push_back(FlowPlot::PopClipRectCommand{});
		}

		for (const ResolvedIR::LegendResolved& legend : resolved.figure.legends)
		{
			render.commands.push_back(legend.frame);
			for (const ResolvedIR::ResolvedBox& icon : legend.iconBoxes)
				render.commands.push_back(icon);
			for (const ResolvedIR::ResolvedText& label : legend.labels)
				render.commands.push_back(label);
		}

		if (resolved.figure.title.has_value())
			render.commands.push_back(*resolved.figure.title);

		return render;
	}

#ifdef FLOW_PLOT_COMPLETE_JSON
	namespace TemplateNormalization
	{
		using JsonAllocator = rapidjson::Document::AllocatorType;

		struct NormalizationDefaults
		{
			const rapidjson::Value& legendElement;
			const rapidjson::Value& legend;
			const rapidjson::Value& scatterMapping;
			const rapidjson::Value& scatterStyle;
			const rapidjson::Value& scatterStats;
			const rapidjson::Value& scatterConfig;
			const rapidjson::Value& histogramMapping;
			const rapidjson::Value& histogramStyle;
			const rapidjson::Value& histogramStats;
			const rapidjson::Value& histogramConfig;
			const rapidjson::Value& layer;
			const rapidjson::Value& panel;
			const rapidjson::Value& master;
		};

		inline void mergeDefaultsInto(rapidjson::Value& target, const rapidjson::Value& overrides, JsonAllocator& allocator)
		{
			if (!target.IsObject() || !overrides.IsObject())
			{
				target.CopyFrom(overrides, allocator);
				return;
			}

			for (auto it = overrides.MemberBegin(); it != overrides.MemberEnd(); ++it)
			{
				auto targetIt = target.FindMember(it->name.GetString());
				if (targetIt == target.MemberEnd())
				{
					rapidjson::Value key;
					key.SetString(it->name.GetString(), it->name.GetStringLength(), allocator);
					rapidjson::Value value;
					value.CopyFrom(it->value, allocator);
					target.AddMember(key, value, allocator);
					continue;
				}

				if (targetIt->value.IsObject() && it->value.IsObject())
					mergeDefaultsInto(targetIt->value, it->value, allocator);
				else
					targetIt->value.CopyFrom(it->value, allocator);
			}
		}

		inline rapidjson::Value mergeDefaults(const rapidjson::Value& defaultsJson, const rapidjson::Value& overridesJson, JsonAllocator& allocator)
		{
			rapidjson::Value merged;
			if (!defaultsJson.IsObject() || !overridesJson.IsObject())
			{
				merged.CopyFrom(overridesJson, allocator);
				return merged;
			}

			merged.CopyFrom(defaultsJson, allocator);
			mergeDefaultsInto(merged, overridesJson, allocator);
			return merged;
		}

		inline void mergeArrayElementsWithDefaults(
			rapidjson::Value& arrayJson,
			const rapidjson::Value& elementDefaults,
			const std::string& path,
			JsonAllocator& allocator)
		{
			if (!arrayJson.IsArray())
				throw std::runtime_error("normalizeTemplate: '" + path + "' must be an array");

			if (arrayJson.Empty())
			{
				rapidjson::Value defaultElement;
				defaultElement.CopyFrom(elementDefaults, allocator);
				arrayJson.PushBack(defaultElement, allocator);
			}

			for (rapidjson::SizeType i = 0; i < arrayJson.Size(); ++i)
			{
				rapidjson::Value& element = arrayJson[i];
				if (!element.IsObject())
					throw std::runtime_error("normalizeTemplate: '" + path + "[" + std::to_string(i) + "]' must be an object");

				rapidjson::Value mergedElement = mergeDefaults(elementDefaults, element, allocator);
				element.Swap(mergedElement);
			}
		}

		inline void mergeLayerTypeDefaults(
			rapidjson::Value& layerJson,
			std::size_t panelIndex,
			std::size_t layerIndex,
			const NormalizationDefaults& defaults,
			JsonAllocator& allocator)
		{
			const std::string layerPath =
				"panels[" + std::to_string(panelIndex) + "].layers[" + std::to_string(layerIndex) + "]";

			if (!layerJson.IsObject())
				throw std::runtime_error("normalizeTemplate: '" + layerPath + "' must be an object");

			rapidjson::Value mergedLayer = mergeDefaults(defaults.layer, layerJson, allocator);
			layerJson.Swap(mergedLayer);

			const rapidjson::Value* typeValue = findJsonMember(layerJson, "type");
			if (typeValue == nullptr || !typeValue->IsString() || typeValue->GetStringLength() == 0)
				throw std::runtime_error("normalizeTemplate: '" + layerPath + ".type' must be a non-empty string");

			const std::string layerType = jsonStringToStdString(*typeValue);

			const rapidjson::Value* mappingDefaults = nullptr;
			const rapidjson::Value* styleDefaults = nullptr;
			const rapidjson::Value* statsDefaults = nullptr;
			const rapidjson::Value* configDefaults = nullptr;

			if (layerType == "scatter")
			{
				mappingDefaults = &defaults.scatterMapping;
				styleDefaults = &defaults.scatterStyle;
				statsDefaults = &defaults.scatterStats;
				configDefaults = &defaults.scatterConfig;
			}
			else if (layerType == "histogram")
			{
				mappingDefaults = &defaults.histogramMapping;
				styleDefaults = &defaults.histogramStyle;
				statsDefaults = &defaults.histogramStats;
				configDefaults = &defaults.histogramConfig;
			}
			else
			{
				throw std::runtime_error("normalizeTemplate: unsupported layer type '" + layerType + "' at '" + layerPath + ".type'");
			}

			auto mergeSection = [&](const char* sectionName, const rapidjson::Value& defaultsJson)
			{
				rapidjson::Value* sectionValue = findJsonMember(layerJson, sectionName);
				if (sectionValue == nullptr)
					throw std::runtime_error("normalizeTemplate: missing required key '" + layerPath + "." + sectionName + "'");

				if (!sectionValue->IsObject())
					throw std::runtime_error("normalizeTemplate: '" + layerPath + "." + sectionName + "' must be an object");

				rapidjson::Value mergedSection = mergeDefaults(defaultsJson, *sectionValue, allocator);
				sectionValue->Swap(mergedSection);
			};

			mergeSection("mapping", *mappingDefaults);
			mergeSection("style", *styleDefaults);
			mergeSection("stats", *statsDefaults);
			mergeSection("config", *configDefaults);
		}
	} // namespace TemplateNormalization

	inline rapidjson::Document normalizeTemplateWithDefaults(const rapidjson::Value& templateJson)
	{
		rapidjson::Document legendElementDefaults = parseJsonDocument(JsonDefaults::kLegendElementDefaultsJson, "normalizeTemplate: legend element defaults");
		rapidjson::Document legendDefaults = parseJsonDocument(JsonDefaults::kLegendDefaultsJson, "normalizeTemplate: legend defaults");
		rapidjson::Document scatterMappingDefaults = parseJsonDocument(JsonDefaults::kScatterMappingDefaultsJson, "normalizeTemplate: scatter mapping defaults");
		rapidjson::Document scatterStyleDefaults = parseJsonDocument(JsonDefaults::kScatterStyleDefaultsJson, "normalizeTemplate: scatter style defaults");
		rapidjson::Document scatterStatsDefaults = parseJsonDocument(JsonDefaults::kScatterStatsDefaultsJson, "normalizeTemplate: scatter stats defaults");
		rapidjson::Document scatterConfigDefaults = parseJsonDocument(JsonDefaults::kScatterConfigDefaultsJson, "normalizeTemplate: scatter config defaults");
		rapidjson::Document histogramMappingDefaults = parseJsonDocument(JsonDefaults::kHistogramMappingDefaultsJson, "normalizeTemplate: histogram mapping defaults");
		rapidjson::Document histogramStyleDefaults = parseJsonDocument(JsonDefaults::kHistogramStyleDefaultsJson, "normalizeTemplate: histogram style defaults");
		rapidjson::Document histogramStatsDefaults = parseJsonDocument(JsonDefaults::kHistogramStatsDefaultsJson, "normalizeTemplate: histogram stats defaults");
		rapidjson::Document histogramConfigDefaults = parseJsonDocument(JsonDefaults::kHistogramConfigDefaultsJson, "normalizeTemplate: histogram config defaults");
		rapidjson::Document layerDefaults = parseJsonDocument(JsonDefaults::kLayerDefaultsJson, "normalizeTemplate: layer defaults");
		rapidjson::Document panelDefaults = parseJsonDocument(JsonDefaults::kPanelDefaultsJson, "normalizeTemplate: panel defaults");
		rapidjson::Document masterDefaults = parseJsonDocument(JsonDefaults::kMasterTemplateJson, "normalizeTemplate: master defaults");

		const TemplateNormalization::NormalizationDefaults defaults{
			legendElementDefaults,
			legendDefaults,
			scatterMappingDefaults,
			scatterStyleDefaults,
			scatterStatsDefaults,
			scatterConfigDefaults,
			histogramMappingDefaults,
			histogramStyleDefaults,
			histogramStatsDefaults,
			histogramConfigDefaults,
			layerDefaults,
			panelDefaults,
			masterDefaults};

		rapidjson::Document resolvedJson;
		resolvedJson.SetObject();
		TemplateNormalization::JsonAllocator& allocator = resolvedJson.GetAllocator();

		rapidjson::Value templateRoot;
		if (templateJson.IsNull())
		{
			templateRoot.SetObject();
		}
		else if (templateJson.IsObject())
		{
			templateRoot.CopyFrom(templateJson, allocator);
		}
		else
		{
			throw std::runtime_error("normalizeTemplate: template root must be a JSON object");
		}

		rapidjson::Value mergedRoot = TemplateNormalization::mergeDefaults(defaults.master, templateRoot, allocator);
		resolvedJson.Swap(mergedRoot);

		rapidjson::Value* figureJson = findJsonMember(resolvedJson, "figure");
		if (figureJson == nullptr || !figureJson->IsObject())
			throw std::runtime_error("normalizeTemplate: 'figure' must be an object");

		rapidjson::Value* legendsJson = findJsonMember(*figureJson, "legends");
		if (legendsJson == nullptr)
			throw std::runtime_error("normalizeTemplate: missing required key 'figure.legends'");

		TemplateNormalization::mergeArrayElementsWithDefaults(*legendsJson, defaults.legend, "figure.legends", allocator);

		for (rapidjson::SizeType legendIndex = 0; legendIndex < legendsJson->Size(); ++legendIndex)
		{
			rapidjson::Value& legendJson = (*legendsJson)[legendIndex];
			rapidjson::Value* legendElementsJson = findJsonMember(legendJson, "legendElements");
			if (legendElementsJson == nullptr)
				throw std::runtime_error("normalizeTemplate: missing required key 'figure.legends[" + std::to_string(legendIndex) + "].legendElements'");

			const std::string legendElementsPath = "figure.legends[" + std::to_string(legendIndex) + "].legendElements";
			TemplateNormalization::mergeArrayElementsWithDefaults(*legendElementsJson, defaults.legendElement, legendElementsPath, allocator);
		}

		rapidjson::Value* panelsJson = findJsonMember(resolvedJson, "panels");
		if (panelsJson == nullptr)
			throw std::runtime_error("normalizeTemplate: missing required key 'panels'");

		TemplateNormalization::mergeArrayElementsWithDefaults(*panelsJson, defaults.panel, "panels", allocator);

		for (rapidjson::SizeType panelIndex = 0; panelIndex < panelsJson->Size(); ++panelIndex)
		{
			rapidjson::Value& panelJson = (*panelsJson)[panelIndex];
			rapidjson::Value* layersJson = findJsonMember(panelJson, "layers");
			if (layersJson == nullptr)
				throw std::runtime_error("normalizeTemplate: missing required key 'panels[" + std::to_string(panelIndex) + "].layers'");

			const std::string layersPath = "panels[" + std::to_string(panelIndex) + "].layers";
			TemplateNormalization::mergeArrayElementsWithDefaults(*layersJson, defaults.layer, layersPath, allocator);

			for (rapidjson::SizeType layerIndex = 0; layerIndex < layersJson->Size(); ++layerIndex)
			{
				TemplateNormalization::mergeLayerTypeDefaults(
					(*layersJson)[layerIndex],
					panelIndex,
					layerIndex,
					defaults,
					allocator);
			}
		}

		return resolvedJson;
	}
#endif // FLOW_PLOT_COMPLETE_JSON
} // namespace FlowInternal
