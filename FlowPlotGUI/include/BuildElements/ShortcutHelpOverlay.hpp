#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicTitle.hpp"

struct shortcutHelpOverlayParams {
	bool* isOpen = nullptr;

	Clay_Color overlayColor = Clay_Color{.r = 96.0f, .g = 96.0f, .b = 96.0f, .a = 30.0f};
	int16_t overlayZIndex = 300;
	int16_t cardZIndex = 301;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(680), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding cardPadding = Clay_Padding{28, 28, 24, 24};
	uint16_t cardChildGap = 14;
	Clay_Color cardBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_Color cardBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth cardBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_CornerRadius cardCornerRadius = CLAY_CORNER_RADIUS(8);

	uint16_t titleFontSize = 24;
	uint16_t introFontSize = 15;
	uint16_t sectionFontSize = 13;
	uint16_t shortcutFontSize = 13;
	uint16_t descriptionFontSize = 14;
	Clay_Color titleTextColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_Color introTextColor = FlowUi::Flow_Color("#cbd2dcff");
	Clay_Color sectionTextColor = FlowUi::Flow_Color("#7dd3a8ff");
	Clay_Color shortcutTextColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_Color shortcutBackgroundColor = FlowUi::Flow_Color("#242a31ff");
	Clay_Color shortcutBorderColor = FlowUi::Flow_Color("#3b4652ff");
	Clay_Color descriptionTextColor = FlowUi::Flow_Color("#dde3ebff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	shortcutHelpOverlayParams,
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, overlayColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, overlayZIndex),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, cardZIndex),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, cardCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, titleFontSize),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, introFontSize),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, sectionFontSize),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, shortcutFontSize),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, descriptionFontSize),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, titleTextColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, introTextColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, sectionTextColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, shortcutTextColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, shortcutBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, shortcutBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(shortcutHelpOverlayParams, descriptionTextColor));

using ShortcutHelpOverlayDef = FlowUi::ElementDefinition<
	shortcutHelpOverlayParams,
	void,
	void,
	FLOW_DEF_ID("ShortcutHelpOverlay")>;

inline basicButtonParams shortcutHelpOverlayTransparentButton(
	Clay_Sizing sizing,
	Clay_Padding padding,
	std::function<void(BasicButtonInteractionContext)> onPressed)
{
	basicButtonParams params{};
	params.contentMode = basicButtonParams::ContentMode::None;
	params.sizing = sizing;
	params.padding = padding;
	params.backgroundColor = FlowUi::Flow_Color("#00000000");
	params.hoverBackgroundColor = FlowUi::Flow_Color("#00000000");
	params.borderColor = FlowUi::Flow_Color("#00000000");
	params.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	params.onPressedCallback = std::move(onPressed);
	return params;
}

inline basicTitleParams shortcutHelpTextParams(
	std::string text,
	uint16_t fontSize,
	Clay_Color textColor,
	Clay_TextAlignment alignment = CLAY_TEXT_ALIGN_LEFT)
{
	basicTitleParams params{};
	params.text = std::move(text);
	params.contentMode = basicTitleParams::ContentMode::TextOnly;
	params.textColor = textColor;
	params.fontSize = fontSize;
	params.textWrapMode = CLAY_TEXT_WRAP_WORDS;
	params.textAlignment = alignment;
	params.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
	return params;
}

inline basicTitleParams shortcutHelpKeyParams(
	std::string text,
	const shortcutHelpOverlayParams& overlayParams)
{
	basicTitleParams params = shortcutHelpTextParams(
		std::move(text),
		overlayParams.shortcutFontSize,
		overlayParams.shortcutTextColor,
		CLAY_TEXT_ALIGN_CENTER);
	params.padding = Clay_Padding{8, 8, 5, 5};
	params.sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(112), .height = CLAY_SIZING_FIT(0)};
	params.backgroundColor = overlayParams.shortcutBackgroundColor;
	params.borderColor = overlayParams.shortcutBorderColor;
	params.borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	params.cornerRadius = CLAY_CORNER_RADIUS(6);
	params.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	return params;
}

