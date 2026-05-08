#pragma once

#include <string>
#include <string_view>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"
#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicTitle.hpp"
#include "fontImport.hpp"
#include "templateImport.hpp"
#include "templateExport.hpp"

struct navBarParams {
	Clay_Color backgroundColor = FlowUi::Flow_Color("#18181aff");
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(52, 72)};
	Clay_Padding padding = Clay_Padding{16, 16, 0, 0};
	uint16_t childGap = 0;
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

	Clay_Sizing spacer1Sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(22, 68), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Sizing spacer2Sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(22, 68), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Color spacerLine1Color = FlowUi::Flow_Color("#5e646eff");
	Clay_Color spacerLine2Color = FlowUi::Flow_Color("#5e646eff");

	uint16_t child2Gap = 8;
	Clay_Padding child2Padding = CLAY_PADDING_ALL(0);
	Clay_ChildAlignment child2Alignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

	uint16_t child3Gap = 8;
	Clay_Padding child3Padding = CLAY_PADDING_ALL(0);
	Clay_ChildAlignment child3Alignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

	Clay_Color dirtyColor = FlowUi::Flow_Color("#f5b84bff");
	Clay_Color cleanColor = FlowUi::Flow_Color("#7dd3a8ff");
	FlowPlotGui::state* guiState = nullptr;
	void* nativeWindowHandle = nullptr;
};

struct navBarState {
	bool isDirty = false;
	bool newTemplatePresetPickerOpen = false;
};

FLOWUI_DEV_REGISTER_STRUCT(
	navBarParams,
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, borderColor),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, childAlignment),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, spacer1Sizing),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, spacer2Sizing),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, spacerLine1Color),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, spacerLine2Color),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, child2Gap),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, child2Padding),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, child2Alignment),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, child3Gap),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, child3Padding),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, child3Alignment),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, dirtyColor),
	FLOWUI_DEV_REFLECT_FIELD(navBarParams, cleanColor));

FLOWUI_DEV_REGISTER_STRUCT(
	navBarState,
	FLOWUI_DEV_REFLECT_FIELD(navBarState, isDirty),
	FLOWUI_DEV_REFLECT_FIELD(navBarState, newTemplatePresetPickerOpen));

using BasicTitleBuilder = FlowUi::ElementBuilder<basicTitleParams, void, void, FLOW_DEF_ID("Basic title")>;

struct navBarResources {
	Clay_ElementId rootId{};
	Clay_ElementId child1Id{};
	Clay_ElementId spacer1Id{};
	Clay_ElementId spacer1LineId{};
	Clay_ElementId child2Id{};
	Clay_ElementId child2GrowSpacerId{};
	Clay_ElementId spacer2Id{};
	Clay_ElementId spacer2LineId{};
	Clay_ElementId child3Id{};

	std::string child1TitlePath{};
	std::string newButtonPath{};
	std::string importButtonPath{};
	std::string exportButtonPath{};
	std::string addFontButtonPath{};
	std::string statusTitlePath{};
	FlowUi::TextureRef newIcon{};
	FlowUi::TextureRef importIcon{};
	FlowUi::TextureRef exportIcon{};
	FlowUi::TextureRef addFontIcon{};
	FontManager* fontManager = nullptr;

	BasicTitleBuilder child1Builder;

