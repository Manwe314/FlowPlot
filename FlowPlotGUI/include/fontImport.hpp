#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "FlowPlotGui.hpp"

namespace FlowPlotGui {

struct FontImportResult {
	bool cancelled = false;
	std::size_t addedCount = 0;
	std::vector<std::string> errors{};
};

FontImportResult importFontFiles(
	state& guiState,
	const std::vector<std::filesystem::path>& paths);

FontImportResult openFontImportDialog(
	state& guiState,
	void* nativeWindowHandle);

} // namespace FlowPlotGui
