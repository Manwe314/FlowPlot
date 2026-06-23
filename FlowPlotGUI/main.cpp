#define FLOW_PLOT_RENDERER
#define FLOW_PLOT_IMPLEMENTATION

#include <FlowUi/Flow.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "elements.hpp"
#include "FlowPlotGui.hpp"
#include "iconRegistry.hpp"
#include "PlotViewportScene.hpp"
#include "runtimePaths.hpp"
#include "TemplatePresets.hpp"
#include "templateClipboard.hpp"
#include "templateExport.hpp"
#include "templateHelper.hpp"

namespace {

#ifndef FLOWPLOTGUI_DEV_MODE
#define FLOWPLOTGUI_DEV_MODE 0
#endif

std::filesystem::path sourceAssetPath(std::initializer_list<std::filesystem::path> parts)
{
#ifdef FLOWPLOTGUI_SOURCE_ASSET_DIR
	std::filesystem::path path = std::filesystem::path(FLOWPLOTGUI_SOURCE_ASSET_DIR);
#else
	std::filesystem::path path = std::filesystem::path(__FILE__).parent_path() / "assets";
#endif
	for (const std::filesystem::path& part : parts)
		path /= part;
	return path;
}

std::filesystem::path assetPath(std::initializer_list<std::filesystem::path> parts)
{
	std::filesystem::path relative = "assets";
	for (const std::filesystem::path& part : parts)
		relative /= part;
	return FlowPlotGui::resourcePathOrSource(relative, sourceAssetPath(parts));
}

constexpr int kKeyTab = 258;
constexpr int kKeyRight = 262;
constexpr int kKeyLeft = 263;
constexpr int kKeyDown = 264;
constexpr int kKeyUp = 265;
constexpr int kKeyC = 67;
constexpr int kKeyD = 68;
constexpr int kKeyQ = 81;
constexpr int kKeyV = 86;
constexpr int kKeyY = 89;
constexpr int kKeyZ = 90;
constexpr int kDocumentShortcutPriority = 100;
constexpr int kInputShortcutPriority = 1000;

Clay_Vector2 scrollOffsetForElementId(FlowUi::UiManager& uiManager, std::string_view elementId)
{
	const Clay_ScrollContainerData data =
		Clay_GetScrollContainerData(uiManager.toClayEID(elementId));
	if (!data.found || data.scrollPosition == nullptr)
	{
		return Clay_Vector2{0.0f, 0.0f};
	}
	return *data.scrollPosition;
}

void ensureDataInputCellVisible(FlowUi::UiManager& ui, const FlowPlotGui::DataInputCellFocus& cell)
{
	const Clay_ScrollContainerData scrollData =
		Clay_GetScrollContainerData(ui.toClayEID(cell.scrollContainerId));
	if (!scrollData.found || scrollData.scrollPosition == nullptr ||
		scrollData.scrollContainerDimensions.height <= 0.0f)
	{
		return;
	}

	const float rowStride = cell.rowHeight + cell.rowGap;
	const float rowTop = static_cast<float>(cell.row) * rowStride;
	const float rowBottom = rowTop + cell.rowHeight;
	const float margin = 6.0f;
	const float viewportHeight = scrollData.scrollContainerDimensions.height;
	float scrollY = -scrollData.scrollPosition->y;
	const float visibleTop = scrollY;
	const float visibleBottom = scrollY + viewportHeight;

	if (rowTop < visibleTop + margin)
	{
		scrollY = std::max(0.0f, rowTop - margin);
	}
	else if (rowBottom > visibleBottom - margin)
	{
		scrollY = std::max(0.0f, rowBottom - viewportHeight + margin);
	}
	else
	{
		return;
	}

	const float maxScrollY = std::max(0.0f, scrollData.contentDimensions.height - viewportHeight);
	scrollData.scrollPosition->y = -std::clamp(scrollY, 0.0f, maxScrollY);
}

void ensurePropertyInputVisible(FlowUi::UiManager& ui, const FlowPlotGui::PropertyInputFocus& field)
{
	const Clay_ScrollContainerData scrollData =
		Clay_GetScrollContainerData(ui.toClayEID(field.scrollContainerId));
	if (!scrollData.found || scrollData.scrollPosition == nullptr ||
		scrollData.scrollContainerDimensions.height <= 0.0f)
	{
		return;
	}

	const Clay_ElementData scrollElementData = Clay_GetElementData(ui.toClayEID(field.scrollContainerId));
	const Clay_ElementData fieldElementData = Clay_GetElementData(ui.toClayEID(field.elementId));
	if (!scrollElementData.found || !fieldElementData.found)
	{
		return;
	}

	const float margin = 8.0f;
	const float visibleTop = scrollElementData.boundingBox.y;
	const float visibleBottom = visibleTop + scrollData.scrollContainerDimensions.height;
	const float fieldTop = fieldElementData.boundingBox.y;
	const float fieldBottom = fieldTop + fieldElementData.boundingBox.height;
	float nextScrollY = scrollData.scrollPosition->y;

	if (fieldTop < visibleTop + margin)
	{
		nextScrollY += (visibleTop + margin) - fieldTop;
	}
	else if (fieldBottom > visibleBottom - margin)
	{
		nextScrollY -= fieldBottom - (visibleBottom - margin);
	}
	else
	{
		return;
	}

	const float maxScrollY = std::max(
		0.0f,
		scrollData.contentDimensions.height - scrollData.scrollContainerDimensions.height);
	scrollData.scrollPosition->y = std::clamp(nextScrollY, -maxScrollY, 0.0f);
}

bool navigateDataInputCell(
	FlowUi::ShortcutContext& context,
	FlowPlotGui::state& guiState,
	FlowPlotGui::DataInputCellNavDirection direction)
{
	FlowPlotGui::DataInputFocusGrid& grid = guiState.dataInputFocusGrid;
	const FlowPlotGui::DataInputCellFocus* current = grid.focusedCell();
	if (current == nullptr)
	{
		return false;
	}

	const FlowPlotGui::DataInputCellFocus* next = grid.neighbor(*current, direction);
	if (next == nullptr)
	{
		return true;
	}

	grid.setFocusedField(next->fieldId);
	context.ui.inputFields().requestCaret(next->fieldId, FlowUi::CaretRequestKind::SetPrimary);
	ensureDataInputCellVisible(context.ui, *next);
	return true;
}

bool navigatePropertyInput(
	FlowUi::ShortcutContext& context,
	FlowPlotGui::state& guiState,
	FlowPlotGui::PropertyInputNavDirection direction)
{
	FlowPlotGui::PropertyInputFocusGrid& grid = guiState.propertyInputFocusGrid;
	const FlowPlotGui::PropertyInputFocus* current = grid.focusedField();
	if (current == nullptr)
	{
		return false;
	}

	const FlowPlotGui::PropertyInputFocus* next = grid.neighbor(*current, direction);
	if (next == nullptr)
	{
		return true;
	}

	grid.setFocusedField(next->fieldId);
	context.ui.inputFields().requestCaret(next->fieldId, FlowUi::CaretRequestKind::SetPrimary);
	ensurePropertyInputVisible(context.ui, *next);
	return true;
}

void populateInitialGuiState(FlowPlotGui::state& guiState)
{
	FlowPlotGui::replaceWithTemplatePreset(guiState, FlowPlotGui::makeScatterPlotPreset(), false);
	FlowPlotGui::resetDocumentSession(guiState);
	guiState.templateRevision = 1;
	guiState.datasetRevision = 1;
	guiState.viewportRevision = 1;
	guiState.textEngine = std::make_shared<FlowPlot::StbTextEngine>(
		assetPath({"Fonts", "Inter-VariableFont_opsz,wght.ttf"}));
}

std::string selectedTemplateNodeKindText(const FlowPlotGui::state& guiState)
{
	if (!guiState.selectedNode.has_value())
	{
		return "No selection";
	}

	switch (guiState.selectedNode->kind)
	{
	case FlowPlotGui::TemplateNodeKind::Figure:
		return "Figure";
	case FlowPlotGui::TemplateNodeKind::FigureTitle:
	case FlowPlotGui::TemplateNodeKind::PanelTitle:
	case FlowPlotGui::TemplateNodeKind::AxisTitle:
		return "Title";
	case FlowPlotGui::TemplateNodeKind::PanelsGroup:
		return "Panels";
	case FlowPlotGui::TemplateNodeKind::Panel:
		return "Panel";
	case FlowPlotGui::TemplateNodeKind::XAxis:
	case FlowPlotGui::TemplateNodeKind::YAxis:
	case FlowPlotGui::TemplateNodeKind::XSecondaryAxis:
	case FlowPlotGui::TemplateNodeKind::YSecondaryAxis:
		return "Axis";
	case FlowPlotGui::TemplateNodeKind::LayersGroup:
		return "Layers";
	case FlowPlotGui::TemplateNodeKind::Layer:
		return "Layer";
	case FlowPlotGui::TemplateNodeKind::LegendsGroup:
		return "Legends";
	case FlowPlotGui::TemplateNodeKind::Legend:
		return "Legend";
	case FlowPlotGui::TemplateNodeKind::LegendElement:
		return "Legend element";
	}

	return "Selection";
}

std::optional<std::string> selectedTemplateNodeEditableId(const FlowPlotGui::state& guiState)
{
	if (!guiState.selectedNode.has_value())
	{
		return std::nullopt;
	}

	const FlowPlotGui::TemplateNodeKey& key = *guiState.selectedNode;
	switch (key.kind)
	{
	case FlowPlotGui::TemplateNodeKind::Panel:
		for (const FlowPlot::Spec::PanelSpec& panel : guiState.activeTemplate.panels)
		{
			if (panel.id == key.outer)
			{
				return panel.id;
			}
		}
		break;
	case FlowPlotGui::TemplateNodeKind::Layer:
		for (const FlowPlot::Spec::PanelSpec& panel : guiState.activeTemplate.panels)
		{
			if (panel.id != key.outer)
			{
				continue;
			}
			for (const FlowPlot::Spec::LayerSpec& layer : panel.layers)
			{
				if (layer.id == key.inner)
				{
					return layer.id;
				}
			}
		}
		break;
	case FlowPlotGui::TemplateNodeKind::Legend:
		for (const FlowPlot::Spec::LegendSpec& legend : guiState.activeTemplate.figure.legends)
		{
			if (legend.id == key.outer)
			{
				return legend.id;
			}
		}
		break;
	case FlowPlotGui::TemplateNodeKind::LegendElement:
		for (const FlowPlot::Spec::LegendSpec& legend : guiState.activeTemplate.figure.legends)
		{
			if (legend.id != key.outer)
			{
				continue;
			}
			for (const FlowPlot::Spec::LegendElementSpec& element : legend.legendElements)
			{
				if (element.id == key.inner)
				{
					return element.id;
				}
			}
		}
		break;
	default:
		break;
	}

	return std::nullopt;
}

bool renameSelectedTemplateNodeId(FlowPlotGui::state& guiState, std::string_view nextId)
{
	if (!guiState.selectedNode.has_value() || nextId.empty())
	{
		return false;
	}

	FlowPlotGui::TemplateNodeKey& key = *guiState.selectedNode;
	const std::string next(nextId);
	switch (key.kind)
	{
	case FlowPlotGui::TemplateNodeKind::Panel: {
		FlowPlot::Spec::PanelSpec* target = nullptr;
		for (FlowPlot::Spec::PanelSpec& panel : guiState.activeTemplate.panels)
		{
			if (panel.id == next && panel.id != key.outer)
			{
				return false;
			}
			if (panel.id == key.outer)
			{
				target = &panel;
			}
		}
		if (target == nullptr || target->id == next)
		{
			return false;
		}
		target->id = next;
		key.outer = next;
		break;
	}
	case FlowPlotGui::TemplateNodeKind::Layer: {
		FlowPlot::Spec::PanelSpec* panel = nullptr;
		for (FlowPlot::Spec::PanelSpec& candidate : guiState.activeTemplate.panels)
		{
			if (candidate.id == key.outer)
			{
				panel = &candidate;
				break;
			}
		}
		if (panel == nullptr)
		{
			return false;
		}
		FlowPlot::Spec::LayerSpec* target = nullptr;
		for (FlowPlot::Spec::LayerSpec& layer : panel->layers)
		{
			if (layer.id == next && layer.id != key.inner)
			{
				return false;
			}
			if (layer.id == key.inner)
			{
				target = &layer;
			}
		}
		if (target == nullptr || target->id == next)
		{
			return false;
		}
		target->id = next;
		key.inner = next;
		break;
	}
	case FlowPlotGui::TemplateNodeKind::Legend: {
		FlowPlot::Spec::LegendSpec* target = nullptr;
		for (FlowPlot::Spec::LegendSpec& legend : guiState.activeTemplate.figure.legends)
		{
			if (legend.id == next && legend.id != key.outer)
			{
				return false;
			}
			if (legend.id == key.outer)
			{
				target = &legend;
			}
		}
		if (target == nullptr || target->id == next)
		{
			return false;
		}
		target->id = next;
		key.outer = next;
		break;
	}
	case FlowPlotGui::TemplateNodeKind::LegendElement: {
		FlowPlot::Spec::LegendSpec* legend = nullptr;
		for (FlowPlot::Spec::LegendSpec& candidate : guiState.activeTemplate.figure.legends)
		{
			if (candidate.id == key.outer)
			{
				legend = &candidate;
				break;
			}
		}
		if (legend == nullptr)
		{
			return false;
		}
		FlowPlot::Spec::LegendElementSpec* target = nullptr;
		for (FlowPlot::Spec::LegendElementSpec& element : legend->legendElements)
		{
			if (element.id == next && element.id != key.inner)
			{
				return false;
			}
			if (element.id == key.inner)
			{
				target = &element;
			}
		}
		if (target == nullptr || target->id == next)
		{
			return false;
		}
		target->id = next;
		key.inner = next;
		break;
	}
	default:
		return false;
	}

	FlowPlotGui::markTemplateChanged(guiState);
	return true;
}

panelTitleParams makePropertiesTitleParams(FlowPlotGui::state& guiState)
{
	panelTitleParams params{};
	params.titleText = "Properties";
	params.showSecondaryTitle = true;
	params.secondaryTitleParams = {
		.text = selectedTemplateNodeKindText(guiState),
		.fontSize = 12,
		.textColor = FlowUi::Flow_Color("#aeb2b8ff"),
	};

	const std::optional<std::string> editableId = selectedTemplateNodeEditableId(guiState);
	if (!editableId.has_value())
	{
		params.rightContentMode = panelTitleParams::RightContentMode::None;
		return params;
	}

	params.rightContentMode = panelTitleParams::RightContentMode::InputField;
	params.rightInputFieldParams.fieldId = "PropsTitle/id-input";
	params.rightInputFieldParams.value = *editableId;
	params.rightInputFieldParams.syncValueFromParams = true;
	params.rightInputFieldParams.defaultText = "id";
	params.rightInputFieldParams.sizing = Clay_Sizing{
		.width = CLAY_SIZING_FIXED(150),
		.height = CLAY_SIZING_FIT(0),
	};
	params.rightInputFieldParams.padding = Clay_Padding{8, 8, 4, 4};
	params.rightInputFieldParams.backgroundColor = FlowUi::Flow_Color("#171a1fff");
	params.rightInputFieldParams.borderColor = FlowUi::Flow_Color("#3d444eff");
	params.rightInputFieldParams.borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	params.rightInputFieldParams.cornerRadius = CLAY_CORNER_RADIUS(5);
	params.rightInputFieldParams.fontSize = 12;
	params.rightInputFieldParams.textColor = FlowUi::Flow_Color("#f4f6f8ff");
	params.rightInputFieldParams.onEditBegin = [&guiState]() {
		FlowPlotGui::beginDeferredDocumentEdit(
			guiState,
			FlowPlotGui::makeTemplateEditTarget(
				"selected-node-id",
				[](const FlowPlotGui::DocumentSnapshot& before, const FlowPlotGui::state& current) {
					return !FlowPlotGui::masterTemplateSpecsEqual(before.activeTemplate, current.activeTemplate);
				}),
			10.0f);
	};
	params.rightInputFieldParams.onEditEnd = [&guiState]() {
		FlowPlotGui::endDeferredDocumentEdit(guiState);
	};
	params.rightInputFieldParams.onTextChangedCallback = [&guiState](std::string_view changed) {
		renameSelectedTemplateNodeId(guiState, changed);
	};

	return params;
}

} // namespace

