#include "templateHelper.hpp"

#include <algorithm>

namespace FlowPlotGui {
namespace {

template <typename Container>
std::string makeUniqueId(const Container& items, std::string_view prefix)
{
	for (std::size_t n = items.size() + 1;; ++n)
	{
		std::string candidate = std::string(prefix) + "_" + std::to_string(n);
		const bool exists = std::any_of(items.begin(), items.end(), [&](const auto& item) {
			return item.id == candidate;
		});
		if (!exists)
		{
			return candidate;
		}
	}
}

std::optional<std::size_t> findPanelIndex(const FlowPlot::Spec::MasterTemplateSpec& spec, std::string_view panelId)
{
	for (std::size_t i = 0; i < spec.panels.size(); ++i)
	{
		if (spec.panels[i].id == panelId)
		{
			return i;
		}
	}
	return std::nullopt;
}

std::optional<std::size_t> findLayerIndex(const FlowPlot::Spec::PanelSpec& panel, std::string_view layerId)
{
	for (std::size_t i = 0; i < panel.layers.size(); ++i)
	{
		if (panel.layers[i].id == layerId)
		{
			return i;
		}
	}
	return std::nullopt;
}

std::optional<std::size_t> findLegendIndex(const FlowPlot::Spec::MasterTemplateSpec& spec, std::string_view legendId)
{
	for (std::size_t i = 0; i < spec.figure.legends.size(); ++i)
	{
		if (spec.figure.legends[i].id == legendId)
		{
			return i;
		}
	}
	return std::nullopt;
}

std::optional<std::size_t> findLegendElementIndex(const FlowPlot::Spec::LegendSpec& legend, std::string_view elementId)
{
	for (std::size_t i = 0; i < legend.legendElements.size(); ++i)
	{
		if (legend.legendElements[i].id == elementId)
		{
			return i;
		}
	}
	return std::nullopt;
}

void clearSelectionIfImpacted(FlowPlotGui::state& guiState, const TemplateNodeKey& deletedKey)
{
	if (!guiState.selectedNode.has_value())
	{
		return;
	}

	const TemplateNodeKey& selected = *guiState.selectedNode;
	if (deletedKey.kind == TemplateNodeKind::Panel && selected.outer == deletedKey.outer)
	{
		guiState.selectedNode = std::nullopt;
		return;
	}
	if (deletedKey.kind == TemplateNodeKind::Legend && selected.outer == deletedKey.outer)
	{
		guiState.selectedNode = std::nullopt;
		return;
	}
	if (selected.kind == deletedKey.kind && selected.outer == deletedKey.outer && selected.inner == deletedKey.inner)
	{
		guiState.selectedNode = std::nullopt;
	}
}

void addTemplateNodeChild(FlowPlotGui::state& guiState, const TemplateNodeKey& parentKey)
{
	switch (parentKey.kind)
	{
	case TemplateNodeKind::PanelsGroup: {
		FlowPlot::Spec::PanelSpec panel{};
		panel.id = makeUniqueId(guiState.activeTemplate.panels, "panel");
		guiState.activeTemplate.panels.push_back(std::move(panel));
		break;
	}
	case TemplateNodeKind::LayersGroup: {
		const auto panelIndex = findPanelIndex(guiState.activeTemplate, parentKey.outer);
		if (!panelIndex)
		{
			break;
		}
		FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
		FlowPlot::Spec::LayerSpec layer{};
		layer.id = makeUniqueId(panel.layers, "layer");
		panel.layers.push_back(std::move(layer));
		break;
	}
	case TemplateNodeKind::LegendsGroup: {
		FlowPlot::Spec::LegendSpec legend{};
		legend.id = makeUniqueId(guiState.activeTemplate.figure.legends, "legend");
		guiState.activeTemplate.figure.legends.push_back(std::move(legend));
		break;
	}
	case TemplateNodeKind::Legend: {
		const auto legendIndex = findLegendIndex(guiState.activeTemplate, parentKey.outer);
		if (!legendIndex)
		{
			break;
		}
		FlowPlot::Spec::LegendSpec& legend = guiState.activeTemplate.figure.legends[*legendIndex];
		FlowPlot::Spec::LegendElementSpec element{};
		element.id = makeUniqueId(legend.legendElements, "legend_element");
		legend.legendElements.push_back(std::move(element));
		break;
	}
	case TemplateNodeKind::AxisTitle:
		break;
	default:
		break;
	}
}

void deleteTemplateNode(FlowPlotGui::state& guiState, const TemplateNodeKey& key)
{
	switch (key.kind)
	{
	case TemplateNodeKind::Panel: {
		const auto panelIndex = findPanelIndex(guiState.activeTemplate, key.outer);
		if (panelIndex)
		{
			guiState.activeTemplate.panels.erase(guiState.activeTemplate.panels.begin() + static_cast<std::ptrdiff_t>(*panelIndex));
			clearSelectionIfImpacted(guiState, key);
		}
		break;
	}
	case TemplateNodeKind::Layer: {
		const auto panelIndex = findPanelIndex(guiState.activeTemplate, key.outer);
		if (!panelIndex)
		{
			break;
		}
		FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
		const auto layerIndex = findLayerIndex(panel, key.inner);
		if (layerIndex)
		{
			panel.layers.erase(panel.layers.begin() + static_cast<std::ptrdiff_t>(*layerIndex));
			clearSelectionIfImpacted(guiState, key);
		}
		break;
	}
	case TemplateNodeKind::Legend: {
		const auto legendIndex = findLegendIndex(guiState.activeTemplate, key.outer);
		if (legendIndex)
		{
			guiState.activeTemplate.figure.legends.erase(guiState.activeTemplate.figure.legends.begin() + static_cast<std::ptrdiff_t>(*legendIndex));
			clearSelectionIfImpacted(guiState, key);
		}
		break;
	}
	case TemplateNodeKind::LegendElement: {
		const auto legendIndex = findLegendIndex(guiState.activeTemplate, key.outer);
		if (!legendIndex)
		{
			break;
		}
		FlowPlot::Spec::LegendSpec& legend = guiState.activeTemplate.figure.legends[*legendIndex];
		const auto elementIndex = findLegendElementIndex(legend, key.inner);
		if (elementIndex)
		{
			legend.legendElements.erase(legend.legendElements.begin() + static_cast<std::ptrdiff_t>(*elementIndex));
			clearSelectionIfImpacted(guiState, key);
		}
		break;
	}
	case TemplateNodeKind::AxisTitle:
		break;
	default:
		break;
	}
}

} // namespace

templateLayerParams makeTemplateLayerParams(
	FlowUi::App& app,
	FlowPlotGui::state& guiState,
	const TemplateNodeRef& node,
	std::string_view elementId)
{
	const FlowPlot::Spec::MasterTemplateSpec& spec = guiState.activeTemplate;
	TemplateNodeKey key = templateNodeKey(spec, node);
	key.flowElementId = std::string(elementId);

	templateLayerParams params{};
	params.guiState = &guiState;
	params.nodeKey = key;
	params.depth = node.depth;
	params.pixelsPerDepth = 18;
	params.mainContentMinWidth = 180;
	params.mainContentMaxWidth = 420;
	params.mainContentPadding = Clay_Padding{0, 8, 0, 0};
	params.mainTextColor = FlowUi::Flow_Color("#f4f4f6ff");
	params.mainTextFontSize = 14;
	params.backgroundColor = FlowUi::Flow_Color("#00000000");
	params.hoverBackgroundColor = FlowUi::Flow_Color("#242428ff");
	params.focusedBackgroundColor = FlowUi::Flow_Color("#1f3a4cff");
	params.buttonHoverBackgroundColor = FlowUi::Flow_Color("#303036ff");
	params.expandedIcon = app.icons().textureRef("DownArrow");
	params.collapsedIcon = app.icons().textureRef("RightArrow");
	params.adderIcon = app.icons().textureRef("Plus");
	params.deleterIcon = app.icons().textureRef("Trash");
	params.mainIconTintColor = FlowUi::Flow_Color("#ffffffff");
	params.buttonIconTintColor = FlowUi::Flow_Color("#ffffffff");

	switch (node.kind)
	{
	case TemplateNodeKind::Figure:
		params.mainText = "Figure";
		params.mainIcon = app.icons().textureRef("Layers");
		params.showExpanderButton = true;
		break;
	case TemplateNodeKind::FigureTitle:
		params.mainText = "Title";
		params.mainIcon = app.icons().textureRef("T");
		break;
	case TemplateNodeKind::PanelsGroup:
		params.mainText = "Panels";
		params.mainIcon = app.icons().textureRef("file");
		params.showExpanderButton = true;
		params.showAdderButton = true;
		break;
	case TemplateNodeKind::Panel:
		params.mainText = "Panel";
		if (node.panelIndex < spec.panels.size() && !spec.panels[node.panelIndex].id.empty())
		{
			params.mainText = spec.panels[node.panelIndex].id;
		}
		params.mainIcon = app.icons().textureRef("Panel");
		params.showExpanderButton = true;
		params.showDeleterButton = true;
		break;
	case TemplateNodeKind::PanelTitle:
		params.mainText = "Panel title";
		params.mainIcon = app.icons().textureRef("T");
		break;
	case TemplateNodeKind::XAxis:
		params.mainText = "X axis";
		params.mainIcon = app.icons().textureRef("Axis");
		params.showExpanderButton = true;
		break;
	case TemplateNodeKind::YAxis:
		params.mainText = "Y axis";
		params.mainIcon = app.icons().textureRef("Axis");
		params.showExpanderButton = true;
		break;
	case TemplateNodeKind::XSecondaryAxis:
		params.mainText = "X secondary axis";
		params.mainIcon = app.icons().textureRef("Axis");
		params.showExpanderButton = true;
		break;
	case TemplateNodeKind::YSecondaryAxis:
		params.mainText = "Y secondary axis";
		params.mainIcon = app.icons().textureRef("Axis");
		params.showExpanderButton = true;
		break;
	case TemplateNodeKind::AxisTitle:
		params.mainText = "Axis title";
		params.mainIcon = app.icons().textureRef("T");
		break;
	case TemplateNodeKind::LayersGroup:
		params.mainText = "Layers";
		params.mainIcon = app.icons().textureRef("file");
		params.showExpanderButton = true;
		params.showAdderButton = true;
		break;
	case TemplateNodeKind::Layer:
		params.mainText = "Layer";
		if (node.panelIndex < spec.panels.size() && node.layerIndex < spec.panels[node.panelIndex].layers.size())
		{
			const FlowPlot::Spec::LayerSpec& layer = spec.panels[node.panelIndex].layers[node.layerIndex];
			params.mainText = layer.id.empty() ? layer.type : layer.id;
		}
		params.mainIcon = app.icons().textureRef("Layers");
		params.showDeleterButton = true;
		break;
	case TemplateNodeKind::LegendsGroup:
		params.mainText = "Legends";
		params.mainIcon = app.icons().textureRef("file");
		params.showExpanderButton = true;
		params.showAdderButton = true;
		break;
	case TemplateNodeKind::Legend:
		params.mainText = "Legend";
		if (node.legendIndex < spec.figure.legends.size() && !spec.figure.legends[node.legendIndex].id.empty())
		{
			params.mainText = spec.figure.legends[node.legendIndex].id;
		}
		params.mainIcon = app.icons().textureRef("Preset");
		params.showExpanderButton = true;
		params.showAdderButton = true;
		params.showDeleterButton = true;
		break;
	case TemplateNodeKind::LegendElement:
		params.mainText = "Legend element";
		if (node.legendIndex < spec.figure.legends.size() && node.legendElementIndex < spec.figure.legends[node.legendIndex].legendElements.size())
		{
			const FlowPlot::Spec::LegendElementSpec& element = spec.figure.legends[node.legendIndex].legendElements[node.legendElementIndex];
			params.mainText = element.id.empty() ? element.text : element.id;
		}
		params.mainIcon = app.icons().textureRef("Hashtag");
		params.showDeleterButton = true;
		break;
	}

	if (params.showAdderButton)
	{
		params.onAddPressedCallback = [&guiState, key]() {
			addTemplateNodeChild(guiState, key);
		};
	}
	if (params.showDeleterButton)
	{
		params.onDeletePressedCallback = [&guiState, key]() {
			deleteTemplateNode(guiState, key);
		};
	}

	return params;
}

} // namespace FlowPlotGui
