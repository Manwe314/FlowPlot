#include "iconRegistry.hpp"

#include <array>
#include <filesystem>
#include <string_view>

#include "Icons.hpp"

namespace FlowPlotGui {



struct InlineIcon {
	std::string_view key;
	std::string_view svg;
};

constexpr std::array<InlineIcon, 15> kInlineIcons{{
	{"DownArrow", kDownArrow},
	{"RightArrow", kRightArrow},
	{"file", kfile},
	{"Plus", kPlus},
	{"Settings", kSettings},
	{"Trash", kTrash},
	{"X", kX},
	{"T", kT},
	{"Layers", kLayers},
	{"Panel", kPanel},
	{"Hashtag", kHashtag},
	{"Export", kExport},
	{"Import", kImport},
	{"Axis", kAxis},
	{"New", kNew},
}};

std::filesystem::path flowPlotIconPath()
{
	const std::filesystem::path sourceFilePath(__FILE__);
	return sourceFilePath.parent_path().parent_path() / "assets" / "Svgs" / "FlowPlotIcon.svg";
}


void registerIcons(FlowUi::App& app)
{
	FlowUi::IconManager& iconManager = app.icons();

	for (const InlineIcon& icon : kInlineIcons)
	{
		iconManager.registerSvg(icon.key, icon.svg);
	}

	iconManager.registerFromFile("FlowPlotIcon", flowPlotIconPath().string());
}

} // namespace FlowPlotGui
