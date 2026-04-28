#pragma once

#include <algorithm>
#include <cstdint>
#include <string>
#include <string_view>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicTitle.hpp"
#include "FlowPlotGui.hpp"

struct propertiesContentParams {
	Clay_Padding padding = Clay_Padding{12, 12, 12, 12};
	uint16_t childGap = 8;
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");

	FlowPlotGui::state* guiState = nullptr;
};

FLOWUI_DEV_REGISTER_STRUCT(
	propertiesContentParams,
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, padding),
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, childGap),
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, backgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, borderWidth),
	FLOWUI_DEV_REFLECT_FIELD(propertiesContentParams, borderColor));

using PropertiesContentDef = FlowUi::ElementDefinition<
	propertiesContentParams,
	void,
	void,
	FLOW_DEF_ID("PropertiesContent")>;

namespace FlowPlotGui
{
	inline Clay_Vector2 propertiesContentScrollOffsetForElementId(
		FlowUi::UiManager& uiManager,
		std::string_view elementId)
	{
		const Clay_ScrollContainerData data =
			Clay_GetScrollContainerData(uiManager.toClayEID(elementId));
		if (!data.found || data.scrollPosition == nullptr)
		{
			return Clay_Vector2{0.0f, 0.0f};
		}
		return *data.scrollPosition;
	}

	inline void drawPropertiesHint(
		FlowUi::UiManager& ui,
		std::string_view elementId,
		std::string text)
	{
		ui.createElement(kBasicTitle, elementId)
			.setParameters({
				.text = std::move(text),
				.padding = Clay_Padding{0, 0, 0, 0},
				.sizing = Clay_Sizing{
					.width = CLAY_SIZING_GROW(0),
					.height = CLAY_SIZING_FIT(0),
				},
				.textWrapMode = CLAY_TEXT_WRAP_WORDS,
				.fontSize = 13,
				.textColor = FlowUi::Flow_Color("#aeb2b8ff"),
			})
			.draw();
	}

	inline void drawFigureEditor(FlowUi::UiManager& ui, state& guiState, std::string_view rootId)
	{
		(void)guiState;
		drawPropertiesHint(ui, std::string(rootId) + "/figure-editor", "Figure editor");
	}

	inline void drawFigureTitleEditor(FlowUi::UiManager& ui, state& guiState, std::string_view rootId)
	{
		(void)guiState;
		drawPropertiesHint(ui, std::string(rootId) + "/figure-title-editor", "Figure title editor");
	}

	inline void drawLayoutEditor(FlowUi::UiManager& ui, state& guiState, std::string_view rootId)
	{
		(void)guiState;
		drawPropertiesHint(ui, std::string(rootId) + "/layout-editor", "Layout editor");
	}

	inline void drawPanelEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/panel-editor", "Panel editor");
	}

	inline void drawPanelTitleEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/panel-title-editor", "Panel title editor");
	}

	inline void drawAxisEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/axis-editor", "Axis editor");
	}

	inline void drawAxisTitleEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/axis-title-editor", "Axis title editor");
	}

	inline void drawLayersEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/layers-editor", "Select a layer to edit its properties");
	}

	inline void drawLayerEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/layer-editor", "Layer editor");
	}

	inline void drawLegendsEditor(FlowUi::UiManager& ui, state& guiState, std::string_view rootId)
	{
		(void)guiState;
		drawPropertiesHint(ui, std::string(rootId) + "/legends-editor", "Add or select a legend to edit its properties");
	}

	inline void drawLegendEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/legend-editor", "Legend editor");
	}

	inline void drawLegendElementEditor(FlowUi::UiManager& ui, state& guiState, const TemplateNodeKey& key, std::string_view rootId)
	{
		(void)guiState;
		(void)key;
		drawPropertiesHint(ui, std::string(rootId) + "/legend-element-editor", "Legend element editor");
	}

	inline void drawPropertiesForSelection(
		FlowUi::UiManager& ui,
		state& guiState,
		std::string_view rootId)
	{
		if (!guiState.selectedNode.has_value())
		{
			drawPropertiesHint(ui, std::string(rootId) + "/no-selection", "Select an item to edit its properties");
			return;
		}

		const TemplateNodeKey& key = *guiState.selectedNode;
		switch (key.kind)
		{
		case TemplateNodeKind::Figure:
			drawFigureEditor(ui, guiState, rootId);
			break;
		case TemplateNodeKind::FigureTitle:
			drawFigureTitleEditor(ui, guiState, rootId);
			break;
		case TemplateNodeKind::PanelsGroup:
			drawLayoutEditor(ui, guiState, rootId);
			break;
		case TemplateNodeKind::Panel:
			drawPanelEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::PanelTitle:
			drawPanelTitleEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::XAxis:
		case TemplateNodeKind::YAxis:
		case TemplateNodeKind::XSecondaryAxis:
		case TemplateNodeKind::YSecondaryAxis:
			drawAxisEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::AxisTitle:
			drawAxisTitleEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::LayersGroup:
			drawLayersEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::Layer:
			drawLayerEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::LegendsGroup:
			drawLegendsEditor(ui, guiState, rootId);
			break;
		case TemplateNodeKind::Legend:
			drawLegendEditor(ui, guiState, key, rootId);
			break;
		case TemplateNodeKind::LegendElement:
			drawLegendElementEditor(ui, guiState, key, rootId);
			break;
		}
	}
} // namespace FlowPlotGui

inline const PropertiesContentDef kPropertiesContent = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](PropertiesContentDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_Vector2 scrollOffset =
			FlowPlotGui::propertiesContentScrollOffsetForElementId(context.uiManager, context.elementID);

		Clay_ElementDeclaration root{};
		root.layout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_GROW(0),
		};
		root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		root.layout.padding = context.params.padding;
		root.layout.childGap = context.params.childGap;
		root.layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		root.backgroundColor = context.params.backgroundColor;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};
		root.clip = {
			.horizontal = false,
			.vertical = true,
			.childOffset = scrollOffset,
		};

		CLAY(rootId, root)
		{
			if (context.params.guiState == nullptr)
			{
				FlowPlotGui::drawPropertiesHint(
					context.uiManager,
					context.createChildElementId("missing-state"),
					"Properties state is unavailable");
			}
			else
			{
				FlowPlotGui::drawPropertiesForSelection(
					context.uiManager,
					*context.params.guiState,
					context.createChildElementId("selection"));
			}
		}
	},
};
