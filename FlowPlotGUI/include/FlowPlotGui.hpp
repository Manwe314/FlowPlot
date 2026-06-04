#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <optional>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
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

using TemplateClipboardValue = std::variant<
	FlowPlot::Spec::FigureSpec,
	FlowPlot::Spec::TextSpec,
	FlowPlot::Spec::PanelSpec,
	FlowPlot::Spec::AxisSpec,
	FlowPlot::Spec::LayerSpec,
	FlowPlot::Spec::LegendSpec,
	FlowPlot::Spec::LegendElementSpec>;

struct TemplateClipboard {
	bool hasValue = false;
	TemplateNodeKind kind = TemplateNodeKind::Figure;
	TemplateClipboardValue value = FlowPlot::Spec::FigureSpec{};
	TemplateNodeKey source{};
};

struct state;

enum class DataInputCellNavDirection : std::uint8_t {
	Left,
	Right,
	Up,
	Down,
	Next,
	Previous,
};

struct DataInputCellFocus {
	std::string fieldId{};
	std::size_t datasetIndex = 0;
	std::size_t row = 0;
	std::size_t column = 0;
	std::string scrollContainerId{};
	float rowHeight = 34.0f;
	float rowGap = 0.0f;
};

struct DataInputFocusGrid {
	std::vector<DataInputCellFocus> cells{};
	std::string focusedFieldId{};

	void clear()
	{
		cells.clear();
	}

	void registerCell(DataInputCellFocus cell)
	{
		cells.push_back(std::move(cell));
	}

	void setFocusedField(std::string fieldId)
	{
		focusedFieldId = std::move(fieldId);
	}

	void clearFocusedField(std::string_view fieldId)
	{
		if (focusedFieldId == fieldId)
		{
			focusedFieldId.clear();
		}
	}

	const DataInputCellFocus* focusedCell() const
	{
		for (const DataInputCellFocus& cell : cells)
		{
			if (cell.fieldId == focusedFieldId)
			{
				return &cell;
			}
		}
		return nullptr;
	}

	const DataInputCellFocus* neighbor(
		const DataInputCellFocus& current,
		DataInputCellNavDirection direction) const
	{
		const DataInputCellFocus* best = nullptr;
		for (const DataInputCellFocus& cell : cells)
		{
			if (cell.datasetIndex != current.datasetIndex || cell.fieldId == current.fieldId)
			{
				continue;
			}

			switch (direction)
			{
			case DataInputCellNavDirection::Left:
				if (cell.row == current.row && cell.column < current.column &&
					(best == nullptr || cell.column > best->column))
				{
					best = &cell;
				}
				break;
			case DataInputCellNavDirection::Right:
				if (cell.row == current.row && cell.column > current.column &&
					(best == nullptr || cell.column < best->column))
				{
					best = &cell;
				}
				break;
			case DataInputCellNavDirection::Up:
				if (cell.column == current.column && cell.row < current.row &&
					(best == nullptr || cell.row > best->row))
				{
					best = &cell;
				}
				break;
			case DataInputCellNavDirection::Down:
				if (cell.column == current.column && cell.row > current.row &&
					(best == nullptr || cell.row < best->row))
				{
					best = &cell;
				}
				break;
			case DataInputCellNavDirection::Next:
				if ((cell.row > current.row || (cell.row == current.row && cell.column > current.column)) &&
					(best == nullptr || cell.row < best->row || (cell.row == best->row && cell.column < best->column)))
				{
					best = &cell;
				}
				break;
			case DataInputCellNavDirection::Previous:
				if ((cell.row < current.row || (cell.row == current.row && cell.column < current.column)) &&
					(best == nullptr || cell.row > best->row || (cell.row == best->row && cell.column > best->column)))
				{
					best = &cell;
				}
				break;
			}
		}
		return best;
	}
};

enum class PropertyInputNavDirection : std::uint8_t {
	Next,
	Previous,
};

struct PropertyInputFocus {
	std::string fieldId{};
	std::string elementId{};
	std::string scrollContainerId{};
	std::size_t order = 0;
};

