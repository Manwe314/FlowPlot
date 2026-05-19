#pragma once

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "FlowPlotGui.hpp"
#include "PlotViewportScene.hpp"
#include "BuildElements/PanelTitle.hpp"

struct plotviewPortParams {
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color viewportColor = FlowUi::Flow_Color("#7a7a7aff");
	FlowPlotGui::state* guiState = nullptr;
};

FLOWUI_DEV_REGISTER_STRUCT(
	plotviewPortParams,
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, sizing),
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, viewportColor),
	FLOWUI_DEV_REFLECT_FIELD(plotviewPortParams, guiState));

struct plotviewPortResources {
	PanelTitleBuilder titleBuilder;
	FlowUi::ViewPortManager* viewPortManager = nullptr;
	FontManager* fontManager = nullptr;
	std::string viewportKey = "FlowPlotGUI/PlotPreview";
	FlowPlotGui::PlotViewportSceneHandle scene{};

	explicit plotviewPortResources(FlowUi::App& app) :
		titleBuilder(makeTitleBuilder(app.ui())),
		viewPortManager(&app.viewPorts()),
		fontManager(&app.fonts()),
		scene(FlowPlotGui::setupPlotViewportScene(app, viewportKey)) {}

private:
	static PanelTitleBuilder makeTitleBuilder(FlowUi::UiManager& uiManager)
	{
		panelTitleParams params{};
		params.titleText = "Visual Preview";
		params.rightContentMode = panelTitleParams::RightContentMode::Button;
		params.rightButtonParams.text = "Reset";
		params.rightButtonParams.contentMode = basicButtonParams::ContentMode::TextOnly;
		params.rightButtonParams.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		PanelTitleBuilder builder = uiManager.createElement(kPanelTitle, "PlotviewPort/shared/title")
		.setParameters(std::move(params))
		.mergeParams([](auto& params) {
		    params.minHeight = 20;
		    params.padding = Clay_Padding{.left = 0, .right = 0, .top = 5, .bottom = 0};
		    params.contentPadding = Clay_Padding{.left = 10, .right = 10, .top = 0, .bottom = 0};
		    params.titleFontSize = 12;
		    params.titleColor = Clay_Color{.r = 244.0f, .g = 246.0f, .b = 248.0f, .a = 89.0f};
		    params.separatorColor = Clay_Color{.r = 94.0f, .g = 100.0f, .b = 110.0f, .a = 255.0f};
		});
		return builder;
	}
};

struct plotviewPortState {
	FlowPlotGui::PlotCamera camera{};
	bool cameraInitialized = false;
	bool cameraDragging = false;
	float lastPointerX = 0.0f;
	float lastPointerY = 0.0f;
};

inline void plotviewPortEnsureCameraInitialized(
	plotviewPortState& state,
	const FlowPlotGui::state* guiState)
{
	if (state.cameraInitialized)
	{
		return;
	}

	if (guiState != nullptr)
	{
		state.camera.centerX = static_cast<float>(guiState->activeTemplate.figure.width) * 0.5f;
		state.camera.centerY = static_cast<float>(guiState->activeTemplate.figure.height) * 0.5f;
	}
	state.camera.zoom = std::max(state.camera.zoom, 1.0e-6f);
	state.cameraInitialized = true;
}

using PlotviewPortDef = FlowUi::ElementDefinition<
	plotviewPortParams,
	plotviewPortState,
	plotviewPortResources,
	FLOW_DEF_ID("PlotviewPort")>;

