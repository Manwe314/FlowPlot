#include "iconRegistry.hpp"

#include <array>
#include <filesystem>
#include <string_view>

#include "Icons.hpp"
#include "elements.hpp"
#include "runtimePaths.hpp"

namespace FlowPlotGui {



struct InlineIcon {
	std::string_view key;
	std::string_view svg;
};

constexpr std::array<InlineIcon, 19> kInlineIcons{{
	{"DownArrow", kDownArrow},
	{"UpArrow", kUpArrow},
	{"RightArrow", kRightArrow},
	{"Link", kLink},
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
	{"Preset", kPresets},
	{"QuestionMark", kquestionMark}
}};

std::filesystem::path flowPlotIconPath()
{
#ifdef FLOWPLOTGUI_SOURCE_ASSET_DIR
	const std::filesystem::path sourceAssetDir(FLOWPLOTGUI_SOURCE_ASSET_DIR);
#else
	const std::filesystem::path sourceFilePath(__FILE__);
	const std::filesystem::path sourceAssetDir = sourceFilePath.parent_path().parent_path() / "assets";
#endif

	return resourcePathOrSource("assets/Svgs/FlowPlotIcon.svg", sourceAssetDir / "Svgs" / "FlowPlotIcon.svg");
}


void registerIcons(FlowUi::App& app)
{
	FlowUi::IconManager& iconManager = app.icons();

	for (const InlineIcon& icon : kInlineIcons)
	{
		iconManager.registerSvg(icon.key, icon.svg);
	}

	iconManager.registerFromFile("FlowPlotIcon", flowPlotIconPath().string());
	(void)NavBarDef::getResources(app);
	(void)EnumPickerMenuDef::getResources(app);
	(void)NineSplitMatrixDef::getResources(app);
	(void)TwoColumnInputTableDef::getResources(app);
	(void)TextSpecEditorDef::getResources(app);
	(void)LayoutEditorDef::getResources(app);
	(void)GridSettingsEditorDef::getResources(app);
	(void)LineSettingsEditorDef::getResources(app);
	(void)TickSettingsEditorDef::getResources(app);
	(void)ScatterMappingSettingsEditorDef::getResources(app);
	(void)ScatterColorMappingSettingsDef::getResources(app);
	(void)ScatterSizeMappingSettingsDef::getResources(app);
	(void)ScatterStyleSettingsEditorDef::getResources(app);
	(void)ScatterStatsSettingsEditorDef::getResources(app);
	(void)ScatterConfigSettingsEditorDef::getResources(app);
	(void)HistogramMappingSettingsEditorDef::getResources(app);
	(void)HistogramColorMappingSettingsDef::getResources(app);
	(void)HistogramStyleSettingsEditorDef::getResources(app);
	(void)HistogramStatsSettingsEditorDef::getResources(app);
	(void)HistogramConfigSettingsEditorDef::getResources(app);
	(void)VariableTypeSettingsEditorDef::getResources(app);
	(void)DataInputHeaderDef::getResources(app);
	(void)DataTabDef::getResources(app);
	(void)DataInputHeaderRowDef::getResources(app);
	(void)PlotviewPortDef::getResources(app);
}

} // namespace FlowPlotGui
