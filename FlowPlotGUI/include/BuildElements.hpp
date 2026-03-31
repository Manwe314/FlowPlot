#pragma once

#include <cmath>
#include <functional>

#include <FlowUi/Flow.hpp>
#include "FlowPlotGui.hpp"


struct basicInputFieldParams {
	std::string defaultText = "...";
	Clay_Padding padding = CLAY_PADDING_ALL(10);
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(30, 90), .height = CLAY_SIZING_FIT(0)};
	Clay_Color borderColor = FlowUi::Flow_Color("#8f8d8dff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#cfcfcfff");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(6);
	Clay_ChildAlignment childTextAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t fontId = 0;
	uint16_t fontSize = 16;
	Clay_Color textColor = FlowUi::Flow_Color("#000000ff");
};

using BasicInputFieldDef = FlowUi::ElementDefinition<basicInputFieldParams, void, void, FLOW_DEF_ID("Basic input field")>;

inline const BasicInputFieldDef kBasicInputField = {
	+[](BasicInputFieldDef::InteractionContext& context) {
		(void)context;
	},
	+[](BasicInputFieldDef::InteractionContext& context) {
		context.uiManager.inputFields().requestCaret(
			context.elementID,
			FlowUi::InputFieldManager::CaretRequestKind::SetPrimary);
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BasicInputFieldDef::BuildContext& context) {
		const Clay_ElementId contentId = context.uiManager.toClayEID(context.elementID);
		const std::string textElementPath = context.createChildElementId("text");
		const Clay_ElementId textId = context.uiManager.toClayEID(textElementPath);

		auto result = context.uiManager.inputFields().requestField({
			.fieldId = context.elementID,
			.initialText = context.params.defaultText,
			.config = FlowUi::InputFieldManager::FieldConfig{
				.readOnly = false,
				.allowNewline = false,
				.allowArrowNavigation = true,
			},
			.textElementId = textId,
			.contentElementId = contentId,
		});
		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childTextAlignment;

		Clay_ElementDeclaration root{};
		root.id = contentId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.clip = {.horizontal = true, .vertical = true};
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = context.params.textWrapMode;
		textConfig.textAlignment = context.params.textAlignment;
		textConfig.fontId = context.params.fontId;

		CLAY(root){
			CLAY({.id = textId}){
				CLAY_TEXT(
					context.uiManager.toClayString(result.text),
					CLAY_TEXT_CONFIG(textConfig)
				);
			};
		};
	},
};

struct basicButtonParams;
using BasicButtonDef = FlowUi::ElementDefinition<basicButtonParams, void, void, FLOW_DEF_ID("Basic button")>;
using BasicButtonInteractionContext = BasicButtonDef::InteractionContext;

struct basicButtonParams {
	enum class ContentMode : uint8_t {
		None,
		TextOnly,
		IconOnly,
		IconThenText,
		TextThenIcon,
	};

	std::string text = "";
	FlowUi::TextureRef icon = FlowUi::TextureRef{};
	std::function<void(BasicButtonInteractionContext)> onHoveredCallback = nullptr;
	std::function<void(BasicButtonInteractionContext)> onPressedCallback = nullptr;
	ContentMode contentMode = ContentMode::None;

	Clay_Padding padding = CLAY_PADDING_ALL(10);
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#cfcfcfff");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(6);
	Clay_Color borderColor = FlowUi::Flow_Color("#8f8d8dff");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{1, 1, 1, 1, 0};
	Clay_LayoutDirection childLayoutDirection = CLAY_LEFT_TO_RIGHT;
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t childGap = 8;

	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t fontId = 0;
	uint16_t fontSize = 16;
	Clay_Color textColor = FlowUi::Flow_Color("#000000ff");

	Clay_Sizing iconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)};
	Clay_Padding iconContainerPadding = CLAY_PADDING_ALL(0);
	Clay_LayoutDirection iconContainerChildLayoutDirection = CLAY_LEFT_TO_RIGHT;
	Clay_ChildAlignment iconContainerChildAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t iconContainerChildGap = 0;
	Clay_Color iconContainerBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color iconContainerBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth iconContainerBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	Clay_Sizing iconSizing = Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Color iconTintColor = FlowUi::Flow_Color("#00000000");
};

