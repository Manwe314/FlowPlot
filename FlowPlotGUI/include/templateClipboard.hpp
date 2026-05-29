#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "FlowPlotGui.hpp"

namespace FlowPlotGui {

inline TemplateNodeKind templateClipboardCompatibleKind(TemplateNodeKind kind)
{
	switch (kind)
	{
	case TemplateNodeKind::FigureTitle:
	case TemplateNodeKind::PanelTitle:
	case TemplateNodeKind::AxisTitle:
		return TemplateNodeKind::AxisTitle;
	case TemplateNodeKind::XAxis:
	case TemplateNodeKind::YAxis:
	case TemplateNodeKind::XSecondaryAxis:
	case TemplateNodeKind::YSecondaryAxis:
		return TemplateNodeKind::XAxis;
	default:
		return kind;
	}
}

inline bool templateClipboardKindIsText(TemplateNodeKind kind)
{
	return templateClipboardCompatibleKind(kind) == TemplateNodeKind::AxisTitle;
}

inline bool templateClipboardKindIsAxis(TemplateNodeKind kind)
{
	return templateClipboardCompatibleKind(kind) == TemplateNodeKind::XAxis;
}

inline std::optional<std::size_t> templateClipboardFindPanelIndex(
	const FlowPlot::Spec::MasterTemplateSpec& spec,
	std::string_view id)
{
	for (std::size_t i = 0; i < spec.panels.size(); ++i)
	{
		if (spec.panels[i].id == id)
		{
			return i;
		}
	}
	return std::nullopt;
}

inline std::optional<std::size_t> templateClipboardFindLayerIndex(
	const FlowPlot::Spec::PanelSpec& panel,
	std::string_view id)
{
	for (std::size_t i = 0; i < panel.layers.size(); ++i)
	{
		if (panel.layers[i].id == id)
		{
			return i;
		}
	}
	return std::nullopt;
}

inline std::optional<std::size_t> templateClipboardFindLegendIndex(
	const FlowPlot::Spec::FigureSpec& figure,
	std::string_view id)
{
	for (std::size_t i = 0; i < figure.legends.size(); ++i)
	{
		if (figure.legends[i].id == id)
		{
			return i;
		}
	}
	return std::nullopt;
}

inline std::optional<std::size_t> templateClipboardFindLegendElementIndex(
	const FlowPlot::Spec::LegendSpec& legend,
	std::string_view id)
{
	for (std::size_t i = 0; i < legend.legendElements.size(); ++i)
	{
		if (legend.legendElements[i].id == id)
		{
			return i;
		}
	}
	return std::nullopt;
}

inline FlowPlot::Spec::AxisSpec* templateClipboardAxisForKind(
	FlowPlot::Spec::PanelSpec& panel,
	TemplateNodeKind kind)
{
	switch (kind)
	{
	case TemplateNodeKind::XAxis:
		return &panel.xAxis;
	case TemplateNodeKind::YAxis:
		return &panel.yAxis;
	case TemplateNodeKind::XSecondaryAxis:
		return &panel.xSecondary;
	case TemplateNodeKind::YSecondaryAxis:
		return &panel.ySecondary;
	default:
		return nullptr;
	}
}

inline const FlowPlot::Spec::AxisSpec* templateClipboardAxisForKind(
	const FlowPlot::Spec::PanelSpec& panel,
	TemplateNodeKind kind)
{
	return templateClipboardAxisForKind(const_cast<FlowPlot::Spec::PanelSpec&>(panel), kind);
}

inline TemplateNodeKind templateClipboardAxisKindFromTitleSlug(std::string_view slug)
{
	if (slug == "y-axis")
	{
		return TemplateNodeKind::YAxis;
	}
	if (slug == "x-secondary-axis")
	{
		return TemplateNodeKind::XSecondaryAxis;
	}
	if (slug == "y-secondary-axis")
	{
		return TemplateNodeKind::YSecondaryAxis;
	}
	return TemplateNodeKind::XAxis;
}

inline FlowPlot::Spec::TextSpec* templateClipboardTextForNode(
	FlowPlot::Spec::MasterTemplateSpec& spec,
	const TemplateNodeKey& key)
{
	switch (key.kind)
	{
	case TemplateNodeKind::FigureTitle:
		return &spec.figure.title;
	case TemplateNodeKind::PanelTitle:
		if (const auto panelIndex = templateClipboardFindPanelIndex(spec, key.outer))
		{
			return &spec.panels[*panelIndex].title;
		}
		break;
	case TemplateNodeKind::AxisTitle:
		if (const auto panelIndex = templateClipboardFindPanelIndex(spec, key.outer))
		{
			if (FlowPlot::Spec::AxisSpec* axis = templateClipboardAxisForKind(
					spec.panels[*panelIndex],
					templateClipboardAxisKindFromTitleSlug(key.inner)))
			{
				return &axis->title;
			}
		}
		break;
	default:
		break;
	}
	return nullptr;
}

inline const FlowPlot::Spec::TextSpec* templateClipboardTextForNode(
	const FlowPlot::Spec::MasterTemplateSpec& spec,
	const TemplateNodeKey& key)
{
	return templateClipboardTextForNode(const_cast<FlowPlot::Spec::MasterTemplateSpec&>(spec), key);
}

template <typename T>
inline bool templateClipboardVectorHasId(const std::vector<T>& values, std::string_view id)
{
	return std::any_of(values.begin(), values.end(), [id](const T& value) {
		return value.id == id;
	});
}

template <typename T>
inline std::string templateClipboardNextUniqueId(const std::vector<T>& values, std::string_view prefix)
{
	for (std::size_t index = values.size() + 1; index < values.size() + 10000; ++index)
	{
		std::string candidate = std::string(prefix) + "_" + std::to_string(index);
		if (!templateClipboardVectorHasId(values, candidate))
		{
			return candidate;
		}
	}

	std::string fallback = std::string(prefix) + "_copy";
	for (std::size_t suffix = 1; templateClipboardVectorHasId(values, fallback); ++suffix)
	{
		fallback = std::string(prefix) + "_copy_" + std::to_string(suffix);
	}
	return fallback;
}

inline void templateClipboardExpandLayoutForPanelCount(
	FlowPlot::Spec::LayoutSpec& layout,
	std::size_t panelCount)
{
	layout.rows = std::max<std::uint32_t>(layout.rows, 1U);
	layout.cols = std::max<std::uint32_t>(layout.cols, 1U);
	while (static_cast<std::size_t>(layout.rows) * static_cast<std::size_t>(layout.cols) < panelCount)
	{
		if (layout.rows == layout.cols)
		{
			++layout.rows;
		}
		else if (layout.rows < layout.cols)
		{
			++layout.rows;
		}
		else
		{
			++layout.cols;
		}
	}
}

inline std::optional<TemplateClipboard> makeTemplateClipboardFromNode(
	const state& guiState,
	const TemplateNodeKey& key)
{
	TemplateClipboard clipboard{};
	clipboard.hasValue = true;
	clipboard.kind = templateClipboardCompatibleKind(key.kind);
	clipboard.source = key;

	switch (key.kind)
	{
	case TemplateNodeKind::Figure:
		clipboard.value = guiState.activeTemplate.figure;
		return clipboard;
	case TemplateNodeKind::FigureTitle:
	case TemplateNodeKind::PanelTitle:
	case TemplateNodeKind::AxisTitle:
		if (const FlowPlot::Spec::TextSpec* text = templateClipboardTextForNode(guiState.activeTemplate, key))
		{
			clipboard.value = *text;
			return clipboard;
		}
		break;
	case TemplateNodeKind::Panel:
		if (const auto panelIndex = templateClipboardFindPanelIndex(guiState.activeTemplate, key.outer))
		{
			clipboard.value = guiState.activeTemplate.panels[*panelIndex];
			return clipboard;
		}
		break;
	case TemplateNodeKind::XAxis:
	case TemplateNodeKind::YAxis:
	case TemplateNodeKind::XSecondaryAxis:
	case TemplateNodeKind::YSecondaryAxis:
		if (const auto panelIndex = templateClipboardFindPanelIndex(guiState.activeTemplate, key.outer))
		{
			if (const FlowPlot::Spec::AxisSpec* axis = templateClipboardAxisForKind(guiState.activeTemplate.panels[*panelIndex], key.kind))
			{
				clipboard.value = *axis;
				return clipboard;
			}
		}
		break;
	case TemplateNodeKind::Layer:
		if (const auto panelIndex = templateClipboardFindPanelIndex(guiState.activeTemplate, key.outer))
		{
			const FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
			if (const auto layerIndex = templateClipboardFindLayerIndex(panel, key.inner))
			{
				clipboard.value = panel.layers[*layerIndex];
				return clipboard;
			}
		}
		break;
	case TemplateNodeKind::Legend:
		if (const auto legendIndex = templateClipboardFindLegendIndex(guiState.activeTemplate.figure, key.outer))
		{
			clipboard.value = guiState.activeTemplate.figure.legends[*legendIndex];
			return clipboard;
		}
		break;
	case TemplateNodeKind::LegendElement:
		if (const auto legendIndex = templateClipboardFindLegendIndex(guiState.activeTemplate.figure, key.outer))
		{
			const FlowPlot::Spec::LegendSpec& legend = guiState.activeTemplate.figure.legends[*legendIndex];
			if (const auto elementIndex = templateClipboardFindLegendElementIndex(legend, key.inner))
			{
				clipboard.value = legend.legendElements[*elementIndex];
				return clipboard;
			}
		}
		break;
	default:
		break;
	}

	return std::nullopt;
}

inline bool copySelectedTemplateNode(state& guiState)
{
	if (!guiState.selectedNode.has_value())
	{
		return false;
	}

	if (std::optional<TemplateClipboard> clipboard = makeTemplateClipboardFromNode(guiState, *guiState.selectedNode))
	{
		guiState.templateClipboard = std::move(*clipboard);
		return true;
	}
	return false;
}

inline bool pasteTemplateClipboardOverwriteSameKind(
	state& guiState,
	const TemplateNodeKey& target,
	const TemplateClipboard& clipboard)
{
	const TemplateNodeKind targetKind = templateClipboardCompatibleKind(target.kind);
	if (targetKind != clipboard.kind)
	{
		return false;
	}

	switch (clipboard.kind)
	{
	case TemplateNodeKind::Figure:
		guiState.activeTemplate.figure = std::get<FlowPlot::Spec::FigureSpec>(clipboard.value);
		return true;
	case TemplateNodeKind::AxisTitle:
		if (FlowPlot::Spec::TextSpec* text = templateClipboardTextForNode(guiState.activeTemplate, target))
		{
			*text = std::get<FlowPlot::Spec::TextSpec>(clipboard.value);
			return true;
		}
		break;
	case TemplateNodeKind::Panel:
		if (const auto panelIndex = templateClipboardFindPanelIndex(guiState.activeTemplate, target.outer))
		{
			const std::string targetId = guiState.activeTemplate.panels[*panelIndex].id;
			FlowPlot::Spec::PanelSpec panel = std::get<FlowPlot::Spec::PanelSpec>(clipboard.value);
			panel.id = targetId;
			guiState.activeTemplate.panels[*panelIndex] = std::move(panel);
			return true;
		}
		break;
	case TemplateNodeKind::XAxis:
		if (const auto panelIndex = templateClipboardFindPanelIndex(guiState.activeTemplate, target.outer))
		{
			if (FlowPlot::Spec::AxisSpec* axis = templateClipboardAxisForKind(guiState.activeTemplate.panels[*panelIndex], target.kind))
			{
				*axis = std::get<FlowPlot::Spec::AxisSpec>(clipboard.value);
				return true;
			}
		}
		break;
	case TemplateNodeKind::Layer:
		if (const auto panelIndex = templateClipboardFindPanelIndex(guiState.activeTemplate, target.outer))
		{
			FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
			if (const auto layerIndex = templateClipboardFindLayerIndex(panel, target.inner))
			{
				const std::string targetId = panel.layers[*layerIndex].id;
				FlowPlot::Spec::LayerSpec layer = std::get<FlowPlot::Spec::LayerSpec>(clipboard.value);
				layer.id = targetId;
				panel.layers[*layerIndex] = std::move(layer);
				return true;
			}
		}
		break;
	case TemplateNodeKind::Legend:
		if (const auto legendIndex = templateClipboardFindLegendIndex(guiState.activeTemplate.figure, target.outer))
		{
			const std::string targetId = guiState.activeTemplate.figure.legends[*legendIndex].id;
			FlowPlot::Spec::LegendSpec legend = std::get<FlowPlot::Spec::LegendSpec>(clipboard.value);
			legend.id = targetId;
			guiState.activeTemplate.figure.legends[*legendIndex] = std::move(legend);
			return true;
		}
		break;
	case TemplateNodeKind::LegendElement:
		if (const auto legendIndex = templateClipboardFindLegendIndex(guiState.activeTemplate.figure, target.outer))
		{
			FlowPlot::Spec::LegendSpec& legend = guiState.activeTemplate.figure.legends[*legendIndex];
			if (const auto elementIndex = templateClipboardFindLegendElementIndex(legend, target.inner))
			{
				const std::string targetId = legend.legendElements[*elementIndex].id;
				FlowPlot::Spec::LegendElementSpec element = std::get<FlowPlot::Spec::LegendElementSpec>(clipboard.value);
				element.id = targetId;
				legend.legendElements[*elementIndex] = std::move(element);
				return true;
			}
		}
		break;
	default:
		break;
	}

	return false;
}

inline bool pasteTemplateClipboardAppendIntoTarget(
	state& guiState,
	const TemplateNodeKey& target,
	const TemplateClipboard& clipboard,
	TemplateNodeKey* outNewSelection = nullptr)
{
	if (clipboard.kind == TemplateNodeKind::Panel
		&& (target.kind == TemplateNodeKind::Figure || target.kind == TemplateNodeKind::PanelsGroup))
	{
		FlowPlot::Spec::PanelSpec panel = std::get<FlowPlot::Spec::PanelSpec>(clipboard.value);
		panel.id = templateClipboardNextUniqueId(guiState.activeTemplate.panels, "panel");
		const std::string newId = panel.id;
		guiState.activeTemplate.panels.push_back(std::move(panel));
		templateClipboardExpandLayoutForPanelCount(guiState.activeTemplate.layout, guiState.activeTemplate.panels.size());
		if (outNewSelection != nullptr)
		{
			*outNewSelection = TemplateNodeKey{.kind = TemplateNodeKind::Panel, .outer = newId};
		}
		return true;
	}

	if (clipboard.kind == TemplateNodeKind::Layer
		&& (target.kind == TemplateNodeKind::Panel || target.kind == TemplateNodeKind::LayersGroup))
	{
		if (const auto panelIndex = templateClipboardFindPanelIndex(guiState.activeTemplate, target.outer))
		{
			FlowPlot::Spec::PanelSpec& panel = guiState.activeTemplate.panels[*panelIndex];
			FlowPlot::Spec::LayerSpec layer = std::get<FlowPlot::Spec::LayerSpec>(clipboard.value);
			layer.id = templateClipboardNextUniqueId(panel.layers, "layer");
			const std::string newId = layer.id;
			panel.layers.push_back(std::move(layer));
			if (outNewSelection != nullptr)
			{
				*outNewSelection = TemplateNodeKey{.kind = TemplateNodeKind::Layer, .outer = panel.id, .inner = newId};
			}
			return true;
		}
	}

	if (clipboard.kind == TemplateNodeKind::Legend
		&& (target.kind == TemplateNodeKind::Figure || target.kind == TemplateNodeKind::LegendsGroup))
	{
		FlowPlot::Spec::LegendSpec legend = std::get<FlowPlot::Spec::LegendSpec>(clipboard.value);
		legend.id = templateClipboardNextUniqueId(guiState.activeTemplate.figure.legends, "legend");
		const std::string newId = legend.id;
		guiState.activeTemplate.figure.legends.push_back(std::move(legend));
		if (outNewSelection != nullptr)
		{
			*outNewSelection = TemplateNodeKey{.kind = TemplateNodeKind::Legend, .outer = newId};
		}
		return true;
	}

	if (clipboard.kind == TemplateNodeKind::LegendElement && target.kind == TemplateNodeKind::Legend)
	{
		if (const auto legendIndex = templateClipboardFindLegendIndex(guiState.activeTemplate.figure, target.outer))
		{
			FlowPlot::Spec::LegendSpec& legend = guiState.activeTemplate.figure.legends[*legendIndex];
			FlowPlot::Spec::LegendElementSpec element = std::get<FlowPlot::Spec::LegendElementSpec>(clipboard.value);
			element.id = templateClipboardNextUniqueId(legend.legendElements, "legend_element");
			const std::string newId = element.id;
			legend.legendElements.push_back(std::move(element));
			if (outNewSelection != nullptr)
			{
				*outNewSelection = TemplateNodeKey{.kind = TemplateNodeKind::LegendElement, .outer = legend.id, .inner = newId};
			}
			return true;
		}
	}

	return false;
}

inline bool pasteTemplateClipboardIntoSelection(state& guiState)
{
	if (!guiState.selectedNode.has_value() || !guiState.templateClipboard.hasValue)
	{
		return false;
	}

	prepareImmediateDocumentChange(guiState);
	const TemplateNodeKey target = *guiState.selectedNode;
	TemplateNodeKey newSelection{};
	const bool changed =
		pasteTemplateClipboardOverwriteSameKind(guiState, target, guiState.templateClipboard) ||
		pasteTemplateClipboardAppendIntoTarget(guiState, target, guiState.templateClipboard, &newSelection);
	if (!changed)
	{
		return false;
	}

	if (newSelection.kind != TemplateNodeKind::Figure || !newSelection.outer.empty() || !newSelection.inner.empty())
	{
		guiState.selectedNode = std::move(newSelection);
	}
	markTemplateChanged(guiState);
	commitImmediateDocumentChange(guiState);
	return true;
}

inline bool templateClipboardNodeIsDuplicable(TemplateNodeKind kind)
{
	switch (kind)
	{
	case TemplateNodeKind::Panel:
	case TemplateNodeKind::Layer:
	case TemplateNodeKind::Legend:
	case TemplateNodeKind::LegendElement:
		return true;
	default:
		return false;
	}
}

inline std::optional<TemplateNodeKey> templateClipboardDuplicateTargetFor(const TemplateNodeKey& key)
{
	switch (key.kind)
	{
	case TemplateNodeKind::Panel:
		return TemplateNodeKey{.kind = TemplateNodeKind::PanelsGroup};
	case TemplateNodeKind::Layer:
		return TemplateNodeKey{.kind = TemplateNodeKind::LayersGroup, .outer = key.outer};
	case TemplateNodeKind::Legend:
		return TemplateNodeKey{.kind = TemplateNodeKind::LegendsGroup};
	case TemplateNodeKind::LegendElement:
		return TemplateNodeKey{.kind = TemplateNodeKind::Legend, .outer = key.outer};
	default:
		return std::nullopt;
	}
}

inline bool duplicateSelectedTemplateNode(state& guiState)
{
	if (!guiState.selectedNode.has_value() || !templateClipboardNodeIsDuplicable(guiState.selectedNode->kind))
	{
		return false;
	}

	const TemplateNodeKey selected = *guiState.selectedNode;
	std::optional<TemplateClipboard> clipboard = makeTemplateClipboardFromNode(guiState, selected);
	std::optional<TemplateNodeKey> target = templateClipboardDuplicateTargetFor(selected);
	if (!clipboard.has_value() || !target.has_value())
	{
		return false;
	}

	prepareImmediateDocumentChange(guiState);
	TemplateNodeKey newSelection{};
	if (!pasteTemplateClipboardAppendIntoTarget(guiState, *target, *clipboard, &newSelection))
	{
		return false;
	}

	guiState.selectedNode = std::move(newSelection);
	markTemplateChanged(guiState);
	commitImmediateDocumentChange(guiState);
	return true;
}

} // namespace FlowPlotGui
