#pragma once

#ifndef FLOW_PLOT_HPP_INCLUDED
#define FLOW_PLOT_HPP_INCLUDED

#include "FlowPlot_Defaults.hpp"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/istreamwrapper.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace FlowPlot
{
	using value = std::variant<int, float, double, bool, const char*, std::string, std::string_view>;

	struct Color
	{
		std::uint8_t r = 0;
		std::uint8_t g = 0;
		std::uint8_t b = 0;
		std::uint8_t a = 255;
	};

	struct PointF
	{
		float x = 0.0f;
		float y = 0.0f;
	};

	struct RectF
	{
		float x = 0.0f;
		float y = 0.0f;
		float w = 0.0f;
		float h = 0.0f;
	};

	enum class HorizontalAlign : std::uint8_t
	{
		Left,
		Center,
		Right
	};

	enum class VerticalAlign : std::uint8_t
	{
		Top,
		Middle,
		Bottom
	};

	enum class MarkerShape : std::uint8_t
	{
		Circle,
		Square,
		Diamond,
		Triangle
	};

	enum class LineCap : std::uint8_t
	{
		Butt,
		Square,
		Round
	};

	enum class LineJoin : std::uint8_t
	{
		Miter,
		Bevel,
		Round
	};

	struct TextMeasurement
	{
		float width = 0.0f;
		float height = 0.0f;
		float ascent = 0.0f;
		float descent = 0.0f;
		float lineGap = 0.0f;
	};

	struct GlyphPlacement
	{
		std::uint32_t codepoint = 0;
		float x = 0.0f;
		float y = 0.0f;
	};

	struct LaidOutText
	{
		std::vector<GlyphPlacement> glyphs{};
		float width = 0.0f;
		float height = 0.0f;
		float ascent = 0.0f;
		float descent = 0.0f;
		float lineGap = 0.0f;
	};

	class ITextEngine
	{
	public:
		virtual ~ITextEngine() = default;
		virtual void registerFont(std::string_view familyName, const std::filesystem::path& ttfPath, std::uint16_t weight = 400) = 0;
		virtual bool hasFont(std::string_view familyName, std::uint16_t weight = 400) const = 0;
		virtual TextMeasurement measureText(
			std::string_view familyName,
			std::uint16_t weight,
			float fontSizePx,
			std::string_view text) const = 0;
		virtual LaidOutText layoutText(
			std::string_view familyName,
			std::uint16_t weight,
			float fontSizePx,
			std::string_view text,
			float maxWidth = std::numeric_limits<float>::infinity()) const = 0;
	};

	struct BoxCommand
	{
		RectF rect{};
		Color fill{255, 255, 255, 255};
		Color stroke{0, 0, 0, 255};
		float strokeWidth = 1.0f;
	};

	struct PolylineCommand
	{
		std::vector<PointF> points{};
		Color color{0, 0, 0, 255};
		float width = 1.0f;
		LineCap cap = LineCap::Butt;
		LineJoin join = LineJoin::Bevel;
	};

	struct TextCommand
	{
		RectF box{};
		std::string text{};
		Color color{0, 0, 0, 255};
		std::string fontFamily{"Default"};
		float fontSize = 12.0f;
		std::uint16_t fontWeight = 400;
		HorizontalAlign hAlign = HorizontalAlign::Left;
		VerticalAlign vAlign = VerticalAlign::Top;
		bool clipToBox = true;
	};

	struct MarkersCommand
	{
 		MarkerShape shape = MarkerShape::Circle;
		std::vector<PointF> positions{};
		std::vector<Color> fills{Color{0, 0, 255, 255}};
		Color stroke{0, 0, 0, 255};
		std::vector<float> sizes{6.0f};
		float strokeWidth = 1.0f;
	};

	struct PushClipRectCommand
	{
		RectF rect{};
	};

	struct PopClipRectCommand
	{
	};

	using RenderCommand = std::variant<
		BoxCommand,
		PolylineCommand,
		TextCommand,
		MarkersCommand,
		PushClipRectCommand,
		PopClipRectCommand>;

	struct RenderPlot
	{
		std::uint32_t width = 0;
		std::uint32_t height = 0;
		Color background{255, 255, 255, 255};
		std::vector<RenderCommand> commands{};
	};

} // namespace FlowPlot

