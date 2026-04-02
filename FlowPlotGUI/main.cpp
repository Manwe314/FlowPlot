#include <FlowUi/Flow.hpp>

#include <cstdio>
#include <exception>

#include "elements.hpp"
#include "iconRegistry.hpp"

int main()
{
	try
	{
		FlowUi::AppConfig config{};
		config.window.title = "FlowPlotGUI";
		config.window.maximized = true;
		config.window.width = 1920;
		config.window.height = 1080;
		FlowUi::App app = FlowUi::makeApplication(config);
		FlowPlotGui::registerIcons(app);
		(void)NavBarDef::getResources(app);

		FlowUi::UiManager& ui = app.ui();




		while (!app.shouldClose())
		{
			app.beginFrame();
			ui.createElement(kRootBackground, "rootBackground")
			.setParameters({.backgroundColor = FlowUi::Flow_Color("#18181aff")})
			.construct(FlowUi::ElementDrawOptions::SkipEventCallbacks);
				ui.createElement(kNavBar, "NavBar").draw();
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
						ui.createElement(kRootBackground, "TemplatePanelBack")
						.setParameters({.backgroundColor = Clay_Color{0.0f,0.0f,0.0f,0.0f}})
						.construct();
						ui.drawConstructed(); // TemplatePanelBack
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
						ui.createElement(kDataInput, "DataInput").draw();
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
					ui.createElement(kPropsPanel, rightId).construct();
						ui.createElement(kPanelTitle, "TemplateTitle")
						.setParameters({
							.titleText = "Properties",
							.showSecondaryTitle = true,
							.secondaryTitleParams = {.text = "panel 1", .fontSize = 12}
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
