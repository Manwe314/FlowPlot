#pragma once

#ifndef FLOW_PLOT_RENDERER_HPP_INCLUDED
#define FLOW_PLOT_RENDERER_HPP_INCLUDED

#ifndef FLOW_PLOT_HPP_INCLUDED
#include "FlowPlot.hpp"
#endif

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iterator>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#ifndef FLOW_PLOT_DEFAULT_FONT_PATH
#define FLOW_PLOT_DEFAULT_FONT_PATH ""
#endif

#if defined(FLOW_PLOT_IMPLEMENTATION) && !defined(FLOW_PLOT_STB_EXTERNAL_IMPLEMENTATION)
#ifndef STBTT_STATIC
#define STBTT_STATIC
#define FLOW_PLOT_DEFINED_STBTT_STATIC
#endif
#ifndef STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_STATIC
#define FLOW_PLOT_DEFINED_STBIW_STATIC
#endif
#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define FLOW_PLOT_DEFINED_STBTT_IMPLEMENTATION
#endif
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define FLOW_PLOT_DEFINED_STBIW_IMPLEMENTATION
#endif
#endif

#include "stb_truetype.h"
#include "stb_image_write.h"

#ifdef FLOW_PLOT_DEFINED_STBTT_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION
#undef FLOW_PLOT_DEFINED_STBTT_IMPLEMENTATION
#endif
#ifdef FLOW_PLOT_DEFINED_STBIW_IMPLEMENTATION
#undef STB_IMAGE_WRITE_IMPLEMENTATION
#undef FLOW_PLOT_DEFINED_STBIW_IMPLEMENTATION
#endif
#ifdef FLOW_PLOT_DEFINED_STBTT_STATIC
#undef STBTT_STATIC
#undef FLOW_PLOT_DEFINED_STBTT_STATIC
#endif
#ifdef FLOW_PLOT_DEFINED_STBIW_STATIC
#undef STB_IMAGE_WRITE_STATIC
#undef FLOW_PLOT_DEFINED_STBIW_STATIC
#endif

namespace FlowPlot
{
	class StbTextEngine final : public ITextEngine
	{
	public:
		StbTextEngine()
		{
			const char* defaultFontPath = FLOW_PLOT_DEFAULT_FONT_PATH;
			if (defaultFontPath != nullptr && defaultFontPath[0] != '\0')
				registerFont("Default", std::filesystem::path(defaultFontPath), 400, FontStyle::Normal);
		}

		explicit StbTextEngine(const std::filesystem::path& defaultFontPath)
		{
			if (!defaultFontPath.empty())
				registerFont("Default", defaultFontPath, 400, FontStyle::Normal);
		}

		void registerFont(
			std::string_view familyName,
			const std::filesystem::path& ttfPath,
			std::uint16_t weight = 400,
			FontStyle style = FontStyle::Normal) override
		{
			if (familyName.empty())
				throw std::invalid_argument("StbTextEngine::registerFont: family name cannot be empty");

			std::ifstream input(ttfPath, std::ios::binary);
			if (!input.is_open())
				throw std::runtime_error("StbTextEngine::registerFont: unable to open font file '" + ttfPath.string() + "'");

			std::vector<unsigned char> fileData(
				(std::istreambuf_iterator<char>(input)),
				std::istreambuf_iterator<char>());

			if (fileData.empty())
				throw std::runtime_error("StbTextEngine::registerFont: font file is empty '" + ttfPath.string() + "'");

			FontFace face;
			face.fileData = std::move(fileData);

			const int fontOffset = stbtt_GetFontOffsetForIndex(face.fileData.data(), 0);
			if (fontOffset < 0 || stbtt_InitFont(&face.fontInfo, face.fileData.data(), fontOffset) == 0)
				throw std::runtime_error("StbTextEngine::registerFont: failed to initialize TTF '" + ttfPath.string() + "'");

			stbtt_GetFontVMetrics(&face.fontInfo, &face.ascent, &face.descent, &face.lineGap);

			const std::string canonicalFamily = canonicalizeFamily(familyName);
			fonts_[FontKey{canonicalFamily, weight, style}] = std::move(face);
		}

		bool hasFont(
			std::string_view familyName,
			std::uint16_t weight = 400,
			FontStyle style = FontStyle::Normal) const override
		{
			const std::string canonicalFamily = canonicalizeFamily(familyName);
			return fonts_.find(FontKey{canonicalFamily, weight, style}) != fonts_.end()
				|| fonts_.find(FontKey{canonicalFamily, weight, FontStyle::Normal}) != fonts_.end()
				|| fonts_.find(FontKey{canonicalFamily, 400, style}) != fonts_.end()
				|| fonts_.find(FontKey{canonicalFamily, 400, FontStyle::Normal}) != fonts_.end();
		}

