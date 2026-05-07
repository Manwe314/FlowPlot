#define FLOW_PLOT_RENDERER
#define FLOW_PLOT_IMPLEMENTATION

#include <FlowUi/Flow.hpp>

#include <cstddef>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <memory>
#include <utility>
#include <vector>

#include "elements.hpp"
#include "FlowPlotGui.hpp"
#include "iconRegistry.hpp"
#include "PlotViewportScene.hpp"
#include "templateHelper.hpp"

namespace {

void populateInitialGuiState(FlowPlotGui::state& guiState)
{
	guiState.activeTemplate = FlowPlot::Spec::MasterTemplateSpec{};
	guiState.datasets.clear();
	guiState.selectedNode.reset();

	FlowPlot::Spec::MasterTemplateSpec& spec = guiState.activeTemplate;
	spec.version = "1.0";
	spec.figure.width = 1200;
	spec.figure.height = 800;
	spec.figure.dpi = 96;
	spec.figure.background = "#ffffff";
	spec.figure.padding = FlowPlot::Spec::PaddingSpec{
		.left = 24.0f,
		.right = 24.0f,
		.top = 24.0f,
		.bottom = 24.0f,
	};
	spec.figure.title.visible = true;
	spec.figure.title.text = "Scatter Plot";
	spec.figure.title.fontFamily = "Default";
	spec.figure.title.fontSize = 24.0f;
	spec.figure.title.fontWeight = 700;
	spec.figure.title.color = "#111111";
	spec.figure.title.overflow = "clip";
	spec.figure.title.clip = true;
	spec.figure.title.box = FlowPlot::Spec::BoxSpec{};
	spec.figure.legends.clear();

	FlowPlot::Spec::DatasetSpec datasetSpec{};
	datasetSpec.name = "main";
	datasetSpec.schema["x"] = FlowPlot::Spec::DatasetSpec::FieldType::Number;
	datasetSpec.schema["y"] = FlowPlot::Spec::DatasetSpec::FieldType::Number;
	spec.datasets.push_back(std::move(datasetSpec));

	FlowPlot::Spec::LayerSpec scatterLayer{};
	scatterLayer.id = "layer_1";
	scatterLayer.type = "scatter";
	scatterLayer.dataset = "main";
	scatterLayer.scatterMapping.xField = "x";
	scatterLayer.scatterMapping.yField = "y";

	FlowPlot::Spec::PanelSpec panel{};
	panel.id = "panel_1";
	panel.layers.clear();
	panel.layers.push_back(std::move(scatterLayer));
	spec.panels.clear();
	spec.panels.push_back(std::move(panel));

	const std::vector<int> x{1, 2, 3, 4, 5};
	const std::vector<int> y{3, 4, 1, 4, 6};

	FlowPlotGui::numericColumn xColumn{};
	xColumn.name = "x";
	xColumn.data.assign(x.begin(), x.end());

	FlowPlotGui::numericColumn yColumn{};
	yColumn.name = "y";
	yColumn.data.assign(y.begin(), y.end());

	FlowPlotGui::RunningDataset runningDataset{};
	runningDataset.name = "main";
	runningDataset.numericColumns.push_back(std::move(xColumn));
	runningDataset.numericColumns.push_back(std::move(yColumn));
	guiState.datasets.push_back(std::move(runningDataset));

	guiState.templateRevision = 1;
	guiState.datasetRevision = 1;
	guiState.viewportRevision = 1;
	guiState.textEngine = std::make_shared<FlowPlot::StbTextEngine>(
		std::filesystem::path(__FILE__).parent_path() / "assets" / "Fonts" / "Inter-VariableFont_opsz,wght.ttf");
}

} // namespace

