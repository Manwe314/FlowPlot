#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "FlowPlot_Defaults.hpp"
#include "FlowPlot.hpp"

namespace FlowPlotGui {
inline constexpr std::size_t kInitialDatasetCapacity = 3;
inline constexpr std::size_t kInitialColumnsPerTypeCapacity = 4;
inline constexpr std::size_t kInitialRowsCapacity = 8;
inline constexpr std::size_t kMaxDatasetCount = 10;
inline constexpr std::size_t kDocumentHistoryCapacity = 30;

enum class GlobalState : uint8_t
{
	working,
	exporting,
};

struct PlotCamera {
	float centerX = 0.0f;
	float centerY = 0.0f;
	float zoom = 1.0f;
};

enum class TemplateNodeKind : uint8_t {
	Figure,
	FigureTitle,
	PanelsGroup,
	Panel,
	PanelTitle,
	XAxis,
	YAxis,
	XSecondaryAxis,
	YSecondaryAxis,
	AxisTitle,
	LayersGroup,
	Layer,
	LegendsGroup,
	Legend,
	LegendElement,
};

struct TemplateNodeKey {
	TemplateNodeKind kind = TemplateNodeKind::Figure;
	std::string outer{};
	std::string inner{};
	std::string flowElementId{};
};

struct numericColumn {
	numericColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<double> data;
	std::string name;
};

struct stringColumn {
	stringColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<std::string> data;
	std::string name;
};

struct boolColumn {
	boolColumn()
	{
		data.reserve(kInitialRowsCapacity);
	}

	std::vector<bool> data;
	std::string name;
};

struct RunningDataset {
	RunningDataset()
	{
		numericColumns.reserve(kInitialColumnsPerTypeCapacity);
		stringColumns.reserve(kInitialColumnsPerTypeCapacity);
		boolColumns.reserve(kInitialColumnsPerTypeCapacity);
	}

	std::vector<numericColumn> numericColumns;
	std::vector<stringColumn> stringColumns;
	std::vector<boolColumn> boolColumns; 

