#pragma once

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

#include "FlowPlotGui.hpp"

namespace FlowUi {
struct FontManager;
}

namespace FlowPlotGui {

struct TemplateImportResult {
	bool cancelled = false;
	std::filesystem::path path{};
	std::size_t importedFontCount = 0;
	std::vector<std::string> errors{};
};

std::vector<RunningDataset> makeRunningDatasetsForTemplate(
	const FlowPlot::Spec::MasterTemplateSpec& activeTemplate);

TemplateImportResult importTemplateFromPath(
	state& guiState,
	FlowUi::FontManager* fontManager,
	const std::filesystem::path& path);

TemplateImportResult ImportTemplateWithImportDialog(
	state& guiState,
	FlowUi::FontManager* fontManager,
	void* nativeWindowHandle);

} // namespace FlowPlotGui