		struct GlyphBitmap
		{
			int width = 0;
			int height = 0;
			int x0 = 0;
			int y0 = 0;
			float advance = 0.0f;
			std::vector<std::uint8_t> alpha{};
		};

		TextMeasurement measureText(
			std::string_view familyName,
			std::uint16_t weight,
			FontStyle style,
			float fontSizePx,
			std::string_view text) const override
		{
			const LaidOutText layout = layoutText(familyName, weight, style, fontSizePx, text);
			TextMeasurement measurement;
			measurement.width = layout.width;
			measurement.height = layout.height;
			measurement.ascent = layout.ascent;
			measurement.descent = layout.descent;
			measurement.lineGap = layout.lineGap;
			return measurement;
		}

		LaidOutText layoutText(
			std::string_view familyName,
			std::uint16_t weight,
			FontStyle style,
			float fontSizePx,
			std::string_view text,
			float maxWidth = std::numeric_limits<float>::infinity()) const override
		{
			if (fontSizePx <= 0.0f)
				throw std::invalid_argument("StbTextEngine::layoutText: fontSizePx must be positive");

			const FontFace& face = resolveFontWithFallback(familyName, weight, style);
			const float scale = stbtt_ScaleForPixelHeight(&face.fontInfo, fontSizePx);
			const float ascentPx = static_cast<float>(face.ascent) * scale;
			const float descentPx = static_cast<float>(face.descent) * scale;
			const float lineGapPx = static_cast<float>(face.lineGap) * scale;
			const float lineHeightPx = static_cast<float>(face.ascent - face.descent + face.lineGap) * scale;
			const bool wrapEnabled = std::isfinite(maxWidth) && maxWidth > 0.0f;

			LaidOutText layout;
			layout.ascent = ascentPx;
			layout.descent = descentPx;
			layout.lineGap = lineGapPx;

			float lineWidthPx = 0.0f;
			float maxLineWidthPx = 0.0f;
			float baselineY = ascentPx;
			std::size_t lineCount = 1;
			std::uint32_t previousCodepoint = 0;
			bool lineHasGlyph = false;

			const std::vector<std::uint32_t> codepoints = decodeUtf8(text);
			for (const std::uint32_t codepoint : codepoints)
			{
				if (codepoint == static_cast<std::uint32_t>('\n'))
				{
					maxLineWidthPx = std::max(maxLineWidthPx, lineWidthPx);
					lineWidthPx = 0.0f;
					previousCodepoint = 0;
					lineHasGlyph = false;
					baselineY += lineHeightPx;
					++lineCount;
					continue;
				}

				int advanceWidth = 0;
				int leftSideBearing = 0;
				stbtt_GetCodepointHMetrics(
					&face.fontInfo,
					static_cast<int>(codepoint),
					&advanceWidth,
					&leftSideBearing);
				(void)leftSideBearing;

				float kernAdvancePx = 0.0f;
				if (previousCodepoint != 0)
				{
					const int kernAdvance = stbtt_GetCodepointKernAdvance(
						&face.fontInfo,
						static_cast<int>(previousCodepoint),
						static_cast<int>(codepoint));
					kernAdvancePx = static_cast<float>(kernAdvance) * scale;
				}

				const float advancePx = static_cast<float>(advanceWidth) * scale;
				if (wrapEnabled && lineHasGlyph && (lineWidthPx + kernAdvancePx + advancePx) > maxWidth)
				{
					maxLineWidthPx = std::max(maxLineWidthPx, lineWidthPx);
					lineWidthPx = 0.0f;
					previousCodepoint = 0;
					lineHasGlyph = false;
					kernAdvancePx = 0.0f;
					baselineY += lineHeightPx;
					++lineCount;
				}

				const float glyphX = lineWidthPx + kernAdvancePx;
				layout.glyphs.push_back(GlyphPlacement{codepoint, glyphX, baselineY});

				lineWidthPx = glyphX + advancePx;
				maxLineWidthPx = std::max(maxLineWidthPx, lineWidthPx);
				previousCodepoint = codepoint;
				lineHasGlyph = true;
			}

			layout.width = maxLineWidthPx;
			layout.height = lineHeightPx * static_cast<float>(lineCount);
			return layout;
		}