int runApplication()
{


	try
	{
		FlowUi::AppConfig config{};
		config.window.title = "FlowPlotGUI";
		config.window.maximized = true;
		config.window.width = 1920;
		config.window.height = 1080;
		config.vk.enableValidation = false;
		config.vk.enableDebugUtils = false;
		config.dev.enabled = FLOWPLOTGUI_DEV_MODE != 0;
		config.dev.panelOpenByDefault = FLOWPLOTGUI_DEV_MODE != 0;
		config.dev.useShortcutManagerForPanelToggle = FLOWPLOTGUI_DEV_MODE != 0;
		config.ui.stringArenaSize = 2 * 1024 * 1024;
		config.ui.defaultFontFamily = FlowUi::FontFamilyCreateInfo{
			.name = "Inter",
			.faces = {
				FlowUi::FontFaceCreateInfo{
					.path = assetPath({"Fonts", "Inter.arfont"}),
					.pixelSize = 18.0f,
					.weight = 400,
					.style = FlowUi::FontStyle::Normal,
				},
			},
		};
		config.ui.fontAtlasSize = 1024;
		FlowUi::App app = FlowUi::makeApplication(config);
		FlowPlotGui::registerIcons(app);


		FlowUi::UiManager& ui = app.ui();
		FlowPlotGui::state guiState{};
		populateInitialGuiState(guiState);

		(void)ui.shortcuts().registerShortcut(
			FlowUi::ShortcutChord{
				.key = kKeyC,
				.ctrl = true,
				.trigger = FlowUi::ShortcutTrigger::Press,
			},
			FlowUi::ShortcutScope::FocusedInput,
			kInputShortcutPriority,
				[](FlowUi::ShortcutContext& context) {
					const std::string selectedText(context.ui.inputFields().getSelectedText());
					if (selectedText.empty())
					{
						return false;
					}
					context.ui.setClipboardText(selectedText);
					return true;
				});
		(void)ui.shortcuts().registerShortcut(
			FlowUi::ShortcutChord{
				.key = kKeyV,
				.ctrl = true,
				.trigger = FlowUi::ShortcutTrigger::Press,
			},
			FlowUi::ShortcutScope::FocusedInput,
			kInputShortcutPriority,
			[](FlowUi::ShortcutContext& context) {
				(void)context.ui.inputFields().insertTextAtPrimaryCaret(context.ui.clipboardText());
				return true;
			});
		auto registerTabNavigationShortcut = [&ui, &guiState](
			FlowPlotGui::DataInputCellNavDirection dataDirection,
			FlowPlotGui::PropertyInputNavDirection propertyDirection,
			bool shift = false) {
			(void)ui.shortcuts().registerShortcut(
				FlowUi::ShortcutChord{
					.key = kKeyTab,
					.shift = shift,
					.trigger = FlowUi::ShortcutTrigger::Press,
				},
				FlowUi::ShortcutScope::FocusedInput,
				kInputShortcutPriority + 10,
				[&guiState, dataDirection, propertyDirection](FlowUi::ShortcutContext& context) {
					if (navigateDataInputCell(context, guiState, dataDirection))
					{
						return true;
					}
					return navigatePropertyInput(context, guiState, propertyDirection);
				});
		};
		auto registerDataCellNavigationShortcut = [&ui, &guiState](
			int key,
			FlowPlotGui::DataInputCellNavDirection direction,
			bool ctrl = false,
			bool shift = false) {
			(void)ui.shortcuts().registerShortcut(
				FlowUi::ShortcutChord{
					.key = key,
					.ctrl = ctrl,
					.shift = shift,
					.trigger = FlowUi::ShortcutTrigger::Press,
				},
				FlowUi::ShortcutScope::FocusedInput,
				kInputShortcutPriority + 10,
				[&guiState, direction](FlowUi::ShortcutContext& context) {
					return navigateDataInputCell(context, guiState, direction);
				});
		};
		registerTabNavigationShortcut(
			FlowPlotGui::DataInputCellNavDirection::Next,
			FlowPlotGui::PropertyInputNavDirection::Next);
		registerTabNavigationShortcut(
			FlowPlotGui::DataInputCellNavDirection::Previous,
			FlowPlotGui::PropertyInputNavDirection::Previous,
			true);
		registerDataCellNavigationShortcut(kKeyLeft, FlowPlotGui::DataInputCellNavDirection::Left, true);
		registerDataCellNavigationShortcut(kKeyRight, FlowPlotGui::DataInputCellNavDirection::Right, true);
		registerDataCellNavigationShortcut(kKeyUp, FlowPlotGui::DataInputCellNavDirection::Up, true);
		registerDataCellNavigationShortcut(kKeyDown, FlowPlotGui::DataInputCellNavDirection::Down, true);
		(void)ui.shortcuts().registerShortcut(
			FlowUi::ShortcutChord{
				.key = kKeyC,
				.ctrl = true,
				.trigger = FlowUi::ShortcutTrigger::Press,
			},
			FlowUi::ShortcutScope::FocusedElement,
			kDocumentShortcutPriority,
			[&guiState](FlowUi::ShortcutContext&) {
				return FlowPlotGui::copySelectedTemplateNode(guiState);
			});
		(void)ui.shortcuts().registerShortcut(
			FlowUi::ShortcutChord{
				.key = kKeyV,
				.ctrl = true,
				.trigger = FlowUi::ShortcutTrigger::Press,
			},
			FlowUi::ShortcutScope::FocusedElement,
			kDocumentShortcutPriority,
			[&guiState](FlowUi::ShortcutContext&) {
				return FlowPlotGui::pasteTemplateClipboardIntoSelection(guiState);
			});
		(void)ui.shortcuts().registerShortcut(
			FlowUi::ShortcutChord{
				.key = kKeyD,
				.ctrl = true,
				.trigger = FlowUi::ShortcutTrigger::Press,
			},
			FlowUi::ShortcutScope::FocusedElement,
			kDocumentShortcutPriority,
			[&guiState](FlowUi::ShortcutContext&) {
				return FlowPlotGui::duplicateSelectedTemplateNode(guiState);
			});
		(void)ui.shortcuts().registerShortcut(
			FlowUi::ShortcutChord{
				.key = kKeyZ,
				.ctrl = true,
				.trigger = FlowUi::ShortcutTrigger::Press,
			},
			FlowUi::ShortcutScope::Global,
			kDocumentShortcutPriority,
			[&guiState](FlowUi::ShortcutContext&) {
				return FlowPlotGui::undoDocument(guiState);
			});
		(void)ui.shortcuts().registerShortcut(
			FlowUi::ShortcutChord{
				.key = kKeyY,
				.ctrl = true,
				.trigger = FlowUi::ShortcutTrigger::Press,
			},
			FlowUi::ShortcutScope::Global,
			kDocumentShortcutPriority,
			[&guiState](FlowUi::ShortcutContext&) {
				return FlowPlotGui::redoDocument(guiState);
			});
		(void)ui.shortcuts().registerShortcut(
			FlowUi::ShortcutChord{
				.key = kKeyQ,
				.ctrl = true,
				.trigger = FlowUi::ShortcutTrigger::Press,
			},
			FlowUi::ShortcutScope::Global,
			kDocumentShortcutPriority,
			[&app](FlowUi::ShortcutContext&) {
				app.setShouldClose(1);
				return true;
			});




		while (!guiState.shouldClose)
		{
			app.beginFrame();
			FlowPlotGui::ensureActiveTemplateExportComparisonChecked(guiState);
			const bool closeRequested = app.shouldClose();
			const bool needsCloseConfirmation =
				closeRequested &&
				(FlowPlotGui::hasDiagnosticError(guiState) || guiState.activeTemplateDiffersFromLastExport);
			if (closeRequested && !needsCloseConfirmation)
			{
				guiState.shouldClose = true;
			}

			ui.createElement(kRootBackground, "rootBackground")
			.setParameters({
			    .backgroundColor = Clay_Color{.r = 22.0f, .g = 22.0f, .b = 24.0f, .a = 255.0f}
			})
			.construct(FlowUi::ElementDrawOptions::SkipEventCallbacks);
				navBarState& navState = NavBarDef::getOrCreateState(FlowUi::toFlowId("NavBar"));
				ui.createElement(kNavBar, "NavBar")
				.setParameters({
				    .borderColor = Clay_Color{.r = 94.0f, .g = 100.0f, .b = 110.0f, .a = 255.0f},
				    .borderWidth = Clay_BorderWidth{.left = 0, .right = 0, .top = 0, .bottom = 2, .betweenChildren = 0},
				    .padding = Clay_Padding{.left = 16, .right = 16, .top = 0, .bottom = 8},
				    .spacer1Sizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 22.0f, .max = 45.0f}}, .type = CLAY__SIZING_TYPE_GROW}, .height = Clay_SizingAxis{.size = {.percent = 0.0f}, .type = CLAY__SIZING_TYPE_PERCENT}},
				    .spacer2Sizing = Clay_Sizing{.width = Clay_SizingAxis{.size = {.minMax = Clay_SizingMinMax{.min = 22.0f, .max = 45.0f}}, .type = CLAY__SIZING_TYPE_GROW}, .height = Clay_SizingAxis{.size = {.percent = 1.0f}, .type = CLAY__SIZING_TYPE_PERCENT}},
				    .spacerLine1Color = Clay_Color{.r = 65.0f, .g = 65.0f, .b = 67.0f, .a = 255.0f},
				    .spacerLine2Color = Clay_Color{.r = 65.0f, .g = 65.0f, .b = 67.0f, .a = 255.0f},
				    .child2Gap = 16,
				    .guiState = &guiState,
				    .nativeWindowHandle = app.nativeWindowHandle()
				}).draw();
				ui.createElement(kMainContent, "MainContent").construct();
					const std::string leftId = "TemplatePanel";
					ui.createElement(kTemplatePanel, leftId).construct();
						ui.createElement(kPanelTitle, "TemplateTitle")
						.setParameters({
							.titleText = "Template Schema",
							.showSecondaryTitle = true,
							.secondaryTitleParams = {.text = "click to edit properties", .fontSize = 12}
						})
						.draw();
						const std::string treeScrollId = "TemplatePanel/tree-scroll";
						Clay_ElementDeclaration treeScroll{};
						treeScroll.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
						treeScroll.layout.sizing = {
							.width = CLAY_SIZING_GROW(0),
							.height = CLAY_SIZING_GROW(0),
						};
						treeScroll.clip = {
							.horizontal = false,
							.vertical = true,
							.childOffset = scrollOffsetForElementId(ui, treeScrollId),
						};
						CLAY(ui.toClayEID(treeScrollId), treeScroll)
						{
							FlowPlotGui::drawTemplateNode(app, ui, guiState, "TemplatePanel/tree", {
								.kind = FlowPlotGui::TemplateNodeKind::Figure,
							});
						};
					ui.drawConstructed(); // TemplatePanel
					ui.createElement(kDynamicSeparator, "separator1")
					.setParameters({
						.hoverColor = FlowUi::Flow_Color("#707074ff"),
						.activeColor = FlowUi::Flow_Color("#409d97ff"),
						.minValue = 360,
						.maxValue = 550,
						.getValue = [leftId](){
							return TemplatePanelDef::getOrCreateState(FlowUi::toFlowId(leftId)).minWidth;
						},
						.setValue = [leftId](int v){
							TemplatePanelDef::getOrCreateState(FlowUi::toFlowId(leftId)).minWidth = v;
						},
					}).draw();
					ui.createElement(kMainContentPanel, "MainContentPanel")
					.setParameters({.backgroundColor = FlowUi::Flow_Color("#18181aff")})
					.construct(FlowUi::ElementDrawOptions::SkipEventCallbacks);
						ui.createElement(kPlotviewPort, "PlotViewPort")
						.setParameters({.guiState = &guiState})
						.draw();
						ui.createElement(kDynamicSeparator, "plotDataSeparator")
						.setParameters({
							.orientation = dynamicSeparatorParams::Orientation::Horizontal,
							.reverseDrag = true,
							.color = FlowUi::Flow_Color("#3f3f41ff"),
							.hoverColor = FlowUi::Flow_Color("#707074ff"),
							.activeColor = FlowUi::Flow_Color("#409d97ff"),
							.minValue = 180,
							.maxValue = 520,
							.getValue = [](){
								return DataInputDef::getOrCreateState(FlowUi::toFlowId("DataInput")).height;
							},
							.setValue = [](int v){
								DataInputDef::getOrCreateState(FlowUi::toFlowId("DataInput")).height = v;
							},
						}).draw();
						const int dataInputHeight = DataInputDef::getOrCreateState(FlowUi::toFlowId("DataInput")).height;
						ui.createElement(kDataInput, "DataInput")
						.setParameters({
							.guiState = &guiState,
							.sizing = Clay_Sizing{
								.width = CLAY_SIZING_GROW(0),
								.height = CLAY_SIZING_FIXED(static_cast<float>(dataInputHeight)),
							},
						})
						.draw();
					ui.drawConstructed(); // MainContentPanel
					const std::string rightId = "PropsPanel";
					ui.createElement(kDynamicSeparator, "separator2")
					.setParameters({
						.reverseDrag = true,
						.hoverColor = FlowUi::Flow_Color("#707074ff"),
						.activeColor = FlowUi::Flow_Color("#409d97ff"),
						.minValue = 436,
						.maxValue = 550,
						.getValue = [rightId](){
							return PropsPanelDef::getOrCreateState(FlowUi::toFlowId(rightId)).minWidth;
						},
						.setValue = [rightId](int v){
							PropsPanelDef::getOrCreateState(FlowUi::toFlowId(rightId)).minWidth = v;
						},
					}).draw();
					ui.createElement(kPropsPanel, rightId)
					.setParameters({.guiState = &guiState})
					.construct();
						ui.createElement(kPanelTitle, "PropsTitle")
						.setParameters(makePropertiesTitleParams(guiState))
						.draw();
						ui.createElement(kPropertiesContent, "PropertiesContent")
						.setParameters({
							.padding = Clay_Padding{12, 12, 12, 12},
							.childGap = 8,
							.guiState = &guiState,
						})
						.draw();
					ui.drawConstructed(); //PropsPanel

				ui.drawConstructed(); // MainContent

				if (navState.newTemplatePresetPickerOpen)
				{
					ui.createElement(kNewTemplatePresetPicker, "NewTemplatePresetPicker")
					.setParameters({
						.guiState = &guiState,
						.isOpen = &navState.newTemplatePresetPickerOpen,
					})
					.draw();
				}
				if (navState.shortcutHelpOpen)
				{
					ui.createElement(kShortcutHelpOverlay, "ShortcutHelpOverlay")
					.setParameters({
						.isOpen = &navState.shortcutHelpOpen,
					})
					.draw();
				}
				if (needsCloseConfirmation)
				{
					ui.createElement(kQuitConfirmationDialog, "QuitConfirmationDialog")
					.setParameters({
						.guiState = &guiState,
						.app = &app,
					})
					.draw();
				}
			
			ui.drawConstructed(); // rootBackground

			FlowPlotGui::tickDeferredDocumentEdit(
				guiState,
				static_cast<float>(ui.getCurrentFrameInput().dt));

			app.endFrame();
			app.drawFrame();
		}
		FlowPlotGui::shutdownPlotViewportScene();
		return 0;
	}
	catch (const std::exception& e)
	{
		std::fprintf(stderr, "FlowPlotGUI fatal error: %s\n", e.what());
		return 1;
	}
}

#if defined(_WIN32)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	return runApplication();
}
#else
int main()
{
	return runApplication();
}
#endif