inline const BasicButtonDef kBasicButton = {
	+[](BasicButtonDef::InteractionContext& context) {
		if (context.params.onHoveredCallback != nullptr)
		{
			context.params.onHoveredCallback(context);
		}
	},
	+[](BasicButtonDef::InteractionContext& context) {
		if (context.params.onPressedCallback != nullptr)
		{
			context.params.onPressedCallback(context);
		}
	},
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BasicButtonDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);

		const basicButtonParams::ContentMode contentMode = context.params.contentMode;
		const bool needsText =
			contentMode == basicButtonParams::ContentMode::TextOnly ||
			contentMode == basicButtonParams::ContentMode::IconThenText ||
			contentMode == basicButtonParams::ContentMode::TextThenIcon;
		const bool needsIcon =
			contentMode == basicButtonParams::ContentMode::IconOnly ||
			contentMode == basicButtonParams::ContentMode::IconThenText ||
			contentMode == basicButtonParams::ContentMode::TextThenIcon;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = context.params.childLayoutDirection;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childAlignment;
		rootLayout.childGap = context.params.childGap;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		Clay_TextElementConfig textConfig{};
		Clay_ElementId textId{};
		if (needsText)
		{
			const std::string textPath = context.createChildElementId("text");
			textId = context.uiManager.toClayEID(textPath);
			textConfig.textColor = context.params.textColor;
			textConfig.fontSize = context.params.fontSize;
			textConfig.wrapMode = context.params.textWrapMode;
			textConfig.textAlignment = context.params.textAlignment;
			textConfig.fontId = context.params.fontId;
		}

		Clay_ElementDeclaration iconContainer{};
		Clay_ElementDeclaration iconElement{};
		if (needsIcon)
		{
			const std::string iconContainerPath = context.createChildElementId("icon-container");
			const std::string iconPath = context.createChildElementId("icon");
			const Clay_ElementId iconContainerId = context.uiManager.toClayEID(iconContainerPath);
			const Clay_ElementId iconId = context.uiManager.toClayEID(iconPath);

			Clay_LayoutConfig iconContainerLayout{};
			iconContainerLayout.layoutDirection = context.params.iconContainerChildLayoutDirection;
			iconContainerLayout.sizing = context.params.iconContainerSizing;
			iconContainerLayout.padding = context.params.iconContainerPadding;
			iconContainerLayout.childAlignment = context.params.iconContainerChildAlignment;
			iconContainerLayout.childGap = context.params.iconContainerChildGap;

			iconContainer.id = iconContainerId;
			iconContainer.layout = iconContainerLayout;
			iconContainer.backgroundColor = context.params.iconContainerBackgroundColor;
			iconContainer.border = {.color = context.params.iconContainerBorderColor, .width = context.params.iconContainerBorderWidth};

			Clay_LayoutConfig iconLayout{};
			iconLayout.sizing = context.params.iconSizing;

			iconElement.id = iconId;
			iconElement.layout = iconLayout;
			iconElement.backgroundColor = context.params.iconTintColor;
			iconElement.image = {
				.imageData = context.uiManager.storeTexture(context.params.icon),
			};
		}

		auto drawTextChild = [&]() {
			CLAY({.id = textId}){
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.text),
					CLAY_TEXT_CONFIG(textConfig)
				);
			};
		};

		auto drawIconChild = [&]() {
			CLAY(iconContainer){
				CLAY(iconElement){};
			};
		};

		CLAY(root){
			switch (contentMode)
			{
			case basicButtonParams::ContentMode::None:
				break;
			case basicButtonParams::ContentMode::TextOnly:
				drawTextChild();
				break;
			case basicButtonParams::ContentMode::IconOnly:
				drawIconChild();
				break;
			case basicButtonParams::ContentMode::IconThenText:
				drawIconChild();
				drawTextChild();
				break;
			case basicButtonParams::ContentMode::TextThenIcon:
				drawTextChild();
				drawIconChild();
				break;
			}
		};
	},
};

struct basicTitleParams {
	enum class ContentMode : uint8_t {
		TextOnly,
		IconThenText,
		TextThenIcon,
	};

	std::string text = "Title";
	FlowUi::TextureRef icon = FlowUi::TextureRef{};
	ContentMode contentMode = ContentMode::TextOnly;

	Clay_Padding padding = CLAY_PADDING_ALL(0);
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_CornerRadius cornerRadius = CLAY_CORNER_RADIUS(0);
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_LayoutDirection childLayoutDirection = CLAY_LEFT_TO_RIGHT;
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t childGap = 8;

	Clay_TextElementConfigWrapMode textWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment textAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t fontId = 0;
	uint16_t fontSize = 16;
	Clay_Color textColor = FlowUi::Flow_Color("#f4f6f8ff");

	Clay_Sizing iconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(12), .height = CLAY_SIZING_FIXED(12)};
	Clay_Padding iconContainerPadding = CLAY_PADDING_ALL(0);
	Clay_LayoutDirection iconContainerChildLayoutDirection = CLAY_LEFT_TO_RIGHT;
	Clay_ChildAlignment iconContainerChildAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
	uint16_t iconContainerChildGap = 0;
	Clay_Color iconContainerBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color iconContainerBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth iconContainerBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	Clay_Sizing iconSizing = Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Color iconTintColor = FlowUi::Flow_Color("#72b6ffff");
};

using BasicTitleDef = FlowUi::ElementDefinition<basicTitleParams, void, void, FLOW_DEF_ID("Basic title")>;