		GlyphBitmap rasterizeGlyph(
			std::string_view familyName,
			std::uint16_t weight,
			FontStyle style,
			float fontSizePx,
			std::uint32_t codepoint) const
		{
			if (fontSizePx <= 0.0f)
				throw std::invalid_argument("StbTextEngine::rasterizeGlyph: fontSizePx must be positive");

			const FontFace& face = resolveFontWithFallback(familyName, weight, style);
			const float scale = stbtt_ScaleForPixelHeight(&face.fontInfo, fontSizePx);

			GlyphBitmap bitmap{};
			int advanceWidth = 0;
			int leftSideBearing = 0;
			stbtt_GetCodepointHMetrics(
				&face.fontInfo,
				static_cast<int>(codepoint),
				&advanceWidth,
				&leftSideBearing);
			(void)leftSideBearing;
			bitmap.advance = static_cast<float>(advanceWidth) * scale;

			int width = 0;
			int height = 0;
			int xoff = 0;
			int yoff = 0;
			unsigned char* raw = stbtt_GetCodepointBitmap(
				&face.fontInfo,
				scale,
				scale,
				static_cast<int>(codepoint),
				&width,
				&height,
				&xoff,
				&yoff);

			bitmap.width = width;
			bitmap.height = height;
			bitmap.x0 = xoff;
			bitmap.y0 = yoff;

			if (raw != nullptr && width > 0 && height > 0)
			{
				const std::size_t count = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
				bitmap.alpha.assign(raw, raw + count);
			}
			if (raw != nullptr)
				stbtt_FreeBitmap(raw, nullptr);

			return bitmap;
		}

	private:
		struct FontKey
		{
			std::string family;
			std::uint16_t weight = 400;
			FontStyle style = FontStyle::Normal;

			bool operator==(const FontKey& other) const
			{
				return family == other.family && weight == other.weight && style == other.style;
			}
		};

		struct FontKeyHash
		{
			std::size_t operator()(const FontKey& key) const noexcept
			{
				const std::size_t familyHash = std::hash<std::string>{}(key.family);
				const std::size_t weightHash = std::hash<std::uint16_t>{}(key.weight);
				const std::size_t styleHash = std::hash<std::uint8_t>{}(static_cast<std::uint8_t>(key.style));
				std::size_t h = familyHash;
				h ^= weightHash + 0x9e3779b97f4a7c15ULL + (h << 6U) + (h >> 2U);
				h ^= styleHash + 0x9e3779b97f4a7c15ULL + (h << 6U) + (h >> 2U);
				return h;
			}
		};

		struct FontFace
		{
			std::vector<unsigned char> fileData;
			stbtt_fontinfo fontInfo{};
			int ascent = 0;
			int descent = 0;
			int lineGap = 0;
		};

		static std::string canonicalizeFamily(std::string_view familyName)
		{
			std::string canonical(familyName);
			std::transform(canonical.begin(), canonical.end(), canonical.begin(), [](unsigned char c)
			{
				return static_cast<char>(std::tolower(c));
			});
			return canonical;
		}

		static std::vector<std::uint32_t> decodeUtf8(std::string_view text)
		{
			std::vector<std::uint32_t> out;
			out.reserve(text.size());

			const std::size_t n = text.size();
			std::size_t i = 0;
			while (i < n)
			{
				const std::uint8_t b0 = static_cast<std::uint8_t>(text[i]);
				if (b0 < 0x80U)
				{
					out.push_back(static_cast<std::uint32_t>(b0));
					++i;
					continue;
				}

				auto appendReplacement = [&]()
				{
					out.push_back(0xFFFDu);
					++i;
				};

				if ((b0 & 0xE0U) == 0xC0U)
				{
					if (i + 1 >= n)
					{
						appendReplacement();
						continue;
					}
					const std::uint8_t b1 = static_cast<std::uint8_t>(text[i + 1]);
					if ((b1 & 0xC0U) != 0x80U || b0 < 0xC2U)
					{
						appendReplacement();
						continue;
					}
					const std::uint32_t cp =
						(static_cast<std::uint32_t>(b0 & 0x1FU) << 6U)
						| static_cast<std::uint32_t>(b1 & 0x3FU);
					out.push_back(cp);
					i += 2;
					continue;
				}

				if ((b0 & 0xF0U) == 0xE0U)
				{
					if (i + 2 >= n)
					{
						appendReplacement();
						continue;
					}
					const std::uint8_t b1 = static_cast<std::uint8_t>(text[i + 1]);
					const std::uint8_t b2 = static_cast<std::uint8_t>(text[i + 2]);
					const bool validCont = ((b1 & 0xC0U) == 0x80U) && ((b2 & 0xC0U) == 0x80U);
					const bool overlong = (b0 == 0xE0U && b1 < 0xA0U);
					const bool surrogate = (b0 == 0xEDU && b1 >= 0xA0U);
					if (!validCont || overlong || surrogate)
					{
						appendReplacement();
						continue;
					}
					const std::uint32_t cp =
						(static_cast<std::uint32_t>(b0 & 0x0FU) << 12U)
						| (static_cast<std::uint32_t>(b1 & 0x3FU) << 6U)
						| static_cast<std::uint32_t>(b2 & 0x3FU);
					out.push_back(cp);
					i += 3;
					continue;
				}

				if ((b0 & 0xF8U) == 0xF0U)
				{
					if (i + 3 >= n)
					{
						appendReplacement();
						continue;
					}
					const std::uint8_t b1 = static_cast<std::uint8_t>(text[i + 1]);
					const std::uint8_t b2 = static_cast<std::uint8_t>(text[i + 2]);
					const std::uint8_t b3 = static_cast<std::uint8_t>(text[i + 3]);
					const bool validCont = ((b1 & 0xC0U) == 0x80U)
						&& ((b2 & 0xC0U) == 0x80U)
						&& ((b3 & 0xC0U) == 0x80U);
					const bool overlong = (b0 == 0xF0U && b1 < 0x90U);
					const bool outOfRange = (b0 > 0xF4U) || (b0 == 0xF4U && b1 >= 0x90U);
					if (!validCont || overlong || outOfRange)
					{
						appendReplacement();
						continue;
					}
					const std::uint32_t cp =
						(static_cast<std::uint32_t>(b0 & 0x07U) << 18U)
						| (static_cast<std::uint32_t>(b1 & 0x3FU) << 12U)
						| (static_cast<std::uint32_t>(b2 & 0x3FU) << 6U)
						| static_cast<std::uint32_t>(b3 & 0x3FU);
					out.push_back(cp);
					i += 4;
					continue;
				}

				appendReplacement();
			}

			return out;
		}

