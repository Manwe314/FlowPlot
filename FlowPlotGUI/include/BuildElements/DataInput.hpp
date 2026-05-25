#pragma once

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"
#include "BuildElements/DataInputHeader.hpp"
#include "BuildElements/DataInputTable.hpp"

struct dataInputParams {
	FlowPlotGui::state* guiState = nullptr;
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#7a7a7aff");
	Clay_Color bodyBackgroundColor = FlowUi::Flow_Color("#2a2a2eff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	dataInputParams,
	FLOWUI_DEV_REFLECT_FIELD(dataInputParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(dataInputParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(dataInputParams, bodyBackgroundColor));

struct dataInputState {
	std::size_t activeDatasetIndex = 0;
	int height = 320;
};

FLOWUI_DEV_REGISTER_STRUCT(
	dataInputState,
	FLOWUI_DEV_REFLECT_FIELD(dataInputState, activeDatasetIndex),
	FLOWUI_DEV_REFLECT_FIELD(dataInputState, height));

using DataInputDef = FlowUi::ElementDefinition<dataInputParams, dataInputState, void, FLOW_DEF_ID("DataInput")>;

inline const DataInputDef kDataInput = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DataInputDef::BuildContext& context) {
		dataInputState& state = DataInputDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (context.params.guiState != nullptr && !context.params.guiState->datasets.empty() &&
			state.activeDatasetIndex >= context.params.guiState->datasets.size())
		{
			state.activeDatasetIndex = context.params.guiState->datasets.size() - 1;
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		rootLayout.sizing = context.params.sizing;

		Clay_ElementDeclaration root{};
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		const Clay_ElementId bodyId = context.uiManager.toClayEID(context.createChildElementId("body"));

		Clay_LayoutConfig bodyLayout{};
		bodyLayout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};

		Clay_ElementDeclaration body{};
		body.layout = bodyLayout;
		body.backgroundColor = context.params.bodyBackgroundColor;
		body.cornerRadius = CLAY_CORNER_RADIUS(0);
		body.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(rootId, root){
			context.uiManager.createElement(kDataInputHeader, context.createChildElementId("header"))
				.setParameters({
				    .guiState = context.params.guiState,
				    .activeDatasetIndex = &state.activeDatasetIndex,
				    .padding = Clay_Padding{.left = 8, .right = 8, .top = 6, .bottom = 0},
				    .childGap = 7,
				    .addRowContainerPadding = Clay_Padding{.left = 6, .right = 0, .top = 0, .bottom = 6}
				})
				.draw();
			CLAY(bodyId, body){
				context.uiManager.createElement(kDataInputTable, context.createChildElementId("table"))
					.setParameters({
						.guiState = context.params.guiState,
						.activeDatasetIndex = &state.activeDatasetIndex,
					})
					.draw();
			};
		};
	},
};