inline const BasicTitleDef kBasicTitle = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](BasicTitleDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const bool needsIcon = context.params.contentMode != basicTitleParams::ContentMode::TextOnly;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = context.params.childLayoutDirection;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = context.params.childAlignment;
		rootLayout.childGap = context.params.childGap;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = context.params.cornerRadius;
		root.border = {.color = context.params.borderColor, .width = context.params.borderWidth};

		const std::string textPath = context.createChildElementId("text");
		const Clay_ElementId textId = context.uiManager.toClayEID(textPath);

		Clay_TextElementConfig textConfig{};
		textConfig.textColor = context.params.textColor;
		textConfig.fontSize = context.params.fontSize;
		textConfig.wrapMode = context.params.textWrapMode;
		textConfig.textAlignment = context.params.textAlignment;
		textConfig.fontId = context.params.fontId;

		Clay_ElementDeclaration iconContainer{};
		Clay_ElementDeclaration iconElement{};
		if (needsIcon)
		{
			const std::string iconContainerPath = context.createChildElementId("icon-container");
			const std::string iconPath = context.createChildElementId("icon");
			const Clay_ElementId iconContainerId = context.uiManager.toClayEID(iconContainerPath);
			const Clay_ElementId iconId = context.uiManager.toClayEID(iconPath);

			Clay_LayoutConfig iconContainerLayout{};
			iconContainerLayout.layoutDirection = context.params.iconContainerChildLayoutDirection;
			iconContainerLayout.sizing = context.params.iconContainerSizing;
			iconContainerLayout.padding = context.params.iconContainerPadding;
			iconContainerLayout.childAlignment = context.params.iconContainerChildAlignment;
			iconContainerLayout.childGap = context.params.iconContainerChildGap;

			iconContainer.id = iconContainerId;
			iconContainer.layout = iconContainerLayout;
			iconContainer.backgroundColor = context.params.iconContainerBackgroundColor;
			iconContainer.border = {.color = context.params.iconContainerBorderColor, .width = context.params.iconContainerBorderWidth};

			Clay_LayoutConfig iconLayout{};
			iconLayout.sizing = context.params.iconSizing;

			iconElement.id = iconId;
			iconElement.layout = iconLayout;
			iconElement.backgroundColor = context.params.iconTintColor;
			iconElement.image = {
				.imageData = context.uiManager.storeTexture(context.params.icon),
			};
		}

		auto drawTextChild = [&]() {
			CLAY({.id = textId}){
				CLAY_TEXT(
					context.uiManager.toClayString(context.params.text),
					CLAY_TEXT_CONFIG(textConfig)
				);
			};
		};

		auto drawIconChild = [&]() {
			CLAY(iconContainer){
				CLAY(iconElement){};
			};
		};

		CLAY(root){
			switch (context.params.contentMode)
			{
			case basicTitleParams::ContentMode::TextOnly:
				drawTextChild();
				break;
			case basicTitleParams::ContentMode::IconThenText:
				drawIconChild();
				drawTextChild();
				break;
			case basicTitleParams::ContentMode::TextThenIcon:
				drawTextChild();
				drawIconChild();
				break;
			}
		};
	},
};

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

struct plotviewPortParams {
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Padding padding = CLAY_PADDING_ALL(0);
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color viewportColor = FlowUi::Flow_Color("#7a7a7aff");
};

struct plotviewPortResources {
	PanelTitleBuilder titleBuilder;

	explicit plotviewPortResources(FlowUi::UiManager& uiManager) :
		titleBuilder(makeTitleBuilder(uiManager)) {}

private:
	static PanelTitleBuilder makeTitleBuilder(FlowUi::UiManager& uiManager)
	{
		panelTitleParams params{};
		params.titleText = "Visual Preview";
		params.rightContentMode = panelTitleParams::RightContentMode::Button;
		params.rightButtonParams.text = "Reset";
		params.rightButtonParams.contentMode = basicButtonParams::ContentMode::TextOnly;
		params.rightButtonParams.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		PanelTitleBuilder builder = uiManager.createElement(kPanelTitle, "PlotviewPort/shared/title");
		builder.setParameters(std::move(params));
		return builder;
	}
};

using PlotviewPortDef = FlowUi::ElementDefinition<
	plotviewPortParams,
	void,
	plotviewPortResources,
	FLOW_DEF_ID("PlotviewPort")>;

inline const PlotviewPortDef kPlotviewPort = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](PlotviewPortDef::BuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},
	+[](PlotviewPortDef::BuildContext& context) {
		if (!PlotviewPortDef::resources.has_value())
		{
			PlotviewPortDef::resources.emplace(context.uiManager);
		}
		plotviewPortResources& resources = *PlotviewPortDef::resources;

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId viewportId = context.uiManager.toClayEID(context.createChildElementId("viewport"));
		const std::string titlePath = context.createChildElementId("title");

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = context.params.padding;
		rootLayout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_TOP};
		rootLayout.childGap = 0;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig viewportLayout{};
		viewportLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_GROW(0),
		};

		Clay_ElementDeclaration viewport{};
		viewport.id = viewportId;
		viewport.layout = viewportLayout;
		viewport.backgroundColor = context.params.viewportColor;
		viewport.cornerRadius = CLAY_CORNER_RADIUS(0);
		viewport.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(root){
			resources.titleBuilder
				.withElementID(titlePath)
				.draw();

			CLAY(viewport){};
		};
	},
};

struct dataInputParams {
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#7a7a7aff");
};

using DataInputDef = FlowUi::ElementDefinition<dataInputParams, void, void, FLOW_DEF_ID("DataInput")>;

inline const DataInputDef kDataInput = {
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	+[](DataInputDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);

		Clay_LayoutConfig rootLayout{};
		rootLayout.sizing = context.params.sizing;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(root){};
	},
};

