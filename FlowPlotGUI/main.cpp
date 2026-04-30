#include <FlowUi/Flow.hpp>

#include <cstddef>
#include <cstdio>
#include <exception>

#include "elements.hpp"
#include "iconRegistry.hpp"
#include "templateHelper.hpp"

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
		FlowUi::App app = FlowUi::makeApplication(config);
		FlowPlotGui::registerIcons(app);


		FlowUi::UiManager& ui = app.ui();
		FlowPlotGui::state guiState{};
		guiState.activeTemplate.panels.emplace_back();
		guiState.activeTemplate.panels.back().id = "panel_1";
		guiState.activeTemplate.panels.back().layers.emplace_back();
		guiState.activeTemplate.panels.back().layers.back().id = "layer_1";




		while (!app.shouldClose())
		{
			app.beginFrame();
			ui.createElement(kRootBackground, "rootBackground")
			.setParameters({.backgroundColor = FlowUi::Flow_Color("#18181aff")})
			.construct(FlowUi::ElementDrawOptions::SkipEventCallbacks);
				ui.createElement(kNavBar, "NavBar")
				.setParameters({
				    .child2Gap = 16,
				    .padding = Clay_Padding{.left = 16, .right = 16, .top = 0, .bottom = 8},
				    .spacer1Sizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 22.0f, .max = 45.0f}}, .type = CLAY__SIZING_TYPE_GROW}, .height = Clay_SizingAxis{.size = {.percent = 0.0f}, .type = CLAY__SIZING_TYPE_PERCENT}},
				    .spacer2Sizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 22.0f, .max = 45.0f}}, .type = CLAY__SIZING_TYPE_GROW}, .height = Clay_SizingAxis{.size = {.percent = 1.0f}, .type = CLAY__SIZING_TYPE_PERCENT}},
				    .spacerLine1Color = Clay_Color{.r = 65.0f, .g = 65.0f, .b = 67.0f, .a = 255.0f},
				    .spacerLine2Color = Clay_Color{.r = 65.0f, .g = 65.0f, .b = 67.0f, .a = 255.0f}
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
						ui.createElement(kPlotviewPort, "PlotViewPort").draw();
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
		return 0;
	}
	catch (const std::exception& e)
	{
		std::fprintf(stderr, "FlowPlotGUI fatal error: %s\n", e.what());
		return 1;
	}
}
