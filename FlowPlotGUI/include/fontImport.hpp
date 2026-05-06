#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "FlowPlotGui.hpp"

struct FontManager;

namespace FlowPlotGui {

struct FontImportResult {
	bool cancelled = false;
	std::size_t addedCount = 0;
	std::vector<std::string> errors{};
};

FontImportResult importFontFiles(
	state& guiState,
	FontManager* fontManager,
	const std::vector<std::filesystem::path>& paths);

FontImportResult openFontImportDialog(
	state& guiState,
	FontManager* fontManager,
	void* nativeWindowHandle);

} // namespace FlowPlotGui