inline const PlotviewPortDef kPlotviewPort = {
	+[](PlotviewPortDef::InteractionContext& context) {
		const Clay_ElementId viewportId = context.uiManager.toClayEID(context.createChildElementId("viewport"));
		if (!context.previousInteraction.isHovered(viewportId))
		{
			return;
		}

		plotviewPortState& state = PlotviewPortDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		plotviewPortEnsureCameraInitialized(state, context.params.guiState);

		const FlowUi::FrameInput& input = context.uiManager.getCurrentFrameInput();
		const bool middleMouseDown = input.mouseDown[2];
		if (middleMouseDown)
		{
			if (state.cameraDragging)
			{
				const float dx = input.mouseX - state.lastPointerX;
				const float dy = input.mouseY - state.lastPointerY;
				const float invZoom = 1.0f / std::max(state.camera.zoom, 1.0e-6f);
				state.camera.centerX -= dx * invZoom;
				state.camera.centerY -= dy * invZoom;
			}
			state.cameraDragging = true;
			state.lastPointerX = input.mouseX;
			state.lastPointerY = input.mouseY;
		}
		else
		{
			state.cameraDragging = false;
			state.lastPointerX = input.mouseX;
			state.lastPointerY = input.mouseY;
		}

		if (input.scrollY == 0.0f)
		{
			return;
		}

		const Clay_ElementData viewportData = Clay_GetElementData(viewportId);
		if (!viewportData.found || viewportData.boundingBox.width <= 0.0f || viewportData.boundingBox.height <= 0.0f)
		{
			return;
		}

		constexpr float kScrollUnitsPerWheelStep = 20.0f;
		constexpr float kZoomBasePerWheelStep = 1.1f;
		constexpr float kMinZoom = 0.02f;
		constexpr float kMaxZoom = 100.0f;

		const float oldZoom = std::max(state.camera.zoom, 1.0e-6f);
		const float zoomFactor = std::pow(kZoomBasePerWheelStep, input.scrollY / kScrollUnitsPerWheelStep);
		const float newZoom = std::clamp(oldZoom * zoomFactor, kMinZoom, kMaxZoom);
		if (newZoom == oldZoom)
		{
			return;
		}

		const float viewportX = input.mouseX - viewportData.boundingBox.x;
		const float viewportY = input.mouseY - viewportData.boundingBox.y;
		const float halfWidth = viewportData.boundingBox.width * 0.5f;
		const float halfHeight = viewportData.boundingBox.height * 0.5f;
		const float worldXUnderCursor = state.camera.centerX + (viewportX - halfWidth) / oldZoom;
		const float worldYUnderCursor = state.camera.centerY + (viewportY - halfHeight) / oldZoom;

		state.camera.zoom = newZoom;
		state.camera.centerX = worldXUnderCursor - (viewportX - halfWidth) / newZoom;
		state.camera.centerY = worldYUnderCursor - (viewportY - halfHeight) / newZoom;
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](PlotviewPortDef::BuildContext& context) {
		if (!PlotviewPortDef::resources.has_value())
		{
			return;
		}
		plotviewPortResources& resources = *PlotviewPortDef::resources;
		plotviewPortState& state = PlotviewPortDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		plotviewPortEnsureCameraInitialized(state, context.params.guiState);

		const Clay_ElementId viewportId = context.uiManager.toClayEID(context.createChildElementId("viewport"));
		const bool viewportHovered = context.uiManager.getPreviousFramesInteraction().isHovered(viewportId);
		if (!context.uiManager.getCurrentFrameInput().mouseDown[2] || !viewportHovered)
		{
			state.cameraDragging = false;
		}

		if (resources.scene.resources)
		{
			resources.scene.resources->setInput({
				.guiState = context.params.guiState,
				.fontManager = resources.fontManager,
				.camera = state.camera,
			});
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
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
		if (resources.viewPortManager != nullptr)
		{
			FlowUi::TextureRef viewportTexture = resources.viewPortManager->getTexture(resources.viewportKey);
			viewportTexture.fitMode = FlowUi::TextureFitMode::Stretch;
			viewport.image = {
				.imageData = context.uiManager.storeTexture(viewportTexture),
			};
		}

		CLAY(rootId, root){
			resources.titleBuilder
				.withElementID(titlePath)
				.draw();

			CLAY(viewportId, viewport){};
		};
	},
};