		const FontFace& resolveFontWithFallback(std::string_view familyName, std::uint16_t weight, FontStyle style) const
		{
			const std::string canonicalFamily = canonicalizeFamily(familyName);
			auto it = fonts_.find(FontKey{canonicalFamily, weight, style});
			if (it != fonts_.end())
				return it->second;

			it = fonts_.find(FontKey{canonicalFamily, weight, FontStyle::Normal});
			if (it != fonts_.end())
				return it->second;

			it = fonts_.find(FontKey{canonicalFamily, 400, style});
			if (it != fonts_.end())
				return it->second;

			it = fonts_.find(FontKey{canonicalFamily, 400, FontStyle::Normal});
			if (it != fonts_.end())
				return it->second;

			it = fonts_.find(FontKey{kDefaultFamilyName, weight, style});
			if (it != fonts_.end())
				return it->second;

			it = fonts_.find(FontKey{kDefaultFamilyName, weight, FontStyle::Normal});
			if (it != fonts_.end())
				return it->second;

			it = fonts_.find(FontKey{kDefaultFamilyName, 400, style});
			if (it != fonts_.end())
				return it->second;

			it = fonts_.find(FontKey{kDefaultFamilyName, 400, FontStyle::Normal});
			if (it != fonts_.end())
				return it->second;

			throw std::runtime_error(
				"StbTextEngine: no registered font for family '" + std::string(familyName) +
				"', weight " + std::to_string(weight) +
				", style '" + fontStyleName(style) +
				"', and no registered 'Default' fallback");
		}

		static constexpr const char* kDefaultFamilyName = "default";
		std::unordered_map<FontKey, FontFace, FontKeyHash> fonts_;
	};

	class CpuRenderer
	{
	public:
		struct ImageRgba8
		{
			std::uint32_t width = 0;
			std::uint32_t height = 0;
			std::vector<std::uint8_t> pixels{};
		};

		ImageRgba8 render(const RenderPlot& plot, const ITextEngine* textEngine = nullptr) const
		{
			ImageRgba8 image;
			image.width = plot.width;
			image.height = plot.height;
			if (image.width == 0 || image.height == 0)
				return image;

			const std::size_t pixelCount = static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height) * 4U;
			image.pixels.resize(pixelCount, 0);
			fillBackground(image, plot.background);

			std::vector<ClipRectI> clipStack;
			GlyphCache glyphCache;
			auto currentClip = [&]() -> const ClipRectI*
			{
				return clipStack.empty() ? nullptr : &clipStack.back();
			};

			for (const RenderCommand& cmd : plot.commands)
			{
				std::visit(
					[&](const auto& concreteCmd)
					{
						using T = std::decay_t<decltype(concreteCmd)>;
						if constexpr (std::is_same_v<T, BoxCommand>)
							drawBox(image, concreteCmd, currentClip());
						else if constexpr (std::is_same_v<T, PolylineCommand>)
							drawPolyline(image, concreteCmd, currentClip());
						else if constexpr (std::is_same_v<T, TextCommand>)
						{
							if (textEngine != nullptr)
								drawText(image, concreteCmd, *textEngine, glyphCache, currentClip());
						}
						else if constexpr (std::is_same_v<T, MarkersCommand>)
							drawMarkers(image, concreteCmd, currentClip());
						else if constexpr (std::is_same_v<T, PushClipRectCommand>)
						{
							const std::optional<ClipRectI> pushed = makeClipRect(image, concreteCmd.rect);
							if (!pushed.has_value())
							{
								clipStack.push_back(ClipRectI{0, 0, 0, 0});
								return;
							}

							if (!clipStack.empty())
							{
								const std::optional<ClipRectI> intersected = intersectClip(clipStack.back(), *pushed);
								clipStack.push_back(intersected.has_value() ? *intersected : ClipRectI{0, 0, 0, 0});
							}
							else
							{
								clipStack.push_back(*pushed);
							}
						}
						else if constexpr (std::is_same_v<T, PopClipRectCommand>)
						{
							if (!clipStack.empty())
								clipStack.pop_back();
						}
					},
					cmd);
			}