	std::string name;
};

struct AddedFontVariant {
	std::string family = "Default";
	std::uint16_t weight = 400;
	FlowPlot::FontStyle style = FlowPlot::FontStyle::Normal;
	std::filesystem::path path{};
};

enum class DiagnosticSeverity : std::uint8_t {
	Warning,
	Error,
};

struct Diagnostic {
	DiagnosticSeverity severity = DiagnosticSeverity::Error;
	std::string source{};
	std::string message{};
};

struct DocumentSnapshot {
	FlowPlot::Spec::MasterTemplateSpec activeTemplate{};
	std::vector<RunningDataset> datasets{};
};

struct DocumentHistory {
	std::array<DocumentSnapshot, kDocumentHistoryCapacity> snapshots{};
	std::size_t start = 0;
	std::size_t count = 0;
	std::size_t cursor = 0;
};

enum class DocumentEditTargetKind : std::uint8_t {
	Unknown,
	Template,
	Dataset,
};

struct state;
using DocumentEditComparator = std::function<bool(const DocumentSnapshot& before, const state& current)>;

struct DocumentEditTarget {
	DocumentEditTargetKind kind = DocumentEditTargetKind::Unknown;
	std::string key{};
	DocumentEditComparator changed = nullptr;
};

struct DeferredDocumentEdit {
	bool active = false;
	DocumentEditTarget target{};
	DocumentSnapshot before{};
	float timeoutSeconds = -1.0f;
	float elapsedSeconds = 0.0f;
};

enum class DocumentIndicatorState : std::uint8_t {
	Saved,
	Unsaved,
	Errors,
};

inline void appendUniqueString(std::vector<std::string>& values, std::string value)
{
	if (std::find(values.begin(), values.end(), value) == values.end())
	{
		values.push_back(std::move(value));
	}
}

inline std::vector<std::string> availableFontFamilies(const std::vector<AddedFontVariant>& fontLibrary)
{
	std::vector<std::string> families{};
	families.reserve(fontLibrary.size() + 1U);
	families.push_back("Default");

	for (const AddedFontVariant& variant : fontLibrary)
	{
		if (!variant.family.empty())
		{
			appendUniqueString(families, variant.family);
		}
	}

	if (families.size() > 1U)
	{
		std::sort(families.begin() + 1, families.end());
	}
	return families;
}

inline std::vector<std::string> availableWeightsForFamily(
	const std::vector<AddedFontVariant>& fontLibrary,
	std::string_view family)
{
	std::vector<std::uint16_t> weights{};
	weights.push_back(400);

	for (const AddedFontVariant& variant : fontLibrary)
	{
		if (variant.family == family
			&& std::find(weights.begin(), weights.end(), variant.weight) == weights.end())
		{
			weights.push_back(variant.weight);
		}
	}

	std::sort(weights.begin(), weights.end());

	std::vector<std::string> options{};
	options.reserve(weights.size());
	for (const std::uint16_t weight : weights)
	{
		options.push_back(std::to_string(weight));
	}
	return options;
}

inline std::vector<std::string> availableStylesForFamilyWeight(
	const std::vector<AddedFontVariant>& fontLibrary,
	std::string_view family,
	std::uint16_t weight)
{
	std::vector<FlowPlot::FontStyle> styles{};
	styles.push_back(FlowPlot::FontStyle::Normal);

	for (const AddedFontVariant& variant : fontLibrary)
	{
		if (variant.family == family
			&& variant.weight == weight
			&& std::find(styles.begin(), styles.end(), variant.style) == styles.end())
		{
			styles.push_back(variant.style);
		}
	}

	std::sort(styles.begin(), styles.end(), [](FlowPlot::FontStyle lhs, FlowPlot::FontStyle rhs) {
		return static_cast<std::uint8_t>(lhs) < static_cast<std::uint8_t>(rhs);
	});

	std::vector<std::string> options{};
	options.reserve(styles.size());
	for (const FlowPlot::FontStyle style : styles)
	{
		options.emplace_back(FlowPlot::fontStyleName(style));
	}
	return options;
}

struct state {
	state()
	{
		activeTemplate.datasets.reserve(kInitialDatasetCapacity);
		datasets.reserve(kInitialDatasetCapacity);
		documentHistory.snapshots[0] = DocumentSnapshot{
			.activeTemplate = activeTemplate,
			.datasets = datasets,
		};
		documentHistory.count = 1;
		lastExportedTemplate = activeTemplate;
	}

