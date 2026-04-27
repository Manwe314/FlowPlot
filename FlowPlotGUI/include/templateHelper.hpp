#pragma once

#include <cstddef>
#include <string>

#include <FlowUi/Flow.hpp>

#include "BuildElements/TemplateLayer.hpp"
#include "FlowPlotGui.hpp"

namespace FlowPlotGui {

struct TemplateNodeRef {
	TemplateNodeKind kind = TemplateNodeKind::Figure;
	std::size_t panelIndex = 0;
	std::size_t layerIndex = 0;
	std::size_t legendIndex = 0;
	std::size_t legendElementIndex = 0;
	int depth = 0;
};

templateLayerParams makeTemplateLayerParams(
	FlowUi::App& app,
	FlowPlotGui::state& guiState,
	const TemplateNodeRef& node,
	std::string_view elementId);

inline TemplateNodeKey templateNodeKey(
	const FlowPlot::Spec::MasterTemplateSpec& spec,
	const TemplateNodeRef& node)
{
	TemplateNodeKey key{};
	key.kind = node.kind;
	switch (node.kind)
	{
	case TemplateNodeKind::Panel:
	case TemplateNodeKind::PanelTitle:
	case TemplateNodeKind::XAxis:
	case TemplateNodeKind::YAxis:
	case TemplateNodeKind::XSecondaryAxis:
	case TemplateNodeKind::YSecondaryAxis:
	case TemplateNodeKind::LayersGroup:
		if (node.panelIndex < spec.panels.size())
		{
			key.outer = spec.panels[node.panelIndex].id;
		}
		break;
	case TemplateNodeKind::Layer:
		if (node.panelIndex < spec.panels.size())
		{
			key.outer = spec.panels[node.panelIndex].id;
			if (node.layerIndex < spec.panels[node.panelIndex].layers.size())
			{
				key.inner = spec.panels[node.panelIndex].layers[node.layerIndex].id;
			}
		}
		break;
	case TemplateNodeKind::Legend:
		if (node.legendIndex < spec.figure.legends.size())
		{
			key.outer = spec.figure.legends[node.legendIndex].id;
		}
		break;
	case TemplateNodeKind::LegendElement:
		if (node.legendIndex < spec.figure.legends.size())
		{
			key.outer = spec.figure.legends[node.legendIndex].id;
			if (node.legendElementIndex < spec.figure.legends[node.legendIndex].legendElements.size())
			{
				key.inner = spec.figure.legends[node.legendIndex].legendElements[node.legendElementIndex].id;
			}
		}
		break;
	default:
		break;
	}
	return key;
}

inline std::string templateNodeId(
	const FlowPlot::Spec::MasterTemplateSpec& spec,
	const std::string& root,
	const TemplateNodeRef& node)
{
	switch (node.kind)
	{
	case TemplateNodeKind::Figure:
		return root + "/figure";
	case TemplateNodeKind::FigureTitle:
		return root + "/figure/title";
	case TemplateNodeKind::PanelsGroup:
		return root + "/panels";
	case TemplateNodeKind::Panel:
		if (node.panelIndex < spec.panels.size())
		{
			return root + "/panels/" + spec.panels[node.panelIndex].id;
		}
		return root + "/panels/invalid";
	case TemplateNodeKind::PanelTitle:
		if (node.panelIndex < spec.panels.size())
		{
			return root + "/panels/" + spec.panels[node.panelIndex].id + "/title";
		}
		return root + "/panels/invalid/title";
	case TemplateNodeKind::XAxis:
		if (node.panelIndex < spec.panels.size())
		{
			return root + "/panels/" + spec.panels[node.panelIndex].id + "/x-axis";
		}
		return root + "/panels/invalid/x-axis";
	case TemplateNodeKind::YAxis:
		if (node.panelIndex < spec.panels.size())
		{
			return root + "/panels/" + spec.panels[node.panelIndex].id + "/y-axis";
		}
		return root + "/panels/invalid/y-axis";
	case TemplateNodeKind::XSecondaryAxis:
		if (node.panelIndex < spec.panels.size())
		{
			return root + "/panels/" + spec.panels[node.panelIndex].id + "/x-secondary-axis";
		}
		return root + "/panels/invalid/x-secondary-axis";
	case TemplateNodeKind::YSecondaryAxis:
		if (node.panelIndex < spec.panels.size())
		{
			return root + "/panels/" + spec.panels[node.panelIndex].id + "/y-secondary-axis";
		}
		return root + "/panels/invalid/y-secondary-axis";
	case TemplateNodeKind::LayersGroup:
		if (node.panelIndex < spec.panels.size())
		{
			return root + "/panels/" + spec.panels[node.panelIndex].id + "/layers";
		}
		return root + "/panels/invalid/layers";
	case TemplateNodeKind::Layer:
		if (node.panelIndex < spec.panels.size() && node.layerIndex < spec.panels[node.panelIndex].layers.size())
		{
			return root + "/panels/" + spec.panels[node.panelIndex].id + "/layers/" + spec.panels[node.panelIndex].layers[node.layerIndex].id;
		}
		return root + "/panels/invalid/layers/invalid";
	case TemplateNodeKind::LegendsGroup:
		return root + "/figure/legends";
	case TemplateNodeKind::Legend:
		if (node.legendIndex < spec.figure.legends.size())
		{
			return root + "/figure/legends/" + spec.figure.legends[node.legendIndex].id;
		}
		return root + "/figure/legends/invalid";
	case TemplateNodeKind::LegendElement:
		if (node.legendIndex < spec.figure.legends.size() && node.legendElementIndex < spec.figure.legends[node.legendIndex].legendElements.size())
		{
			return root + "/figure/legends/" + spec.figure.legends[node.legendIndex].id + "/elements/" + spec.figure.legends[node.legendIndex].legendElements[node.legendElementIndex].id;
		}
		return root + "/figure/legends/invalid/elements/invalid";
	}

	return root + "/unknown";
}

inline void drawTemplateNode(
	FlowUi::App& app,
	FlowUi::UiManager& ui,
	FlowPlotGui::state& guiState,
	const std::string& treeRootId,
	const TemplateNodeRef& node)
{
	const std::string elementId = templateNodeId(guiState.activeTemplate, treeRootId, node);

	ui.createElement(kTemplateLayer, elementId)
		.setParameters(makeTemplateLayerParams(app, guiState, node, elementId))
		.draw();

	const templateLayerState* rowState =
		TemplateLayerDef::tryGetStateConst(FlowUi::toFlowId(elementId));
	if (!rowState || !rowState->isExpanded)
	{
		return;
	}

	switch (node.kind)
	{
	case TemplateNodeKind::Figure:
		drawTemplateNode(app, ui, guiState, treeRootId, {
			.kind = TemplateNodeKind::FigureTitle,
			.depth = node.depth + 1,
		});

		drawTemplateNode(app, ui, guiState, treeRootId, {
			.kind = TemplateNodeKind::PanelsGroup,
			.depth = node.depth + 1,
		});
		drawTemplateNode(app, ui, guiState, treeRootId, {
			.kind = TemplateNodeKind::LegendsGroup,
			.depth = node.depth + 1,
		});
		break;
	case TemplateNodeKind::PanelsGroup:
		for (std::size_t i = 0; i < guiState.activeTemplate.panels.size(); ++i)
		{
			drawTemplateNode(app, ui, guiState, treeRootId, {
				.kind = TemplateNodeKind::Panel,
				.panelIndex = i,
				.depth = node.depth + 1,
			});
		}
		break;
	case TemplateNodeKind::Panel:
		drawTemplateNode(app, ui, guiState, treeRootId, {
			.kind = TemplateNodeKind::PanelTitle,
			.panelIndex = node.panelIndex,
			.depth = node.depth + 1,
		});
		drawTemplateNode(app, ui, guiState, treeRootId, {
			.kind = TemplateNodeKind::XAxis,
			.panelIndex = node.panelIndex,
			.depth = node.depth + 1,
		});
		drawTemplateNode(app, ui, guiState, treeRootId, {
			.kind = TemplateNodeKind::YAxis,
			.panelIndex = node.panelIndex,
			.depth = node.depth + 1,
		});
		drawTemplateNode(app, ui, guiState, treeRootId, {
			.kind = TemplateNodeKind::LayersGroup,
			.panelIndex = node.panelIndex,
			.depth = node.depth + 1,
		});
		break;

	case TemplateNodeKind::LayersGroup:
		for (std::size_t i = 0; node.panelIndex < guiState.activeTemplate.panels.size() && i < guiState.activeTemplate.panels[node.panelIndex].layers.size(); ++i)
		{
			drawTemplateNode(app, ui, guiState, treeRootId, {
				.kind = TemplateNodeKind::Layer,
				.panelIndex = node.panelIndex,
				.layerIndex = i,
				.depth = node.depth + 1,
			});
		}
		break;
	case TemplateNodeKind::LegendsGroup:
		for (std::size_t i = 0; i < guiState.activeTemplate.figure.legends.size(); ++i)
		{
			drawTemplateNode(app, ui, guiState, treeRootId, {
				.kind = TemplateNodeKind::Legend,
				.legendIndex = i,
				.depth = node.depth + 1,
			});
		}
		break;
	case TemplateNodeKind::Legend:
		if (node.legendIndex < guiState.activeTemplate.figure.legends.size())
		{
			for (std::size_t i = 0; i < guiState.activeTemplate.figure.legends[node.legendIndex].legendElements.size(); ++i)
			{
				drawTemplateNode(app, ui, guiState, treeRootId, {
					.kind = TemplateNodeKind::LegendElement,
					.legendIndex = node.legendIndex,
					.legendElementIndex = i,
					.depth = node.depth + 1,
				});
			}
		}
		break;
	default:
		break;
	}
}

} // namespace FlowPlotGui