			return image;
		}

		void writePng(const std::filesystem::path& path, const ImageRgba8& image) const
		{
			if (image.width == 0 || image.height == 0 || image.pixels.empty())
				throw std::runtime_error("CpuRenderer::writePng: image buffer is empty");

			const int stride = static_cast<int>(image.width * 4U);
			const int result = stbi_write_png(
				path.string().c_str(),
				static_cast<int>(image.width),
				static_cast<int>(image.height),
				4,
				image.pixels.data(),
				stride);

			if (result == 0)
				throw std::runtime_error("CpuRenderer::writePng: failed to write '" + path.string() + "'");
		}

		void writePng(const std::filesystem::path& path, const RenderPlot& plot, const ITextEngine* textEngine = nullptr) const
		{
			const ImageRgba8 image = render(plot, textEngine);
			writePng(path, image);
		}

	private:
		struct ClipRectI
		{
			int x0 = 0;
			int y0 = 0;
			int x1 = 0;
			int y1 = 0;
		};

		struct GlyphKey
		{
			const StbTextEngine* engine = nullptr;
			std::string family{};
			std::uint16_t weight = 400;
			FontStyle style = FontStyle::Normal;
			std::uint32_t codepoint = 0;
			float fontSizePx = 0.0f;
			std::uint8_t subX = 0;

			bool operator==(const GlyphKey& other) const noexcept
			{
				return engine == other.engine
					&& family == other.family
					&& weight == other.weight
					&& style == other.style
					&& codepoint == other.codepoint
					&& fontSizePx == other.fontSizePx
					&& subX == other.subX;
			}
		};

		struct GlyphKeyHash
		{
			std::size_t operator()(const GlyphKey& key) const noexcept
			{
				std::size_t h = std::hash<const StbTextEngine*>{}(key.engine);
				h ^= std::hash<std::string>{}(key.family) + 0x9e3779b97f4a7c15ULL + (h << 6U) + (h >> 2U);
				h ^= std::hash<std::uint16_t>{}(key.weight) + 0x9e3779b97f4a7c15ULL + (h << 6U) + (h >> 2U);
				h ^= std::hash<std::uint8_t>{}(static_cast<std::uint8_t>(key.style)) + 0x9e3779b97f4a7c15ULL + (h << 6U) + (h >> 2U);
				h ^= std::hash<std::uint32_t>{}(key.codepoint) + 0x9e3779b97f4a7c15ULL + (h << 6U) + (h >> 2U);
				h ^= std::hash<float>{}(key.fontSizePx) + 0x9e3779b97f4a7c15ULL + (h << 6U) + (h >> 2U);
				h ^= std::hash<std::uint8_t>{}(key.subX) + 0x9e3779b97f4a7c15ULL + (h << 6U) + (h >> 2U);
				return h;
			}
		};

		using GlyphCache = std::unordered_map<GlyphKey, StbTextEngine::GlyphBitmap, GlyphKeyHash>;

		static std::uint8_t clampChannel(int value)
		{
			if (value < 0)
				return 0;
			if (value > 255)
				return 255;
			return static_cast<std::uint8_t>(value);
		}

		static void fillBackground(ImageRgba8& image, Color color)
		{
			for (std::size_t i = 0; i < image.pixels.size(); i += 4U)
			{
				image.pixels[i + 0U] = color.r;
				image.pixels[i + 1U] = color.g;
				image.pixels[i + 2U] = color.b;
				image.pixels[i + 3U] = color.a;
			}
		}

		static std::optional<ClipRectI> makeClipRect(const ImageRgba8& image, const RectF& rect)
		{
			const int x0 = std::max(0, static_cast<int>(std::floor(rect.x)));
			const int y0 = std::max(0, static_cast<int>(std::floor(rect.y)));
			const int x1 = std::min(static_cast<int>(image.width), static_cast<int>(std::ceil(rect.x + rect.w)));
			const int y1 = std::min(static_cast<int>(image.height), static_cast<int>(std::ceil(rect.y + rect.h)));
			if (x0 >= x1 || y0 >= y1)
				return std::nullopt;
			return ClipRectI{x0, y0, x1, y1};
		}

		static std::optional<ClipRectI> intersectClip(const ClipRectI& a, const ClipRectI& b)
		{
			const ClipRectI out{
				std::max(a.x0, b.x0),
				std::max(a.y0, b.y0),
				std::min(a.x1, b.x1),
				std::min(a.y1, b.y1)};
			if (out.x0 >= out.x1 || out.y0 >= out.y1)
				return std::nullopt;
			return out;
		}