struct dynamicSeparatorParams {
	enum class Orientation : uint8_t {
		Vertical,
		Horizontal,
	};

	Orientation orientation = Orientation::Vertical;
	int width = 4;
	int height = 4;
	Clay_Color color = FlowUi::Flow_Color("#5e646eff");
	Clay_Color hoverColor = FlowUi::Flow_Color("#7a828fff");
	Clay_Color activeColor = FlowUi::Flow_Color("#9aa2aeff");

	int minValue = 0;
	int maxValue = 100000;
	std::function<int()> getValue = nullptr;
	std::function<void(int)> setValue = nullptr;
};

struct dynamicSeparatorState {
	bool isPressed = false;
	bool isDragging = false;
	float pressMouseAxis = 0.0f;
	int pressValue = 0;
	int localValue = 0;
};

using DynamicSeparatorDef = FlowUi::ElementDefinition<
	dynamicSeparatorParams,
	dynamicSeparatorState,
	void,
	FLOW_DEF_ID("DynamicSeparator")>;

inline const DynamicSeparatorDef kDynamicSeparator = {
	+[](DynamicSeparatorDef::InteractionContext& context) {
		dynamicSeparatorState& state = DynamicSeparatorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		state.isPressed = true;
		state.isDragging = true;
		const FrameInput& previousInput = context.uiManager.getPreviousFrameInput();
		state.pressMouseAxis =
			(context.params.orientation == dynamicSeparatorParams::Orientation::Horizontal)
			? previousInput.mouseY
			: previousInput.mouseX;

		int minValue = context.params.minValue;
		int maxValue = context.params.maxValue;
		if (maxValue < minValue)
		{
			maxValue = minValue;
		}

		int baseValue = state.localValue;
		if (context.params.getValue != nullptr)
		{
			baseValue = context.params.getValue();
		}
		if (baseValue < minValue)
		{
			baseValue = minValue;
		}
		else if (baseValue > maxValue)
		{
			baseValue = maxValue;
		}
		state.pressValue = baseValue;
	},
	nullptr,
	nullptr,
	+[](DynamicSeparatorDef::InteractionContext& context) {
		dynamicSeparatorState& state = DynamicSeparatorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		state.isPressed = false;
		state.isDragging = false;
	},
	+[](DynamicSeparatorDef::InteractionContext& context) {
		dynamicSeparatorState& state = DynamicSeparatorDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		const FrameInput& input = context.uiManager.getCurrentFrameInput();
		if (!input.mouseDown[0])
		{
			state.isPressed = false;
			state.isDragging = false;
			return;
		}
		if (!state.isDragging)
		{
			return;
		}

		int minValue = context.params.minValue;
		int maxValue = context.params.maxValue;
		if (maxValue < minValue)
		{
			maxValue = minValue;
		}

		const float currentAxis =
			(context.params.orientation == dynamicSeparatorParams::Orientation::Horizontal)
			? input.mouseY
			: input.mouseX;
		const float deltaAxis = currentAxis - state.pressMouseAxis;
		const int deltaPixels = static_cast<int>(std::lround(deltaAxis));
		int nextValue = state.pressValue + deltaPixels;
		if (nextValue < minValue)
		{
			nextValue = minValue;
		}
		else if (nextValue > maxValue)
		{
			nextValue = maxValue;
		}

		state.localValue = nextValue;
		if (context.params.setValue != nullptr)
		{
			context.params.setValue(nextValue);
		}
	},
	nullptr,
	+[](DynamicSeparatorDef::BuildContext& context) {
		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const dynamicSeparatorState* state = DynamicSeparatorDef::tryGetStateConst(FlowUi::toFlowId(context.elementID));
		const FlowUi::InteractionSnapshot& previousInteraction = context.uiManager.getPreviousFramesInteraction();

		int width = context.params.width;
		int height = context.params.height;
		if (width < 1)
		{
			width = 1;
		}
		if (height < 1)
		{
			height = 1;
		}

		Clay_Color separatorColor = context.params.color;
		if (state && state->isPressed)
		{
			separatorColor = context.params.activeColor;
		}
		else if (previousInteraction.isHovered(rootId))
		{
			separatorColor = context.params.hoverColor;
		}

		Clay_LayoutConfig rootLayout{};
		if (context.params.orientation == dynamicSeparatorParams::Orientation::Horizontal)
		{
			rootLayout.sizing = {
				.width = CLAY_SIZING_GROW(0),
				.height = CLAY_SIZING_FIXED(static_cast<float>(height)),
			};
		}
		else
		{
			rootLayout.sizing = {
				.width = CLAY_SIZING_FIXED(static_cast<float>(width)),
				.height = CLAY_SIZING_GROW(0),
			};
		}

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = separatorColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		CLAY(root){};
	},
};

