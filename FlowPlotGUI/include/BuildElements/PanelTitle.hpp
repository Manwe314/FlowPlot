#pragma once

#include <FlowUi/Flow.hpp>

#include "FlowPlotGui.hpp"
#include "BuildElements/BasicButton.hpp"
#include "BuildElements/BasicInputField.hpp"
#include "BuildElements/BasicTitle.hpp"

struct panelTitleParams {
	enum class RightContentMode : uint8_t {
		None,
		Button,
		InputField,
	};

	int minHeight = 56;
	int maxHeight = 88;
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
	uint16_t childGap = 6;

	Clay_Padding contentPadding = CLAY_PADDING_ALL(8);
	Clay_Color contentBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color contentBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth contentBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_ChildAlignment contentChildAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t contentChildGap = 12;

	Clay_ChildAlignment leftColumnChildAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
	uint16_t leftColumnChildGap = 4;

	std::string titleText = "Panel Title";
	Clay_TextElementConfigWrapMode titleWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment titleAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t titleFontId = 0;
	uint16_t titleFontSize = 16;
	Clay_Color titleColor = FlowUi::Flow_Color("#f4f6f8ff");

	bool showSecondaryTitle = false;
	basicTitleParams secondaryTitleParams = basicTitleParams{};

	RightContentMode rightContentMode = RightContentMode::Button;
	basicButtonParams rightButtonParams = basicButtonParams{};
	basicInputFieldParams rightInputFieldParams = basicInputFieldParams{};

	Clay_Color separatorColor = FlowUi::Flow_Color("#5e646eff");
};

using PanelTitleDef = FlowUi::ElementDefinition<panelTitleParams, void, void, FLOW_DEF_ID("PanelTitle")>;

inline const PanelTitleDef kPanelTitle = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](PanelTitleDef::BuildContext& context) {
		int minHeight = context.params.minHeight;
		int maxHeight = context.params.maxHeight;
		if (maxHeight < minHeight)
		{
			maxHeight = minHeight;
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId contentId = context.uiManager.toClayEID(context.createChildElementId("content"));
		const Clay_ElementId leftColumnId = context.uiManager.toClayEID(context.createChildElementId("content/left-column"));
		const Clay_ElementId rightContentId = context.uiManager.toClayEID(context.createChildElementId("content/right-content"));
		const Clay_ElementId titleTextId = context.uiManager.toClayEID(context.createChildElementId("content/left-column/title"));
		const Clay_ElementId separatorId = context.uiManager.toClayEID(context.createChildElementId("separator"));

		const std::string secondaryTitlePath = context.createChildElementId("content/left-column/secondary-title");
		const std::string rightButtonPath = context.createChildElementId("content/right-content/button");
		const std::string rightInputPath = context.createChildElementId("content/right-content/input");

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		rootLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_FIT(static_cast<float>(minHeight), static_cast<float>(maxHeight)),
		};
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childAlignment;
		rootLayout.childGap = context.params.childGap;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_LayoutConfig contentLayout{};
		contentLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		contentLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_GROW(0),
		};
		contentLayout.padding = context.params.contentPadding;
		contentLayout.childAlignment = context.params.contentChildAlignment;
		contentLayout.childGap = context.params.contentChildGap;

		Clay_ElementDeclaration content{};
		content.id = contentId;
		content.layout = contentLayout;
		content.backgroundColor = context.params.contentBackgroundColor;
		content.border = {.color = context.params.contentBorderColor, .width = context.params.contentBorderWidth};

		Clay_LayoutConfig leftColumnLayout{};
		leftColumnLayout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		leftColumnLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_GROW(0),
		};
		leftColumnLayout.padding = CLAY_PADDING_ALL(0);
		leftColumnLayout.childAlignment = context.params.leftColumnChildAlignment;
		leftColumnLayout.childGap = context.params.leftColumnChildGap;

		Clay_ElementDeclaration leftColumn{};
		leftColumn.id = leftColumnId;
		leftColumn.layout = leftColumnLayout;
		leftColumn.backgroundColor = FlowUi::Flow_Color("#00000000");
		leftColumn.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig rightContentLayout{};
		rightContentLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rightContentLayout.sizing = {
			.width = CLAY_SIZING_FIT(0),
			.height = CLAY_SIZING_FIT(0),
		};
		rightContentLayout.padding = CLAY_PADDING_ALL(0);
		rightContentLayout.childAlignment = {.x = CLAY_ALIGN_X_RIGHT, .y = CLAY_ALIGN_Y_CENTER};
		rightContentLayout.childGap = 0;

		Clay_ElementDeclaration rightContent{};
		rightContent.id = rightContentId;
		rightContent.layout = rightContentLayout;
		rightContent.backgroundColor = FlowUi::Flow_Color("#00000000");
		rightContent.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig separatorLayout{};
		separatorLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_FIXED(2),
		};

		Clay_ElementDeclaration separator{};
		separator.id = separatorId;
		separator.layout = separatorLayout;
		separator.backgroundColor = context.params.separatorColor;
		separator.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_TextElementConfig titleTextConfig{};
		titleTextConfig.textColor = context.params.titleColor;
		titleTextConfig.fontSize = context.params.titleFontSize;
		titleTextConfig.wrapMode = context.params.titleWrapMode;
		titleTextConfig.textAlignment = context.params.titleAlignment;
		titleTextConfig.fontId = context.params.titleFontId;

		CLAY(root){
			CLAY(content){
				CLAY(leftColumn){
					CLAY({.id = titleTextId}){
						CLAY_TEXT(
							context.uiManager.toClayString(context.params.titleText),
							CLAY_TEXT_CONFIG(titleTextConfig)
						);
					};

					if (context.params.showSecondaryTitle)
					{
						context.uiManager.createElement(kBasicTitle, secondaryTitlePath)
							.setParameters(context.params.secondaryTitleParams)
							.draw();
					}
				};

				CLAY(rightContent){
					switch (context.params.rightContentMode)
					{
					case panelTitleParams::RightContentMode::None:
						break;
					case panelTitleParams::RightContentMode::Button:
						context.uiManager.createElement(kBasicButton, rightButtonPath)
							.setParameters(context.params.rightButtonParams)
							.draw();
						break;
					case panelTitleParams::RightContentMode::InputField:
						context.uiManager.createElement(kBasicInputField, rightInputPath)
							.setParameters(context.params.rightInputFieldParams)
							.draw();
						break;
					}
				};
			};

			CLAY(separator){};
		};
	},
};

using PanelTitleBuilder = FlowUi::ElementBuilder<panelTitleParams, void, void, FLOW_DEF_ID("PanelTitle")>;