#include "FlowPlot_Internal.hpp"

namespace FlowPlot
{

	class PlotBuilder
	{
	public:
		friend PlotBuilder& plot(const std::filesystem::path& path);

		PlotBuilder& set(std::string_view property, value valueArg)
		{
			if (property.empty())
				throw std::invalid_argument("set: property cannot be empty");

			rapidjson::Value normalizedValue;
			rapidjson::Document::AllocatorType& allocator = template_.GetAllocator();
			std::visit(
				[&](auto&& candidate)
				{
					using T = std::decay_t<decltype(candidate)>;
					if constexpr (std::is_same_v<T, std::string_view>)
					{
						normalizedValue.SetString(
							candidate.data(),
							static_cast<rapidjson::SizeType>(candidate.size()),
							allocator);
					}
					else if constexpr (std::is_same_v<T, const char*>)
					{
						const char* cstr = candidate == nullptr ? "" : candidate;
						normalizedValue.SetString(cstr, allocator);
					}
					else if constexpr (std::is_same_v<T, std::string>)
					{
						normalizedValue.SetString(
							candidate.c_str(),
							static_cast<rapidjson::SizeType>(candidate.size()),
							allocator);
					}
					else if constexpr (std::is_same_v<T, bool>)
					{
						normalizedValue.SetBool(candidate);
					}
					else if constexpr (std::is_floating_point_v<T>)
					{
						normalizedValue.SetDouble(static_cast<double>(candidate));
					}
					else
					{
						normalizedValue.SetInt64(static_cast<std::int64_t>(candidate));
					}
				},
				valueArg);

			return setJsonValue(property, std::move(normalizedValue));
		}

		PlotBuilder& setJsonRaw(std::string_view property, std::string_view jsonText)
		{
			if (property.empty())
				throw std::invalid_argument("setJsonRaw: property cannot be empty");

			rapidjson::Document parsedValue;
			parsedValue.Parse(jsonText.data(), jsonText.size());
			if (parsedValue.HasParseError())
			{
				throw std::runtime_error(
					"setJsonRaw: failed to parse JSON value at offset "
					+ std::to_string(parsedValue.GetErrorOffset())
					+ ": "
					+ rapidjson::GetParseError_En(parsedValue.GetParseError()));
			}

			rapidjson::Value normalizedValue;
			normalizedValue.CopyFrom(parsedValue, template_.GetAllocator());
			return setJsonValue(property, std::move(normalizedValue));
		}