		static bool inClip(const ClipRectI* clip, int x, int y)
		{
			if (clip == nullptr)
				return true;
			return x >= clip->x0 && x < clip->x1 && y >= clip->y0 && y < clip->y1;
		}

		static void blendPixel(ImageRgba8& image, int x, int y, Color color, const ClipRectI* clip = nullptr)
		{
			if (x < 0 || y < 0 || x >= static_cast<int>(image.width) || y >= static_cast<int>(image.height))
				return;
			if (!inClip(clip, x, y))
				return;

			const std::size_t index = (static_cast<std::size_t>(y) * static_cast<std::size_t>(image.width) + static_cast<std::size_t>(x)) * 4U;
			const float srcAlpha = static_cast<float>(color.a) / 255.0f;
			const float dstAlpha = static_cast<float>(image.pixels[index + 3U]) / 255.0f;
			const float outAlpha = srcAlpha + dstAlpha * (1.0f - srcAlpha);

			const auto blendChannel = [&](std::uint8_t src, std::uint8_t dst) -> std::uint8_t
			{
				if (outAlpha <= 0.0f)
					return 0;

				const float srcNorm = static_cast<float>(src) / 255.0f;
				const float dstNorm = static_cast<float>(dst) / 255.0f;
				const float outNorm = (srcNorm * srcAlpha + dstNorm * dstAlpha * (1.0f - srcAlpha)) / outAlpha;
				return clampChannel(static_cast<int>(std::round(outNorm * 255.0f)));
			};

			image.pixels[index + 0U] = blendChannel(color.r, image.pixels[index + 0U]);
			image.pixels[index + 1U] = blendChannel(color.g, image.pixels[index + 1U]);
			image.pixels[index + 2U] = blendChannel(color.b, image.pixels[index + 2U]);
			image.pixels[index + 3U] = clampChannel(static_cast<int>(std::round(outAlpha * 255.0f)));
		}

		static void fillRect(ImageRgba8& image, const RectF& rect, Color color, const ClipRectI* clip = nullptr)
		{
			const int x0 = static_cast<int>(std::floor(rect.x));
			const int y0 = static_cast<int>(std::floor(rect.y));
			const int x1 = static_cast<int>(std::ceil(rect.x + rect.w));
			const int y1 = static_cast<int>(std::ceil(rect.y + rect.h));

			for (int y = y0; y < y1; ++y)
			{
				for (int x = x0; x < x1; ++x)
					blendPixel(image, x, y, color, clip);
			}
		}

		static void strokeRect(ImageRgba8& image, const RectF& rect, Color color, float strokeWidth, const ClipRectI* clip = nullptr)
		{
			if (strokeWidth <= 0.0f)
				return;

			const int thickness = std::max(1, static_cast<int>(std::round(strokeWidth)));
			const int x0 = static_cast<int>(std::floor(rect.x));
			const int y0 = static_cast<int>(std::floor(rect.y));
			const int x1 = static_cast<int>(std::ceil(rect.x + rect.w));
			const int y1 = static_cast<int>(std::ceil(rect.y + rect.h));

			for (int t = 0; t < thickness; ++t)
			{
				for (int x = x0; x < x1; ++x)
				{
					blendPixel(image, x, y0 + t, color, clip);
					blendPixel(image, x, y1 - 1 - t, color, clip);
				}

				for (int y = y0; y < y1; ++y)
				{
					blendPixel(image, x0 + t, y, color, clip);
					blendPixel(image, x1 - 1 - t, y, color, clip);
				}
			}
		}

		static void drawLine(ImageRgba8& image, PointF start, PointF end, Color color, float width, const ClipRectI* clip = nullptr)
		{
			const float dx = end.x - start.x;
			const float dy = end.y - start.y;
			const float stepsF = std::max(std::fabs(dx), std::fabs(dy));
			const int steps = std::max(1, static_cast<int>(std::ceil(stepsF)));
			const int radius = std::max(0, static_cast<int>(std::round(width * 0.5f)));

			for (int i = 0; i <= steps; ++i)
			{
				const float t = static_cast<float>(i) / static_cast<float>(steps);
				const float x = start.x + dx * t;
				const float y = start.y + dy * t;
				const int cx = static_cast<int>(std::round(x));
				const int cy = static_cast<int>(std::round(y));

				for (int oy = -radius; oy <= radius; ++oy)
				{
					for (int ox = -radius; ox <= radius; ++ox)
						blendPixel(image, cx + ox, cy + oy, color, clip);
				}
			}
		}

		static void drawBox(ImageRgba8& image, const BoxCommand& cmd, const ClipRectI* clip = nullptr)
		{
			fillRect(image, cmd.rect, cmd.fill, clip);
			strokeRect(image, cmd.rect, cmd.stroke, cmd.strokeWidth, clip);
		}