struct templateLayerParams {
	Clay_Sizing sizing = Clay_Sizing{.width = CLAY_SIZING_GROW(0, 10000), .height = CLAY_SIZING_FIT(0)};
	Clay_Color backgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color borderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};
	Clay_ChildAlignment childAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};

	Clay_Color indicatorColor = FlowUi::Flow_Color("#4ea9ffff");
	bool focused = false;

	int leftSpacerMinWidth = 0;
	int leftSpacerMaxWidth = 120;

	int mainContentMinWidth = 180;
	int mainContentMaxWidth = 420;
	Clay_Padding mainContentPadding = CLAY_PADDING_ALL(0);
	int mainContentChildGap = 8;
	Clay_ChildAlignment mainContentChildAlignment = Clay_ChildAlignment{.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
	Clay_Color mainContentBackgroundColor = FlowUi::Flow_Color("#00000000");
	Clay_Color mainContentBorderColor = FlowUi::Flow_Color("#00000000");
	Clay_BorderWidth mainContentBorderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

	bool showExpanderButton = false;
	bool showAdderButton = false;

	FlowUi::TextureRef mainIcon = FlowUi::TextureRef{};
	Clay_Sizing mainIconContainerSizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(18), .height = CLAY_SIZING_FIXED(18)};
	Clay_Sizing mainIconSizing = Clay_Sizing{.width = CLAY_SIZING_PERCENT(1.0f), .height = CLAY_SIZING_PERCENT(1.0f)};
	Clay_Color mainIconTintColor = FlowUi::Flow_Color("#00000000");

	std::string mainText = "Template Layer";
	Clay_TextElementConfigWrapMode mainTextWrapMode = CLAY_TEXT_WRAP_NONE;
	Clay_TextAlignment mainTextAlignment = CLAY_TEXT_ALIGN_LEFT;
	uint16_t mainTextFontId = 0;
	uint16_t mainTextFontSize = 16;
	Clay_Color mainTextColor = FlowUi::Flow_Color("#000000ff");
};

struct templateLayerState {
	bool isExpanded = false;
};

using BasicButtonBuilder = FlowUi::ElementBuilder<basicButtonParams, void, void, FLOW_DEF_ID("Basic button")>;

struct templateLayerResources {
	BasicButtonBuilder expanderBuilder;
	BasicButtonBuilder adderBuilder;

	explicit templateLayerResources(FlowUi::UiManager& uiManager) :
		expanderBuilder(makeBuilder(uiManager, "TemplateLayer/shared/expander")),
		adderBuilder(makeBuilder(uiManager, "TemplateLayer/shared/adder")) {}

private:
	static BasicButtonBuilder makeBuilder(FlowUi::UiManager& uiManager, std::string_view path)
	{
		basicButtonParams params{};
		params.text = "";
		params.contentMode = basicButtonParams::ContentMode::IconOnly;
		params.sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(38), .height = CLAY_SIZING_FIXED(38)};
		params.padding = CLAY_PADDING_ALL(0);
		params.backgroundColor = FlowUi::Flow_Color("#00000000");
		params.borderColor = FlowUi::Flow_Color("#00000000");
		params.borderWidth = Clay_BorderWidth{0, 0, 0, 0, 0};

		BasicButtonBuilder builder = uiManager.createElement(kBasicButton, path);
		builder.setParameters(std::move(params));
		return builder;
	}
};

using TemplateLayerDef = FlowUi::ElementDefinition<
	templateLayerParams,
	templateLayerState,
	templateLayerResources,
	FLOW_DEF_ID("TemplateLayer")>;

