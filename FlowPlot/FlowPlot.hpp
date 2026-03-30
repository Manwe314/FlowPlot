#pragma once

#ifndef FLOW_PLOT_HPP_INCLUDED
#define FLOW_PLOT_HPP_INCLUDED

#include "json.hpp"
#include "FlowPlot_Defaults.hpp"

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
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace FlowPlot
{
	using json = nlohmann::json;
	using value = std::variant<int, float, double, json, const char*, std::string, std::string_view>;

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

			json normalizedValue = std::visit(
				[](auto&& candidate) -> json
				{
					using T = std::decay_t<decltype(candidate)>;
					if constexpr (std::is_same_v<T, std::string_view>)
						return json(std::string(candidate));
					else if constexpr (std::is_same_v<T, const char*>)
						return json(candidate == nullptr ? std::string() : std::string(candidate));
					else
						return json(candidate);
				},
				valueArg);

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

			if (!template_.is_object())
				template_ = json::object();

			json* current = &template_;
			std::size_t start = 0;

			while (start < property.size())
			{
				const std::size_t dot = property.find('.', start);
				const bool isLast = (dot == std::string_view::npos);
				const std::size_t tokenEnd = isLast ? property.size() : dot;
				const std::string_view tokenView = property.substr(start, tokenEnd - start);
				const PathToken token = parseToken(tokenView);

				if (!current->is_object())
					*current = json::object();

				if (token.hasIndex)
				{
					json& arrayNode = (*current)[token.key];
					if (!arrayNode.is_array())
						arrayNode = json::array();

					std::size_t targetIndex = token.index;
					if (targetIndex >= arrayNode.size())
					{
						targetIndex = arrayNode.size();
						if (isLast)
						{
							arrayNode.push_back(std::move(normalizedValue));
							return *this;
						}

						arrayNode.push_back(json::object());
					}

					if (isLast)
					{
						arrayNode[targetIndex] = std::move(normalizedValue);
						return *this;
					}

					json& nextNode = arrayNode[targetIndex];
					if (!nextNode.is_object())
						nextNode = json::object();
					current = &nextNode;
				}
				else
				{
					if (isLast)
					{
						(*current)[token.key] = std::move(normalizedValue);
						return *this;
					}

					json& nextNode = (*current)[token.key];
					if (!nextNode.is_object())
						nextNode = json::object();
					current = &nextNode;
				}

				start = dot + 1;
				if (start > property.size())
					throw std::invalid_argument("set: invalid property path '" + std::string(property) + "'");
			}

			throw std::invalid_argument("set: invalid property path '" + std::string(property) + "'");
		}

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
		json template_{};
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

		json parsedTemplate;
		try
		{
			input >> parsedTemplate;
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("plot: failed to parse JSON template '" + jsonPath.string() + "': " + e.what());
		}

		if (!parsedTemplate.is_object())
			throw std::runtime_error("plot: template root must be a JSON object");

		static thread_local PlotBuilder builder;
		builder.template_ = std::move(parsedTemplate);
		builder.data_.clear();
		builder.textEngine_ = nullptr;
		return builder;
	}

#ifdef FLOW_PLOT_COMPLETE_JSON
	inline json getCompleteJson(const json& templateJson)
	{
		return FlowInternal::normalizeTemplateWithDefaults(json(templateJson));
	}
#endif
} // namespace FlowPlot

#endif // FLOW_PLOT_HPP_INCLUDED

#ifdef FLOW_PLOT_RENDERER
#include "FlowPlot_Renderer.hpp"
#endif
