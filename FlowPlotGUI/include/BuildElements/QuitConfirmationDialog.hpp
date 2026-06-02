#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <utility>

#include <FlowUi/Flow.hpp>
#include <devMode/devApi.hpp>

#include "BuildElements/BasicButton.hpp"
#include "FlowPlotGui.hpp"

struct quitConfirmationDialogParams {
	FlowPlotGui::state* guiState = nullptr;
	FlowUi::App* app = nullptr;

	Clay_Color overlayColor = Clay_Color{.r = 96.0f, .g = 96.0f, .b = 96.0f, .a = 30.0f};
	int16_t overlayZIndex = 300;
	int16_t cardZIndex = 301;

	Clay_Sizing cardSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(420), .height = CLAY_SIZING_FIT(0)};
	Clay_Padding cardPadding = Clay_Padding{24, 24, 22, 22};
	uint16_t cardChildGap = 18;
	Clay_Color cardBackgroundColor = FlowUi::Flow_Color("#171a1fff");
	Clay_Color cardBorderColor = FlowUi::Flow_Color("#3d444eff");
	Clay_BorderWidth cardBorderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_CornerRadius cardCornerRadius = CLAY_CORNER_RADIUS(8);

	std::string titleText = "Unexported changes";
	std::string messageText = "There are unexported changes made to the current template. Exit without exporting?";
	uint16_t titleFontId = 0;
	uint16_t titleFontSize = 20;
	uint16_t messageFontId = 0;
	uint16_t messageFontSize = 16;
	Clay_Color titleTextColor = FlowUi::Flow_Color("#f4f6f8ff");
	Clay_Color messageTextColor = FlowUi::Flow_Color("#cbd2dcff");

	Clay_Color cancelButtonColor = FlowUi::Flow_Color("#252b33ff");
	Clay_Color cancelButtonHoverColor = FlowUi::Flow_Color("#323a45ff");
	Clay_Color exitButtonColor = FlowUi::Flow_Color("#b84646ff");
	Clay_Color exitButtonHoverColor = FlowUi::Flow_Color("#d45858ff");
	Clay_Color buttonTextColor = FlowUi::Flow_Color("#f4f6f8ff");
};

FLOWUI_DEV_REGISTER_STRUCT(
	quitConfirmationDialogParams,
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, overlayColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, overlayZIndex),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cardZIndex),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cardSizing),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cardPadding),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cardChildGap),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cardBackgroundColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cardBorderColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cardBorderWidth),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cardCornerRadius),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, titleText),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, messageText),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, titleFontId),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, titleFontSize),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, messageFontId),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, messageFontSize),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, titleTextColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, messageTextColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cancelButtonColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, cancelButtonHoverColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, exitButtonColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, exitButtonHoverColor),
	FLOWUI_DEV_REFLECT_FIELD(quitConfirmationDialogParams, buttonTextColor));

using QuitConfirmationDialogDef = FlowUi::ElementDefinition<
	quitConfirmationDialogParams,
	void,
	void,
	FLOW_DEF_ID("QuitConfirmationDialog")>;

inline basicButtonParams quitConfirmationButtonParams(
	std::string text,
	Clay_Color backgroundColor,
	Clay_Color hoverBackgroundColor,
	Clay_Color textColor,
	std::function<void(BasicButtonInteractionContext)> onPressed)
{
	basicButtonParams params{};
	params.text = std::move(text);
	params.onPressedCallback = std::move(onPressed);
	params.contentMode = basicButtonParams::ContentMode::TextOnly;
	params.padding = Clay_Padding{18, 18, 10, 10};
	params.sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(120), .height = CLAY_SIZING_FIXED(42)};
	params.backgroundColor = backgroundColor;
	params.hoverBackgroundColor = hoverBackgroundColor;
	params.cornerRadius = CLAY_CORNER_RADIUS(6);
	params.borderColor = FlowUi::Flow_Color("#00000000");
	params.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	params.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	params.fontSize = 16;
	params.textColor = textColor;
	return params;
}

inline const QuitConfirmationDialogDef kQuitConfirmationDialog = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](QuitConfirmationDialogDef::BuildContext& context) {
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

		Clay_TextElementConfig titleTextConfig{};
		titleTextConfig.textColor = context.params.titleTextColor;
		titleTextConfig.fontSize = context.params.titleFontSize;
		titleTextConfig.fontId = context.params.titleFontId;
		titleTextConfig.wrapMode = CLAY_TEXT_WRAP_WORDS;
		titleTextConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;

		Clay_TextElementConfig messageTextConfig{};
		messageTextConfig.textColor = context.params.messageTextColor;
		messageTextConfig.fontSize = context.params.messageFontSize;
		messageTextConfig.fontId = context.params.messageFontId;
		messageTextConfig.wrapMode = CLAY_TEXT_WRAP_WORDS;
		messageTextConfig.textAlignment = CLAY_TEXT_ALIGN_LEFT;

		CLAY(rootId, root)
		{
			CLAY(cardId, card)
			{
				CLAY(context.uiManager.toClayEID(context.createChildElementId("title")), {})
				{
					CLAY_TEXT(
						context.uiManager.toClayString(context.params.titleText),
						CLAY_TEXT_CONFIG(titleTextConfig));
				};
				CLAY(context.uiManager.toClayEID(context.createChildElementId("message")), {})
				{
					CLAY_TEXT(
						context.uiManager.toClayString(context.params.messageText),
						CLAY_TEXT_CONFIG(messageTextConfig));
				};

				Clay_ElementDeclaration buttonRow{};
				buttonRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
				buttonRow.layout.sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_FIT(0)};
				buttonRow.layout.childGap = 12;
				buttonRow.layout.childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_RIGHT, .y = CLAY_ALIGN_Y_CENTER};
				CLAY(context.uiManager.toClayEID(context.createChildElementId("buttons")), buttonRow)
				{
					context.uiManager.createElement(kBasicButton, context.createChildElementId("buttons/cancel"))
						.setParameters(quitConfirmationButtonParams(
							"Cancel",
							context.params.cancelButtonColor,
							context.params.cancelButtonHoverColor,
							context.params.buttonTextColor,
							[guiState = context.params.guiState, app = context.params.app](BasicButtonInteractionContext) {
								if (app != nullptr)
								{
									app->setShouldClose(0);
								}
								if (guiState != nullptr)
								{
									guiState->shouldClose = false;
								}
							}))
						.draw();

					context.uiManager.createElement(kBasicButton, context.createChildElementId("buttons/exit"))
						.setParameters(quitConfirmationButtonParams(
							"Exit",
							context.params.exitButtonColor,
							context.params.exitButtonHoverColor,
							context.params.buttonTextColor,
							[guiState = context.params.guiState, app = context.params.app](BasicButtonInteractionContext) {
								if (app != nullptr)
								{
									app->setShouldClose(1);
								}
								if (guiState != nullptr)
								{
									guiState->shouldClose = true;
								}
							}))
						.draw();
				};
			};
		};
	},
};