	private:
		PlotBuilder& setJsonValue(std::string_view property, rapidjson::Value&& normalizedValue)
		{
			struct PathToken
			{
				std::string key;
				bool hasIndex = false;
				std::size_t index = 0;
			};

			auto parseToken = [&](std::string_view token) -> PathToken
			{
				if (token.empty())
					throw std::invalid_argument("set: invalid property path '" + std::string(property) + "'");

				PathToken parsed;
				const std::size_t open = token.find('[');
				if (open == std::string_view::npos)
				{
					parsed.key = std::string(token);
					return parsed;
				}

				const std::size_t close = token.find(']', open + 1);
				if (close == std::string_view::npos || close != token.size() - 1 || open == 0)
					throw std::invalid_argument("set: invalid token '" + std::string(token) + "'");

				parsed.key = std::string(token.substr(0, open));
				parsed.hasIndex = true;

				const std::string_view indexView = token.substr(open + 1, close - open - 1);
				if (indexView.empty())
					throw std::invalid_argument("set: missing array index in token '" + std::string(token) + "'");

				for (const char c : indexView)
				{
					if (c < '0' || c > '9')
						throw std::invalid_argument("set: array index must be numeric in token '" + std::string(token) + "'");

					parsed.index = (parsed.index * 10U) + static_cast<std::size_t>(c - '0');
				}

				return parsed;
			};

			if (!template_.IsObject())
				template_.SetObject();

			rapidjson::Document::AllocatorType& allocator = template_.GetAllocator();
			rapidjson::Value* current = &template_;
			std::size_t start = 0;

			while (start < property.size())
			{
				const std::size_t dot = property.find('.', start);
				const bool isLast = (dot == std::string_view::npos);
				const std::size_t tokenEnd = isLast ? property.size() : dot;
				const std::string_view tokenView = property.substr(start, tokenEnd - start);
				const PathToken token = parseToken(tokenView);

				if (!current->IsObject())
					current->SetObject();

				if (token.hasIndex)
				{
					auto memberIt = current->FindMember(token.key.c_str());
					if (memberIt == current->MemberEnd())
					{
						rapidjson::Value keyJson;
						keyJson.SetString(token.key.c_str(), static_cast<rapidjson::SizeType>(token.key.size()), allocator);
						rapidjson::Value arrayJson(rapidjson::kArrayType);
						current->AddMember(keyJson, arrayJson, allocator);
						memberIt = current->FindMember(token.key.c_str());
					}

					rapidjson::Value& arrayNode = memberIt->value;
					if (!arrayNode.IsArray())
						arrayNode.SetArray();

					std::size_t targetIndex = token.index;
					if (targetIndex >= arrayNode.Size())
					{
						targetIndex = arrayNode.Size();
						if (isLast)
						{
							arrayNode.PushBack(normalizedValue, allocator);
							return *this;
						}

						rapidjson::Value objectNode(rapidjson::kObjectType);
						arrayNode.PushBack(objectNode, allocator);
					}

					if (isLast)
					{
						arrayNode[static_cast<rapidjson::SizeType>(targetIndex)].Swap(normalizedValue);
						return *this;
					}

					rapidjson::Value& nextNode = arrayNode[static_cast<rapidjson::SizeType>(targetIndex)];
					if (!nextNode.IsObject())
						nextNode.SetObject();
					current = &nextNode;
				}
				else
				{
					auto memberIt = current->FindMember(token.key.c_str());
					if (memberIt == current->MemberEnd())
					{
						rapidjson::Value keyJson;
						keyJson.SetString(token.key.c_str(), static_cast<rapidjson::SizeType>(token.key.size()), allocator);
						rapidjson::Value initialNode;
						if (isLast)
							initialNode.SetNull();
						else
							initialNode.SetObject();
						current->AddMember(keyJson, initialNode, allocator);
						memberIt = current->FindMember(token.key.c_str());
					}

					if (isLast)
					{
						memberIt->value.Swap(normalizedValue);
						return *this;
					}

					rapidjson::Value& nextNode = memberIt->value;
					if (!nextNode.IsObject())
						nextNode.SetObject();
					current = &nextNode;
				}

				start = dot + 1;
				if (start > property.size())
					throw std::invalid_argument("set: invalid property path '" + std::string(property) + "'");
			}

			throw std::invalid_argument("set: invalid property path '" + std::string(property) + "'");
		}

	public:

		template<typename T>
		PlotBuilder& withData(std::string_view datasetField, std::span<const T> data)
		{
			if (datasetField.empty())
				throw std::invalid_argument("withData: datasetField cannot be empty");

			if (data.data() == nullptr && !data.empty())
				throw std::invalid_argument("withData: invalid span");

			const std::size_t dot = datasetField.find('.');
			if (dot == std::string_view::npos || dot == 0 || dot == datasetField.size() - 1)
				throw std::invalid_argument("withData: expected format 'dataset.field'");

			FlowInternal::DataKey key{
				std::string(datasetField.substr(0, dot)),
				std::string(datasetField.substr(dot + 1))
			};

			FlowInternal::DataView view;
			view.ptr = static_cast<const void*>(data.data());
			view.count = data.size();
			view.elemSize = sizeof(T);
			view.type = std::type_index(typeid(T));
			view.valueType = FlowInternal::inferValueType<T>();
			if (view.valueType == FlowInternal::DataView::ValueType::Unknown)
				throw std::invalid_argument("withData: unsupported data element type for '" + std::string(datasetField) + "'");

			data_[std::move(key)] = view;
			return *this;
		}