inline const TemplateLayerDef kTemplateLayer = {
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)context;
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)context;
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)context;
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)context;
	},
	+[](TemplateLayerDef::InteractionContext& context) {
		(void)TemplateLayerDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
	},
	+[](TemplateLayerDef::BuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},
	+[](TemplateLayerDef::BuildContext& context) {
		(void)TemplateLayerDef::getOrCreateState(FlowUi::toFlowId(context.elementID));
		if (!TemplateLayerDef::resources.has_value())
		{
			TemplateLayerDef::resources.emplace(context.uiManager);
		}
		templateLayerResources& resources = *TemplateLayerDef::resources;

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId indicatorId = context.uiManager.toClayEID(context.createChildElementId("indicator"));
		const Clay_ElementId leftSpacerId = context.uiManager.toClayEID(context.createChildElementId("left-spacer"));
		const Clay_ElementId mainContentId = context.uiManager.toClayEID(context.createChildElementId("main-content"));
		const Clay_ElementId mainLeadingButtonSlotId = context.uiManager.toClayEID(context.createChildElementId("main-content/leading-button-slot"));
		const Clay_ElementId mainIconContainerId = context.uiManager.toClayEID(context.createChildElementId("main-content/icon-container"));
		const Clay_ElementId mainIconId = context.uiManager.toClayEID(context.createChildElementId("main-content/icon"));
		const Clay_ElementId mainTextId = context.uiManager.toClayEID(context.createChildElementId("main-content/text"));
		const Clay_ElementId rightSpacerId = context.uiManager.toClayEID(context.createChildElementId("right-spacer"));

		int leftSpacerMinWidth = context.params.leftSpacerMinWidth;
		int leftSpacerMaxWidth = context.params.leftSpacerMaxWidth;
		if (leftSpacerMaxWidth < leftSpacerMinWidth)
		{
			leftSpacerMaxWidth = leftSpacerMinWidth;
		}

		int mainContentMinWidth = context.params.mainContentMinWidth;
		int mainContentMaxWidth = context.params.mainContentMaxWidth;
		if (mainContentMaxWidth < mainContentMinWidth)
		{
			mainContentMaxWidth = mainContentMinWidth;
		}

		const bool hasMainIcon = context.params.mainIcon.id != 0;

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.sizing = context.params.sizing;
		rootLayout.padding = CLAY_PADDING_ALL(0);
		rootLayout.childAlignment = context.params.childAlignment;
		rootLayout.childGap = 0;

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {
			.color = context.params.borderColor,
			.width = context.params.borderWidth,
		};

		Clay_LayoutConfig indicatorLayout{};
		indicatorLayout.sizing = {
			.width = CLAY_SIZING_FIXED(2),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration indicator{};
		indicator.id = indicatorId;
		indicator.layout = indicatorLayout;
		indicator.backgroundColor = context.params.indicatorColor;
		indicator.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig leftSpacerLayout{};
		leftSpacerLayout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(leftSpacerMinWidth), static_cast<float>(leftSpacerMaxWidth)),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration leftSpacer{};
		leftSpacer.id = leftSpacerId;
		leftSpacer.layout = leftSpacerLayout;

		Clay_LayoutConfig mainContentLayout{};
		mainContentLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainContentLayout.sizing = {
			.width = CLAY_SIZING_FIT(static_cast<float>(mainContentMinWidth), static_cast<float>(mainContentMaxWidth)),
			.height = CLAY_SIZING_FIT(0),
		};
		mainContentLayout.padding = context.params.mainContentPadding;
		mainContentLayout.childAlignment = context.params.mainContentChildAlignment;
		mainContentLayout.childGap = static_cast<uint16_t>(context.params.mainContentChildGap);

		Clay_ElementDeclaration mainContent{};
		mainContent.id = mainContentId;
		mainContent.layout = mainContentLayout;
		mainContent.backgroundColor = context.params.mainContentBackgroundColor;
		mainContent.cornerRadius = CLAY_CORNER_RADIUS(0);
		mainContent.border = {
			.color = context.params.mainContentBorderColor,
			.width = context.params.mainContentBorderWidth,
		};

		Clay_LayoutConfig mainLeadingButtonSlotLayout{};
		mainLeadingButtonSlotLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainLeadingButtonSlotLayout.sizing = Clay_Sizing{.width = CLAY_SIZING_FIXED(38), .height = CLAY_SIZING_FIXED(38)};
		mainLeadingButtonSlotLayout.padding = CLAY_PADDING_ALL(0);
		mainLeadingButtonSlotLayout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		mainLeadingButtonSlotLayout.childGap = 0;

		Clay_ElementDeclaration mainLeadingButtonSlot{};
		mainLeadingButtonSlot.id = mainLeadingButtonSlotId;
		mainLeadingButtonSlot.layout = mainLeadingButtonSlotLayout;
		mainLeadingButtonSlot.backgroundColor = FlowUi::Flow_Color("#00000000");
		mainLeadingButtonSlot.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig mainIconContainerLayout{};
		mainIconContainerLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		mainIconContainerLayout.sizing = context.params.mainIconContainerSizing;
		mainIconContainerLayout.padding = CLAY_PADDING_ALL(0);
		mainIconContainerLayout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		mainIconContainerLayout.childGap = 0;

		Clay_ElementDeclaration mainIconContainer{};
		mainIconContainer.id = mainIconContainerId;
		mainIconContainer.layout = mainIconContainerLayout;
		mainIconContainer.backgroundColor = FlowUi::Flow_Color("#00000000");
		mainIconContainer.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig mainIconLayout{};
		mainIconLayout.sizing = context.params.mainIconSizing;

		Clay_ElementDeclaration mainIcon{};
		mainIcon.id = mainIconId;
		mainIcon.layout = mainIconLayout;
		mainIcon.backgroundColor = context.params.mainIconTintColor;
		mainIcon.image = {
			.imageData = context.uiManager.storeTexture(context.params.mainIcon),
		};

		Clay_TextElementConfig mainTextConfig{};
		mainTextConfig.textColor = context.params.mainTextColor;
		mainTextConfig.fontSize = context.params.mainTextFontSize;
		mainTextConfig.wrapMode = context.params.mainTextWrapMode;
		mainTextConfig.textAlignment = context.params.mainTextAlignment;
		mainTextConfig.fontId = context.params.mainTextFontId;

		Clay_LayoutConfig rightSpacerLayout{};
		rightSpacerLayout.sizing = {
			.width = CLAY_SIZING_GROW(0, 100000),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration rightSpacer{};
		rightSpacer.id = rightSpacerId;
		rightSpacer.layout = rightSpacerLayout;

		const std::string expanderPath = context.createChildElementId("main-content/expander");
		const std::string adderPath = context.createChildElementId("adder");

		CLAY(root){
			if (context.params.focused)
			{
				CLAY(indicator){};
			}
			CLAY(leftSpacer){};

			CLAY(mainContent){
				CLAY(mainLeadingButtonSlot){
					if (context.params.showExpanderButton)
					{
						resources.expanderBuilder
							.withElementID(expanderPath)
							.draw();
					}
				};

				CLAY(mainIconContainer){
					if (hasMainIcon)
					{
						CLAY(mainIcon){};
					}
				};

				CLAY({.id = mainTextId}){
					CLAY_TEXT(
						context.uiManager.toClayString(context.params.mainText),
						CLAY_TEXT_CONFIG(mainTextConfig)
					);
				};
			};

			CLAY(rightSpacer){};

			if (context.params.showAdderButton)
			{
				resources.adderBuilder
					.withElementID(adderPath)
					.draw();
			}
		};
	},
};