inline const ShortcutHelpOverlayDef kShortcutHelpOverlay = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](ShortcutHelpOverlayDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId cardId = context.uiManager.toClayEID(context.createChildElementId("card"));

		Clay_ElementDeclaration root{};
		root.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		root.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
		root.layout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		root.backgroundColor = context.params.overlayColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};
		root.floating = {
			.zIndex = context.params.overlayZIndex,
			.attachPoints = {
				.element = CLAY_ATTACH_POINT_LEFT_TOP,
				.parent = CLAY_ATTACH_POINT_LEFT_TOP,
			},
			.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
			.attachTo = CLAY_ATTACH_TO_ROOT,
		};

		Clay_ElementDeclaration card{};
		card.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		card.layout.sizing = context.params.cardSizing;
		card.layout.padding = context.params.cardPadding;
		card.layout.childGap = context.params.cardChildGap;
		card.backgroundColor = context.params.cardBackgroundColor;
		card.cornerRadius = context.params.cardCornerRadius;
		card.border = {.color = context.params.cardBorderColor, .width = context.params.cardBorderWidth};
		card.floating = {
			.zIndex = context.params.cardZIndex,
			.attachPoints = {
				.element = CLAY_ATTACH_POINT_CENTER_CENTER,
				.parent = CLAY_ATTACH_POINT_CENTER_CENTER,
			},
			.pointerCaptureMode = CLAY_POINTER_CAPTURE_MODE_CAPTURE,
			.attachTo = CLAY_ATTACH_TO_PARENT,
		};

		const auto closeOverlay = [isOpen = context.params.isOpen](BasicButtonInteractionContext) {
			if (isOpen != nullptr)
			{
				*isOpen = false;
			}
		};

		auto drawTitle = [&](std::string_view id, std::string text, uint16_t fontSize, Clay_Color textColor) {
			context.uiManager.createElement(kBasicTitle, context.createChildElementId(id))
				.setParameters(shortcutHelpTextParams(std::move(text), fontSize, textColor))
				.draw();
		};

		auto drawSection = [&](std::string_view id, std::string text) {
			basicTitleParams params = shortcutHelpTextParams(
				std::move(text),
				context.params.sectionFontSize,
				context.params.sectionTextColor);
			params.padding = Clay_Padding{0, 0, 8, 0};
			context.uiManager.createElement(kBasicTitle, context.createChildElementId(id))
				.setParameters(std::move(params))
				.draw();
		};

		auto drawShortcut = [&](std::string_view id, std::string shortcut, std::string description) {
			Clay_ElementDeclaration row{};
			row.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
			row.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
			row.layout.childGap = 14;
			row.layout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

			CLAY(context.uiManager.toClayEID(context.createChildElementId(id)), row)
			{
				context.uiManager.createElement(kBasicTitle, context.createChildElementId(std::string(id) + "/key"))
					.setParameters(shortcutHelpKeyParams(std::move(shortcut), context.params))
					.draw();
				context.uiManager.createElement(kBasicTitle, context.createChildElementId(std::string(id) + "/description"))
					.setParameters(shortcutHelpTextParams(
						std::move(description),
						context.params.descriptionFontSize,
						context.params.descriptionTextColor))
					.draw();
			};
		};

		CLAY(rootId, root)
		{
			context.uiManager.createElement(kBasicButton, context.createChildElementId("dismiss-button"))
				.setParameters(shortcutHelpOverlayTransparentButton(
					Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
					CLAY_PADDING_ALL(0),
					closeOverlay))
				.draw();

			CLAY(cardId, card)
			{
				drawTitle("title", "Shortcuts", context.params.titleFontSize, context.params.titleTextColor);
				drawTitle(
					"intro",
					"Use these commands while editing a template. Template hierarchy shortcuts act on the currently selected tree item.",
					context.params.introFontSize,
					context.params.introTextColor);

				drawSection("template-section", "Template hierarchy");
				drawShortcut(
					"copy-template",
					"Ctrl+C",
					"Copy the selected template node. Copy panels, layers, legends, legend elements, axes, titles, and the figure.");
				drawShortcut(
					"paste-template",
					"Ctrl+V",
					"Paste into the selected node. Same-kind targets replace properties while keeping the target id. Container targets create a new panel, layer, legend, or legend element.");
				drawShortcut(
					"duplicate-template",
					"Ctrl+D",
					"Duplicate the selected panel, layer, legend, or legend element and select the new copy.");

				drawSection("document-section", "Document");
				drawShortcut("undo", "Ctrl+Z", "Undo the last committed template or data edit.");
				drawShortcut("redo", "Ctrl+Y", "Redo the next document edit.");
				drawShortcut("quit", "Ctrl+Q", "Request quit. If the current template has unexported changes or errors, FlowPlot asks before exiting.");

				drawSection("input-section", "Text and data input");
				drawShortcut("copy-input", "Ctrl+C", "When an input field is focused, copy the selected text.");
				drawShortcut("paste-input", "Ctrl+V", "When an input field is focused, paste text from the clipboard.");
				drawShortcut("tab-input", "Tab", "Move to the next editable data or property field.");
				drawShortcut("shift-tab-input", "Shift+Tab", "Move to the previous editable data or property field.");
				drawShortcut("cell-nav", "Ctrl+Arrows", "Move between cells in the data table.");
				drawShortcut("rename-dataset-tab", "Ctrl+Click", "Rename a dataset from its tab in the dataset panel.");
			};
		};
	},
};