	GlobalState globalAppState = GlobalState::working;
	std::uint64_t templateRevision = 1;
	std::uint64_t datasetRevision = 1;
	std::uint64_t viewportRevision = 1;
	FlowPlot::Spec::MasterTemplateSpec activeTemplate{};
	std::optional<TemplateNodeKey> selectedNode{};
	std::vector<RunningDataset> datasets;
	std::vector<AddedFontVariant> fontLibrary{};
	std::shared_ptr<FlowPlot::ITextEngine> textEngine{};
	std::filesystem::path lastFontDialogDirectory{};
	std::filesystem::path lastTemplateDialogDirectory{};
	DocumentHistory documentHistory{};
	FlowPlot::Spec::MasterTemplateSpec lastExportedTemplate{};
	bool activeTemplateExportComparisonChecked = true;
	bool activeTemplateDiffersFromLastExport = false;
	std::vector<Diagnostic> diagnostics{};
	DeferredDocumentEdit deferredEdit{};
};

inline std::size_t documentHistoryPhysicalIndex(const DocumentHistory& history, std::size_t cursor)
{
	return (history.start + cursor) % kDocumentHistoryCapacity;
}

inline DocumentSnapshot makeDocumentSnapshot(const state& guiState)
{
	return DocumentSnapshot{
		.activeTemplate = guiState.activeTemplate,
		.datasets = guiState.datasets,
	};
}

inline DocumentEditTarget makeTemplateEditTarget(std::string key, DocumentEditComparator changed = nullptr)
{
	return DocumentEditTarget{
		.kind = DocumentEditTargetKind::Template,
		.key = std::move(key),
		.changed = std::move(changed),
	};
}

inline DocumentEditTarget makeDatasetEditTarget(std::string key, DocumentEditComparator changed = nullptr)
{
	return DocumentEditTarget{
		.kind = DocumentEditTargetKind::Dataset,
		.key = std::move(key),
		.changed = std::move(changed),
	};
}

inline bool sameDocumentEditTarget(const DocumentEditTarget& lhs, const DocumentEditTarget& rhs)
{
	return lhs.kind == rhs.kind && lhs.key == rhs.key;
}

inline bool sameTemplateNodeKey(const TemplateNodeKey& lhs, const TemplateNodeKey& rhs)
{
	return lhs.kind == rhs.kind && lhs.outer == rhs.outer && lhs.inner == rhs.inner;
}

inline void commitDocumentSnapshot(state& guiState)
{
	DocumentHistory& history = guiState.documentHistory;
	if (history.count == 0)
	{
		history.snapshots[0] = makeDocumentSnapshot(guiState);
		history.start = 0;
		history.count = 1;
		history.cursor = 0;
		return;
	}

	if (history.cursor + 1 < history.count)
	{
		history.count = history.cursor + 1;
	}

	if (history.count < kDocumentHistoryCapacity)
	{
		history.cursor = history.count;
		++history.count;
	}
	else
	{
		history.start = (history.start + 1) % kDocumentHistoryCapacity;
		history.cursor = history.count - 1;
	}

	history.snapshots[documentHistoryPhysicalIndex(history, history.cursor)] = makeDocumentSnapshot(guiState);
}

inline bool documentEditTargetChanged(
	const DocumentSnapshot& before,
	const state& current,
	const DocumentEditTarget& target)
{
	if (target.changed != nullptr)
	{
		return target.changed(before, current);
	}

	// Unsupported keys are treated as changed only at commit boundaries. This keeps
	// the manager useful before each editor supplies exact field comparators.
	return true;
}

inline void remapSelectedNodeForSnapshot(state& guiState, const DocumentSnapshot& snapshot)
{
	if (!guiState.selectedNode.has_value())
	{
		return;
	}

	TemplateNodeKey& key = *guiState.selectedNode;
	auto panelIndexInCurrent = [&]() -> std::optional<std::size_t> {
		for (std::size_t i = 0; i < guiState.activeTemplate.panels.size(); ++i)
		{
			if (guiState.activeTemplate.panels[i].id == key.outer)
			{
				return i;
			}
		}
		return std::nullopt;
	};
	auto legendIndexInCurrent = [&]() -> std::optional<std::size_t> {
		for (std::size_t i = 0; i < guiState.activeTemplate.figure.legends.size(); ++i)
		{
			if (guiState.activeTemplate.figure.legends[i].id == key.outer)
			{
				return i;
			}
		}
		return std::nullopt;
	};

	switch (key.kind)
	{
	case TemplateNodeKind::Panel:
	case TemplateNodeKind::PanelTitle:
	case TemplateNodeKind::XAxis:
	case TemplateNodeKind::YAxis:
	case TemplateNodeKind::XSecondaryAxis:
	case TemplateNodeKind::YSecondaryAxis:
	case TemplateNodeKind::AxisTitle:
	case TemplateNodeKind::LayersGroup:
		if (const std::optional<std::size_t> index = panelIndexInCurrent();
			index.has_value() && *index < snapshot.activeTemplate.panels.size())
		{
			key.outer = snapshot.activeTemplate.panels[*index].id;
		}
		break;
	case TemplateNodeKind::Layer:
		if (const std::optional<std::size_t> panelIndex = panelIndexInCurrent();
			panelIndex.has_value() && *panelIndex < snapshot.activeTemplate.panels.size())
		{
			const FlowPlot::Spec::PanelSpec& currentPanel = guiState.activeTemplate.panels[*panelIndex];
			for (std::size_t layerIndex = 0; layerIndex < currentPanel.layers.size(); ++layerIndex)
			{
				if (currentPanel.layers[layerIndex].id == key.inner
					&& layerIndex < snapshot.activeTemplate.panels[*panelIndex].layers.size())
				{
					key.outer = snapshot.activeTemplate.panels[*panelIndex].id;
					key.inner = snapshot.activeTemplate.panels[*panelIndex].layers[layerIndex].id;
					break;
				}
			}
		}
		break;
	case TemplateNodeKind::Legend:
		if (const std::optional<std::size_t> index = legendIndexInCurrent();
			index.has_value() && *index < snapshot.activeTemplate.figure.legends.size())
		{
			key.outer = snapshot.activeTemplate.figure.legends[*index].id;
		}
		break;
	case TemplateNodeKind::LegendElement:
		if (const std::optional<std::size_t> legendIndex = legendIndexInCurrent();
			legendIndex.has_value() && *legendIndex < snapshot.activeTemplate.figure.legends.size())
		{
			const FlowPlot::Spec::LegendSpec& currentLegend = guiState.activeTemplate.figure.legends[*legendIndex];
			for (std::size_t elementIndex = 0; elementIndex < currentLegend.legendElements.size(); ++elementIndex)
			{
				if (currentLegend.legendElements[elementIndex].id == key.inner
					&& elementIndex < snapshot.activeTemplate.figure.legends[*legendIndex].legendElements.size())
				{
					key.outer = snapshot.activeTemplate.figure.legends[*legendIndex].id;
					key.inner = snapshot.activeTemplate.figure.legends[*legendIndex].legendElements[elementIndex].id;
					break;
				}
			}
		}
		break;
	default:
		break;
	}
}

inline void applyDocumentSnapshot(state& guiState, const DocumentSnapshot& snapshot)
{
	remapSelectedNodeForSnapshot(guiState, snapshot);
	guiState.activeTemplate = snapshot.activeTemplate;
	guiState.datasets = snapshot.datasets;
	guiState.activeTemplateExportComparisonChecked = false;
}

inline void resetDocumentHistory(state& guiState)
{
	guiState.documentHistory.start = 0;
	guiState.documentHistory.count = 1;
	guiState.documentHistory.cursor = 0;
	guiState.documentHistory.snapshots[0] = makeDocumentSnapshot(guiState);
}

inline void clearDeferredDocumentEdit(state& guiState)
{
	guiState.deferredEdit = DeferredDocumentEdit{};
}

inline void acceptCurrentTemplateAsExported(state& guiState)
{
	guiState.lastExportedTemplate = guiState.activeTemplate;
	guiState.activeTemplateExportComparisonChecked = true;
	guiState.activeTemplateDiffersFromLastExport = false;
}

inline void resetDocumentSession(state& guiState)
{
	resetDocumentHistory(guiState);
	acceptCurrentTemplateAsExported(guiState);
	guiState.diagnostics.clear();
	clearDeferredDocumentEdit(guiState);
}

inline void endDeferredDocumentEdit(state& guiState);

inline void beginDeferredDocumentEdit(
	state& guiState,
	DocumentEditTarget target,
	float timeoutSeconds = -1.0f)
{
	DeferredDocumentEdit& edit = guiState.deferredEdit;
	if (edit.active)
	{
		if (sameDocumentEditTarget(edit.target, target))
		{
			edit.timeoutSeconds = timeoutSeconds;
			edit.elapsedSeconds = 0.0f;
			return;
		}
		endDeferredDocumentEdit(guiState);
	}

	edit.active = true;
	edit.target = std::move(target);
	edit.before = makeDocumentSnapshot(guiState);
	edit.timeoutSeconds = timeoutSeconds;
	edit.elapsedSeconds = 0.0f;
}

inline bool deferredDocumentEditChanged(const state& guiState)
{
	const DeferredDocumentEdit& edit = guiState.deferredEdit;
	return edit.active && documentEditTargetChanged(edit.before, guiState, edit.target);
}

inline void endDeferredDocumentEdit(state& guiState)
{
	if (!guiState.deferredEdit.active)
	{
		return;
	}

	if (deferredDocumentEditChanged(guiState))
	{
		commitDocumentSnapshot(guiState);
	}
	clearDeferredDocumentEdit(guiState);
}

inline void tickDeferredDocumentEdit(state& guiState, float dtSeconds)
{
	DeferredDocumentEdit& edit = guiState.deferredEdit;
	if (!edit.active || edit.timeoutSeconds < 0.0f)
	{
		return;
	}

	edit.elapsedSeconds += std::max(0.0f, dtSeconds);
	if (edit.elapsedSeconds < edit.timeoutSeconds)
	{
		return;
	}

	if (deferredDocumentEditChanged(guiState))
	{
		commitDocumentSnapshot(guiState);
		edit.before = makeDocumentSnapshot(guiState);
	}
	edit.elapsedSeconds = 0.0f;
}

inline void prepareImmediateDocumentChange(state& guiState)
{
	endDeferredDocumentEdit(guiState);
}

inline void commitImmediateDocumentChange(state& guiState)
{
	commitDocumentSnapshot(guiState);
}

inline void commitImmediateDocumentChangeIfNoDeferredEdit(state& guiState)
{
	if (!guiState.deferredEdit.active)
	{
		commitImmediateDocumentChange(guiState);
	}
}

template <typename Fn>
inline void applyImmediateDocumentChange(state& guiState, Fn&& fn)
{
	prepareImmediateDocumentChange(guiState);
	std::forward<Fn>(fn)();
	commitImmediateDocumentChange(guiState);
}

inline bool undoDocument(state& guiState)
{
	endDeferredDocumentEdit(guiState);
	DocumentHistory& history = guiState.documentHistory;
	if (history.count == 0 || history.cursor == 0)
	{
		return false;
	}

	--history.cursor;
	applyDocumentSnapshot(guiState, history.snapshots[documentHistoryPhysicalIndex(history, history.cursor)]);
	++guiState.templateRevision;
	++guiState.datasetRevision;
	++guiState.viewportRevision;
	return true;
}

inline bool redoDocument(state& guiState)
{
	endDeferredDocumentEdit(guiState);
	DocumentHistory& history = guiState.documentHistory;
	if (history.count == 0 || history.cursor + 1 >= history.count)
	{
		return false;
	}

	++history.cursor;
	applyDocumentSnapshot(guiState, history.snapshots[documentHistoryPhysicalIndex(history, history.cursor)]);
	++guiState.templateRevision;
	++guiState.datasetRevision;
	++guiState.viewportRevision;
	return true;
}

inline void clearDiagnosticsBySource(state& guiState, std::string_view source)
{
	guiState.diagnostics.erase(
		std::remove_if(
			guiState.diagnostics.begin(),
			guiState.diagnostics.end(),
			[source](const Diagnostic& diagnostic) {
				return diagnostic.source == source;
			}),
		guiState.diagnostics.end());
}

inline void recordDiagnostic(state& guiState, Diagnostic diagnostic)
{
	clearDiagnosticsBySource(guiState, diagnostic.source);
	guiState.diagnostics.push_back(std::move(diagnostic));
}

inline const Diagnostic* latestDiagnosticWithSeverity(const state& guiState, DiagnosticSeverity severity)
{
	for (auto it = guiState.diagnostics.rbegin(); it != guiState.diagnostics.rend(); ++it)
	{
		if (it->severity == severity)
		{
			return &*it;
		}
	}
	return nullptr;
}

inline bool hasDiagnosticError(const state& guiState)
{
	return latestDiagnosticWithSeverity(guiState, DiagnosticSeverity::Error) != nullptr;
}

inline void markTemplateChanged(state& guiState)
{
	++guiState.templateRevision;
	guiState.activeTemplateExportComparisonChecked = false;
}

inline void markDatasetsChanged(state& guiState)
{
	++guiState.datasetRevision;
}

inline void markViewportChanged(state& guiState)
{
	++guiState.viewportRevision;
}



} // namespace FlowPlotGui