struct navBarParams {
	Clay_Color backgroundColor = FlowUi::Flow_Color("#20232aff");
	int heightMin = 52;
	int heightMax = 72;
	int spacer1MinWidth = 22;
	int spacer1MaxWidth = 68;
	int spacer2MinWidth = 22;
	int spacer2MaxWidth = 68;
};

using BasicTitleBuilder = FlowUi::ElementBuilder<basicTitleParams, void, void, FLOW_DEF_ID("Basic title")>;

struct navBarResources {
	BasicTitleBuilder child1Builder;

	explicit navBarResources(FlowUi::UiManager& uiManager) :
		child1Builder(makeChild1Builder(uiManager)) {}

private:
	static BasicTitleBuilder makeChild1Builder(FlowUi::UiManager& uiManager)
	{
		basicTitleParams params{};
		params.text = "FlowPlot";
		params.contentMode = basicTitleParams::ContentMode::IconThenText;
		params.iconTintColor = FlowUi::Flow_Color("#72b6ffff");
		params.textColor = FlowUi::Flow_Color("#f4f6f8ff");
		params.fontSize = 16;
		params.fontId = 0;
		params.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		BasicTitleBuilder builder = uiManager.createElement(kBasicTitle, "NavBar/shared/child-1/title");
		builder.setParameters(std::move(params));
		return builder;
	}
};

using NavBarDef = FlowUi::ElementDefinition<
	navBarParams,
	void,
	navBarResources,
	FLOW_DEF_ID("NavBar")>;

