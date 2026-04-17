#pragma once

#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>

#include "FlowPlotGui.hpp"
#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicTitle.hpp"

struct navBarParams {
	Clay_Color backgroundColor = FlowUi::Flow_Color("#18181aff");
	int heightMin = 52;
	int heightMax = 72;
	int spacer1MinWidth = 22;
	int spacer1MaxWidth = 68;
	int spacer2MinWidth = 22;
	int spacer2MaxWidth = 68;
};

using BasicTitleBuilder = FlowUi::ElementBuilder<basicTitleParams, void, void, FLOW_DEF_ID("Basic title")>;
using NavBarButtonBuilder = FlowUi::ElementBuilder<basicButtonParams, void, void, FLOW_DEF_ID("Basic button")>;

struct navBarResources {
	BasicTitleBuilder child1Builder;
	NavBarButtonBuilder importButtonBuilder;
	NavBarButtonBuilder layersButtonBuilder;
	NavBarButtonBuilder exportButtonBuilder;
	NavBarButtonBuilder settingsButtonBuilder;

	explicit navBarResources(FlowUi::App& app) :
		child1Builder(makeChild1Builder(app)),
		importButtonBuilder(makeimportButtonBuilder(app)),
		layersButtonBuilder(makeLayersButtonBuilder(app)),
		exportButtonBuilder(makeExportButtonBuilder(app)),
		settingsButtonBuilder(makeSettingsButtonBuilder(app)) {}

private:
	static BasicTitleBuilder makeChild1Builder(FlowUi::App& app)
	{
		basicTitleParams params{};
		params.text = "FlowPlot";
		params.contentMode = basicTitleParams::ContentMode::IconThenText;
		params.iconTintColor = FlowUi::Flow_Color("#72b6ff00");
		params.textColor = FlowUi::Flow_Color("#f4f4f6ff");
		params.fontSize = 16;
		params.fontId = 0;
		params.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
		params.icon = app.icons().textureRef("FlowPlotIcon");

		BasicTitleBuilder builder = app.ui().createElement(kBasicTitle, "NavBar/shared/child-1/title");
		builder.setParameters(std::move(params));
		return builder;
	}

	static NavBarButtonBuilder makeButtonBuilder(
		FlowUi::App& app,
		std::string_view sharedPath,
		basicButtonParams& parameters)
	{
		NavBarButtonBuilder builder = app.ui().createElement(kBasicButton, sharedPath).setParameters(std::move(parameters))
		/* V1 cant Update parameters made with variables */
		.mergeParams([](auto& params) {
		    params.text = "Export";
		});
		return builder;
	}

	// Placeholder factories: keep per-button entry points so each one can be
	// customized independently later without touching constructor wiring.
	static NavBarButtonBuilder makeimportButtonBuilder(FlowUi::App& app)
	{
		basicButtonParams params{};
		params.text = "Import";
		params.icon = app.icons().textureRef("Import");
		params.contentMode = basicButtonParams::ContentMode::IconThenText;
		return makeButtonBuilder(app, "NavBar/shared/child-2/button-1", params);
	}

	static NavBarButtonBuilder makeLayersButtonBuilder(FlowUi::App& app)
	{
		basicButtonParams params{};
		params.text = "Import";
		return makeButtonBuilder(app, "NavBar/shared/child-2/button-2",params);
	}

	static NavBarButtonBuilder makeExportButtonBuilder(FlowUi::App& app)
	{
		basicButtonParams params{};
		params.text = "Import";
		return makeButtonBuilder(app, "NavBar/shared/child-3/button-1", params);
	}

	static NavBarButtonBuilder makeSettingsButtonBuilder(FlowUi::App& app)
	{
		basicButtonParams params{};
		params.text = "Import";
		return makeButtonBuilder(app, "NavBar/shared/child-3/button-2", params);
	}
};

using NavBarDef = FlowUi::ElementDefinition<
	navBarParams,
	void,
	navBarResources,
	FLOW_DEF_ID("NavBar")>;