struct PropertyInputFocusGrid {
	std::vector<PropertyInputFocus> fields{};
	std::string focusedFieldId{};
	std::string scrollContainerId{};
	std::size_t nextOrder = 0;

	void beginFrame(std::string scrollContainer)
	{
		fields.clear();
		nextOrder = 0;
		scrollContainerId = std::move(scrollContainer);
	}

	std::size_t allocateOrder()
	{
		return nextOrder++;
	}

	void registerField(PropertyInputFocus field)
	{
		fields.push_back(std::move(field));
	}

	void setFocusedField(std::string fieldId)
	{
		focusedFieldId = std::move(fieldId);
	}

	void clearFocusedField(std::string_view fieldId)
	{
		if (focusedFieldId == fieldId)
		{
			focusedFieldId.clear();
		}
	}

	const PropertyInputFocus* focusedField() const
	{
		for (const PropertyInputFocus& field : fields)
		{
			if (field.fieldId == focusedFieldId)
			{
				return &field;
			}
		}
		return nullptr;
	}

	const PropertyInputFocus* neighbor(const PropertyInputFocus& current, PropertyInputNavDirection direction) const
	{
		const PropertyInputFocus* best = nullptr;
		for (const PropertyInputFocus& field : fields)
		{
			if (field.fieldId == current.fieldId)
			{
				continue;
			}

			if (direction == PropertyInputNavDirection::Next)
			{
				if (field.order > current.order && (best == nullptr || field.order < best->order))
				{
					best = &field;
				}
			}
			else if (field.order < current.order && (best == nullptr || field.order > best->order))
			{
				best = &field;
			}
		}
		return best;
	}
};

inline void chainVoidCallback(std::function<void()>& callback, std::function<void()> next)
{
	if (next == nullptr)
	{
		return;
	}
	if (callback == nullptr)
	{
		callback = std::move(next);
		return;
	}

	auto previous = std::move(callback);
	callback = [previous = std::move(previous), next = std::move(next)]() {
		previous();
		next();
	};
}

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

	std::vector<std::uint8_t> data;
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
	TemplateClipboard templateClipboard{};
	DataInputFocusGrid dataInputFocusGrid{};
	PropertyInputFocusGrid propertyInputFocusGrid{};
	std::vector<RunningDataset> datasets;
	std::vector<AddedFontVariant> fontLibrary{};
	std::shared_ptr<FlowPlot::ITextEngine> textEngine{};
	std::filesystem::path lastFontDialogDirectory{};
	std::filesystem::path lastTemplateDialogDirectory{};
	DocumentHistory documentHistory{};
	FlowPlot::Spec::MasterTemplateSpec lastExportedTemplate{};
	bool activeTemplateExportComparisonChecked = true;
	bool activeTemplateDiffersFromLastExport = false;
	bool shouldClose = false;
	std::vector<Diagnostic> diagnostics{};
	DeferredDocumentEdit deferredEdit{};
};

inline void wirePropertyInputFocusCallbacks(
	state& guiState,
	const std::string& fieldId,
	std::function<void()>& onEditBegin,
	std::function<void()>& onEditEnd)
{
	chainVoidCallback(onEditBegin, [&guiState, fieldId]() {
		guiState.propertyInputFocusGrid.setFocusedField(fieldId);
	});
	chainVoidCallback(onEditEnd, [&guiState, fieldId]() {
		guiState.propertyInputFocusGrid.clearFocusedField(fieldId);
	});
}

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