inline const NavBarDef kNavBar = {
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::InteractionContext& context) {
		(void)context;
	},
	+[](NavBarDef::BuildContext& context) -> Clay_ElementDeclaration {
		(void)context;
		return Clay_ElementDeclaration{};
	},
	+[](NavBarDef::BuildContext& context) {
		if (!NavBarDef::resources.has_value())
		{
			NavBarDef::resources.emplace(context.uiManager);
		}
		navBarResources& resources = *NavBarDef::resources;

		int navHeightMin = context.params.heightMin;
		int navHeightMax = context.params.heightMax;
		if (navHeightMax < navHeightMin)
		{
			navHeightMax = navHeightMin;
		}

		int spacer1MinWidth = context.params.spacer1MinWidth;
		int spacer1MaxWidth = context.params.spacer1MaxWidth;
		if (spacer1MaxWidth < spacer1MinWidth)
		{
			spacer1MaxWidth = spacer1MinWidth;
		}

		int spacer2MinWidth = context.params.spacer2MinWidth;
		int spacer2MaxWidth = context.params.spacer2MaxWidth;
		if (spacer2MaxWidth < spacer2MinWidth)
		{
			spacer2MaxWidth = spacer2MinWidth;
		}

		const Clay_ElementId rootId = context.uiManager.toClayEID(context.elementID);
		const Clay_ElementId child1Id = context.uiManager.toClayEID(context.createChildElementId("child-1"));
		const Clay_ElementId spacer1Id = context.uiManager.toClayEID(context.createChildElementId("spacer-1"));
		const Clay_ElementId spacer1LineId = context.uiManager.toClayEID(context.createChildElementId("spacer-1/line"));
		const Clay_ElementId child2Id = context.uiManager.toClayEID(context.createChildElementId("child-2"));
		const Clay_ElementId child2GrowSpacerId = context.uiManager.toClayEID(context.createChildElementId("child-2/grow-spacer"));
		const Clay_ElementId spacer2Id = context.uiManager.toClayEID(context.createChildElementId("spacer-2"));
		const Clay_ElementId spacer2LineId = context.uiManager.toClayEID(context.createChildElementId("spacer-2/line"));
		const Clay_ElementId child3Id = context.uiManager.toClayEID(context.createChildElementId("child-3"));

		const std::string button21Path = context.createChildElementId("child-2/button-1");
		const std::string button22Path = context.createChildElementId("child-2/button-2");
		const std::string button31Path = context.createChildElementId("child-3/button-1");
		const std::string button32Path = context.createChildElementId("child-3/button-2");
		const std::string child1TitlePath = context.createChildElementId("child-1/title");

		Clay_LayoutConfig rootLayout{};
		rootLayout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		rootLayout.childGap = 0;
		rootLayout.padding = CLAY_PADDING_ALL(0);
		rootLayout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		rootLayout.sizing = {
			.width = CLAY_SIZING_GROW(0),
			.height = CLAY_SIZING_FIT(static_cast<float>(navHeightMin), static_cast<float>(navHeightMax)),
		};

		Clay_ElementDeclaration root{};
		root.id = rootId;
		root.layout = rootLayout;
		root.backgroundColor = context.params.backgroundColor;
		root.cornerRadius = CLAY_CORNER_RADIUS(0);
		root.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig child1Layout{};
		child1Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child1Layout.childGap = 8;
		child1Layout.padding = CLAY_PADDING_ALL(0);
		child1Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child1Layout.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child1{};
		child1.id = child1Id;
		child1.layout = child1Layout;
		child1.backgroundColor = FlowUi::Flow_Color("#00000000");
		child1.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig spacer1Layout{};
		spacer1Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		spacer1Layout.childGap = 0;
		spacer1Layout.padding = CLAY_PADDING_ALL(0);
		spacer1Layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		spacer1Layout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(spacer1MinWidth), static_cast<float>(spacer1MaxWidth)),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration spacer1{};
		spacer1.id = spacer1Id;
		spacer1.layout = spacer1Layout;
		spacer1.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer1.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig spacerLineLayout{};
		spacerLineLayout.sizing = {.width = CLAY_SIZING_FIXED(2), .height = CLAY_SIZING_FIXED(20)};

		Clay_ElementDeclaration spacer1Line{};
		spacer1Line.id = spacer1LineId;
		spacer1Line.layout = spacerLineLayout;
		spacer1Line.backgroundColor = FlowUi::Flow_Color("#5e646eff");
		spacer1Line.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig child2Layout{};
		child2Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child2Layout.childGap = 8;
		child2Layout.padding = CLAY_PADDING_ALL(0);
		child2Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child2Layout.sizing = {.width = CLAY_SIZING_GROW(0, 100000), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child2{};
		child2.id = child2Id;
		child2.layout = child2Layout;
		child2.backgroundColor = FlowUi::Flow_Color("#00000000");
		child2.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_LayoutConfig child2GrowSpacerLayout{};
		child2GrowSpacerLayout.sizing = {.width = CLAY_SIZING_GROW(0, 100000), .height = CLAY_SIZING_PERCENT(1.0f)};

		Clay_ElementDeclaration child2GrowSpacer{};
		child2GrowSpacer.id = child2GrowSpacerId;
		child2GrowSpacer.layout = child2GrowSpacerLayout;

		Clay_LayoutConfig spacer2Layout{};
		spacer2Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		spacer2Layout.childGap = 0;
		spacer2Layout.padding = CLAY_PADDING_ALL(0);
		spacer2Layout.childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER};
		spacer2Layout.sizing = {
			.width = CLAY_SIZING_GROW(static_cast<float>(spacer2MinWidth), static_cast<float>(spacer2MaxWidth)),
			.height = CLAY_SIZING_PERCENT(1.0f),
		};

		Clay_ElementDeclaration spacer2{};
		spacer2.id = spacer2Id;
		spacer2.layout = spacer2Layout;
		spacer2.backgroundColor = FlowUi::Flow_Color("#00000000");
		spacer2.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		Clay_ElementDeclaration spacer2Line{};
		spacer2Line.id = spacer2LineId;
		spacer2Line.layout = spacerLineLayout;
		spacer2Line.backgroundColor = FlowUi::Flow_Color("#5e646eff");
		spacer2Line.cornerRadius = CLAY_CORNER_RADIUS(0);

		Clay_LayoutConfig child3Layout{};
		child3Layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		child3Layout.childGap = 8;
		child3Layout.padding = CLAY_PADDING_ALL(0);
		child3Layout.childAlignment = {.x = CLAY_ALIGN_X_LEFT, .y = CLAY_ALIGN_Y_CENTER};
		child3Layout.sizing = {.width = CLAY_SIZING_FIT(0), .height = CLAY_SIZING_FIT(0)};

		Clay_ElementDeclaration child3{};
		child3.id = child3Id;
		child3.layout = child3Layout;
		child3.backgroundColor = FlowUi::Flow_Color("#00000000");
		child3.border = {.color = FlowUi::Flow_Color("#00000000"), .width = Clay_BorderWidth{0, 0, 0, 0, 0}};

		basicButtonParams button21{};
		button21.text = "Templates";
		button21.contentMode = basicButtonParams::ContentMode::TextOnly;
		basicButtonParams button22 = button21;
		button22.text = "Layers";
		basicButtonParams button31 = button21;
		button31.text = "Export";
		basicButtonParams button32 = button21;
		button32.text = "Settings";

		CLAY(root){
			CLAY(child1){
				resources.child1Builder
					.withElementID(child1TitlePath)
					.draw();
			};

			CLAY(spacer1){
				CLAY(spacer1Line){};
			};

			CLAY(child2){
				context.uiManager.createElement(kBasicButton, button21Path)
					.setParameters(button21)
					.draw();

				context.uiManager.createElement(kBasicButton, button22Path)
					.setParameters(button22)
					.draw();

				CLAY(child2GrowSpacer){};
			};

			CLAY(spacer2){
				CLAY(spacer2Line){};
			};

			CLAY(child3){
				context.uiManager.createElement(kBasicButton, button31Path)
					.setParameters(button31)
					.draw();

				context.uiManager.createElement(kBasicButton, button32Path)
					.setParameters(button32)
					.draw();
			};
		};
	},
};