inline const NavBarDef kNavBar = {
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::BuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},
	+[](NavBarDef::BuildContext& context) {

		int navHeightMin = context.params.heightMin;
		int navHeightMax = context.params.heightMax;
		if (navHeightMax < navHeightMin)
		{
			navHeightMax = navHeightMin;
		}

		int spacer1MinWidth = context.params.spacer1MinWidth;
		int spacer1MaxWidth = context.params.spacer1MaxWidth;
		if (spacer1MaxWidth < spacer1MinWidth)
		{
			spacer1MaxWidth = spacer1MinWidth;
		}

		int spacer2MinWidth = context.params.spacer2MinWidth;
		int spacer2MaxWidth = context.params.spacer2MaxWidth;
		if (spacer2MaxWidth < spacer2MinWidth)
		{
			spacer2MaxWidth = spacer2MinWidth;
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId child1Id = context.uiManager.toClayEID(context.createChildElementId("child-1"));
		const Clay_ElementId spacer1Id = context.uiManager.toClayEID(context.createChildElementId("spacer-1"));
		const Clay_ElementId spacer1LineId = context.uiManager.toClayEID(context.createChildElementId("spacer-1/line"));
		const Clay_ElementId child2Id = context.uiManager.toClayEID(context.createChildElementId("child-2"));
		const Clay_ElementId child2GrowSpacerId = context.uiManager.toClayEID(context.createChildElementId("child-2/grow-spacer"));
		const Clay_ElementId spacer2Id = context.uiManager.toClayEID(context.createChildElementId("spacer-2"));
		const Clay_ElementId spacer2LineId = context.uiManager.toClayEID(context.createChildElementId("spacer-2/line"));
		const Clay_ElementId child3Id = context.uiManager.toClayEID(context.createChildElementId("child-3"));

		const std::string button21Path = context.createChildElementId("child-2/button-1");
		const std::string button22Path = context.createChildElementId("child-2/button-2");
		const std::string button31Path = context.createChildElementId("child-3/button-1");
		const std::string button32Path = context.createChildElementId("child-3/button-2");
		const std::string child1TitlePath = context.createChildElementId("child-1/title");

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.childGap = 0;
		rootLayout.padding = CLAY_PADDING_ALL(0);
		rootLayout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		rootLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_FIT(static_cast<float>(navHeightMin), static_cast<float>(navHeightMax)),
		};

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig child1Layout{};
		child1Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child1Layout.childGap = 8;
		child1Layout.padding = CLAY_PADDING_ALL(0);
		child1Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child1Layout.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child1{};
		child1.id = child1Id;
		child1.layout = child1Layout;
		child1.backgroundColor = FlowUi::Flow_Color("#00000000");
		child1.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig spacer1Layout{};
		spacer1Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		spacer1Layout.childGap = 0;
		spacer1Layout.padding = CLAY_PADDING_ALL(0);
		spacer1Layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		spacer1Layout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(spacer1MinWidth), static_cast<float>(spacer1MaxWidth)),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration spacer1{};
		spacer1.id = spacer1Id;
		spacer1.layout = spacer1Layout;
		spacer1.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer1.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig spacerLineLayout{};
		spacerLineLayout.sizing = {.width = CLAY_SIZING_FIXED(2), .height = CLAY_SIZING_FIXED(20)};

		Clay_ElementDeclaration spacer1Line{};
		spacer1Line.id = spacer1LineId;
		spacer1Line.layout = spacerLineLayout;
		spacer1Line.backgroundColor = FlowUi::Flow_Color("#5e646eff");
		spacer1Line.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig child2Layout{};
		child2Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child2Layout.childGap = 8;
		child2Layout.padding = CLAY_PADDING_ALL(0);
		child2Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child2Layout.sizing = {.width = CLAY_SIZING_GROW(0, 100000), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child2{};
		child2.id = child2Id;
		child2.layout = child2Layout;
		child2.backgroundColor = FlowUi::Flow_Color("#00000000");
		child2.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig child2GrowSpacerLayout{};
		child2GrowSpacerLayout.sizing = {.width = CLAY_SIZING_GROW(0, 100000), .height = CLAY_SIZING_PERCENT(1.0f)};

		Clay_ElementDeclaration child2GrowSpacer{};
		child2GrowSpacer.id = child2GrowSpacerId;
		child2GrowSpacer.layout = child2GrowSpacerLayout;

		Clay_LayoutConfig spacer2Layout{};
		spacer2Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		spacer2Layout.childGap = 0;
		spacer2Layout.padding = CLAY_PADDING_ALL(0);
		spacer2Layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		spacer2Layout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(spacer2MinWidth), static_cast<float>(spacer2MaxWidth)),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration spacer2{};
		spacer2.id = spacer2Id;
		spacer2.layout = spacer2Layout;
		spacer2.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer2.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_ElementDeclaration spacer2Line{};
		spacer2Line.id = spacer2LineId;
		spacer2Line.layout = spacerLineLayout;
		spacer2Line.backgroundColor = FlowUi::Flow_Color("#5e646eff");
		spacer2Line.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig child3Layout{};
		child3Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child3Layout.childGap = 8;
		child3Layout.padding = CLAY_PADDING_ALL(0);
		child3Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child3Layout.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child3{};
		child3.id = child3Id;
		child3.layout = child3Layout;
		child3.backgroundColor = FlowUi::Flow_Color("#00000000");
		child3.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(root){
			CLAY(child1){
				if (NavBarDef::resources.has_value())
				{
					navBarResources& resources = *NavBarDef::resources;
					resources.child1Builder
					.withElementID(child1TitlePath)
					.draw();
				}
			};

			CLAY(spacer1){
				CLAY(spacer1Line){};
			};

			CLAY(child2){
				if (NavBarDef::resources.has_value())
				{
					navBarResources& resources = *NavBarDef::resources;
					resources.importButtonBuilder
						.withElementID(button21Path)
						.draw();
					resources.layersButtonBuilder
						.withElementID(button22Path)
						.draw();
				}

				CLAY(child2GrowSpacer){};
			};

			CLAY(spacer2){
				CLAY(spacer2Line){};
			};

			CLAY(child3){
				if (NavBarDef::resources.has_value())
				{
					navBarResources& resources = *NavBarDef::resources;
					resources.exportButtonBuilder
						.withElementID(button31Path)
						.draw();
					resources.settingsButtonBuilder
						.withElementID(button32Path)
						.draw();
				}
			};
		};
	},
};
