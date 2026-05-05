#include "fontImport.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string_view>

#if defined(_WIN32)
#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__APPLE__)
#define GLFW_EXPOSE_NATIVE_COCOA
#elif defined(__linux__)
#define GLFW_EXPOSE_NATIVE_X11
#endif

#include <nfd_glfw3.h>

#define STBTT_STATIC
#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define FLOWPLOTGUI_UNDEF_STB_TRUETYPE_IMPLEMENTATION
#endif
#include "stb_truetype.h"
#ifdef FLOWPLOTGUI_UNDEF_STB_TRUETYPE_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION
#undef FLOWPLOTGUI_UNDEF_STB_TRUETYPE_IMPLEMENTATION
#endif

namespace FlowPlotGui {
namespace {

struct ParsedFontFace {
	AddedFontVariant variant{};
};

std::string toLowerAscii(std::string value)
{
	for (char& c : value)
	{
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	}
	return value;
}

bool containsAscii(std::string_view haystack, std::string_view needle)
{
	return toLowerAscii(std::string(haystack)).find(needle) != std::string::npos;
}

std::string trimAscii(std::string value)
{
	const auto isSpace = [](unsigned char c) { return std::isspace(c) != 0; };
	const auto first = std::find_if_not(value.begin(), value.end(), isSpace);
	const auto last = std::find_if_not(value.rbegin(), value.rend(), isSpace).base();
	if (first >= last)
	{
		return {};
	}
	return std::string(first, last);
}

void appendUtf8(std::string& out, std::uint32_t codepoint)
{
	if (codepoint <= 0x7Fu)
	{
		out.push_back(static_cast<char>(codepoint));
	}
	else if (codepoint <= 0x7FFu)
	{
		out.push_back(static_cast<char>(0xC0u | (codepoint >> 6u)));
		out.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
	}
	else
	{
		out.push_back(static_cast<char>(0xE0u | (codepoint >> 12u)));
		out.push_back(static_cast<char>(0x80u | ((codepoint >> 6u) & 0x3Fu)));
		out.push_back(static_cast<char>(0x80u | (codepoint & 0x3Fu)));
	}
}

std::string decodeFontName(const char* raw, int length, bool utf16BigEndian)
{
	if (raw == nullptr || length <= 0)
	{
		return {};
	}

	std::string decoded{};
	if (utf16BigEndian)
	{
		for (int i = 0; i + 1 < length; i += 2)
		{
			const std::uint32_t codepoint =
				(static_cast<std::uint8_t>(raw[i]) << 8u) |
				static_cast<std::uint8_t>(raw[i + 1]);
			if (codepoint != 0u)
			{
				appendUtf8(decoded, codepoint);
			}
		}
	}
	else
	{
		decoded.assign(raw, raw + length);
	}
	return trimAscii(decoded);
}

std::string fontNameString(const stbtt_fontinfo& font, int nameId)
{
	struct Candidate {
		int platform;
		int encoding;
		int language;
		bool utf16BigEndian;
	};

	constexpr std::array<Candidate, 6> kCandidates{{
		{3, 10, 0x0409, true},
		{3, 1, 0x0409, true},
		{3, 10, 0, true},
		{3, 1, 0, true},
		{0, 4, 0, true},
		{1, 0, 0, false},
	}};

	for (const Candidate candidate : kCandidates)
	{
		int length = 0;
		const char* raw = stbtt_GetFontNameString(
			&font,
			&length,
			candidate.platform,
			candidate.encoding,
			candidate.language,
			nameId);
		std::string decoded = decodeFontName(raw, length, candidate.utf16BigEndian);
		if (!decoded.empty())
		{
			return decoded;
		}
	}
	return {};
}

std::uint16_t readU16Be(const unsigned char* data)
{
	return static_cast<std::uint16_t>((data[0] << 8u) | data[1]);
}

std::uint16_t weightFromSubfamily(std::string_view subfamily)
{
	const std::string lower = toLowerAscii(std::string(subfamily));
	if (lower.find("thin") != std::string::npos)
	{
		return 100;
	}
	if (lower.find("extra light") != std::string::npos || lower.find("extralight") != std::string::npos
		|| lower.find("ultra light") != std::string::npos || lower.find("ultralight") != std::string::npos)
	{
		return 200;
	}
	if (lower.find("light") != std::string::npos)
	{
		return 300;
	}
	if (lower.find("medium") != std::string::npos)
	{
		return 500;
	}
	if (lower.find("semi bold") != std::string::npos || lower.find("semibold") != std::string::npos
		|| lower.find("demi bold") != std::string::npos || lower.find("demibold") != std::string::npos)
	{
		return 600;
	}
	if (lower.find("extra bold") != std::string::npos || lower.find("extrabold") != std::string::npos
		|| lower.find("ultra bold") != std::string::npos || lower.find("ultrabold") != std::string::npos)
	{
		return 800;
	}
	if (lower.find("black") != std::string::npos || lower.find("heavy") != std::string::npos)
	{
		return 900;
	}
	if (lower.find("bold") != std::string::npos)
	{
		return 700;
	}
	return 400;
}

std::string familyFromFilename(const std::filesystem::path& path)
{
	std::string name = path.stem().string();
	const std::array<std::string_view, 13> styleTokens = {
		"-Regular", "-Normal", "-Book", "-Medium", "-Light", "-Thin", "-Bold",
		"-SemiBold", "-DemiBold", "-ExtraBold", "-Black", "-Italic", "-Oblique",
	};
	for (const std::string_view token : styleTokens)
	{
		const auto pos = name.find(token);
		if (pos != std::string::npos)
		{
			name.erase(pos, token.size());
		}
	}
	return trimAscii(name.empty() ? path.stem().string() : name);
}

std::vector<unsigned char> readFileBytes(const std::filesystem::path& path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
	{
		throw std::runtime_error("could not open file");
	}
	return std::vector<unsigned char>(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>());
}

std::vector<ParsedFontFace> parseFontFile(const std::filesystem::path& path)
{
	std::vector<unsigned char> bytes = readFileBytes(path);
	if (bytes.empty())
	{
		throw std::runtime_error("file is empty");
	}

	const int fontCount = std::max(1, stbtt_GetNumberOfFonts(bytes.data()));
	std::vector<ParsedFontFace> faces{};
	faces.reserve(static_cast<std::size_t>(fontCount));

	for (int index = 0; index < fontCount; ++index)
	{
		const int offset = stbtt_GetFontOffsetForIndex(bytes.data(), index);
		if (offset < 0)
		{
			continue;
		}

		stbtt_fontinfo font{};
		if (stbtt_InitFont(&font, bytes.data(), offset) == 0)
		{
			continue;
		}

		std::string family = fontNameString(font, 16);
		if (family.empty())
		{
			family = fontNameString(font, 1);
		}
		if (family.empty())
		{
			family = familyFromFilename(path);
		}

		std::string subfamily = fontNameString(font, 17);
		if (subfamily.empty())
		{
			subfamily = fontNameString(font, 2);
		}

		std::uint16_t weight = weightFromSubfamily(subfamily);
		bool os2Italic = false;
		const int os2 = stbtt__find_table(font.data, font.fontstart, "OS/2");
		if (os2 > 0)
		{
			weight = readU16Be(font.data + os2 + 4);
			const std::uint16_t fsSelection = readU16Be(font.data + os2 + 62);
			os2Italic = (fsSelection & 0x0001u) != 0u;
		}

		FlowPlot::FontStyle style = FlowPlot::FontStyle::Normal;
		if (containsAscii(subfamily, "oblique"))
		{
			style = FlowPlot::FontStyle::Oblique;
		}
		else if (os2Italic || containsAscii(subfamily, "italic"))
		{
			style = FlowPlot::FontStyle::Italic;
		}

		ParsedFontFace face{};
		face.variant.family = std::move(family);
		face.variant.weight = weight;
		face.variant.style = style;
		face.variant.path = std::filesystem::absolute(path);
		faces.push_back(std::move(face));
	}

	if (faces.empty())
	{
		throw std::runtime_error("no usable TrueType faces found");
	}
	return faces;
}

void upsertFontVariant(std::vector<AddedFontVariant>& fontLibrary, AddedFontVariant variant)
{
	const auto existing = std::find_if(fontLibrary.begin(), fontLibrary.end(), [&](const AddedFontVariant& current) {
		return current.family == variant.family
			&& current.weight == variant.weight
			&& current.style == variant.style;
	});

	if (existing != fontLibrary.end())
	{
		*existing = std::move(variant);
		return;
	}
	fontLibrary.push_back(std::move(variant));
}

void logImportResult(const FontImportResult& result)
{
	for (const std::string& error : result.errors)
	{
		std::fprintf(stderr, "[FlowPlotGUI] Font import: %s\n", error.c_str());
	}
	if (result.addedCount > 0)
	{
		std::fprintf(stderr, "[FlowPlotGUI] Imported %zu font variant(s).\n", result.addedCount);
	}
}

} // namespace

FontImportResult importFontFiles(
	state& guiState,
	const std::vector<std::filesystem::path>& paths)
{
	FontImportResult result{};
	for (const std::filesystem::path& path : paths)
	{
		try
		{
			std::vector<ParsedFontFace> faces = parseFontFile(path);
			for (ParsedFontFace& face : faces)
			{
				upsertFontVariant(guiState.fontLibrary, std::move(face.variant));
				++result.addedCount;
			}
			if (path.has_parent_path())
			{
				guiState.lastFontDialogDirectory = path.parent_path();
			}
		}
		catch (const std::exception& e)
		{
			result.errors.push_back(path.string() + ": " + e.what());
		}
	}
	return result;
}

FontImportResult openFontImportDialog(
	state& guiState,
	void* nativeWindowHandle)
{
	FontImportResult result{};
	if (NFD_Init() != NFD_OKAY)
	{
		const char* error = NFD_GetError();
		result.errors.emplace_back(error != nullptr ? error : "NFD initialization failed");
		logImportResult(result);
		return result;
	}

	NFD_SetDisplayPropertiesFromGLFW();

	nfdu8filteritem_t filters[] = {
		{"TrueType fonts", "ttf,ttc"},
	};
	std::string defaultPath;
	if (!guiState.lastFontDialogDirectory.empty())
	{
		defaultPath = guiState.lastFontDialogDirectory.string();
	}

	nfdopendialogu8args_t args{};
	args.filterList = filters;
	args.filterCount = 1;
	args.defaultPath = defaultPath.empty() ? nullptr : defaultPath.c_str();
	if (nativeWindowHandle != nullptr)
	{
		NFD_GetNativeWindowFromGLFWWindow(static_cast<GLFWwindow*>(nativeWindowHandle), &args.parentWindow);
	}

	const nfdpathset_t* pathSet = nullptr;
	const nfdresult_t dialogResult = NFD_OpenDialogMultipleU8_With(&pathSet, &args);
	if (dialogResult == NFD_CANCEL)
	{
		result.cancelled = true;
		NFD_Quit();
		return result;
	}
	if (dialogResult != NFD_OKAY)
	{
		const char* error = NFD_GetError();
		result.errors.emplace_back(error != nullptr ? error : "font file dialog failed");
		logImportResult(result);
		NFD_Quit();
		return result;
	}

	nfdpathsetsize_t count = 0;
	if (NFD_PathSet_GetCount(pathSet, &count) != NFD_OKAY)
	{
		const char* error = NFD_GetError();
		result.errors.emplace_back(error != nullptr ? error : "could not read selected font paths");
		NFD_PathSet_Free(pathSet);
		logImportResult(result);
		NFD_Quit();
		return result;
	}

	std::vector<std::filesystem::path> selectedPaths{};
	selectedPaths.reserve(count);
	for (nfdpathsetsize_t index = 0; index < count; ++index)
	{
		nfdu8char_t* rawPath = nullptr;
		if (NFD_PathSet_GetPathU8(pathSet, index, &rawPath) == NFD_OKAY && rawPath != nullptr)
		{
			selectedPaths.emplace_back(reinterpret_cast<const char*>(rawPath));
			NFD_PathSet_FreePathU8(rawPath);
		}
	}
	NFD_PathSet_Free(pathSet);
	NFD_Quit();

	if (!selectedPaths.empty() && selectedPaths.back().has_parent_path())
	{
		guiState.lastFontDialogDirectory = selectedPaths.back().parent_path();
	}

	result = importFontFiles(guiState, selectedPaths);
	logImportResult(result);
	return result;
}

} // namespace FlowPlotGui