		static void drawPolyline(ImageRgba8& image, const PolylineCommand& cmd, const ClipRectI* clip = nullptr)
		{
			if (cmd.points.size() < 2U)
				return;

			for (std::size_t i = 1; i < cmd.points.size(); ++i)
				drawLine(image, cmd.points[i - 1], cmd.points[i], cmd.color, cmd.width, clip);
		}

		static void drawMarkers(ImageRgba8& image, const MarkersCommand& cmd, const ClipRectI* clip = nullptr)
		{
			if (cmd.positions.empty())
				return;

			if (cmd.fills.empty())
				throw std::runtime_error("CpuRenderer::drawMarkers: 'fills' must contain at least one color");
			if (cmd.sizes.empty())
				throw std::runtime_error("CpuRenderer::drawMarkers: 'sizes' must contain at least one size");

			const bool broadcastFill = (cmd.fills.size() == 1U);
			const bool broadcastSize = (cmd.sizes.size() == 1U);
			if (!broadcastFill && cmd.fills.size() != cmd.positions.size())
			{
				throw std::runtime_error(
					"CpuRenderer::drawMarkers: 'fills' must have size 1 or match 'positions' size");
			}
			if (!broadcastSize && cmd.sizes.size() != cmd.positions.size())
			{
				throw std::runtime_error(
					"CpuRenderer::drawMarkers: 'sizes' must have size 1 or match 'positions' size");
			}

			for (std::size_t idx = 0; idx < cmd.positions.size(); ++idx)
			{
				const PointF& position = cmd.positions[idx];
				const int cx = static_cast<int>(std::round(position.x));
				const int cy = static_cast<int>(std::round(position.y));

				const float markerSize = broadcastSize ? cmd.sizes[0] : cmd.sizes[idx];
				const float radius = std::max(0.5f, markerSize * 0.5f);
				const int radiusI = static_cast<int>(std::ceil(radius));

				const Color fillColor = broadcastFill ? cmd.fills[0] : cmd.fills[idx];

				if (cmd.shape == MarkerShape::Square)
				{
					RectF square;
					square.x = static_cast<float>(cx) - radius;
					square.y = static_cast<float>(cy) - radius;
					square.w = radius * 2.0f;
					square.h = radius * 2.0f;
					fillRect(image, square, fillColor, clip);
					strokeRect(image, square, cmd.stroke, cmd.strokeWidth, clip);
					continue;
				}

				for (int y = -radiusI; y <= radiusI; ++y)
				{
					for (int x = -radiusI; x <= radiusI; ++x)
					{
						bool inside = false;
						if (cmd.shape == MarkerShape::Circle)
						{
							const float dist2 = static_cast<float>(x * x + y * y);
							inside = dist2 <= (radius * radius);
						}
						else if (cmd.shape == MarkerShape::Diamond)
						{
							inside = (std::abs(x) + std::abs(y)) <= radiusI;
						}
						else
						{
							inside = (y >= -radiusI) && (y <= radiusI)
								&& (std::abs(x) <= (radiusI - (y + radiusI) / 2));
						}

						if (inside)
							blendPixel(image, cx + x, cy + y, fillColor, clip);
					}
				}
			}
		}

		static const StbTextEngine::GlyphBitmap& getGlyphBitmap(
			GlyphCache& cache,
			const StbTextEngine& engine,
			const TextCommand& cmd,
			std::uint32_t codepoint)
		{
			GlyphKey key;
			key.engine = &engine;
			key.family = cmd.fontFamily;
			key.weight = cmd.fontWeight;
			key.style = cmd.fontStyle;
			key.codepoint = codepoint;
			key.fontSizePx = cmd.fontSize;
			key.subX = 0;

			auto it = cache.find(key);
			if (it != cache.end())
				return it->second;

			StbTextEngine::GlyphBitmap bitmap = engine.rasterizeGlyph(
				cmd.fontFamily,
				cmd.fontWeight,
				cmd.fontStyle,
				cmd.fontSize,
				codepoint);
			const auto inserted = cache.emplace(std::move(key), std::move(bitmap));
			return inserted.first->second;
		}

		static void blendGlyphMask(
			ImageRgba8& image,
			int dstX,
			int dstY,
			const StbTextEngine::GlyphBitmap& bitmap,
			Color color,
			const ClipRectI* clip)
		{
			if (bitmap.width <= 0 || bitmap.height <= 0 || bitmap.alpha.empty() || color.a == 0)
				return;

			for (int y = 0; y < bitmap.height; ++y)
			{
				for (int x = 0; x < bitmap.width; ++x)
				{
					const std::size_t idx = static_cast<std::size_t>(y) * static_cast<std::size_t>(bitmap.width)
						+ static_cast<std::size_t>(x);
					const std::uint8_t coverage = bitmap.alpha[idx];
					if (coverage == 0)
						continue;

					const float alphaScale = static_cast<float>(coverage) / 255.0f;
					Color sampled = color;
					sampled.a = clampChannel(static_cast<int>(std::lround(static_cast<float>(color.a) * alphaScale)));
					if (sampled.a == 0)
						continue;

					blendPixel(image, dstX + x, dstY + y, sampled, clip);
				}
			}
		}

