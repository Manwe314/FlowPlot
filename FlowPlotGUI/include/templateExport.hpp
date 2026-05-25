#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include "FlowPlotGui.hpp"

namespace FlowPlotGui {

inline constexpr std::string_view kTemplateExportVersion = "1.0";

struct TemplateExportResult {
	bool cancelled = false;
	std::filesystem::path path{};
	std::vector<std::string> errors{};
};

std::filesystem::path normalizeTemplateExportPath(std::filesystem::path path);

std::string exportTemplateJson(
	const FlowPlot::Spec::MasterTemplateSpec& activeTemplate,
	const std::vector<AddedFontVariant>& fontLibrary);

TemplateExportResult exportTemplateToPath(
	const FlowPlot::Spec::MasterTemplateSpec& activeTemplate,
	const std::vector<AddedFontVariant>& fontLibrary,
	const std::filesystem::path& path);

bool masterTemplateSpecsEqual(
	const FlowPlot::Spec::MasterTemplateSpec& lhs,
	const FlowPlot::Spec::MasterTemplateSpec& rhs);

void ensureActiveTemplateExportComparisonChecked(state& guiState);

TemplateExportResult ExportTemplateWithExportDialog(
	state& guiState,
	void* nativeWindowHandle);

} // namespace FlowPlotGui