	explicit navBarResources(FlowUi::App& app) :
		rootId(app.ui().toClayEID("NavBar")),
		child1Id(app.ui().toClayEID("NavBar/child-1")),
		spacer1Id(app.ui().toClayEID("NavBar/spacer-1")),
		spacer1LineId(app.ui().toClayEID("NavBar/spacer-1/line")),
		child2Id(app.ui().toClayEID("NavBar/child-2")),
		child2GrowSpacerId(app.ui().toClayEID("NavBar/child-2/grow-spacer")),
		spacer2Id(app.ui().toClayEID("NavBar/spacer-2")),
		spacer2LineId(app.ui().toClayEID("NavBar/spacer-2/line")),
		child3Id(app.ui().toClayEID("NavBar/child-3")),
		child1TitlePath("NavBar/child-1/title"),
		newButtonPath("NavBar/child-2/new"),
		importButtonPath("NavBar/child-2/import"),
		exportButtonPath("NavBar/child-2/export"),
		addFontButtonPath("NavBar/child-2/add-font"),
		statusTitlePath("NavBar/child-3/status"),
		newIcon(app.icons().textureRef("New")),
		importIcon(app.icons().textureRef("Import")),
		exportIcon(app.icons().textureRef("Export")),
		addFontIcon(app.icons().textureRef("T")),
		fontManager(&app.fonts()),
		child1Builder(makeChild1Builder(app, child1TitlePath)) {}

private:
	static BasicTitleBuilder makeChild1Builder(FlowUi::App& app, std::string_view path)
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

		BasicTitleBuilder builder = app.ui().createElement(kBasicTitle, path)
			.setParameters({
			    .contentMode = basicTitleParams::ContentMode::IconThenText,
			    .childGap = 13,
			    .fontSize = 32,
			    .textColor = Clay_Color{.r = 243.0f, .g = 243.0f, .b = 243.0f, .a = 255.0f},
			    .iconTintColor = Clay_Color{.r = 122.0f, .g = 209.0f, .b = 230.0f, .a = 0.0f}
			});
		builder.setParameters(std::move(params));
		return builder;
	}
};

