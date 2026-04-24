#pragma once

#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"
#include "BuildElements/PanelTitle.hpp"

struct plotviewPortParams {
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color viewportColor = FlowUi::Flow_Color("#7a7a7aff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	plotviewPortParams,
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, viewportColor));

struct plotviewPortResources {
	PanelTitleBuilder titleBuilder;

	explicit plotviewPortResources(FlowUi::UiManager& uiManager) :
		titleBuilder(makeTitleBuilder(uiManager)) {}

private:
	static PanelTitleBuilder makeTitleBuilder(FlowUi::UiManager& uiManager)
	{
		panelTitleParams params{};
		params.titleText = "Visual Preview";
		params.rightContentMode = panelTitleParams::RightContentMode::Button;
		params.rightButtonParams.text = "Reset";
		params.rightButtonParams.contentMode = basicButtonParams::ContentMode::TextOnly;
		params.rightButtonParams.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		PanelTitleBuilder builder = uiManager.createElement(kPanelTitle, "PlotviewPort/shared/title");
		builder.setParameters(std::move(params));
		return builder;
	}
};

using PlotviewPortDef = FlowUi::ElementDefinition<
	plotviewPortParams,
	void,
	plotviewPortResources,
	FLOW_DEF_ID("PlotviewPort")>;

inline const PlotviewPortDef kPlotviewPort = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](PlotviewPortDef::BuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},
	+[](PlotviewPortDef::BuildContext& context) {
		if (!PlotviewPortDef::resources.has_value())
		{
			PlotviewPortDef::resources.emplace(context.uiManager);
		}
		plotviewPortResources& resources = *PlotviewPortDef::resources;

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId viewportId = context.uiManager.toClayEID(context.createChildElementId("viewport"));
		const std::string titlePath = context.createChildElementId("title");

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		rootLayout.childGap = 0;

		Clay_ElementDeclaration root{};
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig viewportLayout{};
		viewportLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_GROW(0),
		};

		Clay_ElementDeclaration viewport{};
		viewport.layout = viewportLayout;
		viewport.backgroundColor = context.params.viewportColor;
		viewport.cornerRadius = CLAY_CORNER_RADIUS(0);
		viewport.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(rootId, root){
			resources.titleBuilder
				.withElementID(titlePath)
				.draw();

			CLAY(viewportId, viewport){};
		};
	},
};