inline bool runningDatasetsEqual(const std::vector<RunningDataset>& lhs, const std::vector<RunningDataset>& rhs)
{
	if (lhs.size() != rhs.size())
	{
		return false;
	}

	for (std::size_t datasetIndex = 0; datasetIndex < lhs.size(); ++datasetIndex)
	{
		const RunningDataset& leftDataset = lhs[datasetIndex];
		const RunningDataset& rightDataset = rhs[datasetIndex];
		if (leftDataset.name != rightDataset.name ||
			leftDataset.numericColumns.size() != rightDataset.numericColumns.size() ||
			leftDataset.stringColumns.size() != rightDataset.stringColumns.size() ||
			leftDataset.boolColumns.size() != rightDataset.boolColumns.size())
		{
			return false;
		}

		for (std::size_t columnIndex = 0; columnIndex < leftDataset.numericColumns.size(); ++columnIndex)
		{
			const numericColumn& leftColumn = leftDataset.numericColumns[columnIndex];
			const numericColumn& rightColumn = rightDataset.numericColumns[columnIndex];
			if (leftColumn.name != rightColumn.name || leftColumn.data != rightColumn.data)
			{
				return false;
			}
		}
		for (std::size_t columnIndex = 0; columnIndex < leftDataset.stringColumns.size(); ++columnIndex)
		{
			const stringColumn& leftColumn = leftDataset.stringColumns[columnIndex];
			const stringColumn& rightColumn = rightDataset.stringColumns[columnIndex];
			if (leftColumn.name != rightColumn.name || leftColumn.data != rightColumn.data)
			{
				return false;
			}
		}
		for (std::size_t columnIndex = 0; columnIndex < leftDataset.boolColumns.size(); ++columnIndex)
		{
			const boolColumn& leftColumn = leftDataset.boolColumns[columnIndex];
			const boolColumn& rightColumn = rightDataset.boolColumns[columnIndex];
			if (leftColumn.name != rightColumn.name || leftColumn.data != rightColumn.data)
			{
				return false;
			}
		}
	}

	return true;
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

inline DocumentEditComparator makeDatasetsEditComparator()
{
	return [](const DocumentSnapshot& before, const state& current) {
		return !runningDatasetsEqual(before.datasets, current.datasets);
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

inline std::string trimDiagnosticText(std::string_view text)
{
	while (!text.empty() && (text.front() == ' ' || text.front() == '\t' || text.front() == '\n' || text.front() == '\r'))
	{
		text.remove_prefix(1);
	}
	while (!text.empty() && (text.back() == ' ' || text.back() == '\t' || text.back() == '\n' || text.back() == '\r'))
	{
		text.remove_suffix(1);
	}
	return std::string(text);
}

inline std::string diagnosticThrowerLabel(std::string_view thrower)
{
	if (thrower == "buildBoundIR")
	{
		return "BoundIR";
	}
	if (thrower == "resolvePlotIR")
	{
		return "Plot";
	}
	if (thrower == "compileTemplateToSpec")
	{
		return "Template";
	}
	if (thrower.find("Renderer") != std::string_view::npos)
	{
		return "Renderer";
	}
	if (thrower.empty())
	{
		return "FlowPlot";
	}
	return std::string(thrower);
}

inline bool parseDiagnosticIndex(std::string_view text, std::size_t& index)
{
	index = 0;
	const char* begin = text.data();
	const char* end = text.data() + text.size();
	const std::from_chars_result result = std::from_chars(begin, end, index);
	return result.ec == std::errc{} && result.ptr == end;
}

inline std::string diagnosticPanelId(const FlowPlot::Spec::MasterTemplateSpec& spec, std::size_t index)
{
	if (index < spec.panels.size() && !spec.panels[index].id.empty())
	{
		return spec.panels[index].id;
	}
	return "panel_" + std::to_string(index + 1);
}

inline std::string diagnosticLayerId(const FlowPlot::Spec::MasterTemplateSpec& spec, std::size_t panelIndex, std::size_t layerIndex)
{
	if (panelIndex < spec.panels.size() && layerIndex < spec.panels[panelIndex].layers.size()
		&& !spec.panels[panelIndex].layers[layerIndex].id.empty())
	{
		return spec.panels[panelIndex].layers[layerIndex].id;
	}
	return "layer_" + std::to_string(layerIndex + 1);
}

inline std::string diagnosticLegendId(const FlowPlot::Spec::MasterTemplateSpec& spec, std::size_t index)
{
	if (index < spec.figure.legends.size() && !spec.figure.legends[index].id.empty())
	{
		return spec.figure.legends[index].id;
	}
	return "legend_" + std::to_string(index + 1);
}

inline std::string diagnosticLegendElementId(
	const FlowPlot::Spec::MasterTemplateSpec& spec,
	std::size_t legendIndex,
	std::size_t elementIndex)
{
	if (legendIndex < spec.figure.legends.size()
		&& elementIndex < spec.figure.legends[legendIndex].legendElements.size()
		&& !spec.figure.legends[legendIndex].legendElements[elementIndex].id.empty())
	{
		return spec.figure.legends[legendIndex].legendElements[elementIndex].id;
	}
	return "legend_element_" + std::to_string(elementIndex + 1);
}

inline std::string diagnosticDatasetName(const FlowPlot::Spec::MasterTemplateSpec& spec, std::size_t index)
{
	if (index < spec.datasets.size() && !spec.datasets[index].name.empty())
	{
		return spec.datasets[index].name;
	}
	return "dataset_" + std::to_string(index + 1);
}

inline std::string diagnosticHumanSegment(std::string_view segment)
{
	if (segment == "xAxis")
	{
		return "X axis parameters";
	}
	if (segment == "yAxis")
	{
		return "Y axis parameters";
	}
	if (segment == "xSecondary")
	{
		return "secondary X axis parameters";
	}
	if (segment == "ySecondary")
	{
		return "secondary Y axis parameters";
	}
	if (segment == "scatterMapping")
	{
		return "scatter mapping parameters";
	}
	if (segment == "scatterStyle")
	{
		return "scatter style parameters";
	}
	if (segment == "scatterStats")
	{
		return "scatter stats parameters";
	}
	if (segment == "scatterConfig")
	{
		return "scatter config parameters";
	}
	if (segment == "histogramMapping")
	{
		return "histogram mapping parameters";
	}
	if (segment == "histogramStyle")
	{
		return "histogram style parameters";
	}
	if (segment == "histogramStats")
	{
		return "histogram stats parameters";
	}
	if (segment == "histogramConfig")
	{
		return "histogram config parameters";
	}
	if (segment == "axisData")
	{
		return "axis binding parameters";
	}
	if (segment == "title")
	{
		return "title parameters";
	}
	if (segment == "box")
	{
		return "box parameters";
	}
	if (segment == "padding")
	{
		return "padding parameters";
	}
	if (segment == "layout")
	{
		return "layout parameters";
	}
	return std::string(segment);
}

inline std::string humanizeDiagnosticPath(const FlowPlot::Spec::MasterTemplateSpec& spec, std::string_view path)
{
	std::string phrase{};
	std::size_t panelIndex = static_cast<std::size_t>(-1);
	std::size_t legendIndex = static_cast<std::size_t>(-1);
	bool hasSubject = false;
	bool hasParameter = false;

	for (std::size_t cursor = 0; cursor < path.size();)
	{
		const std::size_t dot = path.find('.', cursor);
		std::string_view segment = path.substr(cursor, dot == std::string_view::npos ? std::string_view::npos : dot - cursor);
		cursor = dot == std::string_view::npos ? path.size() : dot + 1;

		const std::size_t bracket = segment.find('[');
		std::string_view name = bracket == std::string_view::npos ? segment : segment.substr(0, bracket);
		std::size_t index = 0;
		bool hasIndex = false;
		if (bracket != std::string_view::npos && segment.ends_with(']'))
		{
			hasIndex = parseDiagnosticIndex(segment.substr(bracket + 1, segment.size() - bracket - 2), index);
		}

		if (name == "panels" && hasIndex)
		{
			panelIndex = index;
			phrase += hasSubject ? " at " : "at ";
			phrase += diagnosticPanelId(spec, index);
			hasSubject = true;
			continue;
		}
		if (name == "layers" && hasIndex)
		{
			phrase += hasSubject ? " on " : "at ";
			phrase += diagnosticLayerId(spec, panelIndex, index);
			hasSubject = true;
			continue;
		}
		if (name == "legends" && hasIndex)
		{
			legendIndex = index;
			phrase += hasSubject ? " on " : "at ";
			phrase += diagnosticLegendId(spec, index);
			hasSubject = true;
			continue;
		}
		if (name == "legendElements" && hasIndex)
		{
			phrase += hasSubject ? " on " : "at ";
			phrase += diagnosticLegendElementId(spec, legendIndex, index);
			hasSubject = true;
			continue;
		}
		if (name == "datasets" && hasIndex)
		{
			phrase += hasSubject ? " on " : "at ";
			phrase += diagnosticDatasetName(spec, index);
			hasSubject = true;
			continue;
		}
		if (name == "figure")
		{
			phrase += hasSubject ? " in figure" : "at figure";
			hasSubject = true;
			continue;
		}

		if (!segment.empty() && !hasParameter)
		{
			phrase += hasSubject ? " in " : "at ";
			phrase += diagnosticHumanSegment(segment);
			hasSubject = true;
			hasParameter = true;
		}
	}

	if (phrase.empty())
	{
		phrase = "at " + std::string(path);
	}
	return phrase;
}

inline std::string findDiagnosticPath(std::string_view text, std::size_t& removeStart, std::size_t& removeLength)
{
	removeStart = std::string_view::npos;
	removeLength = 0;

	if (const std::size_t atQuoted = text.rfind(" at '"); atQuoted != std::string_view::npos)
	{
		const std::size_t pathStart = atQuoted + 5;
		if (const std::size_t pathEnd = text.find('\'', pathStart); pathEnd != std::string_view::npos)
		{
			removeStart = atQuoted;
			removeLength = pathEnd + 1 - atQuoted;
			return std::string(text.substr(pathStart, pathEnd - pathStart));
		}
	}

	if (const std::size_t atPlain = text.rfind(" at "); atPlain != std::string_view::npos)
	{
		std::string path = trimDiagnosticText(text.substr(atPlain + 4));
		while (!path.empty() && (path.back() == '.' || path.back() == ')' || path.back() == '\'' || path.back() == '"'))
		{
			path.pop_back();
		}
		if (!path.empty())
		{
			removeStart = atPlain;
			removeLength = text.size() - atPlain;
			return path;
		}
	}

	for (std::size_t quote = text.find('\''); quote != std::string_view::npos; quote = text.find('\'', quote + 1))
	{
		const std::size_t end = text.find('\'', quote + 1);
		if (end == std::string_view::npos)
		{
			break;
		}
		std::string_view candidate = text.substr(quote + 1, end - quote - 1);
		if (candidate.find('.') != std::string_view::npos || candidate.find('[') != std::string_view::npos)
		{
			removeStart = quote;
			removeLength = end + 1 - quote;
			return std::string(candidate);
		}
		quote = end;
	}

	return {};
}

inline std::string formatFlowPlotExceptionMessage(
	const FlowPlot::Spec::MasterTemplateSpec& spec,
	std::string_view rawMessage)
{
	std::string_view thrower{};
	std::string_view detail = rawMessage;
	if (const std::size_t colon = rawMessage.find(':'); colon != std::string_view::npos)
	{
		thrower = rawMessage.substr(0, colon);
		detail = rawMessage.substr(colon + 1);
	}

	std::string reason = trimDiagnosticText(detail);
	std::size_t pathStart = std::string_view::npos;
	std::size_t pathLength = 0;
	const std::string path = findDiagnosticPath(reason, pathStart, pathLength);
	if (pathStart != std::string_view::npos)
	{
		reason.erase(pathStart, pathLength);
		reason = trimDiagnosticText(reason);
	}
	if (!reason.empty() && reason.back() == ':')
	{
		reason.pop_back();
		reason = trimDiagnosticText(reason);
	}
	if (reason.empty())
	{
		reason = trimDiagnosticText(detail);
	}

	std::string formatted = diagnosticThrowerLabel(trimDiagnosticText(thrower)) + ":\n";
	formatted += "Where: ";
	formatted += path.empty() ? "at plot" : humanizeDiagnosticPath(spec, path);
	formatted += ":\n";
	formatted += reason;
	return formatted;
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