		static void drawText(
			ImageRgba8& image,
			const TextCommand& cmd,
			const ITextEngine& engine,
			GlyphCache& glyphCache,
			const ClipRectI* clip)
		{
			if (cmd.text.empty())
				return;

			const LaidOutText layout = engine.layoutText(
				cmd.fontFamily,
				cmd.fontWeight,
				cmd.fontStyle,
				cmd.fontSize,
				cmd.text);

			float originX = cmd.box.x;
			if (cmd.hAlign == HorizontalAlign::Center)
				originX = cmd.box.x + (cmd.box.w - layout.width) * 0.5f;
			else if (cmd.hAlign == HorizontalAlign::Right)
				originX = cmd.box.x + (cmd.box.w - layout.width);

			float originY = cmd.box.y;
			if (cmd.vAlign == VerticalAlign::Middle)
				originY = cmd.box.y + (cmd.box.h - layout.height) * 0.5f;
			else if (cmd.vAlign == VerticalAlign::Bottom)
				originY = cmd.box.y + (cmd.box.h - layout.height);

			const std::optional<ClipRectI> textBoxClip = cmd.clipToBox ? makeClipRect(image, cmd.box) : std::optional<ClipRectI>{};
			std::optional<ClipRectI> combinedClip;
			if (clip != nullptr && textBoxClip.has_value())
				combinedClip = intersectClip(*clip, *textBoxClip);
			else if (clip != nullptr)
				combinedClip = *clip;
			else if (textBoxClip.has_value())
				combinedClip = *textBoxClip;

			const ClipRectI* finalClip = combinedClip.has_value() ? &(*combinedClip) : nullptr;
			if ((clip != nullptr || textBoxClip.has_value()) && finalClip == nullptr)
				return;

			const StbTextEngine* stbEngine = dynamic_cast<const StbTextEngine*>(&engine);
			if (stbEngine == nullptr)
			{
				strokeRect(image, cmd.box, cmd.color, 1.0f, finalClip);
				return;
			}

			for (const GlyphPlacement& glyph : layout.glyphs)
			{
				const StbTextEngine::GlyphBitmap& bitmap = getGlyphBitmap(glyphCache, *stbEngine, cmd, glyph.codepoint);
				const int dstX = static_cast<int>(std::lround(originX + glyph.x)) + bitmap.x0;
				const int dstY = static_cast<int>(std::lround(originY + glyph.y)) + bitmap.y0;
				blendGlyphMask(image, dstX, dstY, bitmap, cmd.color, finalClip);
			}
		}
	};

	inline RenderPlot PlotBuilder::getCommands() const
	{
		const ITextEngine* activeTextEngine = textEngine_;
		std::optional<StbTextEngine> fallbackEngine;
		if (activeTextEngine == nullptr)
		{
			fallbackEngine.emplace();
			registerFonts(*fallbackEngine, template_);
			activeTextEngine = &(*fallbackEngine);
		}

		Spec::MasterTemplateSpec compiledTemplate = FlowInternal::compileTemplateToSpec(template_);
		FlowInternal::BoundIR::PlotBoundIR bound = FlowInternal::buildBoundIR(compiledTemplate, data_);
		FlowInternal::ResolvedIR::PlotResolvedIR resolved =
			FlowInternal::resolvePlotIR(compiledTemplate, bound, activeTextEngine);
		return FlowInternal::buildRenderPlot(resolved);
	}

	inline void PlotBuilder::writePng(const std::filesystem::path& outputPath) const
	{
		const ITextEngine* activeTextEngine = textEngine_;
		std::optional<StbTextEngine> fallbackEngine;
		if (activeTextEngine == nullptr)
		{
			fallbackEngine.emplace();
			registerFonts(*fallbackEngine, template_);
			activeTextEngine = &(*fallbackEngine);
		}

		Spec::MasterTemplateSpec compiledTemplate = FlowInternal::compileTemplateToSpec(template_);
		FlowInternal::BoundIR::PlotBoundIR bound = FlowInternal::buildBoundIR(compiledTemplate, data_);
		FlowInternal::ResolvedIR::PlotResolvedIR resolved =
			FlowInternal::resolvePlotIR(compiledTemplate, bound, activeTextEngine);
		const RenderPlot renderPlot = FlowInternal::buildRenderPlot(resolved);

		CpuRenderer renderer;
		renderer.writePng(outputPath, renderPlot, activeTextEngine);
	}
} // namespace FlowPlot

#endif // FLOW_PLOT_RENDERER_HPP_INCLUDED