int main()
{


	try
	{
		FlowUi::AppConfig config{};
		config.window.title = "FlowPlotGUI";
		config.window.maximized = true;
		config.window.width = 1920;
		config.window.height = 1080;
		config.dev.enabled = true;
		config.dev.panelOpenByDefault = true;
		config.dev.useShortcutManagerForPanelToggle = true;
		config.ui.stringArenaSize = 2 * 1024 * 1024;
		config.ui.defaultFontFamily = FlowUi::FontFamilyCreateInfo{
			.name = "Inter",
			.faces = {
				FlowUi::FontFaceCreateInfo{
					.path = std::filesystem::path(__FILE__).parent_path() / "assets" / "Fonts" / "Inter.arfont",
					.pixelSize = 18.0f,
					.weight = 400,
					.style = FlowUi::FontStyle::Normal,
				},
			},
		};
		config.ui.fontAtlasSize = 1024;
		FlowUi::App app = FlowUi::makeApplication(config);
		FlowPlotGui::registerIcons(app);


		FlowUi::UiManager& ui = app.ui();
		FlowPlotGui::state guiState{};
		populateInitialGuiState(guiState);




		while (!app.shouldClose())
		{
			app.beginFrame();
			ui.createElement(kRootBackground, "rootBackground")
			.setParameters({.backgroundColor = FlowUi::Flow_Color("#18181aff")})
			.construct(FlowUi::ElementDrawOptions::SkipEventCallbacks);
				ui.createElement(kNavBar, "NavBar")
				.setParameters({
				    .padding = Clay_Padding{.left = 16, .right = 16, .top = 0, .bottom = 8},
				    .spacer1Sizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 22.0f, .max = 45.0f}}, .type = CLAY__SIZING_TYPE_GROW}, .height = Clay_SizingAxis{.size = {.percent = 0.0f}, .type = CLAY__SIZING_TYPE_PERCENT}},
				    .spacer2Sizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 22.0f, .max = 45.0f}}, .type = CLAY__SIZING_TYPE_GROW}, .height = Clay_SizingAxis{.size = {.percent = 1.0f}, .type = CLAY__SIZING_TYPE_PERCENT}},
				    .spacerLine1Color = Clay_Color{.r = 65.0f, .g = 65.0f, .b = 67.0f, .a = 255.0f},
				    .spacerLine2Color = Clay_Color{.r = 65.0f, .g = 65.0f, .b = 67.0f, .a = 255.0f},
				    .child2Gap = 16,
				    .guiState = &guiState,
				    .nativeWindowHandle = app.nativeWindowHandle()
				}).draw();
				ui.createElement(kMainContent, "MainContent").construct();
					const std::string leftId = "TemplatePanel";
					ui.createElement(kTemplatePanel, leftId).construct();
						ui.createElement(kPanelTitle, "TemplateTitle")
						.setParameters({
							.titleText = "Template Schema",
							.showSecondaryTitle = true,
							.secondaryTitleParams = {.text = "click to edit properties", .fontSize = 12}
						})
						.draw();
						FlowPlotGui::drawTemplateNode(app, ui, guiState, "TemplatePanel/tree", {
							.kind = FlowPlotGui::TemplateNodeKind::Figure,
						});
					ui.drawConstructed(); // TemplatePanel
					ui.createElement(kDynamicSeparator, "separator1")
					.setParameters({
						.color = FlowUi::Flow_Color("#3f3f41ff"),
						.hoverColor = FlowUi::Flow_Color("#707074ff"),
						.activeColor = FlowUi::Flow_Color("#409d97ff"),
						.minValue = 220,
						.maxValue = 550,
						.getValue = [leftId](){
							return TemplatePanelDef::getOrCreateState(FlowUi::toFlowId(leftId)).minWidth;
						},
						.setValue = [leftId](int v){
							TemplatePanelDef::getOrCreateState(FlowUi::toFlowId(leftId)).minWidth = v;
						},
					}).draw();
					ui.createElement(kMainContentPanel, "MainContentPanel")
					.setParameters({.backgroundColor = FlowUi::Flow_Color("#18181aff")})
					.construct(FlowUi::ElementDrawOptions::SkipEventCallbacks);
						ui.createElement(kPlotviewPort, "PlotViewPort")
						.setParameters({.guiState = &guiState})
						.draw();
						ui.createElement(kDataInput, "DataInput")
						.setParameters({.guiState = &guiState})
						.draw();
					ui.drawConstructed(); // MainContentPanel
					const std::string rightId = "PropsPanel";
					ui.createElement(kDynamicSeparator, "separator2")
					.setParameters({
						.reverseDrag = true,
						.color = FlowUi::Flow_Color("#3f3f41ff"),
						.hoverColor = FlowUi::Flow_Color("#707074ff"),
						.activeColor = FlowUi::Flow_Color("#409d97ff"),
						.minValue = 220,
						.maxValue = 550,
						.getValue = [rightId](){
							return PropsPanelDef::getOrCreateState(FlowUi::toFlowId(rightId)).minWidth;
						},
						.setValue = [rightId](int v){
							PropsPanelDef::getOrCreateState(FlowUi::toFlowId(rightId)).minWidth = v;
						},
					}).draw();
					ui.createElement(kPropsPanel, rightId)
					.setParameters({.guiState = &guiState})
					.construct();
						ui.createElement(kPanelTitle, "PropsTitle")
						.setParameters({
							.titleText = "Properties",
							.showSecondaryTitle = true,
							.secondaryTitleParams = {.text = "panel 1", .fontSize = 12}
						})
						.draw();
						ui.createElement(kPropertiesContent, "PropertiesContent")
						.setParameters({
							.padding = Clay_Padding{12, 12, 12, 12},
							.childGap = 8,
							.guiState = &guiState,
						})
						.draw();
					ui.drawConstructed(); //PropsPanel

				ui.drawConstructed(); // MainContent
			
			ui.drawConstructed(); // rootBackground


			app.endFrame();
			app.drawFrame();
		}
		FlowPlotGui::shutdownPlotViewportScene();
		return 0;
	}
	catch (const std::exception& e)
	{
		std::fprintf(stderr, "FlowPlotGUI fatal error: %s\n", e.what());
		return 1;
	}
}