		template<typename T, typename Allocator>
		PlotBuilder& withData(std::string_view datasetField, const std::vector<T, Allocator>& data)
		{
			return withData<T>(datasetField, std::span<const T>(data.data(), data.size()));
		}

		PlotBuilder& useTextEngine(const ITextEngine& textEngine)
		{
			textEngine_ = &textEngine;
			return *this;
		}

		RenderPlot getCommands() const
		{
			Spec::MasterTemplateSpec 			     compiledTemplate = FlowInternal::compileTemplateToSpec(template_);
			FlowInternal::BoundIR::PlotBoundIR 		 bound = FlowInternal::buildBoundIR(compiledTemplate, data_);
			FlowInternal::ResolvedIR::PlotResolvedIR resolved = FlowInternal::resolvePlotIR(compiledTemplate, bound, textEngine_);
			return FlowInternal::buildRenderPlot(resolved);
		}

#ifdef FLOW_PLOT_RENDERER
		void writePng(const std::filesystem::path& outputPath) const;
#endif

	private:
		rapidjson::Document template_{};
		std::unordered_map<FlowInternal::DataKey, FlowInternal::DataView, FlowInternal::DataKeyHash> data_{};
		const ITextEngine* textEngine_ = nullptr;
	};

	inline PlotBuilder& plot(const std::filesystem::path& path)
	{
		std::filesystem::path jsonPath = path;
		if (jsonPath.extension() != ".json")
			jsonPath += ".json";

		std::ifstream input(jsonPath);
		if (!input.is_open())
			throw std::runtime_error("plot: unable to open template file '" + jsonPath.string() + "'");

		rapidjson::IStreamWrapper streamWrapper(input);
		rapidjson::Document parsedTemplate;
		parsedTemplate.ParseStream(streamWrapper);
		if (parsedTemplate.HasParseError())
		{
			throw std::runtime_error(
				"plot: failed to parse JSON template '"
				+ jsonPath.string()
				+ "' at offset "
				+ std::to_string(parsedTemplate.GetErrorOffset())
				+ ": "
				+ rapidjson::GetParseError_En(parsedTemplate.GetParseError()));
		}

		if (!parsedTemplate.IsObject())
			throw std::runtime_error("plot: template root must be a JSON object");

		static thread_local PlotBuilder builder;
		builder.template_.Swap(parsedTemplate);
		builder.data_.clear();
		builder.textEngine_ = nullptr;
		return builder;
	}

#ifdef FLOW_PLOT_COMPLETE_JSON
	inline std::string getCompleteJson(std::string_view templateJsonText, bool pretty = true)
	{
		rapidjson::Document templateJson;
		templateJson.Parse(templateJsonText.data(), templateJsonText.size());
		if (templateJson.HasParseError())
		{
			throw std::runtime_error(
				"getCompleteJson: failed to parse template JSON at offset "
				+ std::to_string(templateJson.GetErrorOffset())
				+ ": "
				+ rapidjson::GetParseError_En(templateJson.GetParseError()));
		}

		rapidjson::Document resolvedJson = FlowInternal::normalizeTemplateWithDefaults(templateJson);
		return FlowInternal::serializeJson(resolvedJson, pretty);
	}
#endif
} // namespace FlowPlot

#endif // FLOW_PLOT_HPP_INCLUDED

#ifdef FLOW_PLOT_RENDERER
#include "FlowPlot_Renderer.hpp"
#endif