using NavBarDef = FlowUi::ElementDefinition<
	navBarParams,
	navBarState,
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
		(void)NavBarDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
	},
	+[](NavBarDef::BuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},
	+[](NavBarDef::BuildContext& context) {
		navBarState& state = NavBarDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!NavBarDef::resources.has_value())
		{
			return;
		}
		navBarResources& resources = *NavBarDef::resources;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.childGap = context.params.childGap;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childAlignment;
		rootLayout.sizing = context.params.sizing;

		Clay_ElementDeclaration root{};
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_LayoutConfig child1Layout{};
		child1Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child1Layout.childGap = 8;
		child1Layout.padding = CLAY_PADDING_ALL(0);
		child1Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child1Layout.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child1{};
		child1.layout = child1Layout;
		child1.backgroundColor = FlowUi::Flow_Color("#00000000");
		child1.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig spacer1Layout{};
		spacer1Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		spacer1Layout.childGap = 0;
		spacer1Layout.padding = CLAY_PADDING_ALL(0);
		spacer1Layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		spacer1Layout.sizing = context.params.spacer1Sizing;

		Clay_ElementDeclaration spacer1{};
		spacer1.layout = spacer1Layout;
		spacer1.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer1.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig spacerLineLayout{};
		spacerLineLayout.sizing = {.width = CLAY_SIZING_FIXED(2), .height = CLAY_SIZING_FIXED(20)};

		Clay_ElementDeclaration spacer1Line{};
		spacer1Line.layout = spacerLineLayout;
		spacer1Line.backgroundColor = context.params.spacerLine1Color;
		spacer1Line.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig child2Layout{};
		child2Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child2Layout.childGap = context.params.child2Gap;
		child2Layout.padding = context.params.child2Padding;
		child2Layout.childAlignment = context.params.child2Alignment;
		child2Layout.sizing = {.width = CLAY_SIZING_GROW(0, 100000), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child2{};
		child2.layout = child2Layout;
		child2.backgroundColor = FlowUi::Flow_Color("#00000000");
		child2.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig child2GrowSpacerLayout{};
		child2GrowSpacerLayout.sizing = {.width = CLAY_SIZING_GROW(0, 100000), .height = CLAY_SIZING_PERCENT(1.0f)};

		Clay_ElementDeclaration child2GrowSpacer{};
		child2GrowSpacer.layout = child2GrowSpacerLayout;

		Clay_LayoutConfig spacer2Layout{};
		spacer2Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		spacer2Layout.childGap = 0;
		spacer2Layout.padding = CLAY_PADDING_ALL(0);
		spacer2Layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		spacer2Layout.sizing = context.params.spacer2Sizing;

		Clay_ElementDeclaration spacer2{};
		spacer2.layout = spacer2Layout;
		spacer2.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer2.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_ElementDeclaration spacer2Line{};
		spacer2Line.layout = spacerLineLayout;
		spacer2Line.backgroundColor = context.params.spacerLine2Color;
		spacer2Line.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig child3Layout{};
		child3Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child3Layout.childGap = context.params.child3Gap;
		child3Layout.padding = context.params.child3Padding;
		child3Layout.childAlignment = context.params.child3Alignment;
		child3Layout.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child3{};
		child3.layout = child3Layout;
		child3.backgroundColor = FlowUi::Flow_Color("#00000000");
		child3.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		basicTitleParams statusParams{};
		statusParams.text = state.isDirty ? "Unsaved" : "Saved";
		statusParams.contentMode = basicTitleParams::ContentMode::TextOnly;
		statusParams.textColor = state.isDirty ? context.params.dirtyColor : context.params.cleanColor;
		statusParams.fontSize = 13;
		statusParams.padding = Clay_Padding{8, 8, 4, 4};
		statusParams.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
		statusParams.backgroundColor = FlowUi::Flow_Color("#242428ff");
		statusParams.borderColor = FlowUi::Flow_Color("#3a3a40ff");
		statusParams.borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
		statusParams.cornerRadius = CLAY_CORNER_RADIUS(6);

		auto makeButtonParams = [](std::string_view text, FlowUi::TextureRef icon) {
			basicButtonParams params{};
			params.text = std::string(text);
			params.icon = icon;
			params.contentMode = basicButtonParams::ContentMode::IconThenText;
			params.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
			params.padding = Clay_Padding{10, 10, 8, 8};
			params.backgroundColor = FlowUi::Flow_Color("#242428ff");
			params.borderColor = FlowUi::Flow_Color("#3a3a40ff");
			params.borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
			params.cornerRadius = CLAY_CORNER_RADIUS(6);
			params.childGap = 8;
			params.fontSize = 14;
			params.textColor = FlowUi::Flow_Color("#f4f4f6ff");
			params.iconContainerSizing = {.width = CLAY_SIZING_FIXED(16), .height = CLAY_SIZING_FIXED(16)};
			params.iconTintColor = FlowUi::Flow_Color("#00000000");
			return params;
		};

		auto applyNavButtonVisuals = [](auto& params) {
			params.backgroundColor = Clay_Color{.r = 36.0f, .g = 36.0f, .b = 40.0f, .a = 0.0f};
			params.borderColor = Clay_Color{.r = 58.0f, .g = 58.0f, .b = 64.0f, .a = 0.0f};
			params.borderWidth = Clay_BorderWidth{.left = 0, .right = 0, .top = 0, .bottom = 0, .betweenChildren = 0};
			params.cornerRadius = Clay_CornerRadius{.topLeft = 10.0f, .topRight = 10.0f, .bottomLeft = 10.0f, .bottomRight = 10.0f};
			params.fontSize = 14;
			params.hoverBackgroundColor = Clay_Color{.r = 36.0f, .g = 36.0f, .b = 40.0f, .a = 255.0f};
			params.iconContainerSizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 20.0f, .max = 20.0f}}, .type = CLAY__SIZING_TYPE_FIXED}, .height = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 20.0f, .max = 20.0f}}, .type = CLAY__SIZING_TYPE_FIXED}};
			params.iconTintColor = Clay_Color{.r = 0.0f, .g = 0.0f, .b = 0.0f, .a = 0.0f};
			params.textColor = Clay_Color{.r = 243.0f, .g = 243.0f, .b = 243.0f, .a = 255.0f};
		};

		basicButtonParams newButtonParams = makeButtonParams("New", resources.newIcon);
		newButtonParams.onPressedCallback = [elementFlowId = FlowUi::toFlowId(context.elementID)](BasicButtonInteractionContext) {
			if (navBarState* latestState = NavBarDef::tryGetState(elementFlowId))
			{
				latestState->newTemplatePresetPickerOpen = true;
			}
		};

		basicButtonParams exportButtonParams = makeButtonParams("Export", resources.exportIcon);
		exportButtonParams.onPressedCallback = [
			guiState = context.params.guiState,
			nativeWindowHandle = context.params.nativeWindowHandle
		](BasicButtonInteractionContext) {
			if (guiState != nullptr)
			{
				(void)FlowPlotGui::ExportTemplateWithExportDialog(*guiState, nativeWindowHandle);
			}
		};

		basicButtonParams importButtonParams = makeButtonParams("Import", resources.importIcon);
		importButtonParams.onPressedCallback = [
			guiState = context.params.guiState,
			fontManager = resources.fontManager,
			nativeWindowHandle = context.params.nativeWindowHandle
		](BasicButtonInteractionContext) {
			if (guiState != nullptr)
			{
				(void)FlowPlotGui::ImportTemplateWithImportDialog(*guiState, fontManager, nativeWindowHandle);
			}
		};

		CLAY(resources.rootId, root){
			CLAY(resources.child1Id, child1){
				resources.child1Builder
					.withElementID(resources.child1TitlePath)
					.draw();
			};

			CLAY(resources.spacer1Id, spacer1){
				CLAY(resources.spacer1LineId, spacer1Line){};
			};

			CLAY(resources.child2Id, child2){
				context.uiManager.createElement(kBasicButton, resources.newButtonPath)
					.setParameters(std::move(newButtonParams))
					/* V1 cant Update parameters made with variables */
					.mergeParams(applyNavButtonVisuals)
					.draw();
				context.uiManager.createElement(kBasicButton, resources.importButtonPath)
					.setParameters(std::move(importButtonParams))
					/* V1 cant Update parameters made with variables */
					.mergeParams(applyNavButtonVisuals)
					.draw();
				context.uiManager.createElement(kBasicButton, resources.exportButtonPath)
					.setParameters(std::move(exportButtonParams))
					/* V1 cant Update parameters made with variables */
					.mergeParams(applyNavButtonVisuals)
					.draw();
				basicButtonParams addFontParams = makeButtonParams("Add Font", resources.addFontIcon);
				addFontParams.onPressedCallback = [
					guiState = context.params.guiState,
					fontManager = resources.fontManager,
					nativeWindowHandle = context.params.nativeWindowHandle
				](BasicButtonInteractionContext) {
					if (guiState != nullptr)
					{
						FlowPlotGui::openFontImportDialog(*guiState, fontManager, nativeWindowHandle);
					}
				};
				context.uiManager.createElement(kBasicButton, resources.addFontButtonPath)
					.setParameters(std::move(addFontParams))
					/* V1 cant Update parameters made with variables */
					.mergeParams(applyNavButtonVisuals)
					.draw();

				CLAY(resources.child2GrowSpacerId, child2GrowSpacer){};
			};

			CLAY(resources.spacer2Id, spacer2){
				CLAY(resources.spacer2LineId, spacer2Line){};
			};

			CLAY(resources.child3Id, child3){
				context.uiManager.createElement(kBasicTitle, resources.statusTitlePath)
					.setParameters(std::move(statusParams))
					/* V1 cant Update parameters made with variables */
					.mergeParams([](auto& params) {
					    params.backgroundColor = Clay_Color{.r = 31.0f, .g = 46.0f, .b = 60.0f, .a = 150.0f};
					    params.borderColor = Clay_Color{.r = 0.0f, .g = 151.0f, .b = 137.0f, .a = 255.0f};
					})
					.draw();
			};
		};
	},
};
